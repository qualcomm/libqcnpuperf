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
 * @file dsp_lib.c
 * @brief DSP library implementation
 * @author Himanshu Keshri (hkeshri@qti.qualcomm.com)
 */

#include "qcom_dsp.h"
#include "qcom_dsp_priv.h"
#include "dspquery_stub.h"
#include "remote.h"
#include "rpcmem.h"

#include <stdlib.h>
#include <string.h>

static void get_full_uri_info(const char *uri, char *full_uri, int size,
                              enum DspDomainId domain_id)
{
    memset(full_uri, 0, (size_t)size);
    strlcpy(full_uri, uri, (size_t)size);
    remote_handle64 fd;

    if (domain_id == DSP_ADSP) {
        strlcat(full_uri, ADSP_DOMAIN, (size_t)size);
        remote_handle64_open(ITRANSPORT_PREFIX "attachuserpd&_dom=adsp", &fd);
    } else if (domain_id == DSP_NPU0) {
        strlcat(full_uri, CDSP_DOMAIN, (size_t)size);
        remote_handle64_open(ITRANSPORT_PREFIX "attachuserpd&_dom=cdsp", &fd);
    } else {
        memset(full_uri, 0, (size_t)size);
    }
}

struct qcom_dsp_ctx *qcom_dsp_open(enum DspDomainId domain_id)
{
    char full_uri[256] = {0};
    remote_handle64 h;

    struct qcom_dsp_ctx *ctx = calloc(1, sizeof(*ctx));
    if (!ctx)
        return NULL;

    ctx->domain_id = domain_id;

    get_full_uri_info(sysmonquery_URI, full_uri, sizeof(full_uri), domain_id);
    if (sysmonquery_open(full_uri, &h) != 0)
        goto err_free;
    ctx->h = h;

    if (sysmonquery_init(h, 0) != 0)
        goto err_close;

    ctx->prof_data = rpcmem_alloc(RPCMEM_DEFAULT_HEAP,
                                  RPCMEM_DEFAULT_FLAGS | RPCMEM_HEAP_NONCOHERENT,
                                  sizeof(struct sysmon_query_prof_data));
    if (!ctx->prof_data)
        goto err_deinit;

    qcom_dsp_set_arch_version(ctx);
    memset(ctx->prof_data, 0, sizeof(struct sysmon_query_prof_data));
    return ctx;

err_deinit:
    sysmonquery_deinit(h, 0);
err_close:
    sysmonquery_close(h);
err_free:
    free(ctx);
    return NULL;
}

struct sysmon_query_prof_data *qcom_dsp_get_prof_data(struct qcom_dsp_ctx *ctx,
                                                      int *no_metrics)
{
    if (!ctx || !no_metrics)
        return NULL;

    int ret = sysmonquery_get_profdata((remote_handle64)ctx->h,
                                       (unsigned char *)ctx->prof_data,
                                       sizeof(struct sysmon_query_prof_data),
                                       no_metrics, 0);
    return ret == 0 ? ctx->prof_data : NULL;
}

unsigned int qcom_dsp_prof_get_q6_arch_version(struct qcom_dsp_ctx *ctx)
{
	return ctx ? ctx->arch_ver : 0;
}

float qcom_dsp_prof_get_q6_utilization(const struct sysmon_query_prof_data *data)
{
    return data ? data->q6_utilization : 0.0f;
}

unsigned int qcom_dsp_prof_get_q6_clock(const struct sysmon_query_prof_data *data)
{
    return data ? data->q6_clock : 0;
}

float qcom_dsp_prof_get_hvx_utilization(const struct sysmon_query_prof_data *data)
{
    return data ? data->hvx_utilization : 0.0f;
}

float qcom_dsp_prof_get_hmx_utilization(const struct sysmon_query_prof_data *data)
{
    return data ? data->hmx_utilization : 0.0f;
}

void qcom_dsp_close(struct qcom_dsp_ctx *ctx)
{
    if (!ctx)
        return;

    sysmonquery_deinit((remote_handle64)ctx->h, 0);

    if (ctx->prof_data)
        rpcmem_free(ctx->prof_data);

    if (ctx->h)
        sysmonquery_close((remote_handle64)ctx->h);

    free(ctx);
}
