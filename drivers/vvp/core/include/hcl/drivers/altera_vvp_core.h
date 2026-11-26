/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HCL_VVP_CORE_VID_PID_PRODUCT_ID_MASK        0x0000FFFFu
#define HCL_VVP_CORE_VID_PID_PRODUCT_ID_SHIFT       0u
#define HCL_VVP_CORE_VID_PID_VENDOR_ID_MASK         0xFFFF0000u
#define HCL_VVP_CORE_VID_PID_VENDOR_ID_SHIFT        16u

#define HCL_VVP_CORE_VERSION_QPDS_MAJOR_MASK        0xFF000000u
#define HCL_VVP_CORE_VERSION_QPDS_MAJOR_SHIFT       24u
#define HCL_VVP_CORE_VERSION_QPDS_UPDATE_MASK       0x00FF0000u
#define HCL_VVP_CORE_VERSION_QPDS_UPDATE_SHIFT      16u
#define HCL_VVP_CORE_VERSION_QPDS_PATCH_MASK        0x0000FF00u
#define HCL_VVP_CORE_VERSION_QPDS_PATCH_SHIFT       8u
#define HCL_VVP_CORE_VERSION_REGMAP_VERSION_MASK    0x000000FFu
#define HCL_VVP_CORE_VERSION_REGMAP_VERSION_SHIFT   0u

#define HCL_VVP_CORE_RT_BASE_REG_ADDR               0x140u
#define HCL_VVP_VENDOR_ID                           0x6AF7u


typedef struct hcl_altera_vvp_id {
    uint16_t vendor_id;
    uint16_t product_id;
} hcl_altera_vvp_id;

typedef struct hcl_altera_vvp_version {
    uint8_t qpds_major;
    uint8_t qpds_update;
    uint8_t qpds_patch;
    uint8_t regmap_version;
} hcl_altera_vvp_version;

static inline hcl_status hcl_altera_vvp_core_parse_id(uint32_t vid_pid,
                                                      hcl_altera_vvp_id* out_id) {
    if (out_id == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    out_id->vendor_id = (uint16_t)((vid_pid & HCL_VVP_CORE_VID_PID_VENDOR_ID_MASK) >> HCL_VVP_CORE_VID_PID_VENDOR_ID_SHIFT);
    out_id->product_id = (uint16_t)((vid_pid & HCL_VVP_CORE_VID_PID_PRODUCT_ID_MASK) >> HCL_VVP_CORE_VID_PID_PRODUCT_ID_SHIFT);

    return hcl_status_ok();
}

static inline hcl_status hcl_altera_vvp_core_parse_version(uint32_t version,
                                                           hcl_altera_vvp_version* out_version) {
    if (out_version == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    out_version->qpds_major = (uint8_t)((version & HCL_VVP_CORE_VERSION_QPDS_MAJOR_MASK) >> HCL_VVP_CORE_VERSION_QPDS_MAJOR_SHIFT);
    out_version->qpds_update = (uint8_t)((version & HCL_VVP_CORE_VERSION_QPDS_UPDATE_MASK) >> HCL_VVP_CORE_VERSION_QPDS_UPDATE_SHIFT);
    out_version->qpds_patch = (uint8_t)((version & HCL_VVP_CORE_VERSION_QPDS_PATCH_MASK) >> HCL_VVP_CORE_VERSION_QPDS_PATCH_SHIFT);
    out_version->regmap_version = (uint8_t)((version & HCL_VVP_CORE_VERSION_REGMAP_VERSION_MASK) >> HCL_VVP_CORE_VERSION_REGMAP_VERSION_SHIFT);

    return hcl_status_ok();
}

#ifdef __cplusplus
}
#endif
