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

#include "qcom_dsp.h"
#include <unistd.h>
#include <ncurses.h>

static const char *domain_to_str(enum DspDomainId domain)
{
	switch (domain) {
	case DSP_ADSP: return "ADSP";
	case DSP_NPU0: return "NPU0 (CDSP)";
	default:       return "UNKNOWN";
	}
}

/*TODO: Extend it to other DSPS */ 
int main(int argc, char *argv[])
{
	struct sysmon_query_prof_data *data;
	struct qcom_dsp_ctx *ctx;
	int no_metrics = 0;

	initscr();
	noecho();
	curs_set(FALSE);

	ctx = qcom_dsp_open(DSP_NPU0);
	if (!ctx) {
		fprintf(stderr, "qcom_dsp_open failed\n");
		endwin();
		return EXIT_FAILURE;
	}

	while (true) {
		data = qcom_dsp_get_prof_data(ctx, &no_metrics);
		if (!data || no_metrics <= 0) {
			fprintf(stderr, "qcom_dsp_get_prof_data failed\n");
			qcom_dsp_close(ctx);
			endwin();
			return EXIT_FAILURE;
		}

		mvprintw(0, 0, "----------------- %s Stats---------------------\n", domain_to_str(DSP_NPU0));
		mvprintw(1, 0, "Q6 Utilization        : %.2f %%\n", data->q6_utilization);
		mvprintw(2, 0, "Q6 Clock              : %u KHz\n", data->q6_clock);
		mvprintw(3, 0, "HVX Utilization       : %.2f %%\n", data->hvx_utilization);
		mvprintw(4, 0, "HMX Utiliziation       : %.2f %%\n", data->hmx_utilization);
		mvprintw(6, 0, "-------------------------------------------------\n");
		refresh();
		sleep(1);
	}

	qcom_dsp_close(ctx);
	endwin();
	return EXIT_SUCCESS;
}
