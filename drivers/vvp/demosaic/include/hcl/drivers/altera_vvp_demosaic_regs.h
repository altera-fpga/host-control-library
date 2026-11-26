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


#define HCL_VVP_DEMOSAIC_PRODUCT_ID                      0x0246u
#define HCL_VVP_DEMOSAIC_MIN_SUPPORTED_REGMAP_VERSION    2u
#define HCL_VVP_DEMOSAIC_MAX_SUPPORTED_REGMAP_VERSION    2u

/* Parameterization registers */
#define HCL_VVP_DEMOSAIC_VID_PID_REG_ADDR                0x0000u
#define HCL_VVP_DEMOSAIC_VERSION_REG_ADDR                0x0004u
#define HCL_VVP_DEMOSAIC_LITE_MODE_REG_ADDR              0x0008u
#define HCL_VVP_DEMOSAIC_DEBUG_ENABLED_REG_ADDR          0x000Cu
#define HCL_VVP_DEMOSAIC_BPS_IN_REG_ADDR                 0x0010u
#define HCL_VVP_DEMOSAIC_BPS_OUT_REG_ADDR                0x0014u
#define HCL_VVP_DEMOSAIC_NUM_COLOR_IN_REG_ADDR           0x0018u
#define HCL_VVP_DEMOSAIC_NUM_COLOR_OUT_REG_ADDR          0x001Cu
#define HCL_VVP_DEMOSAIC_PIP_REG_ADDR                    0x0020u
#define HCL_VVP_DEMOSAIC_MAX_WIDTH_REG_ADDR              0x0024u
#define HCL_VVP_DEMOSAIC_MAX_HEIGHT_REG_ADDR             0x0028u

/* Control, debug and statistics registers */
#define HCL_VVP_DEMOSAIC_IMG_INFO_WIDTH_REG_ADDR         0x0120u
#define HCL_VVP_DEMOSAIC_IMG_INFO_HEIGHT_REG_ADDR        0x0124u
#define HCL_VVP_DEMOSAIC_STATUS_REG_ADDR                 0x0140u
#define HCL_VVP_DEMOSAIC_FRAME_STATS_REG_ADDR            0x0144u
#define HCL_VVP_DEMOSAIC_CONTROL_REG_ADDR                0x014Cu

/* STATUS register fields */
#define HCL_VVP_DEMOSAIC_STATUS_RUNNING_MASK             0x00000001u

/* FRAME_STATS register fields */
#define HCL_VVP_DEMOSAIC_FRAME_STATS_CHKSUM_MASK         0x000000FFu

/* CONTROL register fields */
#define HCL_VVP_DEMOSAIC_CONTROL_BYPASS_MASK            0x00000001u
#define HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_MASK         0x00000006u
#define HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_SHIFT        1u
#define HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_MAX          3u
