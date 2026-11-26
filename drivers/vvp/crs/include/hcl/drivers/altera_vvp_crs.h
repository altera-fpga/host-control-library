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

#include <stdbool.h>
#include <stdint.h>

#include "hcl/drivers/altera_vvp_crs_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_vvp_crs hcl_altera_vvp_crs;
typedef struct hcl_altera_vvp_core hcl_altera_vvp_core;

typedef enum hcl_altera_vvp_crs_subsampling {
    HCL_VVP_CRS_SUBSAMPLING_MODE_420 = HCL_VVP_CRS_SUBSAMPLING_420,
    HCL_VVP_CRS_SUBSAMPLING_MODE_444 = HCL_VVP_CRS_SUBSAMPLING_444,
    HCL_VVP_CRS_SUBSAMPLING_MODE_422 = HCL_VVP_CRS_SUBSAMPLING_422,
    HCL_VVP_CRS_SUBSAMPLING_MODE_INVALID = HCL_VVP_CRS_SUBSAMPLING_INVALID,
} hcl_altera_vvp_crs_subsampling;

hcl_altera_vvp_crs* hcl_altera_vvp_crs_create(hcl_register_interface* reg,
                                              uint32_t base_address);

void hcl_altera_vvp_crs_destroy(hcl_altera_vvp_crs* driver);

hcl_status hcl_altera_vvp_crs_init(hcl_altera_vvp_crs* driver);

bool hcl_altera_vvp_crs_get_lite_mode(const hcl_altera_vvp_crs* driver);
bool hcl_altera_vvp_crs_get_debug_enabled(const hcl_altera_vvp_crs* driver);
uint32_t hcl_altera_vvp_crs_get_img_info_width(const hcl_altera_vvp_crs* driver);
hcl_status hcl_altera_vvp_crs_set_img_info_width(hcl_altera_vvp_crs* driver, uint32_t width);
uint32_t hcl_altera_vvp_crs_get_img_info_height(const hcl_altera_vvp_crs* driver);
hcl_status hcl_altera_vvp_crs_set_img_info_height(hcl_altera_vvp_crs* driver, uint32_t height);
uint32_t hcl_altera_vvp_crs_get_img_info_interlace(const hcl_altera_vvp_crs* driver);
hcl_status hcl_altera_vvp_crs_set_img_info_interlace(hcl_altera_vvp_crs* driver, uint32_t interlace);
uint32_t hcl_altera_vvp_crs_get_img_info_colorspace(const hcl_altera_vvp_crs* driver);
hcl_status hcl_altera_vvp_crs_set_img_info_colorspace(hcl_altera_vvp_crs* driver, uint32_t colorspace);
uint32_t hcl_altera_vvp_crs_get_img_info_subsampling(const hcl_altera_vvp_crs* driver);
hcl_status hcl_altera_vvp_crs_set_img_info_subsampling(hcl_altera_vvp_crs* driver, uint32_t subsampling);
uint32_t hcl_altera_vvp_crs_get_img_info_cositing(const hcl_altera_vvp_crs* driver);
hcl_status hcl_altera_vvp_crs_set_img_info_cositing(hcl_altera_vvp_crs* driver, uint32_t cositing);
uint32_t hcl_altera_vvp_crs_get_img_info_field_count(const hcl_altera_vvp_crs* driver);

hcl_status hcl_altera_vvp_crs_is_running(hcl_altera_vvp_crs* driver, bool* out_running);
hcl_status hcl_altera_vvp_crs_commit_is_pending(hcl_altera_vvp_crs* driver, bool* out_pending);
hcl_status hcl_altera_vvp_crs_get_status(hcl_altera_vvp_crs* driver, uint32_t* out_status);

bool hcl_altera_vvp_crs_is_conversion_supported(
    const hcl_altera_vvp_crs* driver,
    hcl_altera_vvp_crs_subsampling input_subsampling,
    hcl_altera_vvp_crs_subsampling output_subsampling);

hcl_status hcl_altera_vvp_crs_get_output_subsampling(
    hcl_altera_vvp_crs* driver,
    hcl_altera_vvp_crs_subsampling* out_output_subsampling);

hcl_status hcl_altera_vvp_crs_set_output_subsampling(
    hcl_altera_vvp_crs* driver,
    hcl_altera_vvp_crs_subsampling output_subsampling);

hcl_status hcl_altera_vvp_crs_commit(hcl_altera_vvp_crs* driver);

#ifdef __cplusplus
}
#endif
