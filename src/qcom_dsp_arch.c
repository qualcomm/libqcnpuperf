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

/*
 * Kept in a separate translation unit from qcom_dsp.c so that
 * <misc/fastrpc.h> (kernel UAPI) can be included directly without
 * conflicting with the fastrpc_map_flags definition in remote.h, which
 * qcom_dsp.h pulls in transitively.
 */

#include "qcom_dsp_priv.h"

#include <misc/fastrpc.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* ARCH_VER is the 7th entry (0-indexed) of enum remote_dsp_attributes in
 * remote.h.  Defined here as a plain constant to avoid including remote.h
 * in this translation unit. */
#define ARCH_VER 6

enum DspReturnCode qcom_dsp_set_arch_version(struct qcom_dsp_ctx *ctx)
{
    const char *dev;

    if (!ctx)
        return RETURN_CODE_DSP_LIB_FAIL;

    switch (ctx->domain_id) {
    case DSP_ADSP: dev = "/dev/fastrpc-adsp"; break;
    case DSP_NPU0: dev = "/dev/fastrpc-cdsp"; break;
    default:       return RETURN_CODE_DSP_LIB_FAIL;
    }

    int fd = open(dev, O_RDWR);
    if (fd < 0)
        return RETURN_CODE_DSP_LIB_FAIL;

    struct fastrpc_ioctl_capability cap = {
        .attribute_id = ARCH_VER,
    };

    enum DspReturnCode ret = RETURN_CODE_DSP_LIB_FAIL;
    if (ioctl(fd, FASTRPC_IOCTL_GET_DSP_INFO, &cap) == 0) {
        ctx->arch_ver = cap.capability & 0xFF;
        ret = RETURN_CODE_DSP_LIB_SUCCESS;
    }

    close(fd);
    return ret;
}
