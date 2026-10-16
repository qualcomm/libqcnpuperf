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

#ifndef QCOM_DSP_PRIV_H_
#define QCOM_DSP_PRIV_H_

#include "qcom_dsp_types.h"
#include <stdint.h>

/*
 * Full definition of the opaque context.  Not installed — callers only
 * see the forward declaration in qcom_dsp.h.
 *
 * remote_handle64 is typedef uint64_t in remote.h; stored as uint64_t here
 * to keep the type explicit.
 */
struct sysmon_query_prof_data {
    float        q6_utilization;   /* avg effective q6 clock / max q6 clock (%) */
    unsigned int q6_clock;         /* avg q6 clock (KHz) */
    float        reserved0;
    float        hvx_utilization;  /* avg HVX utilization / max q6 clock (%) */
    float        hmx_utilization;  /* avg HMX utilization / max q6 clock (%) */
    float        reserved1;
    float        reserved2;
    float        reserved3;
    float        reserved4;
    float        reserved5;
    float        reserved6;
    float        reserved7;
    float        reserved8;
    float        reserved9;
};

struct qcom_dsp_ctx {
    enum DspDomainId              domain_id;
    uint64_t                      h;         /* remote_handle64 */
    uint64_t                      attach_h;  /* remote_handle64 */
    int                           attach_h_valid;
    struct sysmon_query_prof_data *prof_data;
    unsigned int                  arch_ver;  /* cached; 0 = not yet queried */
};

enum DspReturnCode qcom_dsp_set_arch_version(struct qcom_dsp_ctx *ctx);
#endif /* QCOM_DSP_PRIV_H_ */
