/*
    Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted (subject to the limitations in the
    disclaimer below) provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above
          copyright notice, this list of conditions and the following
          disclaimer in the documentation and/or other materials provided
          with the distribution.
        * Neither the name of Qualcomm Technologies, Inc. nor the names of its
          contributors may be used to endorse or promote products derived
          from this software without specific prior written permission.
    NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
    GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
    HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
    GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
    IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
    IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * @file qcnpuperfd.c
 * @brief Daemon that writes NPU/DSP performance stats to a file every second.
 *
 * Stats are written to a temporary file then atomically renamed into place so
 * that readers always see a complete, consistent snapshot.
 *
 * Usage:
 *   qcnpuperfd [output_path]
 *
 * output_path defaults to /tmp/qcnpuperf_metrics if not supplied.
 *
 * Designed to run under systemd (no self-daemonization). Errors are written to
 * stderr and captured by journald.
 *
 * Output file format (plain key=value, one metric per line):
 *   q6_utilization=<float>
 *   q6_clock_khz=<uint>
 *   hvx_utilization=<float>
 *   hmx_utilization=<float>
 */

#include "qcom_dsp.h"

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_OUTPUT_PATH "/tmp/qcnpuperf_metrics"

/* Set to 0 by the signal handler to break the poll loop. */
static volatile sig_atomic_t running = 1;

static void handle_signal(int sig)
{
	(void)sig;
	running = 0;
}

/**
 * install_signal_handlers - catch SIGTERM and SIGINT for clean shutdown.
 *
 * Returns 0 on success, -1 on failure (with a message on stderr).
 */
static int install_signal_handlers(void)
{
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_signal;
	sigemptyset(&sa.sa_mask);
	/* SA_RESTART is intentionally NOT set so that sleep() is interrupted. */
	sa.sa_flags = 0;

	if (sigaction(SIGTERM, &sa, NULL) != 0) {
		fprintf(stderr, "qcnpuperfd: sigaction(SIGTERM): %s\n", strerror(errno));
		return -1;
	}
	if (sigaction(SIGINT, &sa, NULL) != 0) {
		fprintf(stderr, "qcnpuperfd: sigaction(SIGINT): %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

/**
 * build_tmp_path - construct the temp file path in the same directory as
 * output_path so that rename(2) is guaranteed to be atomic (same filesystem).
 *
 * The caller must free() the returned string.
 * Returns NULL on allocation failure.
 */
static char *build_tmp_path(const char *output_path)
{
	/* dirname() may modify its argument; work on a copy. */
	char *path_copy = strdup(output_path);
	if (!path_copy)
		return NULL;

	const char *dir = dirname(path_copy);

	/* "<dir>/.qcnpuperfd_tmp.<pid>" */
	char *tmp = NULL;
	int ret = asprintf(&tmp, "%s/.qcnpuperfd_tmp.%d", dir, (int)getpid());
	free(path_copy);

	if (ret < 0)
		return NULL;
	return tmp;
}

/**
 * write_stats - write the four NPU metrics to fd in key=value format.
 *
 * Returns the number of bytes written, or -1 on error.
 */
static int write_stats(int fd, const struct sysmon_query_prof_data *data)
{
	char buf[256];
	int len = snprintf(buf, sizeof(buf),
		"q6_utilization=%.2f\n"
		"q6_clock_khz=%u\n"
		"hvx_utilization=%.2f\n"
		"hmx_utilization=%.2f\n",
		data->q6_utilization,
		data->q6_clock,
		data->hvx_utilization,
		data->hmx_utilization);

	if (len < 0 || (size_t)len >= sizeof(buf)) {
		fprintf(stderr, "qcnpuperfd: snprintf overflow\n");
		return -1;
	}

	ssize_t written = write(fd, buf, (size_t)len);
	if (written < 0) {
		fprintf(stderr, "qcnpuperfd: write: %s\n", strerror(errno));
		return -1;
	}
	return (int)written;
}

int main(int argc, char *argv[])
{
	const char *output_path = (argc > 1) ? argv[1] : DEFAULT_OUTPUT_PATH;
	int exit_code = EXIT_SUCCESS;

	if (install_signal_handlers() != 0)
		return EXIT_FAILURE;

	char *tmp_path = build_tmp_path(output_path);
	if (!tmp_path) {
		fprintf(stderr, "qcnpuperfd: out of memory building tmp path\n");
		return EXIT_FAILURE;
	}

	fprintf(stderr, "qcnpuperfd: starting, output=%s\n", output_path);

	enum DspReturnCode ret = qcom_dsp_init(DSP_NPU0);
	if (ret != RETURN_CODE_DSP_LIB_SUCCESS) {
		fprintf(stderr, "qcnpuperfd: qcom_dsp_init failed, ret=%d\n", ret);
		free(tmp_path);
		return EXIT_FAILURE;
	}

	while (running) {
		int no_metrics = 0;
		struct sysmon_query_prof_data *data =
			qcom_dsp_get_prof_data(DSP_NPU0, &no_metrics);

		if (!data || no_metrics <= 0) {
			fprintf(stderr, "qcnpuperfd: qcom_dsp_get_prof_data failed\n");
			exit_code = EXIT_FAILURE;
			break;
		}

		/* Write to temp file, then atomically rename into place. */
		int fd = open(tmp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0) {
			fprintf(stderr, "qcnpuperfd: open(%s): %s\n",
				tmp_path, strerror(errno));
			exit_code = EXIT_FAILURE;
			break;
		}

		if (write_stats(fd, data) < 0) {
			close(fd);
			unlink(tmp_path);
			exit_code = EXIT_FAILURE;
			break;
		}

		if (close(fd) != 0) {
			fprintf(stderr, "qcnpuperfd: close(%s): %s\n",
				tmp_path, strerror(errno));
			unlink(tmp_path);
			exit_code = EXIT_FAILURE;
			break;
		}

		if (rename(tmp_path, output_path) != 0) {
			fprintf(stderr, "qcnpuperfd: rename(%s -> %s): %s\n",
				tmp_path, output_path, strerror(errno));
			unlink(tmp_path);
			exit_code = EXIT_FAILURE;
			break;
		}

		sleep(1);
	}

	/* Clean up any leftover temp file (e.g. if we broke before rename). */
	unlink(tmp_path);
	free(tmp_path);

	ret = qcom_dsp_deinit(DSP_NPU0);
	if (ret != RETURN_CODE_DSP_LIB_SUCCESS)
		fprintf(stderr, "qcnpuperfd: qcom_dsp_deinit failed, ret=%d\n", ret);

	fprintf(stderr, "qcnpuperfd: exiting\n");
	return exit_code;
}
