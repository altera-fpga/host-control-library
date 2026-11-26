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

#define HCL_VVP_WBC_PRODUCT_ID                          0x017Au
#define HCL_VVP_WBC_MIN_SUPPORTED_REGMAP_VERSION        1u
#define HCL_VVP_WBC_MAX_SUPPORTED_REGMAP_VERSION        1u

/* Compile-time parameter registers. */
#define HCL_VVP_WBC_VID_PID_REG_ADDR                    0x000u
#define HCL_VVP_WBC_VERSION_REG_ADDR                    0x004u
#define HCL_VVP_WBC_LITE_MODE_REG_ADDR                  0x008u
#define HCL_VVP_WBC_DEBUG_ENABLED_REG_ADDR              0x00Cu
#define HCL_VVP_WBC_BPS_IN_REG_ADDR                     0x010u
#define HCL_VVP_WBC_BPS_OUT_REG_ADDR                    0x014u
#define HCL_VVP_WBC_NUM_COLOR_IN_REG_ADDR               0x018u
#define HCL_VVP_WBC_NUM_COLOR_OUT_REG_ADDR              0x01Cu
#define HCL_VVP_WBC_PIP_REG_ADDR                        0x020u
#define HCL_VVP_WBC_MAX_WIDTH_REG_ADDR                  0x024u
#define HCL_VVP_WBC_MAX_HEIGHT_REG_ADDR                 0x028u

/* Runtime registers. */
#define HCL_VVP_WBC_IMG_INFO_WIDTH_REG_ADDR             0x0120u
#define HCL_VVP_WBC_IMG_INFO_HEIGHT_REG_ADDR            0x0124u
#define HCL_VVP_WBC_STATUS_REG_ADDR                     (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x000u)
#define HCL_VVP_WBC_FRAME_STATS_REG_ADDR                (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x004u)
#define HCL_VVP_WBC_COMMIT_REG_ADDR                     (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x008u)
#define HCL_VVP_WBC_SETTINGS_REG_ADDR                   (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x00Cu)
#define HCL_VVP_WBC_CFA_00_COLOR_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x010u)
#define HCL_VVP_WBC_CFA_01_COLOR_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x014u)
#define HCL_VVP_WBC_CFA_10_COLOR_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x018u)
#define HCL_VVP_WBC_CFA_11_COLOR_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x01Cu)

/* STATUS register fields. */
#define HCL_VVP_WBC_STATUS_RUNNING_MASK                 0x00000001u
#define HCL_VVP_WBC_STATUS_COMMIT_PENDING_MASK          0x00000002u
#define HCL_VVP_WBC_STATUS_STATS_FROZEN_MASK            0x00000004u

/* FRAME_STATS register fields. */
#define HCL_VVP_WBC_FRAME_STATS_CHKSM_MASK              0x000000FFu

/* SETTINGS register fields. */
#define HCL_VVP_WBC_SETTINGS_BYPASS_MASK                0x00000001u
#define HCL_VVP_WBC_SETTINGS_CFA_PHASE_MASK             0x00000006u
#define HCL_VVP_WBC_SETTINGS_CFA_PHASE_SHIFT            1u
#define HCL_VVP_WBC_SETTINGS_FREEZE_STATS_MASK          0x80000000u

/* CFA color scaler masks. */
#define HCL_VVP_WBC_CFA_00_COLOR_SCALER_MASK            0x0007FFFFu
#define HCL_VVP_WBC_CFA_01_COLOR_SCALER_MASK            0x0007FFFFu
#define HCL_VVP_WBC_CFA_10_COLOR_SCALER_MASK            0x0007FFFFu
#define HCL_VVP_WBC_CFA_11_COLOR_SCALER_MASK            0x0007FFFFu

#define HCL_VVP_WBC_COLOR_SCALER_MAX                    HCL_VVP_WBC_CFA_00_COLOR_SCALER_MASK
#define HCL_VVP_WBC_CFA_PHASE_MAX                       3u
