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

#define HCL_VVP_PIP_CONV_PRODUCT_ID                    0x0239u
#define HCL_VVP_PIP_CONV_MIN_SUPPORTED_REGMAP_VERSION  1u
#define HCL_VVP_PIP_CONV_MAX_SUPPORTED_REGMAP_VERSION  1u

/* Compile-time map. */
#define HCL_VVP_PIP_CONV_VID_PID_REG_ADDR              0x000u
#define HCL_VVP_PIP_CONV_VERSION_REG_ADDR              0x004u
#define HCL_VVP_PIP_CONV_LITE_MODE_REG_ADDR            0x008u
#define HCL_VVP_PIP_CONV_DEBUG_ENABLED_REG_ADDR        0x00Cu

/* Runtime map. */
#define HCL_VVP_PIP_CONV_IMG_INFO_WIDTH_REG_ADDR        0x120u
#define HCL_VVP_PIP_CONV_IMG_INFO_HEIGHT_REG_ADDR       0x124u
#define HCL_VVP_PIP_CONV_IMG_INFO_INTERLACE_REG_ADDR    0x128u
#define HCL_VVP_PIP_CONV_IMG_INFO_COLORSPACE_REG_ADDR   0x130u
#define HCL_VVP_PIP_CONV_IMG_INFO_SUBSAMPLING_REG_ADDR  0x134u
#define HCL_VVP_PIP_CONV_IMG_INFO_COSITING_REG_ADDR     0x138u
#define HCL_VVP_PIP_CONV_IMG_INFO_FIELD_COUNT_REG_ADDR  0x13Cu
#define HCL_VVP_PIP_CONV_STATUS_REG_ADDR               (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x000u)

/* STATUS register fields. */
#define HCL_VVP_PIP_CONV_STATUS_RUNNING_MASK           0x00000001u
