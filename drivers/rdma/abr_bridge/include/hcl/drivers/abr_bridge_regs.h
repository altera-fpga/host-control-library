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

/* Global registers. */
#define HCL_ABR_BRIDGE_CONFIGURATION_REG_ADDR  0x000u
#define HCL_ABR_BRIDGE_CONTROL_REG_ADDR        0x004u
#define HCL_ABR_BRIDGE_STATUS_REG_ADDR         0x008u

/* CONFIGURATION register fields. */
#define HCL_ABR_BRIDGE_CFG_MINOR_VERSION_MASK   0x0000000Fu
#define HCL_ABR_BRIDGE_CFG_MAJOR_VERSION_SHIFT  4u
#define HCL_ABR_BRIDGE_CFG_MAJOR_VERSION_MASK   0x000000F0u
#define HCL_ABR_BRIDGE_CFG_NUM_CHANNELS_SHIFT   8u
#define HCL_ABR_BRIDGE_CFG_NUM_CHANNELS_MASK    0x00000700u

/* Per-channel register block. */
#define HCL_ABR_BRIDGE_CHAN_BASE_ADDR         0x400u
#define HCL_ABR_BRIDGE_CHAN_STRIDE_BYTES      0x040u
#define HCL_ABR_BRIDGE_CHAN_CONFIG_OFF        0x000u
#define HCL_ABR_BRIDGE_CHAN_CONTROL_OFF       0x004u
#define HCL_ABR_BRIDGE_CHAN_STATUS_OFF        0x008u
#define HCL_ABR_BRIDGE_CHAN_ROCE_CONTROL_OFF  0x00Cu
#define HCL_ABR_BRIDGE_CHAN_ROCE_KEY_OFF      0x010u
#define HCL_ABR_BRIDGE_CHAN_OFFSET_OFF        0x014u
#define HCL_ABR_BRIDGE_CHAN_REMOTE_ADDR_LO_OFF  0x018u
#define HCL_ABR_BRIDGE_CHAN_REMOTE_ADDR_HI_OFF  0x01Cu
#define HCL_ABR_BRIDGE_CHAN_METADATA_ADDR_LO_OFF 0x020u
#define HCL_ABR_BRIDGE_CHAN_METADATA_ADDR_HI_OFF 0x024u
#define HCL_ABR_BRIDGE_CHAN_METADATA_KEY_OFF     0x028u

/* CONTROL register fields. */
#define HCL_ABR_BRIDGE_CTRL_CHANNEL_ENABLE_MASK  0x000000FFu

/* Channel CONFIG register fields. */
#define HCL_ABR_BRIDGE_CHAN_WIDTH_MASK  0x000000FFu
#define HCL_ABR_BRIDGE_CHAN_TYPE_SHIFT  8u
#define HCL_ABR_BRIDGE_CHAN_TYPE_MASK   0x00000F00u

/* Channel CONTROL register fields. */
#define HCL_ABR_BRIDGE_CHAN_NUM_LINES_MASK    0x0000FFFFu
#define HCL_ABR_BRIDGE_CHAN_NUM_BUFFERS_SHIFT 24u
#define HCL_ABR_BRIDGE_CHAN_NUM_BUFFERS_MASK  (0x7u << HCL_ABR_BRIDGE_CHAN_NUM_BUFFERS_SHIFT)
#define HCL_ABR_BRIDGE_CHAN_LOAD_BUFFER_SHIFT 28u
#define HCL_ABR_BRIDGE_CHAN_LOAD_BUFFER_MASK  (0x7u << HCL_ABR_BRIDGE_CHAN_LOAD_BUFFER_SHIFT)

/* Must match IP parameter C_LOG2_QUEUE_PAIRS. */
#define HCL_ABR_BRIDGE_LOG2_QUEUE_PAIRS     5u
#define HCL_ABR_BRIDGE_CHAN_ROCE_QP_SHIFT   8u
#define HCL_ABR_BRIDGE_CHAN_ROCE_QP_MASK    (((1u << HCL_ABR_BRIDGE_LOG2_QUEUE_PAIRS) - 1u) << HCL_ABR_BRIDGE_CHAN_ROCE_QP_SHIFT)

/* Measurement registers. */
#define HCL_ABR_BRIDGE_MEASURE_VID_REG_ADDR  0x700u
#define HCL_ABR_BRIDGE_MEASURE_ETH_REG_ADDR  0x704u

/* Channel type constants. */
#define HCL_ABR_BRIDGE_CHAN_TYPE_VVP420  0u
#define HCL_ABR_BRIDGE_CHAN_TYPE_LIDAR   1u

/* Channel width constants. */
#define HCL_ABR_BRIDGE_CHAN_WIDTH_VVP420  24u
#define HCL_ABR_BRIDGE_CHAN_WIDTH_AUDIO   32u
#define HCL_ABR_BRIDGE_CHAN_WIDTH_LIDAR   64u

/* Limits and defaults. */
#define HCL_ABR_BRIDGE_MAX_CHANNELS        8u
#define HCL_ABR_BRIDGE_MAX_BUFFERS         8u
#define HCL_ABR_BRIDGE_DEFAULT_NUM_LINES   1080u
#define HCL_ABR_BRIDGE_DEFAULT_CHAN_OFFSET 0x001FA400u
