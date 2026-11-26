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

#define HCL_VVP_CSC_PRODUCT_ID                              0x022Fu
#define HCL_VVP_CSC_MIN_SUPPORTED_REGMAP_VERSION            1u
#define HCL_VVP_CSC_MAX_SUPPORTED_REGMAP_VERSION            1u

#define HCL_VVP_CSC_NUM_COLOR_PLANES                        3u
#define HCL_VVP_CSC_NUM_COEFFS                              9u
#define HCL_VVP_CSC_NUM_SUMMANDS                            3u

/* Compile-time parameter registers. */
#define HCL_VVP_CSC_VID_PID_REG_ADDR                        0x000u
#define HCL_VVP_CSC_VERSION_REG_ADDR                        0x004u
#define HCL_VVP_CSC_LITE_MODE_REG_ADDR                      0x008u
#define HCL_VVP_CSC_DEBUG_ENABLED_REG_ADDR                  0x00Cu
#define HCL_VVP_CSC_BPS_IN_REG_ADDR                         0x010u
#define HCL_VVP_CSC_BPS_OUT_REG_ADDR                        0x014u
#define HCL_VVP_CSC_FRAC_BITS_REG_ADDR                      0x018u
#define HCL_VVP_CSC_COEFFS_SIGNED_REG_ADDR                  0x01Cu
#define HCL_VVP_CSC_COEFFS_INT_BITS_REG_ADDR                0x020u
#define HCL_VVP_CSC_SUMMANDS_SIGNED_REG_ADDR                0x024u
#define HCL_VVP_CSC_SUMMANDS_INT_BITS_REG_ADDR              0x028u
#define HCL_VVP_CSC_BINARY_POINT_RIGHT_MOVE_REG_ADDR        0x02Cu
#define HCL_VVP_CSC_ROUND_METHOD_REG_ADDR                   0x030u

/* Runtime registers */
#define HCL_VVP_CSC_IMG_INFO_WIDTH_REG_ADDR                 0x0120u
#define HCL_VVP_CSC_IMG_INFO_HEIGHT_REG_ADDR                0x0124u
#define HCL_VVP_CSC_IMG_INFO_INTERLACE_REG_ADDR             0x0128u
#define HCL_VVP_CSC_IMG_INFO_COLORSPACE_REG_ADDR            0x0130u
#define HCL_VVP_CSC_IMG_INFO_SUBSAMPLING_REG_ADDR           0x0134u
#define HCL_VVP_CSC_IMG_INFO_COSITING_REG_ADDR              0x0138u
#define HCL_VVP_CSC_IMG_INFO_FIELD_COUNT_REG_ADDR           0x013Cu
#define HCL_VVP_CSC_STATUS_REG_ADDR                         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x000u)
#define HCL_VVP_CSC_COMMIT_REG_ADDR                         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x004u)
#define HCL_VVP_CSC_COEFFS_BASE_REG_ADDR                    (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x008u)
#define HCL_VVP_CSC_SUMMANDS_BASE_REG_ADDR                  (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x02Cu)
#define HCL_VVP_CSC_OUTPUT_CS_REG_ADDR                      (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x038u)

#define HCL_VVP_CSC_COEFF_REG_ADDR(index)                   (HCL_VVP_CSC_COEFFS_BASE_REG_ADDR + ((uint32_t)(index) * 0x004u))
#define HCL_VVP_CSC_SUMMAND_REG_ADDR(index)                 (HCL_VVP_CSC_SUMMANDS_BASE_REG_ADDR + ((uint32_t)(index) * 0x004u))

/* Status register fields */
#define HCL_VVP_CSC_STATUS_RUNNING_MASK                     0x00000001u
#define HCL_VVP_CSC_STATUS_PENDING_COMMIT_MASK              0x00000002u

/* Rounding methods */
#define HCL_VVP_CSC_ROUND_UP                                1u
#define HCL_VVP_CSC_ROUND_HALF_EVEN                         2u
#define HCL_VVP_CSC_ROUND_TRUNC                             3u

/* Output color space values */
#define HCL_VVP_CSC_OUTPUT_CS_RGB                           0u
#define HCL_VVP_CSC_OUTPUT_CS_YCC                           1u
#define HCL_VVP_CSC_OUTPUT_CS_YCCSD                         2u
#define HCL_VVP_CSC_OUTPUT_CS_YCCHD                         3u
#define HCL_VVP_CSC_OUTPUT_CS_MONO                          4u
#define HCL_VVP_CSC_OUTPUT_CS_RAW                           5u
#define HCL_VVP_CSC_OUTPUT_CS_RGB565                        6u
