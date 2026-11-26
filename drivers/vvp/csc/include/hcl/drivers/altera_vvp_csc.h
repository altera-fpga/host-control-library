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

#include "hcl/drivers/altera_vvp_csc_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_vvp_csc hcl_altera_vvp_csc;

typedef enum hcl_altera_vvp_csc_rounding {
    HCL_VVP_CSC_ROUNDING_UP = (int32_t)HCL_VVP_CSC_ROUND_UP,
    HCL_VVP_CSC_ROUNDING_HALF_EVEN = (int32_t)HCL_VVP_CSC_ROUND_HALF_EVEN,
    HCL_VVP_CSC_ROUNDING_TRUNCATE = (int32_t)HCL_VVP_CSC_ROUND_TRUNC,
    HCL_VVP_CSC_ROUNDING_INVALID = -1,
} hcl_altera_vvp_csc_rounding;

typedef enum hcl_altera_vvp_csc_color_space {
    HCL_VVP_CSC_COLOR_SPACE_RGB = (int32_t)HCL_VVP_CSC_OUTPUT_CS_RGB,
    HCL_VVP_CSC_COLOR_SPACE_YCC = (int32_t)HCL_VVP_CSC_OUTPUT_CS_YCC,
    HCL_VVP_CSC_COLOR_SPACE_YCCSD = (int32_t)HCL_VVP_CSC_OUTPUT_CS_YCCSD,
    HCL_VVP_CSC_COLOR_SPACE_YCCHD = (int32_t)HCL_VVP_CSC_OUTPUT_CS_YCCHD,
    HCL_VVP_CSC_COLOR_SPACE_MONO = (int32_t)HCL_VVP_CSC_OUTPUT_CS_MONO,
    HCL_VVP_CSC_COLOR_SPACE_RAW = (int32_t)HCL_VVP_CSC_OUTPUT_CS_RAW,
    HCL_VVP_CSC_COLOR_SPACE_RGB565 = (int32_t)HCL_VVP_CSC_OUTPUT_CS_RGB565,
    HCL_VVP_CSC_COLOR_SPACE_INVALID = -1,
} hcl_altera_vvp_csc_color_space;

typedef struct hcl_altera_vvp_csc_coefficients {
    float coeffs[HCL_VVP_CSC_NUM_COEFFS];
    float summands[HCL_VVP_CSC_NUM_SUMMANDS];
} hcl_altera_vvp_csc_coefficients;

typedef struct hcl_altera_vvp_csc_quantized_coefficients {
    int32_t coeffs[HCL_VVP_CSC_NUM_COEFFS];
    int32_t summands[HCL_VVP_CSC_NUM_SUMMANDS];
} hcl_altera_vvp_csc_quantized_coefficients;

hcl_altera_vvp_csc* hcl_altera_vvp_csc_create(hcl_register_interface* reg,
                                              uint32_t base_address);

void hcl_altera_vvp_csc_destroy(hcl_altera_vvp_csc* driver);

hcl_status hcl_altera_vvp_csc_init(hcl_altera_vvp_csc* driver);

bool hcl_altera_vvp_csc_get_lite_mode(const hcl_altera_vvp_csc* driver);
bool hcl_altera_vvp_csc_get_debug_enabled(const hcl_altera_vvp_csc* driver);
uint32_t hcl_altera_vvp_csc_get_img_info_width(const hcl_altera_vvp_csc* driver);
hcl_status hcl_altera_vvp_csc_set_img_info_width(hcl_altera_vvp_csc* driver, uint32_t width);
uint32_t hcl_altera_vvp_csc_get_img_info_height(const hcl_altera_vvp_csc* driver);
hcl_status hcl_altera_vvp_csc_set_img_info_height(hcl_altera_vvp_csc* driver, uint32_t height);
uint32_t hcl_altera_vvp_csc_get_img_info_interlace(const hcl_altera_vvp_csc* driver);
hcl_status hcl_altera_vvp_csc_set_img_info_interlace(hcl_altera_vvp_csc* driver, uint32_t interlace);
uint32_t hcl_altera_vvp_csc_get_img_info_colorspace(const hcl_altera_vvp_csc* driver);
hcl_status hcl_altera_vvp_csc_set_img_info_colorspace(hcl_altera_vvp_csc* driver, uint32_t colorspace);
uint32_t hcl_altera_vvp_csc_get_img_info_subsampling(const hcl_altera_vvp_csc* driver);
hcl_status hcl_altera_vvp_csc_set_img_info_subsampling(hcl_altera_vvp_csc* driver, uint32_t subsampling);
uint32_t hcl_altera_vvp_csc_get_img_info_cositing(const hcl_altera_vvp_csc* driver);
hcl_status hcl_altera_vvp_csc_set_img_info_cositing(hcl_altera_vvp_csc* driver, uint32_t cositing);
uint32_t hcl_altera_vvp_csc_get_img_info_field_count(const hcl_altera_vvp_csc* driver);
uint8_t hcl_altera_vvp_csc_get_bits_per_sample_in(const hcl_altera_vvp_csc* driver);
uint8_t hcl_altera_vvp_csc_get_bits_per_sample_out(const hcl_altera_vvp_csc* driver);
uint8_t hcl_altera_vvp_csc_get_coeffs_frac_bits(const hcl_altera_vvp_csc* driver);
bool hcl_altera_vvp_csc_are_coeffs_signed(const hcl_altera_vvp_csc* driver);
uint8_t hcl_altera_vvp_csc_get_coeffs_int_bits(const hcl_altera_vvp_csc* driver);
bool hcl_altera_vvp_csc_are_summands_signed(const hcl_altera_vvp_csc* driver);
uint8_t hcl_altera_vvp_csc_get_summands_int_bits(const hcl_altera_vvp_csc* driver);
int8_t hcl_altera_vvp_csc_get_binary_point_right_move(const hcl_altera_vvp_csc* driver);
hcl_altera_vvp_csc_rounding hcl_altera_vvp_csc_get_rounding_method(const hcl_altera_vvp_csc* driver);

hcl_status hcl_altera_vvp_csc_is_running(hcl_altera_vvp_csc* driver, bool* out_running);
hcl_status hcl_altera_vvp_csc_commit_is_pending(hcl_altera_vvp_csc* driver, bool* out_pending);
hcl_status hcl_altera_vvp_csc_get_status(hcl_altera_vvp_csc* driver, uint32_t* out_status);

hcl_status hcl_altera_vvp_csc_set_quantized_coeff_data(
    hcl_altera_vvp_csc* driver,
    const hcl_altera_vvp_csc_quantized_coefficients* quantized_coeffs);

hcl_status hcl_altera_vvp_csc_get_quantized_coeff_data(
    hcl_altera_vvp_csc* driver,
    hcl_altera_vvp_csc_quantized_coefficients* out_quantized_coeffs);

hcl_status hcl_altera_vvp_csc_set_coeff_data(
    hcl_altera_vvp_csc* driver,
    const hcl_altera_vvp_csc_coefficients* coeffs,
    int32_t summand_rescale);

hcl_status hcl_altera_vvp_csc_get_coeff_data(
    hcl_altera_vvp_csc* driver,
    hcl_altera_vvp_csc_coefficients* out_coeffs);

hcl_status hcl_altera_vvp_csc_set_output_color_space(
    hcl_altera_vvp_csc* driver,
    hcl_altera_vvp_csc_color_space output_color_space);

hcl_status hcl_altera_vvp_csc_get_output_color_space(
    hcl_altera_vvp_csc* driver,
    hcl_altera_vvp_csc_color_space* out_output_color_space);

hcl_status hcl_altera_vvp_csc_commit(hcl_altera_vvp_csc* driver);

#ifdef __cplusplus
}
#endif
