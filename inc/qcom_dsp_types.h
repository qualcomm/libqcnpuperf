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
 * Minimal types shared between qcom_dsp.h and translation units that use
 * the kernel UAPI directly (e.g. qcom_dsp_arch.c).  Kept separate so that
 * code needing <misc/fastrpc.h> can include this header without pulling in
 * remote.h, which redefines enum fastrpc_map_flags and conflicts with the
 * kernel header.
 *
 * Values match ADSP_DOMAIN_ID / CDSP_DOMAIN_ID from remote.h.
 */
enum DspDomainId {
    DSP_ADSP = 0,
    DSP_NPU0 = 3,
};

/* Opaque session context returned by qcom_dsp_open(). */
struct qcom_dsp_ctx;

/* Forward declaration so qcom_dsp_priv.h can hold a pointer without
 * pulling in qcom_dsp.h (which pulls in remote.h). */
struct sysmon_query_prof_data;

enum DspReturnCode {
    RETURN_CODE_DSP_LIB_SUCCESS = 0,
    RETURN_CODE_DSP_LIB_FAIL = 1,
    RETURN_CODE_DSP_SYSMON_QUERY_OPEN_FAILED,
    RETURN_CODE_DSP_SYSMON_QUERY_INIT_FAILED,
    RETURN_CODE_DSP_SYSMON_QUERY_RPC_MEM_ALLOC_FAILED,
    RETURN_CODE_DSP_SYSMON_QUERY_GET_PROF_DATA_FAILED,
    RETURN_CODE_DSP_SYSMON_QUERY_DEINIT_FAILED,
};

#endif /* QCOM_DSP_TYPES_H_ */
