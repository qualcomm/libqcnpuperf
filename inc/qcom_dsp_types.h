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

#ifndef QCOM_DSP_TYPES_H_
#define QCOM_DSP_TYPES_H_

/*
 * Minimal types shared between qcom_dsp.h and translation units that need
 * the kernel UAPI directly (e.g. qcom_dsp_arch.c).  Kept separate so that
 * code including <misc/fastrpc.h> can use this header without also pulling
 * in remote.h, which redefines enum fastrpc_map_flags and causes a conflict.
 */

/**
 * @brief DSP domain identifiers.
 *
 * Values match ADSP_DOMAIN_ID / CDSP_DOMAIN_ID in remote.h.
 */
enum DspDomainId {
    DSP_ADSP = 0, /**< Audio DSP (ADSP) */
    DSP_NPU0 = 3, /**< Compute DSP / NPU (CDSP) */
};

/**
 * @brief Opaque session context.
 *
 * Allocated by qcom_dsp_open() and released by qcom_dsp_close().
 * Callers must not access members directly.
 */
struct qcom_dsp_ctx;

/**
 * @brief Opaque profiling data snapshot.
 *
 * Returned by qcom_dsp_get_prof_data().  Individual fields are accessed
 * through the qcom_dsp_prof_get_*() accessor functions.
 * Callers must not access members directly or free the pointer.
 */
struct sysmon_query_prof_data;

/**
 * @brief Return codes for internal DSP library operations.
 */
enum DspReturnCode {
    RETURN_CODE_DSP_LIB_SUCCESS = 0,              /**< Operation succeeded */
    RETURN_CODE_DSP_LIB_FAIL = 1,                 /**< Generic failure */
    RETURN_CODE_DSP_SYSMON_QUERY_OPEN_FAILED,     /**< FastRPC handle open failed */
    RETURN_CODE_DSP_SYSMON_QUERY_INIT_FAILED,     /**< Sysmon query init failed */
    RETURN_CODE_DSP_SYSMON_QUERY_RPC_MEM_ALLOC_FAILED, /**< Shared memory allocation failed */
    RETURN_CODE_DSP_SYSMON_QUERY_GET_PROF_DATA_FAILED,  /**< Profiling data query failed */
    RETURN_CODE_DSP_SYSMON_QUERY_DEINIT_FAILED,   /**< Sysmon query deinit failed */
};

#endif /* QCOM_DSP_TYPES_H_ */
