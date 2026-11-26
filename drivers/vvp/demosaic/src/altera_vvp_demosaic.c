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

#include "hcl/drivers/altera_vvp_demosaic.h"
#include "hcl/drivers/altera_vvp_core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct hcl_altera_vvp_demosaic {
    hcl_register_interface* reg;
    uint32_t base_address;

    hcl_altera_vvp_id id;
    hcl_altera_vvp_version version;

    bool lite_mode;
    bool debug_enabled;
    uint8_t bps_in;
    uint8_t bps_out;
    uint8_t num_color_in;
    uint8_t num_color_out;
    uint8_t pip;
    uint32_t max_width;
    uint32_t max_height;
};

static hcl_status write_reg(hcl_altera_vvp_demosaic* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_altera_vvp_demosaic* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, d->base_address, offset, out);
}

static hcl_status execute_ops(hcl_altera_vvp_demosaic* d,
                              hcl_register_operation* ops,
                              size_t operation_count) {
    size_t i;

    if (d == NULL || ops == NULL || operation_count == 0u) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    for (i = 0u; i < operation_count; ++i) {
        ops[i].base_address = d->base_address;
    }

    return hcl_register_interface_execute(d->reg, ops, operation_count);
}

hcl_altera_vvp_demosaic* hcl_altera_vvp_demosaic_create(hcl_register_interface* reg,
                                                        uint32_t base_address) {
    hcl_altera_vvp_demosaic* d;

    if (reg == NULL) {
        return NULL;
    }

    d = (hcl_altera_vvp_demosaic*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }

    d->reg = reg;
    d->base_address = base_address;
    return d;
}

void hcl_altera_vvp_demosaic_destroy(hcl_altera_vvp_demosaic* driver) {
    free(driver);
}

hcl_status hcl_altera_vvp_demosaic_init(hcl_altera_vvp_demosaic* d) {
    uint32_t vid_pid = 0u;
    uint32_t version = 0u;
    uint8_t regmap_version;
    hcl_altera_vvp_id parsed_id = {0u};
    hcl_altera_vvp_version parsed_version = {0u};
    hcl_status s;
    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_VID_PID_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_VERSION_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_LITE_MODE_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_DEBUG_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_BPS_IN_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_BPS_OUT_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_NUM_COLOR_IN_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_NUM_COLOR_OUT_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_PIP_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_MAX_WIDTH_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_VVP_DEMOSAIC_MAX_HEIGHT_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
    };

    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = execute_ops(d, ops, sizeof(ops) / sizeof(ops[0]));
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
    if (d->id.product_id != HCL_VVP_DEMOSAIC_PRODUCT_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    regmap_version = d->version.regmap_version;
    if (regmap_version < HCL_VVP_DEMOSAIC_MIN_SUPPORTED_REGMAP_VERSION ||
        regmap_version > HCL_VVP_DEMOSAIC_MAX_SUPPORTED_REGMAP_VERSION) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->lite_mode = (ops[2].read_value != 0u);
    d->debug_enabled = (ops[3].read_value != 0u);
    d->bps_in = (uint8_t)(ops[4].read_value & 0xFFu);
    d->bps_out = (uint8_t)(ops[5].read_value & 0xFFu);
    d->num_color_in = (uint8_t)(ops[6].read_value & 0xFFu);
    d->num_color_out = (uint8_t)(ops[7].read_value & 0xFFu);
    d->pip = (uint8_t)(ops[8].read_value & 0xFFu);
    d->max_width = ops[9].read_value;
    d->max_height = ops[10].read_value;

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_demosaic_get_id(const hcl_altera_vvp_demosaic* d,
                                          hcl_altera_vvp_id* out_id) {
    if (d == NULL || out_id == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    *out_id = d->id;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_demosaic_get_version(const hcl_altera_vvp_demosaic* d,
                                               hcl_altera_vvp_version* out_version) {
    if (d == NULL || out_version == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    *out_version = d->version;
    return hcl_status_ok();
}

bool hcl_altera_vvp_demosaic_get_lite_mode(const hcl_altera_vvp_demosaic* d) {
    return (d != NULL) && d->lite_mode;
}

bool hcl_altera_vvp_demosaic_get_debug_enabled(const hcl_altera_vvp_demosaic* d) {
    return (d != NULL) && d->debug_enabled;
}

uint8_t hcl_altera_vvp_demosaic_get_bits_per_sample_in(const hcl_altera_vvp_demosaic* d) {
    return (d == NULL) ? 0u : d->bps_in;
}

uint8_t hcl_altera_vvp_demosaic_get_bits_per_sample_out(const hcl_altera_vvp_demosaic* d) {
    return (d == NULL) ? 0u : d->bps_out;
}

uint8_t hcl_altera_vvp_demosaic_get_num_color_planes_in(const hcl_altera_vvp_demosaic* d) {
    return (d == NULL) ? 0u : d->num_color_in;
}

uint8_t hcl_altera_vvp_demosaic_get_num_color_planes_out(const hcl_altera_vvp_demosaic* d) {
    return (d == NULL) ? 0u : d->num_color_out;
}

uint8_t hcl_altera_vvp_demosaic_get_pixels_in_parallel(const hcl_altera_vvp_demosaic* d) {
    return (d == NULL) ? 0u : d->pip;
}

uint32_t hcl_altera_vvp_demosaic_get_max_width(const hcl_altera_vvp_demosaic* d) {
    return (d == NULL) ? 0u : d->max_width;
}

uint32_t hcl_altera_vvp_demosaic_get_max_height(const hcl_altera_vvp_demosaic* d) {
    return (d == NULL) ? 0u : d->max_height;
}

uint32_t hcl_altera_vvp_demosaic_get_img_info_width(const hcl_altera_vvp_demosaic* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_demosaic*)d, HCL_VVP_DEMOSAIC_IMG_INFO_WIDTH_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_demosaic_set_img_info_width(hcl_altera_vvp_demosaic* d, uint32_t width) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_DEMOSAIC_IMG_INFO_WIDTH_REG_ADDR, width);
}

uint32_t hcl_altera_vvp_demosaic_get_img_info_height(const hcl_altera_vvp_demosaic* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_demosaic*)d, HCL_VVP_DEMOSAIC_IMG_INFO_HEIGHT_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_demosaic_set_img_info_height(hcl_altera_vvp_demosaic* d, uint32_t height) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_DEMOSAIC_IMG_INFO_HEIGHT_REG_ADDR, height);
}

hcl_status hcl_altera_vvp_demosaic_is_running(hcl_altera_vvp_demosaic* d, bool* out_running) {
    uint32_t status = 0u;
    hcl_status s;

    if (d == NULL || out_running == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_DEMOSAIC_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_running = (status & HCL_VVP_DEMOSAIC_STATUS_RUNNING_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_demosaic_get_status(hcl_altera_vvp_demosaic* d, uint32_t* out_status) {
    if (d == NULL || out_status == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return read_reg(d, HCL_VVP_DEMOSAIC_STATUS_REG_ADDR, out_status);
}

hcl_status hcl_altera_vvp_demosaic_get_frame_stats(hcl_altera_vvp_demosaic* d, uint32_t* out_stats) {
    if (d == NULL || out_stats == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return read_reg(d, HCL_VVP_DEMOSAIC_FRAME_STATS_REG_ADDR, out_stats);
}

bool hcl_altera_vvp_demosaic_get_bypass(const hcl_altera_vvp_demosaic* d) {
    uint32_t settings = 0u;

    if (d == NULL) {
        return false;
    }
    if (read_reg((hcl_altera_vvp_demosaic*)d, HCL_VVP_DEMOSAIC_CONTROL_REG_ADDR, &settings).code != HCL_STATUS_OK) {
        return false;
    }

    return (settings & HCL_VVP_DEMOSAIC_CONTROL_BYPASS_MASK) != 0u;
}

hcl_status hcl_altera_vvp_demosaic_set_bypass(hcl_altera_vvp_demosaic* d, bool bypass) {
    uint32_t settings = 0u;
    hcl_status s;

    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_DEMOSAIC_CONTROL_REG_ADDR, &settings);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    if (bypass) {
        settings |= HCL_VVP_DEMOSAIC_CONTROL_BYPASS_MASK;
    } else {
        settings &= ~HCL_VVP_DEMOSAIC_CONTROL_BYPASS_MASK;
    }

    return write_reg(d, HCL_VVP_DEMOSAIC_CONTROL_REG_ADDR, settings);
}

uint8_t hcl_altera_vvp_demosaic_get_cfa_phase(const hcl_altera_vvp_demosaic* d) {
    uint32_t settings = 0u;

    if (d == NULL) {
        return 0xFFu;
    }
    if (read_reg((hcl_altera_vvp_demosaic*)d, HCL_VVP_DEMOSAIC_CONTROL_REG_ADDR, &settings).code != HCL_STATUS_OK) {
        return 0xFFu;
    }

    return (uint8_t)((settings & HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_MASK) >>
                     HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_SHIFT);
}

hcl_status hcl_altera_vvp_demosaic_set_cfa_phase(hcl_altera_vvp_demosaic* d, uint8_t cfa_phase) {
    uint32_t settings = 0u;
    hcl_status s;

    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (cfa_phase > HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_MAX) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    s = read_reg(d, HCL_VVP_DEMOSAIC_CONTROL_REG_ADDR, &settings);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    settings = (settings & ~HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_MASK) |
               (((uint32_t)cfa_phase << HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_SHIFT) &
                HCL_VVP_DEMOSAIC_CONTROL_CFA_PHASE_MASK);

    return write_reg(d, HCL_VVP_DEMOSAIC_CONTROL_REG_ADDR, settings);
}
