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

#include "hcl/drivers/altera_vvp_scaler_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_vvp_scaler hcl_altera_vvp_scaler;

typedef enum hcl_altera_vvp_scaler_algorithm {
    HCL_VVP_SCALER_ALGORITHM_NEAREST_NEIGHBOR = HCL_VVP_SCALER_NEAREST_NEIGHBOR_SCALING,
    HCL_VVP_SCALER_ALGORITHM_BILINEAR = HCL_VVP_SCALER_BILINEAR_SCALING,
    HCL_VVP_SCALER_ALGORITHM_POLYPHASE = HCL_VVP_SCALER_POLYPHASE_SCALING,
    HCL_VVP_SCALER_ALGORITHM_INVALID = 0xFF
} hcl_altera_vvp_scaler_algorithm;

hcl_altera_vvp_scaler* hcl_altera_vvp_scaler_create(hcl_register_interface* reg,
                                                    uint32_t base_address);
void hcl_altera_vvp_scaler_destroy(hcl_altera_vvp_scaler* driver);
hcl_status hcl_altera_vvp_scaler_init(hcl_altera_vvp_scaler* driver);

bool hcl_altera_vvp_scaler_get_lite_mode(const hcl_altera_vvp_scaler* driver);
bool hcl_altera_vvp_scaler_get_debug_enabled(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_pixels_in_parallel(const hcl_altera_vvp_scaler* driver);
uint32_t hcl_altera_vvp_scaler_get_max_input_width(const hcl_altera_vvp_scaler* driver);
uint32_t hcl_altera_vvp_scaler_get_max_output_width(const hcl_altera_vvp_scaler* driver);
uint32_t hcl_altera_vvp_scaler_get_img_info_width(const hcl_altera_vvp_scaler* driver);
hcl_status hcl_altera_vvp_scaler_set_img_info_width(hcl_altera_vvp_scaler* driver, uint32_t width);
uint32_t hcl_altera_vvp_scaler_get_img_info_height(const hcl_altera_vvp_scaler* driver);
hcl_status hcl_altera_vvp_scaler_set_img_info_height(hcl_altera_vvp_scaler* driver, uint32_t height);
uint32_t hcl_altera_vvp_scaler_get_img_info_interlace(const hcl_altera_vvp_scaler* driver);
hcl_status hcl_altera_vvp_scaler_set_img_info_interlace(hcl_altera_vvp_scaler* driver, uint32_t interlace);
uint32_t hcl_altera_vvp_scaler_get_img_info_colorspace(const hcl_altera_vvp_scaler* driver);
hcl_status hcl_altera_vvp_scaler_set_img_info_colorspace(hcl_altera_vvp_scaler* driver, uint32_t colorspace);
uint32_t hcl_altera_vvp_scaler_get_img_info_subsampling(const hcl_altera_vvp_scaler* driver);
hcl_status hcl_altera_vvp_scaler_set_img_info_subsampling(hcl_altera_vvp_scaler* driver, uint32_t subsampling);
uint32_t hcl_altera_vvp_scaler_get_img_info_cositing(const hcl_altera_vvp_scaler* driver);
hcl_status hcl_altera_vvp_scaler_set_img_info_cositing(hcl_altera_vvp_scaler* driver, uint32_t cositing);
uint32_t hcl_altera_vvp_scaler_get_img_info_field_count(const hcl_altera_vvp_scaler* driver);
hcl_altera_vvp_scaler_algorithm hcl_altera_vvp_scaler_get_scaling_algorithm(const hcl_altera_vvp_scaler* driver);
bool hcl_altera_vvp_scaler_is_coeffs_loading_enabled(const hcl_altera_vvp_scaler* driver);
bool hcl_altera_vvp_scaler_are_coeffs_preinitialized(const hcl_altera_vvp_scaler* driver);

bool hcl_altera_vvp_scaler_is_vertical_scaling_enabled(const hcl_altera_vvp_scaler* driver);
bool hcl_altera_vvp_scaler_is_partial_vertical_scaling_enabled(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_num_vertical_taps(const hcl_altera_vvp_scaler* driver);
uint16_t hcl_altera_vvp_scaler_get_num_vertical_phases(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_num_vertical_banks(const hcl_altera_vvp_scaler* driver);
bool hcl_altera_vvp_scaler_are_vertical_coeffs_signed(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_vertical_coeffs_int_bits(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_vertical_coeffs_frac_bits(const hcl_altera_vvp_scaler* driver);

bool hcl_altera_vvp_scaler_is_horizontal_scaling_enabled(const hcl_altera_vvp_scaler* driver);
bool hcl_altera_vvp_scaler_is_partial_horizontal_scaling_enabled(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_num_horizontal_taps(const hcl_altera_vvp_scaler* driver);
uint16_t hcl_altera_vvp_scaler_get_num_horizontal_phases(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_num_horizontal_banks(const hcl_altera_vvp_scaler* driver);
bool hcl_altera_vvp_scaler_are_horizontal_coeffs_signed(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_horizontal_coeffs_int_bits(const hcl_altera_vvp_scaler* driver);
uint8_t hcl_altera_vvp_scaler_get_horizontal_coeffs_frac_bits(const hcl_altera_vvp_scaler* driver);

hcl_status hcl_altera_vvp_scaler_is_running(hcl_altera_vvp_scaler* driver, bool* out_running);
hcl_status hcl_altera_vvp_scaler_commit_is_pending(hcl_altera_vvp_scaler* driver, bool* out_pending);
hcl_status hcl_altera_vvp_scaler_get_status(hcl_altera_vvp_scaler* driver, uint32_t* out_status);

hcl_status hcl_altera_vvp_scaler_set_h_bank(hcl_altera_vvp_scaler* driver, uint8_t bank);
hcl_status hcl_altera_vvp_scaler_set_v_bank(hcl_altera_vvp_scaler* driver, uint8_t bank);

hcl_status hcl_altera_vvp_scaler_set_output_width(hcl_altera_vvp_scaler* driver, uint32_t output_width);
hcl_status hcl_altera_vvp_scaler_set_output_height(hcl_altera_vvp_scaler* driver, uint32_t output_height);
hcl_status hcl_altera_vvp_scaler_set_total_input_width(hcl_altera_vvp_scaler* driver, uint32_t total_input_width);
hcl_status hcl_altera_vvp_scaler_set_total_output_width(hcl_altera_vvp_scaler* driver, uint32_t total_output_width);
hcl_status hcl_altera_vvp_scaler_set_total_input_height(hcl_altera_vvp_scaler* driver, uint32_t total_input_height);
hcl_status hcl_altera_vvp_scaler_set_total_output_height(hcl_altera_vvp_scaler* driver, uint32_t total_output_height);
hcl_status hcl_altera_vvp_scaler_set_h_pos_error(hcl_altera_vvp_scaler* driver, uint32_t h_pos_err);
hcl_status hcl_altera_vvp_scaler_set_v_pos_error(hcl_altera_vvp_scaler* driver, uint32_t v_pos_err);
hcl_status hcl_altera_vvp_scaler_set_h_extended_fill(hcl_altera_vvp_scaler* driver, bool extended_fill);
hcl_status hcl_altera_vvp_scaler_set_v_extended_fill(hcl_altera_vvp_scaler* driver, bool extended_fill);
hcl_status hcl_altera_vvp_scaler_set_h_phase_offset_error(hcl_altera_vvp_scaler* driver, uint32_t phase_offset, uint32_t phase_error);
hcl_status hcl_altera_vvp_scaler_set_v_phase_offset_error(hcl_altera_vvp_scaler* driver, uint32_t phase_offset, uint32_t phase_error);

hcl_status hcl_altera_vvp_scaler_set_split_h_scaling(
    hcl_altera_vvp_scaler* driver,
    uint32_t total_input_width,
    uint32_t total_output_width,
    uint32_t split_output_width,
    uint32_t split_output_left_offset,
    bool extended_fill,
    uint32_t* split_input_width,
    uint32_t* split_input_left_offset);

hcl_status hcl_altera_vvp_scaler_set_split_v_scaling(
    hcl_altera_vvp_scaler* driver,
    uint32_t total_input_height,
    uint32_t total_output_height,
    uint32_t split_output_height,
    uint32_t split_output_top_offset,
    bool extended_fill,
    uint32_t* split_input_height,
    uint32_t* split_input_top_offset);

hcl_status hcl_altera_vvp_scaler_get_h_bank(hcl_altera_vvp_scaler* driver, uint8_t* out_h_bank);
hcl_status hcl_altera_vvp_scaler_get_v_bank(hcl_altera_vvp_scaler* driver, uint8_t* out_v_bank);
hcl_status hcl_altera_vvp_scaler_get_output_width(hcl_altera_vvp_scaler* driver, uint32_t* out_output_width);
hcl_status hcl_altera_vvp_scaler_get_output_height(hcl_altera_vvp_scaler* driver, uint32_t* out_output_height);
hcl_status hcl_altera_vvp_scaler_get_total_input_width(hcl_altera_vvp_scaler* driver, uint32_t* out_total_input_width);
hcl_status hcl_altera_vvp_scaler_get_total_output_width(hcl_altera_vvp_scaler* driver, uint32_t* out_total_output_width);
hcl_status hcl_altera_vvp_scaler_get_total_input_height(hcl_altera_vvp_scaler* driver, uint32_t* out_total_input_height);
hcl_status hcl_altera_vvp_scaler_get_total_output_height(hcl_altera_vvp_scaler* driver, uint32_t* out_total_output_height);
hcl_status hcl_altera_vvp_scaler_get_h_pos_error(hcl_altera_vvp_scaler* driver, uint32_t* out_h_pos_error);
hcl_status hcl_altera_vvp_scaler_get_v_pos_error(hcl_altera_vvp_scaler* driver, uint32_t* out_v_pos_error);
hcl_status hcl_altera_vvp_scaler_get_h_extended_fill(hcl_altera_vvp_scaler* driver, bool* out_extended_fill);
hcl_status hcl_altera_vvp_scaler_get_v_extended_fill(hcl_altera_vvp_scaler* driver, bool* out_extended_fill);
hcl_status hcl_altera_vvp_scaler_get_h_phase_offset_error(hcl_altera_vvp_scaler* driver, uint32_t* out_phase_offset, uint32_t* out_phase_error);
hcl_status hcl_altera_vvp_scaler_get_v_phase_offset_error(hcl_altera_vvp_scaler* driver, uint32_t* out_phase_offset, uint32_t* out_phase_error);

hcl_status hcl_altera_vvp_scaler_set_quantized_coeffs(
    hcl_altera_vvp_scaler* driver,
    bool h_coeffs,
    const int32_t* quantized_coeffs,
    uint8_t num_coeffs,
    uint8_t bank,
    uint8_t phase);

hcl_status hcl_altera_vvp_scaler_set_coeffs(
    hcl_altera_vvp_scaler* driver,
    bool h_coeffs,
    const float* coeffs,
    uint8_t num_coeffs,
    uint8_t bank,
    uint8_t phase);

hcl_status hcl_altera_vvp_scaler_commit(hcl_altera_vvp_scaler* driver);

#ifdef __cplusplus
}
#endif
