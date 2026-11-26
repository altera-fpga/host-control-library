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

#include "hcl/drivers/altera_vvp_switch.h"

#include "hcl/drivers/altera_vvp_core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct hcl_altera_vvp_switch {
    hcl_register_interface* reg;
    uint32_t base_address;
    hcl_altera_vvp_id id;
    hcl_altera_vvp_version version;

    bool debug_enabled;
    hcl_altera_vvp_switch_intf_type intf_type;
    bool uninterrupted_inputs;
    bool auto_consume;
    uint8_t num_inputs;
    uint8_t num_outputs;
    bool crash_switch;
    bool use_treadies;
};

static hcl_status write_reg(hcl_altera_vvp_switch* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_altera_vvp_switch* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, d->base_address, offset, out);
}

hcl_altera_vvp_switch* hcl_altera_vvp_switch_create(hcl_register_interface* reg,
                                                    uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }

    hcl_altera_vvp_switch* d = (hcl_altera_vvp_switch*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }

    d->reg = reg;
    d->base_address = base_address;

    return d;
}

void hcl_altera_vvp_switch_destroy(hcl_altera_vvp_switch* driver) {
    free(driver);
}

hcl_status hcl_altera_vvp_switch_init(hcl_altera_vvp_switch* d) {
    uint8_t regmap_version = 0u;
    uint32_t vid_pid = 0u;
    uint32_t version = 0u;
    hcl_altera_vvp_id parsed_id = {0u};
    hcl_altera_vvp_version parsed_version = {0u};
    hcl_status s;

    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_VID_PID_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_VERSION_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_INTF_TYPE_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_DEBUG_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_UNINTERRUPTED_INPUTS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_AUTO_CONSUME_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_NUM_INPUTS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_NUM_OUTPUTS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_USE_TREADIES_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SWITCH_CRASH_SWITCH_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
    };

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    vid_pid = ops[0].read_value;
    version = ops[1].read_value;

    s = hcl_altera_vvp_core_parse_id(vid_pid, &parsed_id);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    s = hcl_altera_vvp_core_parse_version(version, &parsed_version);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    d->id = parsed_id;
    d->version = parsed_version;

    if (d->id.vendor_id != HCL_VVP_VENDOR_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }
    if (d->id.product_id != HCL_VVP_SWITCH_PRODUCT_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }
    regmap_version = d->version.regmap_version;
    if (regmap_version < HCL_VVP_SWITCH_MIN_SUPPORTED_REGMAP_VERSION ||
        regmap_version > HCL_VVP_SWITCH_MAX_SUPPORTED_REGMAP_VERSION) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->intf_type = (hcl_altera_vvp_switch_intf_type)ops[2].read_value;
    d->debug_enabled = (ops[3].read_value != 0u);
    d->uninterrupted_inputs = (ops[4].read_value != 0u);
    d->auto_consume = (ops[5].read_value != 0u);
    d->num_inputs = (uint8_t)(ops[6].read_value & 0xFFu);
    d->num_outputs = (uint8_t)(ops[7].read_value & 0xFFu);
    d->use_treadies = (ops[8].read_value != 0u);
    d->crash_switch = (ops[9].read_value != 0u);

    if (d->num_inputs > HCL_VVP_SWITCH_MAX_NUM_INPUTS ||
        d->num_outputs > HCL_VVP_SWITCH_MAX_NUM_OUTPUTS) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    if (d->intf_type != HCL_VVP_SWITCH_INTF_FULL &&
        d->intf_type != HCL_VVP_SWITCH_INTF_LITE &&
        d->intf_type != HCL_VVP_SWITCH_INTF_FR) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    return hcl_status_ok();
}

hcl_altera_vvp_switch_intf_type hcl_altera_vvp_switch_get_intf_type(const hcl_altera_vvp_switch* d) {
    if (d == NULL) {
        return HCL_VVP_SWITCH_INTF_FULL;
    }
    return d->intf_type;
}

bool hcl_altera_vvp_switch_get_debug_enabled(const hcl_altera_vvp_switch* d) {
    return (d != NULL) && d->debug_enabled;
}

bool hcl_altera_vvp_switch_has_uninterrupted_inputs(const hcl_altera_vvp_switch* d) {
    return (d != NULL) && d->uninterrupted_inputs;
}

bool hcl_altera_vvp_switch_has_auto_consume(const hcl_altera_vvp_switch* d) {
    return (d != NULL) && d->auto_consume;
}

uint8_t hcl_altera_vvp_switch_get_num_inputs(const hcl_altera_vvp_switch* d) {
    return (d == NULL) ? 0u : d->num_inputs;
}

uint8_t hcl_altera_vvp_switch_get_num_outputs(const hcl_altera_vvp_switch* d) {
    return (d == NULL) ? 0u : d->num_outputs;
}

bool hcl_altera_vvp_switch_is_crash_switch(const hcl_altera_vvp_switch* d) {
    return (d != NULL) && d->crash_switch;
}

bool hcl_altera_vvp_switch_uses_tready(const hcl_altera_vvp_switch* d) {
    return (d != NULL) && d->use_treadies;
}

hcl_status hcl_altera_vvp_switch_is_running(hcl_altera_vvp_switch* d, bool* out_running) {
    uint32_t status = 0u;
    hcl_status s;

    if (d == NULL || out_running == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_SWITCH_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_running = (status & HCL_VVP_SWITCH_STATUS_RUNNING_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_switch_commit_is_pending(hcl_altera_vvp_switch* d, bool* out_pending) {
    uint32_t status = 0u;
    hcl_status s;

    if (d == NULL || out_pending == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_SWITCH_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_pending = (status & HCL_VVP_SWITCH_STATUS_PENDING_COMMIT_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_switch_get_status(hcl_altera_vvp_switch* d, uint32_t* out_status) {
    if (d == NULL || out_status == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_VVP_SWITCH_STATUS_REG_ADDR, out_status);
}

hcl_status hcl_altera_vvp_switch_set_input_config(hcl_altera_vvp_switch* d,
                                                  uint8_t input,
                                                  hcl_altera_vvp_switch_input_config input_config) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (input >= d->num_inputs) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    if (input_config != HCL_VVP_SWITCH_INPUT_DISABLED &&
        input_config != HCL_VVP_SWITCH_INPUT_ENABLED &&
        input_config != HCL_VVP_SWITCH_INPUT_CONSUMED) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return write_reg(d,
                     HCL_VVP_SWITCH_INPUT_CONTROL_REG_ADDR(input),
                     ((uint32_t)input_config & HCL_VVP_SWITCH_INPUT_CONTROL_INPUT_CONFIG_MASK));
}

hcl_status hcl_altera_vvp_switch_get_input_config(hcl_altera_vvp_switch* d,
                                                  uint8_t input,
                                                  hcl_altera_vvp_switch_input_config* out_input_config) {
    uint32_t input_config = 0u;
    hcl_status s;

    if (d == NULL || out_input_config == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    if (input >= d->num_inputs) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    s = read_reg(d, HCL_VVP_SWITCH_INPUT_CONTROL_REG_ADDR(input), &input_config);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    input_config &= HCL_VVP_SWITCH_INPUT_CONTROL_INPUT_CONFIG_MASK;

    if (input_config == (uint32_t)HCL_VVP_SWITCH_INPUT_DISABLED) {
        *out_input_config = HCL_VVP_SWITCH_INPUT_DISABLED;
    } else if (input_config == (uint32_t)HCL_VVP_SWITCH_INPUT_CONSUMED) {
        *out_input_config = HCL_VVP_SWITCH_INPUT_CONSUMED;
    } else {
        *out_input_config = HCL_VVP_SWITCH_INPUT_ENABLED;
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_switch_set_output_config(hcl_altera_vvp_switch* d,
                                                   uint8_t output,
                                                   bool enabled,
                                                   uint8_t input) {
    uint32_t output_config = 0u;

    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (output >= d->num_outputs) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    if (enabled && input >= d->num_inputs) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    if (enabled) {
        output_config |= (((uint32_t)input << HCL_VVP_SWITCH_OUTPUT_CONTROL_INPUT_SHIFT) &
                          HCL_VVP_SWITCH_OUTPUT_CONTROL_INPUT_MASK);
        output_config |= HCL_VVP_SWITCH_OUTPUT_CONTROL_ENABLED_MASK;
    }

    return write_reg(d, HCL_VVP_SWITCH_OUTPUT_CONTROL_REG_ADDR(output), output_config);
}

hcl_status hcl_altera_vvp_switch_get_output_config(hcl_altera_vvp_switch* d,
                                                   uint8_t output,
                                                   hcl_altera_vvp_switch_output_config* out_output_config) {
    uint32_t output_config = 0u;
    uint8_t input;
    hcl_status s;

    if (d == NULL || out_output_config == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    if (output >= d->num_outputs) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    s = read_reg(d, HCL_VVP_SWITCH_OUTPUT_CONTROL_REG_ADDR(output), &output_config);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    out_output_config->enabled =
        (output_config & HCL_VVP_SWITCH_OUTPUT_CONTROL_ENABLED_MASK) != 0u;

    input = (uint8_t)((output_config & HCL_VVP_SWITCH_OUTPUT_CONTROL_INPUT_MASK) >>
                      HCL_VVP_SWITCH_OUTPUT_CONTROL_INPUT_SHIFT);
    if (input >= d->num_inputs) {
        out_output_config->enabled = false;
        out_output_config->input = 0u;
    } else {
        out_output_config->input = input;
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_switch_commit(hcl_altera_vvp_switch* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_SWITCH_COMMIT_REG_ADDR, 1u);
}
