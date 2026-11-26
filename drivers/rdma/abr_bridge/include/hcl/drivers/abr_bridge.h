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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hcl/drivers/abr_bridge_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_abr_bridge hcl_abr_bridge;

typedef struct hcl_abr_bridge_configuration {
    uint8_t minor_version;
    uint8_t major_version;
    uint8_t num_channels;
} hcl_abr_bridge_configuration;

typedef struct hcl_abr_bridge_control {
    uint8_t channel_enable;  /* bitmask */
} hcl_abr_bridge_control;

typedef struct hcl_abr_bridge_channel_config {
    uint8_t channel_width;
    uint8_t channel_type;
} hcl_abr_bridge_channel_config;

typedef struct hcl_abr_bridge_channel_params {
    uint8_t  channel_width;
    uint8_t  channel_type;
    uint16_t num_lines;    /* 0 => hardware default (1080) */
    uint8_t  roce_qp;
    uint8_t  num_buffers;  /* encoded field: 0 => 1 buffer, 7 => 8 buffers */
    uint32_t offset;       /* 0 => hardware default */
    uint32_t roce_rkeys[HCL_ABR_BRIDGE_MAX_BUFFERS];
    uint64_t remote_addresses[HCL_ABR_BRIDGE_MAX_BUFFERS];
    uint64_t metadata_address;
    uint32_t metadata_key;
} hcl_abr_bridge_channel_params;

hcl_abr_bridge* hcl_abr_bridge_create(hcl_register_interface* reg, uint32_t base_address);

void hcl_abr_bridge_destroy(hcl_abr_bridge* driver);

hcl_status hcl_abr_bridge_read_configuration(hcl_abr_bridge* driver,
                                             hcl_abr_bridge_configuration* out);

hcl_status hcl_abr_bridge_write_control(hcl_abr_bridge* driver,
                                        const hcl_abr_bridge_control* ctrl);

hcl_status hcl_abr_bridge_read_control(hcl_abr_bridge* driver,
                                       hcl_abr_bridge_control* out);

hcl_status hcl_abr_bridge_read_status(hcl_abr_bridge* driver,
                                      uint32_t* out);

hcl_status hcl_abr_bridge_set_channel_enable(hcl_abr_bridge* driver,
                                             uint32_t channel);

hcl_status hcl_abr_bridge_read_channel_config(hcl_abr_bridge* driver,
                                              uint32_t channel,
                                              hcl_abr_bridge_channel_config* out);

hcl_status hcl_abr_bridge_write_channel_config(hcl_abr_bridge* driver,
                                               uint32_t channel,
                                               const hcl_abr_bridge_channel_config* cfg);

hcl_status hcl_abr_bridge_set_channel_num_lines(hcl_abr_bridge* driver,
                                                uint32_t channel,
                                                uint16_t num_lines);

hcl_status hcl_abr_bridge_get_channel_num_lines(hcl_abr_bridge* driver,
                                                uint32_t channel,
                                                uint16_t* out);

hcl_status hcl_abr_bridge_set_channel_num_buffers(hcl_abr_bridge* driver,
                                                  uint32_t channel,
                                                  uint8_t num_buffers);

hcl_status hcl_abr_bridge_get_channel_num_buffers(hcl_abr_bridge* driver,
                                                  uint32_t channel,
                                                  uint8_t* out);

hcl_status hcl_abr_bridge_set_channel_load_buffer(hcl_abr_bridge* driver,
                                                  uint32_t channel,
                                                  uint8_t load_buffer);

hcl_status hcl_abr_bridge_set_channel_roce_qp(hcl_abr_bridge* driver,
                                              uint32_t channel,
                                              uint8_t qp);

hcl_status hcl_abr_bridge_set_channel_roce_rkey(hcl_abr_bridge* driver,
                                                uint32_t channel,
                                                uint8_t buffer,
                                                uint32_t rkey);

hcl_status hcl_abr_bridge_set_channel_offset(hcl_abr_bridge* driver,
                                             uint32_t channel,
                                             uint32_t offset);

hcl_status hcl_abr_bridge_set_channel_remote_address(hcl_abr_bridge* driver,
                                                     uint32_t channel,
                                                     uint8_t buffer,
                                                     uint64_t remote_address);

hcl_status hcl_abr_bridge_set_channel_metadata_address(hcl_abr_bridge* driver,
                                                       uint32_t channel,
                                                       uint64_t metadata_address);

hcl_status hcl_abr_bridge_set_channel_metadata_key(hcl_abr_bridge* driver,
                                                   uint32_t channel,
                                                   uint32_t metadata_key);

hcl_status hcl_abr_bridge_read_measured_vid_clk_hz(hcl_abr_bridge* driver,
                                                   uint32_t* out);

hcl_status hcl_abr_bridge_read_measured_eth_clk_hz(hcl_abr_bridge* driver,
                                                   uint32_t* out);

hcl_status hcl_abr_bridge_enable_channel(hcl_abr_bridge* driver,
                                         uint32_t channel);

hcl_status hcl_abr_bridge_configure_channel(hcl_abr_bridge* driver,
                                            uint32_t channel,
                                            const hcl_abr_bridge_channel_params* params);

#ifdef __cplusplus
}
#endif
