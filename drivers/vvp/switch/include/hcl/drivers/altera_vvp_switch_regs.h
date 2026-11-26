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

#define HCL_VVP_SWITCH_PRODUCT_ID                         0x0235u
#define HCL_VVP_SWITCH_MIN_SUPPORTED_REGMAP_VERSION       2u
#define HCL_VVP_SWITCH_MAX_SUPPORTED_REGMAP_VERSION       2u

#define HCL_VVP_SWITCH_MAX_NUM_INPUTS                     16u
#define HCL_VVP_SWITCH_MAX_NUM_OUTPUTS                    16u

/* Compile-time parameter registers. */
#define HCL_VVP_SWITCH_VID_PID_REG_ADDR                   0x000u
#define HCL_VVP_SWITCH_VERSION_REG_ADDR                   0x004u
#define HCL_VVP_SWITCH_INTF_TYPE_REG_ADDR                 0x008u
#define HCL_VVP_SWITCH_DEBUG_ENABLED_REG_ADDR             0x00Cu
#define HCL_VVP_SWITCH_UNINTERRUPTED_INPUTS_REG_ADDR      0x010u
#define HCL_VVP_SWITCH_AUTO_CONSUME_REG_ADDR              0x014u
#define HCL_VVP_SWITCH_NUM_INPUTS_REG_ADDR                0x018u
#define HCL_VVP_SWITCH_NUM_OUTPUTS_REG_ADDR               0x01Cu
#define HCL_VVP_SWITCH_USE_TREADIES_REG_ADDR              0x020u
#define HCL_VVP_SWITCH_CRASH_SWITCH_REG_ADDR              0x024u

/* Runtime registers. */
#define HCL_VVP_SWITCH_STATUS_REG_ADDR                    (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x000u)
#define HCL_VVP_SWITCH_COMMIT_REG_ADDR                    (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x004u)

#define HCL_VVP_SWITCH_INPUT_CONTROL_BASE_REG_ADDR        (HCL_VVP_CORE_RT_BASE_REG_ADDR + 0x008u)
#define HCL_VVP_SWITCH_OUTPUT_CONTROL_BASE_REG_ADDR       (HCL_VVP_SWITCH_INPUT_CONTROL_BASE_REG_ADDR + (HCL_VVP_SWITCH_MAX_NUM_INPUTS * 0x4u))

#define HCL_VVP_SWITCH_INPUT_CONTROL_REG_ADDR(input)      (HCL_VVP_SWITCH_INPUT_CONTROL_BASE_REG_ADDR + ((uint32_t)(input) * 0x4u))
#define HCL_VVP_SWITCH_OUTPUT_CONTROL_REG_ADDR(output)    (HCL_VVP_SWITCH_OUTPUT_CONTROL_BASE_REG_ADDR + ((uint32_t)(output) * 0x4u))

/* STATUS register fields. */
#define HCL_VVP_SWITCH_STATUS_RUNNING_MASK                0x00000001u
#define HCL_VVP_SWITCH_STATUS_PENDING_COMMIT_MASK         0x00000002u

/* OUTPUT_CONTROL register fields. */
#define HCL_VVP_SWITCH_OUTPUT_CONTROL_INPUT_MASK          0x0000000Fu
#define HCL_VVP_SWITCH_OUTPUT_CONTROL_INPUT_SHIFT         0u
#define HCL_VVP_SWITCH_OUTPUT_CONTROL_ENABLED_MASK        0x00000100u

/* INPUT_CONTROL register fields. */
#define HCL_VVP_SWITCH_INPUT_CONTROL_INPUT_CONFIG_MASK    0x00000003u
