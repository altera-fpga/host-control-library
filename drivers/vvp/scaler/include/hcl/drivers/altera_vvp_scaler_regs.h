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

#define HCL_VVP_SCALER_PRODUCT_ID                           0x0234u
#define HCL_VVP_SCALER_MIN_SUPPORTED_REGMAP_VERSION         1u
#define HCL_VVP_SCALER_MAX_SUPPORTED_REGMAP_VERSION         1u
#define HCL_VVP_SCALER_MAX_NUM_TAPS                         64u
#define HCL_VVP_SCALER_MAX_COEFF_WIDTH                      31u

#define HCL_VVP_SCALER_NEAREST_NEIGHBOR_SCALING             0u
#define HCL_VVP_SCALER_BILINEAR_SCALING                     1u
#define HCL_VVP_SCALER_POLYPHASE_SCALING                    2u

/* Compile-time parameter registers. */
#define HCL_VVP_SCALER_VID_PID_REG_ADDR                     0x000u
#define HCL_VVP_SCALER_VERSION_REG_ADDR                     0x004u
#define HCL_VVP_SCALER_LITE_MODE_REG_ADDR                   0x008u
#define HCL_VVP_SCALER_DEBUG_ENABLED_REG_ADDR               0x00Cu
#define HCL_VVP_SCALER_PIXELS_IN_PARALLEL_REG_ADDR          0x010u
#define HCL_VVP_SCALER_MAX_INPUT_WIDTH_REG_ADDR             0x014u
#define HCL_VVP_SCALER_MAX_OUTPUT_WIDTH_REG_ADDR            0x018u
#define HCL_VVP_SCALER_ALGORITHM_REG_ADDR                   0x01Cu
#define HCL_VVP_SCALER_COEFFS_RUNTIME_LOAD_REG_ADDR         0x020u
#define HCL_VVP_SCALER_COEFFS_MEM_INIT_REG_ADDR             0x024u

#define HCL_VVP_SCALER_V_SCALING_ENABLED_REG_ADDR           0x028u
#define HCL_VVP_SCALER_V_NUM_TAPS_REG_ADDR                  0x02Cu
#define HCL_VVP_SCALER_V_NUM_PHASES_REG_ADDR                0x030u
#define HCL_VVP_SCALER_V_NUM_BANKS_REG_ADDR                 0x034u
#define HCL_VVP_SCALER_V_COEFFS_SIGNED_REG_ADDR             0x038u
#define HCL_VVP_SCALER_V_COEFFS_INT_BITS_REG_ADDR           0x03Cu
#define HCL_VVP_SCALER_V_COEFFS_FRAC_BITS_REG_ADDR          0x040u

#define HCL_VVP_SCALER_H_SCALING_ENABLED_REG_ADDR           0x044u
#define HCL_VVP_SCALER_H_NUM_TAPS_REG_ADDR                  0x048u
#define HCL_VVP_SCALER_H_NUM_PHASES_REG_ADDR                0x04Cu
#define HCL_VVP_SCALER_H_NUM_BANKS_REG_ADDR                 0x050u
#define HCL_VVP_SCALER_H_COEFFS_SIGNED_REG_ADDR             0x054u
#define HCL_VVP_SCALER_H_COEFFS_INT_BITS_REG_ADDR           0x058u
#define HCL_VVP_SCALER_H_COEFFS_FRAC_BITS_REG_ADDR          0x05Cu

#define HCL_VVP_SCALER_V_PARTIAL_SCALING_ENABLED_REG_ADDR   0x060u
#define HCL_VVP_SCALER_H_PARTIAL_SCALING_ENABLED_REG_ADDR   0x064u

/* Runtime registers. */
#define HCL_VVP_SCALER_IMG_INFO_WIDTH_REG_ADDR              0x0120u
#define HCL_VVP_SCALER_IMG_INFO_HEIGHT_REG_ADDR             0x0124u
#define HCL_VVP_SCALER_IMG_INFO_INTERLACE_REG_ADDR          0x0128u
#define HCL_VVP_SCALER_IMG_INFO_COLORSPACE_REG_ADDR         0x0130u
#define HCL_VVP_SCALER_IMG_INFO_SUBSAMPLING_REG_ADDR        0x0134u
#define HCL_VVP_SCALER_IMG_INFO_COSITING_REG_ADDR           0x0138u
#define HCL_VVP_SCALER_IMG_INFO_FIELD_COUNT_REG_ADDR        0x013Cu
#define HCL_VVP_SCALER_STATUS_REG_ADDR                      (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x000u)
#define HCL_VVP_SCALER_COMMIT_REG_ADDR                      (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x004u)
#define HCL_VVP_SCALER_OUTPUT_WIDTH_REG_ADDR                (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x008u)
#define HCL_VVP_SCALER_OUTPUT_HEIGHT_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x00Cu)
#define HCL_VVP_SCALER_H_BANK_REG_ADDR                      (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x010u)
#define HCL_VVP_SCALER_V_BANK_REG_ADDR                      (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x014u)

#define HCL_VVP_SCALER_TOTAL_IN_WIDTH_REG_ADDR              (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x018u)
#define HCL_VVP_SCALER_TOTAL_OUT_WIDTH_REG_ADDR             (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x01Cu)
#define HCL_VVP_SCALER_H_POS_ERROR_REG_ADDR                 (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x020u)
#define HCL_VVP_SCALER_H_PHASE_OFFSET_REG_ADDR              (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x024u)
#define HCL_VVP_SCALER_H_PHASE_ERROR_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x028u)
#define HCL_VVP_SCALER_H_EXTENDED_FILL_REG_ADDR             (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x02Cu)

#define HCL_VVP_SCALER_TOTAL_IN_HEIGHT_REG_ADDR             (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x030u)
#define HCL_VVP_SCALER_TOTAL_OUT_HEIGHT_REG_ADDR            (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x034u)
#define HCL_VVP_SCALER_V_POS_ERROR_REG_ADDR                 (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x038u)
#define HCL_VVP_SCALER_V_PHASE_OFFSET_REG_ADDR              (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x03Cu)
#define HCL_VVP_SCALER_V_PHASE_ERROR_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x040u)
#define HCL_VVP_SCALER_V_EXTENDED_FILL_REG_ADDR             (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x044u)

/* Runtime status fields. */
#define HCL_VVP_SCALER_STATUS_RUNNING_MASK                  0x00000001u
#define HCL_VVP_SCALER_STATUS_PENDING_COMMIT_MASK           0x00000002u

/* Coefficient-load map (word-addressed map location 128 => byte offset 0x200). */
#define HCL_VVP_SCALER_RT_COEFF_LOAD_BASE_REG_ADDR          (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x200u)
#define HCL_VVP_SCALER_RT_COEFF_LOAD_BANK_SELECT_REG_ADDR   (HCL_VVP_SCALER_RT_COEFF_LOAD_BASE_REG_ADDR + 0x000u)
#define HCL_VVP_SCALER_RT_COEFF_LOAD_PHASE_SELECT_REG_ADDR  (HCL_VVP_SCALER_RT_COEFF_LOAD_BASE_REG_ADDR + 0x004u)
#define HCL_VVP_SCALER_RT_COEFF_LOAD_COMMIT_REG_ADDR        (HCL_VVP_SCALER_RT_COEFF_LOAD_BASE_REG_ADDR + 0x008u)
#define HCL_VVP_SCALER_RT_COEFF_LOAD_TAP_BASE_REG_ADDR      (HCL_VVP_SCALER_RT_COEFF_LOAD_BASE_REG_ADDR + 0x00Cu)
#define HCL_VVP_SCALER_RT_COEFF_LOAD_TAP_REG_ADDR(tap)      (HCL_VVP_SCALER_RT_COEFF_LOAD_TAP_BASE_REG_ADDR + ((uint32_t)(tap) * 0x4u))
