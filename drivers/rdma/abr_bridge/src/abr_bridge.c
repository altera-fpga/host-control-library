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

#include "hcl/drivers/abr_bridge.h"
#include "hcl/drivers/abr_bridge_regs.h"
#include "hcl/register_interface_common.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct hcl_abr_bridge {
    hcl_register_interface* reg;
    uint32_t base_address;
};

static hcl_status write_reg(hcl_abr_bridge* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_abr_bridge* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, d->base_address, offset, out);
}

static uint32_t chan_reg_offset(uint32_t channel, uint32_t reg_off) {
    return HCL_ABR_BRIDGE_CHAN_BASE_ADDR + (channel * HCL_ABR_BRIDGE_CHAN_STRIDE_BYTES) + reg_off;
}

static hcl_status read_channel_control(hcl_abr_bridge* d, uint32_t channel, uint32_t* out) {
    return read_reg(d, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONTROL_OFF), out);
}

static hcl_status write_channel_control(hcl_abr_bridge* d, uint32_t channel, uint32_t value) {
    return write_reg(d, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONTROL_OFF), value);
}

static uint32_t channel_control_update_num_lines(uint32_t word, uint16_t num_lines) {
    return (word & ~HCL_ABR_BRIDGE_CHAN_NUM_LINES_MASK)
         | ((uint32_t)num_lines & HCL_ABR_BRIDGE_CHAN_NUM_LINES_MASK);
}

static uint32_t channel_control_update_num_buffers(uint32_t word, uint8_t num_buffers) {
    return (word & ~HCL_ABR_BRIDGE_CHAN_NUM_BUFFERS_MASK)
         | ((((uint32_t)num_buffers) & 0x7u) << HCL_ABR_BRIDGE_CHAN_NUM_BUFFERS_SHIFT);
}

static uint32_t channel_control_update_load_buffer(uint32_t word, uint8_t load_buffer) {
    return (word & ~HCL_ABR_BRIDGE_CHAN_LOAD_BUFFER_MASK)
         | ((((uint32_t)load_buffer) & 0x7u) << HCL_ABR_BRIDGE_CHAN_LOAD_BUFFER_SHIFT);
}

hcl_abr_bridge* hcl_abr_bridge_create(hcl_register_interface* reg, uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }
    hcl_abr_bridge* d = (hcl_abr_bridge*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }
    d->reg = reg;
    d->base_address = base_address;
    return d;
}

void hcl_abr_bridge_destroy(hcl_abr_bridge* driver) {
    free(driver);
}

hcl_status hcl_abr_bridge_read_configuration(hcl_abr_bridge* d,
                                             hcl_abr_bridge_configuration* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    uint32_t reg = 0u;
    const hcl_status s = read_reg(d, HCL_ABR_BRIDGE_CONFIGURATION_REG_ADDR, &reg);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    out->minor_version = (uint8_t)(reg & HCL_ABR_BRIDGE_CFG_MINOR_VERSION_MASK);
    out->major_version = (uint8_t)((reg & HCL_ABR_BRIDGE_CFG_MAJOR_VERSION_MASK) >> HCL_ABR_BRIDGE_CFG_MAJOR_VERSION_SHIFT);
    out->num_channels  = (uint8_t)((reg & HCL_ABR_BRIDGE_CFG_NUM_CHANNELS_MASK)  >> HCL_ABR_BRIDGE_CFG_NUM_CHANNELS_SHIFT);
    return hcl_status_ok();
}

hcl_status hcl_abr_bridge_write_control(hcl_abr_bridge* d,
                                        const hcl_abr_bridge_control* ctrl) {
    if (d == NULL || ctrl == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_ABR_BRIDGE_CONTROL_REG_ADDR,
                     (uint32_t)ctrl->channel_enable & HCL_ABR_BRIDGE_CTRL_CHANNEL_ENABLE_MASK);
}

hcl_status hcl_abr_bridge_read_control(hcl_abr_bridge* d,
                                       hcl_abr_bridge_control* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    uint32_t word = 0u;
    const hcl_status s = read_reg(d, HCL_ABR_BRIDGE_CONTROL_REG_ADDR, &word);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    out->channel_enable = (uint8_t)(word & HCL_ABR_BRIDGE_CTRL_CHANNEL_ENABLE_MASK);
    return hcl_status_ok();
}

hcl_status hcl_abr_bridge_read_status(hcl_abr_bridge* d, uint32_t* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_ABR_BRIDGE_STATUS_REG_ADDR, out);
}

hcl_status hcl_abr_bridge_set_channel_enable(hcl_abr_bridge* d, uint32_t channel) {
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    hcl_abr_bridge_control ctrl = {0};
    hcl_status s = hcl_abr_bridge_read_control(d, &ctrl);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    ctrl.channel_enable |= (uint8_t)(1u << channel);
    return hcl_abr_bridge_write_control(d, &ctrl);
}

hcl_status hcl_abr_bridge_read_channel_config(hcl_abr_bridge* d,
                                              uint32_t channel,
                                              hcl_abr_bridge_channel_config* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    uint32_t word = 0u;
    const hcl_status s = read_reg(d, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONFIG_OFF),
                                          &word);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    out->channel_width = (uint8_t)(word & HCL_ABR_BRIDGE_CHAN_WIDTH_MASK);
    out->channel_type  = (uint8_t)((word & HCL_ABR_BRIDGE_CHAN_TYPE_MASK) >> HCL_ABR_BRIDGE_CHAN_TYPE_SHIFT);
    return hcl_status_ok();
}

hcl_status hcl_abr_bridge_write_channel_config(hcl_abr_bridge* d,
                                               uint32_t channel,
                                               const hcl_abr_bridge_channel_config* cfg) {
    if (d == NULL || cfg == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    const uint32_t word = ((uint32_t)cfg->channel_width & HCL_ABR_BRIDGE_CHAN_WIDTH_MASK)
                        | (((uint32_t)cfg->channel_type & 0x0Fu) << HCL_ABR_BRIDGE_CHAN_TYPE_SHIFT);
    return write_reg(d, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONFIG_OFF), word);
}

hcl_status hcl_abr_bridge_set_channel_num_lines(hcl_abr_bridge* d,
                                                uint32_t channel,
                                                uint16_t num_lines) {
    uint32_t word = 0u;
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    const hcl_status s = read_channel_control(d, channel, &word);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return write_channel_control(d, channel, channel_control_update_num_lines(word, num_lines));
}

hcl_status hcl_abr_bridge_get_channel_num_lines(hcl_abr_bridge* d,
                                                uint32_t channel,
                                                uint16_t* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    uint32_t word = 0u;
    const hcl_status s = read_reg(d, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONTROL_OFF),
                                          &word);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out = (uint16_t)(word & HCL_ABR_BRIDGE_CHAN_NUM_LINES_MASK);
    return hcl_status_ok();
}

hcl_status hcl_abr_bridge_set_channel_num_buffers(hcl_abr_bridge* d,
                                                  uint32_t channel,
                                                  uint8_t num_buffers) {
    uint32_t word = 0u;
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS || num_buffers >= HCL_ABR_BRIDGE_MAX_BUFFERS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    const hcl_status s = read_channel_control(d, channel, &word);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return write_channel_control(d, channel, channel_control_update_num_buffers(word, num_buffers));
}

hcl_status hcl_abr_bridge_get_channel_num_buffers(hcl_abr_bridge* d,
                                                  uint32_t channel,
                                                  uint8_t* out) {
    uint32_t word = 0u;
    if (d == NULL || out == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    const hcl_status s = read_channel_control(d, channel, &word);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out = (uint8_t)((word & HCL_ABR_BRIDGE_CHAN_NUM_BUFFERS_MASK) >> HCL_ABR_BRIDGE_CHAN_NUM_BUFFERS_SHIFT);
    return hcl_status_ok();
}

hcl_status hcl_abr_bridge_set_channel_load_buffer(hcl_abr_bridge* d,
                                                  uint32_t channel,
                                                  uint8_t load_buffer) {
    uint32_t word = 0u;
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS || load_buffer >= HCL_ABR_BRIDGE_MAX_BUFFERS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    const hcl_status s = read_channel_control(d, channel, &word);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return write_channel_control(d, channel, channel_control_update_load_buffer(word, load_buffer));
}

hcl_status hcl_abr_bridge_set_channel_roce_qp(hcl_abr_bridge* d,
                                              uint32_t channel,
                                              uint8_t qp) {
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    const uint32_t qp_mask = (1u << HCL_ABR_BRIDGE_LOG2_QUEUE_PAIRS) - 1u;
    return write_reg(d, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_ROCE_CONTROL_OFF),
                     ((uint32_t)qp & qp_mask) << HCL_ABR_BRIDGE_CHAN_ROCE_QP_SHIFT);
}

hcl_status hcl_abr_bridge_set_channel_roce_rkey(hcl_abr_bridge* d,
                                                uint32_t channel,
                                                uint8_t buffer,
                                                uint32_t rkey) {
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS || buffer >= HCL_ABR_BRIDGE_MAX_BUFFERS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONTROL_OFF), 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_ROCE_KEY_OFF), rkey, 0u, HCL_STATUS_OK},
    };
    uint32_t control = 0u;
    hcl_status s = read_channel_control(d, channel, &control);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    ops[0].write_value = channel_control_update_load_buffer(control, buffer);
    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_abr_bridge_set_channel_offset(hcl_abr_bridge* d,
                                             uint32_t channel,
                                             uint32_t offset) {
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_OFFSET_OFF), offset);
}

hcl_status hcl_abr_bridge_set_channel_remote_address(hcl_abr_bridge* d,
                                                     uint32_t channel,
                                                     uint8_t buffer,
                                                     uint64_t remote_address) {
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS || buffer >= HCL_ABR_BRIDGE_MAX_BUFFERS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONTROL_OFF), 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_REMOTE_ADDR_LO_OFF), (uint32_t)(remote_address & 0xFFFFFFFFu), 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_REMOTE_ADDR_HI_OFF), (uint32_t)(remote_address >> 32u), 0u, HCL_STATUS_OK},
    };
    uint32_t control = 0u;
    hcl_status s = read_channel_control(d, channel, &control);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    ops[0].write_value = channel_control_update_load_buffer(control, buffer);
    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_abr_bridge_set_channel_metadata_address(hcl_abr_bridge* d,
                                                       uint32_t channel,
                                                       uint64_t metadata_address) {
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address,
         chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_METADATA_ADDR_LO_OFF),
         (uint32_t)(metadata_address & 0xFFFFFFFFu), 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address,
         chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_METADATA_ADDR_HI_OFF),
         (uint32_t)(metadata_address >> 32u), 0u, HCL_STATUS_OK},
    };

    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_abr_bridge_set_channel_metadata_key(hcl_abr_bridge* d,
                                                   uint32_t channel,
                                                   uint32_t metadata_key) {
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d,
                     chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_METADATA_KEY_OFF),
                     metadata_key);
}

hcl_status hcl_abr_bridge_read_measured_vid_clk_hz(hcl_abr_bridge* d, uint32_t* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_ABR_BRIDGE_MEASURE_VID_REG_ADDR, out);
}

hcl_status hcl_abr_bridge_read_measured_eth_clk_hz(hcl_abr_bridge* d, uint32_t* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_ABR_BRIDGE_MEASURE_ETH_REG_ADDR, out);
}

hcl_status hcl_abr_bridge_enable_channel(hcl_abr_bridge* d, uint32_t channel) {
    if (d == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return hcl_abr_bridge_set_channel_enable(d, channel);
}

hcl_status hcl_abr_bridge_configure_channel(hcl_abr_bridge* d,
                                            uint32_t channel,
                                            const hcl_abr_bridge_channel_params* params) {
    uint8_t buffer = 0u;

    if (d == NULL || params == NULL || channel >= HCL_ABR_BRIDGE_MAX_CHANNELS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (params->num_buffers >= HCL_ABR_BRIDGE_MAX_BUFFERS) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    const uint32_t cfg_word = ((uint32_t)params->channel_width & HCL_ABR_BRIDGE_CHAN_WIDTH_MASK)
                            | (((uint32_t)params->channel_type & 0x0Fu) << HCL_ABR_BRIDGE_CHAN_TYPE_SHIFT);
    const uint16_t num_lines = (params->num_lines == 0u) ? HCL_ABR_BRIDGE_DEFAULT_NUM_LINES : params->num_lines;
    const uint32_t qp_mask = (1u << HCL_ABR_BRIDGE_LOG2_QUEUE_PAIRS) - 1u;
    const uint32_t roce_qp_word = ((uint32_t)params->roce_qp & qp_mask) << HCL_ABR_BRIDGE_CHAN_ROCE_QP_SHIFT;
    const uint32_t offset = (params->offset == 0u) ? HCL_ABR_BRIDGE_DEFAULT_CHAN_OFFSET : params->offset;
    const uint32_t control_word = channel_control_update_num_buffers(
        channel_control_update_load_buffer(channel_control_update_num_lines(0u, num_lines), 0u),
        params->num_buffers);
    
    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONFIG_OFF), cfg_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_CONTROL_OFF), control_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_ROCE_CONTROL_OFF), roce_qp_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_OFFSET_OFF), offset, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_METADATA_ADDR_LO_OFF), (uint32_t)(params->metadata_address & 0xFFFFFFFFu), 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_METADATA_ADDR_HI_OFF), (uint32_t)(params->metadata_address >> 32u), 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, chan_reg_offset(channel, HCL_ABR_BRIDGE_CHAN_METADATA_KEY_OFF), params->metadata_key, 0u, HCL_STATUS_OK},
    };
    hcl_status s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    for (buffer = 0u; buffer <= params->num_buffers; ++buffer) {
        s = hcl_abr_bridge_set_channel_roce_rkey(d, channel, buffer, params->roce_rkeys[buffer]);
        if (s.code != HCL_STATUS_OK) {
            return s;
        }
        s = hcl_abr_bridge_set_channel_remote_address(d, channel, buffer, params->remote_addresses[buffer]);
        if (s.code != HCL_STATUS_OK) {
            return s;
        }
    }

    return hcl_status_ok();
}
