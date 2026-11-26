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

#define HCL_VVP_BLC_PRODUCT_ID                      0x0177u
#define HCL_VVP_BLC_MIN_SUPPORTED_REGMAP_VERSION    1u
#define HCL_VVP_BLC_MAX_SUPPORTED_REGMAP_VERSION    1u

#define HCL_VVP_BLC_VID_PID_REG_ADDR                0x000u
#define HCL_VVP_BLC_VERSION_REG_ADDR                0x004u
#define HCL_VVP_BLC_LITE_MODE_REG_ADDR              0x008u
#define HCL_VVP_BLC_DEBUG_ENABLED_REG_ADDR          0x00Cu
#define HCL_VVP_BLC_BPS_IN_REG_ADDR                 0x010u
#define HCL_VVP_BLC_BPS_OUT_REG_ADDR                0x014u
#define HCL_VVP_BLC_NUM_COLOR_IN_REG_ADDR           0x018u
#define HCL_VVP_BLC_NUM_COLOR_OUT_REG_ADDR          0x01Cu
#define HCL_VVP_BLC_PIP_REG_ADDR                    0x020u
#define HCL_VVP_BLC_MAX_WIDTH_REG_ADDR              0x024u
#define HCL_VVP_BLC_MAX_HEIGHT_REG_ADDR             0x028u
#define HCL_VVP_BLC_REFLECT_AROUND_ZERO_REG_ADDR    0x02Cu

/* Runtime registers */
#define HCL_VVP_BLC_IMG_INFO_WIDTH_REG_ADDR         0x0120u
#define HCL_VVP_BLC_IMG_INFO_HEIGHT_REG_ADDR        0x0124u
#define HCL_VVP_BLC_STATUS_REG_ADDR                 0x140u
#define HCL_VVP_BLC_FRAME_STATS_REG_ADDR            0x144u
#define HCL_VVP_BLC_COMMIT_REG_ADDR                 0x148u
#define HCL_VVP_BLC_SETTINGS_REG_ADDR               0x14Cu
#define HCL_VVP_BLC_CFA_00_BLACK_REG_ADDR           0x150u
#define HCL_VVP_BLC_CFA_00_COLOR_REG_ADDR           0x154u
#define HCL_VVP_BLC_CFA_01_BLACK_REG_ADDR           0x158u
#define HCL_VVP_BLC_CFA_01_COLOR_REG_ADDR           0x15Cu
#define HCL_VVP_BLC_CFA_10_BLACK_REG_ADDR           0x160u
#define HCL_VVP_BLC_CFA_10_COLOR_REG_ADDR           0x164u
#define HCL_VVP_BLC_CFA_11_BLACK_REG_ADDR           0x168u
#define HCL_VVP_BLC_CFA_11_COLOR_REG_ADDR           0x16Cu

/* STATUS register bits */
#define HCL_VVP_BLC_STATUS_RUNNING_MASK             0x00000001u
#define HCL_VVP_BLC_STATUS_COMMIT_PENDING_MASK      0x00000002u

/* SETTINGS register fields */
#define HCL_VVP_BLC_SETTINGS_BYPASS_MASK            0x00000001u
#define HCL_VVP_BLC_SETTINGS_CFA_PHASE_MASK         0x00000006u
#define HCL_VVP_BLC_SETTINGS_CFA_PHASE_SHIFT        1u
#define HCL_VVP_BLC_SETTINGS_CLIP_ZERO_EN_MASK      0x00000008u

/* CFA register field masks */
#define HCL_VVP_BLC_BLACK_PEDESTAL_MASK             0x0000FFFFu
#define HCL_VVP_BLC_COLOR_SCALER_MASK               0x0003FFFFu

/* Valid ranges */
#define HCL_VVP_BLC_CFA_PHASE_MAX                   3u
