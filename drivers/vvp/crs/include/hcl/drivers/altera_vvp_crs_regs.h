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

#define HCL_VVP_CRS_PRODUCT_ID                          0x022Eu
#define HCL_VVP_CRS_MIN_SUPPORTED_REGMAP_VERSION        1u
#define HCL_VVP_CRS_MAX_SUPPORTED_REGMAP_VERSION        2u

/* Subsampling encoding in hardware register values. */
#define HCL_VVP_CRS_SUBSAMPLING_420                     0u
#define HCL_VVP_CRS_SUBSAMPLING_INVALID                 1u
#define HCL_VVP_CRS_SUBSAMPLING_422                     2u
#define HCL_VVP_CRS_SUBSAMPLING_444                     3u
#define HCL_VVP_CRS_NUM_SUBSAMPLINGS                    4u

/* Parameterization registers */
#define HCL_VVP_CRS_VID_PID_REG_ADDR                    0x000u
#define HCL_VVP_CRS_VERSION_REG_ADDR                    0x004u
#define HCL_VVP_CRS_LITE_MODE_REG_ADDR                  0x008u
#define HCL_VVP_CRS_DEBUG_ENABLED_REG_ADDR              0x00Cu
#define HCL_VVP_CRS_ENABLE_444_444_REG_ADDR             0x010u
#define HCL_VVP_CRS_ENABLE_444_422_REG_ADDR             0x014u
#define HCL_VVP_CRS_ENABLE_444_420_REG_ADDR             0x018u
#define HCL_VVP_CRS_ENABLE_422_444_REG_ADDR             0x01Cu
#define HCL_VVP_CRS_ENABLE_422_422_REG_ADDR             0x020u
#define HCL_VVP_CRS_ENABLE_422_420_REG_ADDR             0x024u
#define HCL_VVP_CRS_ENABLE_420_444_REG_ADDR             0x028u
#define HCL_VVP_CRS_ENABLE_420_422_REG_ADDR             0x02Cu
#define HCL_VVP_CRS_ENABLE_420_420_REG_ADDR             0x030u

/* Control and debug registers. */
#define HCL_VVP_CRS_IMG_INFO_WIDTH_REG_ADDR             0x120u
#define HCL_VVP_CRS_IMG_INFO_HEIGHT_REG_ADDR            0x124u
#define HCL_VVP_CRS_IMG_INFO_INTERLACE_REG_ADDR         0x128u
#define HCL_VVP_CRS_IMG_INFO_COLORSPACE_REG_ADDR        0x130u
#define HCL_VVP_CRS_IMG_INFO_SUBSAMPLING_REG_ADDR       0x134u
#define HCL_VVP_CRS_IMG_INFO_COSITING_REG_ADDR          0x138u
#define HCL_VVP_CRS_IMG_INFO_FIELD_COUNT_REG_ADDR       0x13Cu
#define HCL_VVP_CRS_STATUS_REG_ADDR                     0x140u
#define HCL_VVP_CRS_COMMIT_REG_ADDR                     0x144u
#define HCL_VVP_CRS_OUTPUT_MODE_REG_ADDR                0x148u

/* STATUS register fields. */
#define HCL_VVP_CRS_STATUS_RUNNING_MASK                 0x00000001u
#define HCL_VVP_CRS_STATUS_PENDING_COMMIT_MASK          0x00000002u
