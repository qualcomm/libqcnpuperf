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
 * @file qcom_dsp.h
 * @brief Qualcomm DSP performance library — public API
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 * @author Srinivas Kandagatla (srinivas.kandagatla@oss.qualcomm.com)
 *
 * Typical usage:
 * @code
 *   struct qcom_dsp_ctx *ctx = qcom_dsp_open(DSP_NPU0);
 *   if (!ctx) { ... handle error ... }
 *
 *   int n;
 *   struct sysmon_query_prof_data *data = qcom_dsp_get_prof_data(ctx, &n);
 *   if (data && n > 0) {
 *       float util = qcom_dsp_prof_get_q6_utilization(data);
 *       unsigned int clk = qcom_dsp_prof_get_q6_clock(data);
 *   }
 *
 *   qcom_dsp_close(ctx);
 * @endcode
 */

#ifndef QCOM_DSP_H_
#define QCOM_DSP_H_

#include "qcom_dsp_types.h"

/**
 * @brief Open a session to the specified DSP domain.
 *
 * Establishes a FastRPC connection to the sysmon query interface on the
 * requested domain and allocates shared memory for profiling data.
 * The Hexagon architecture version is queried and cached at this point.
 *
 * @param domain_id  DSP domain to connect to (e.g. DSP_NPU0 for CDSP).
 * @return Opaque context pointer on success, NULL on failure.
 *         Must be released with qcom_dsp_close().
 */
struct qcom_dsp_ctx *qcom_dsp_open(enum DspDomainId domain_id);

/**
 * @brief Query the DSP for a fresh set of profiling metrics.
 *
 * Performs a synchronous FastRPC call to populate the internal profiling
 * buffer.  The returned pointer is valid until the next call to this
 * function or until qcom_dsp_close() is called.
 *
 * @param ctx         Context obtained from qcom_dsp_open().
 * @param no_metrics  Set to the number of valid metric slots on success.
 * @return Pointer to the profiling data on success, NULL on failure.
 *         Do not free the returned pointer.
 */
struct sysmon_query_prof_data *qcom_dsp_get_prof_data(struct qcom_dsp_ctx *ctx, int *no_metrics);

/**
 * @brief Return the average effective Q6 utilisation as a percentage.
 * @param data  Profiling snapshot from qcom_dsp_get_prof_data(). May be NULL.
 * @return Utilisation in % [0.0, 100.0], or 0.0 if @p data is NULL.
 */
float qcom_dsp_prof_get_q6_utilization(const struct sysmon_query_prof_data *data);

/**
 * @brief Return the average Q6 clock frequency.
 * @param data  Profiling snapshot from qcom_dsp_get_prof_data(). May be NULL.
 * @return Clock frequency in KHz, or 0 if @p data is NULL.
 */
unsigned int qcom_dsp_prof_get_q6_clock(const struct sysmon_query_prof_data *data);

/**
 * @brief Return the average HVX utilisation as a percentage.
 * @param data  Profiling snapshot from qcom_dsp_get_prof_data(). May be NULL.
 * @return Utilisation in % [0.0, 100.0], or 0.0 if @p data is NULL.
 */
float qcom_dsp_prof_get_hvx_utilization(const struct sysmon_query_prof_data *data);

/**
 * @brief Return the average HMX utilisation as a percentage.
 * @param data  Profiling snapshot from qcom_dsp_get_prof_data(). May be NULL.
 * @return Utilisation in % [0.0, 100.0], or 0.0 if @p data is NULL.
 */
float qcom_dsp_prof_get_hmx_utilization(const struct sysmon_query_prof_data *data);

/**
 * @brief Return the Hexagon processor architecture version.
 *
 * The version is read from the kernel via FASTRPC_IOCTL_GET_DSP_INFO at
 * qcom_dsp_open() time and cached in the context.  Typical values are 73
 * (v73) or 75 (v75).
 *
 * @param ctx  Context obtained from qcom_dsp_open(). May be NULL.
 * @return Architecture version number, or 0 if unavailable or @p ctx is NULL.
 */
unsigned int qcom_dsp_prof_get_q6_arch_version(struct qcom_dsp_ctx *ctx);

/**
 * @brief Close a DSP session and release all associated resources.
 *
 * Deinitialises the sysmon query interface, frees shared memory, and
 * closes the FastRPC handle.  Safe to call with NULL.
 *
 * @param ctx  Context obtained from qcom_dsp_open(), or NULL.
 */
void qcom_dsp_close(struct qcom_dsp_ctx *ctx);

#endif /* QCOM_DSP_H_ */
