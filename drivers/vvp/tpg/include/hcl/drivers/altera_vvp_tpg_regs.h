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

#define HCL_VVP_TPG_PRODUCT_ID                          0x0236u
#define HCL_VVP_TPG_MIN_SUPPORTED_REGMAP_VERSION        1u
#define HCL_VVP_TPG_MAX_SUPPORTED_REGMAP_VERSION        1u

/* Compile-time map. */
#define HCL_VVP_TPG_VID_PID_REG_ADDR                    0x000u
#define HCL_VVP_TPG_VERSION_REG_ADDR                    0x004u
#define HCL_VVP_TPG_LITE_MODE_REG_ADDR                  0x008u
#define HCL_VVP_TPG_DEBUG_ENABLED_REG_ADDR              0x00Cu
#define HCL_VVP_TPG_NUM_PATTERNS_REG_ADDR               0x010u
#define HCL_VVP_TPG_BPS_REG_ADDR                        0x018u
#define HCL_VVP_TPG_PIP_REG_ADDR                        0x01Cu

#define HCL_VVP_TPG_MAX_NUM_PATTERNS                    8u
#define HCL_VVP_TPG_PATTERN_BASE_REG_ADDR               0x020u
#define HCL_VVP_TPG_PATTERN_TYPE_REG_ADDR(pattern)      (HCL_VVP_TPG_PATTERN_BASE_REG_ADDR + ((uint32_t)(pattern) * 0x008u))
#define HCL_VVP_TPG_PATTERN_COLOR_REG_ADDR(pattern)     (HCL_VVP_TPG_PATTERN_BASE_REG_ADDR + ((uint32_t)(pattern) * 0x008u) + 0x004u)

/* Pattern type values. */
#define HCL_VVP_TPG_BARS_PATTERN                        0u
#define HCL_VVP_TPG_UNIFORM_PATTERN                     1u
#define HCL_VVP_TPG_PATHOLOGICAL_PATTERN                2u
#define HCL_VVP_TPG_ZONE_PLATE_PATTERN                  3u
#define HCL_VVP_TPG_DIGITAL_CLOCK_PATTERN               4u
#define HCL_VVP_TPG_SIGNALTAP_COUNTER_PATTERN           5u

/* Pattern color values. */
#define HCL_VVP_TPG_RGB                                 0u
#define HCL_VVP_TPG_YCC_444                             1u
#define HCL_VVP_TPG_YCC_422                             2u
#define HCL_VVP_TPG_YCC_420                             3u
#define HCL_VVP_TPG_MONO                                4u

/* Runtime map. */
#define HCL_VVP_TPG_IMG_INFO_WIDTH_REG_ADDR             0x120u
#define HCL_VVP_TPG_IMG_INFO_HEIGHT_REG_ADDR            0x124u
#define HCL_VVP_TPG_IMG_INFO_INTERLACE_REG_ADDR         0x128u
#define HCL_VVP_TPG_STATUS_REG_ADDR                     (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x000u)
#define HCL_VVP_TPG_FIELD_COUNT_REG_ADDR                (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x004u)
#define HCL_VVP_TPG_CONTROL_REG_ADDR                    (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x008u)
#define HCL_VVP_TPG_COMMIT_REG_ADDR                     (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x00Cu)
#define HCL_VVP_TPG_PATTERN_SELECT_REG_ADDR             (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x010u)
#define HCL_VVP_TPG_C0_REG_ADDR                         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x01Cu)
#define HCL_VVP_TPG_C1_REG_ADDR                         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x020u)
#define HCL_VVP_TPG_C2_REG_ADDR                         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x024u)
#define HCL_VVP_TPG_BARS_SELECT_REG_ADDR                (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x028u)
#define HCL_VVP_TPG_ZONE_X_ORIGIN_REG_ADDR              (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x02Cu)
#define HCL_VVP_TPG_ZONE_Y_ORIGIN_REG_ADDR              (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x030u)
#define HCL_VVP_TPG_ZONE_SCALING_FACTOR_REG_ADDR        (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x034u)
#define HCL_VVP_TPG_ZONE_FINE_TUNE_FACTOR_REG_ADDR      (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x038u)
#define HCL_VVP_TPG_CLOCK_B_BACKGROUND_REG_ADDR         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x03Cu)
#define HCL_VVP_TPG_CLOCK_G_BACKGROUND_REG_ADDR         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x040u)
#define HCL_VVP_TPG_CLOCK_R_BACKGROUND_REG_ADDR         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x044u)
#define HCL_VVP_TPG_CLOCK_B_FONT_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x048u)
#define HCL_VVP_TPG_CLOCK_G_FONT_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x04Cu)
#define HCL_VVP_TPG_CLOCK_R_FONT_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x050u)
#define HCL_VVP_TPG_CLOCK_LOCATION_X_REG_ADDR           (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x054u)
#define HCL_VVP_TPG_CLOCK_LOCATION_Y_REG_ADDR           (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x058u)
#define HCL_VVP_TPG_CLOCK_SCALE_FACTOR_REG_ADDR         (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x05Cu)
#define HCL_VVP_TPG_CLOCK_FPS_REG_ADDR                  (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x060u)

/* Status and control masks. */
#define HCL_VVP_TPG_STATUS_RUNNING_MASK                 0x00000001u
#define HCL_VVP_TPG_STATUS_PENDING_COMMIT_MASK          0x00000002u
#define HCL_VVP_TPG_CONTROL_GO_MASK                     0x00000001u

/* Bars type values. */
#define HCL_VVP_TPG_COLOR_BARS                          0u
#define HCL_VVP_TPG_GREY_BARS                           1u
#define HCL_VVP_TPG_BLACK_WHITE_BARS                    2u
#define HCL_VVP_TPG_MIXED_BARS                          3u
