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

#include "hcl/drivers/altera_vvp_blc.h"
#include "hcl/drivers/altera_vvp_core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct hcl_altera_vvp_blc {
    hcl_register_interface* reg;
    uint32_t base_address;

    hcl_altera_vvp_id id;
    hcl_altera_vvp_version version;

    bool     lite_mode;
    bool     debug_enabled;
    uint8_t  bps_in;
    uint8_t  bps_out;
    uint8_t  num_color_in;
    uint8_t  num_color_out;
    uint8_t  pip;
    uint32_t max_width;
    uint32_t max_height;
    bool     reflect_around_zero;

    uint32_t settings_reg;
};

static hcl_status write_reg(hcl_altera_vvp_blc* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_altera_vvp_blc* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, d->base_address, offset, out);
}

hcl_altera_vvp_blc* hcl_altera_vvp_blc_create(hcl_register_interface* reg,
                                              uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }
    hcl_altera_vvp_blc* d = (hcl_altera_vvp_blc*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }
    d->reg = reg;
    d->base_address = base_address;
    return d;
}

void hcl_altera_vvp_blc_destroy(hcl_altera_vvp_blc* driver) {
    free(driver);
}

hcl_status hcl_altera_vvp_blc_init(hcl_altera_vvp_blc* d) {
    uint8_t regmap_version = 0u;
    hcl_status s;

    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_VID_PID_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_VERSION_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_LITE_MODE_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_DEBUG_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_BPS_IN_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_BPS_OUT_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_NUM_COLOR_IN_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_NUM_COLOR_OUT_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_PIP_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_MAX_WIDTH_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_MAX_HEIGHT_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_REFLECT_AROUND_ZERO_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_BLC_SETTINGS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
    };

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    uint32_t vid_pid = ops[0].read_value;
    uint32_t version = ops[1].read_value;
    hcl_altera_vvp_id parsed_id = {0u};
    hcl_altera_vvp_version parsed_version = {0u};

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
    if (d->id.product_id != HCL_VVP_BLC_PRODUCT_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }
    regmap_version = d->version.regmap_version;

    if (regmap_version < HCL_VVP_BLC_MIN_SUPPORTED_REGMAP_VERSION ||
        regmap_version > HCL_VVP_BLC_MAX_SUPPORTED_REGMAP_VERSION) {
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
    d->reflect_around_zero = (ops[11].read_value != 0u);
    d->settings_reg = ops[12].read_value;

    return hcl_status_ok();
}

bool hcl_altera_vvp_blc_get_lite_mode(const hcl_altera_vvp_blc* d) {
    return (d != NULL) && d->lite_mode;
}

bool hcl_altera_vvp_blc_get_debug_enabled(const hcl_altera_vvp_blc* d) {
    return (d != NULL) && d->debug_enabled;
}

uint8_t hcl_altera_vvp_blc_get_bps_in(const hcl_altera_vvp_blc* d) {
    return (d == NULL) ? 0u : d->bps_in;
}

uint8_t hcl_altera_vvp_blc_get_bps_out(const hcl_altera_vvp_blc* d) {
    return (d == NULL) ? 0u : d->bps_out;
}

uint8_t hcl_altera_vvp_blc_get_num_color_in(const hcl_altera_vvp_blc* d) {
    return (d == NULL) ? 0u : d->num_color_in;
}

uint8_t hcl_altera_vvp_blc_get_num_color_out(const hcl_altera_vvp_blc* d) {
    return (d == NULL) ? 0u : d->num_color_out;
}

uint8_t hcl_altera_vvp_blc_get_pip(const hcl_altera_vvp_blc* d) {
    return (d == NULL) ? 0u : d->pip;
}

uint32_t hcl_altera_vvp_blc_get_max_width(const hcl_altera_vvp_blc* d) {
    return (d == NULL) ? 0u : d->max_width;
}

uint32_t hcl_altera_vvp_blc_get_max_height(const hcl_altera_vvp_blc* d) {
    return (d == NULL) ? 0u : d->max_height;
}

uint32_t hcl_altera_vvp_blc_get_img_info_width(const hcl_altera_vvp_blc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_blc*)d, HCL_VVP_BLC_IMG_INFO_WIDTH_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_blc_set_img_info_width(hcl_altera_vvp_blc* d, uint32_t width) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_BLC_IMG_INFO_WIDTH_REG_ADDR, width);
}

uint32_t hcl_altera_vvp_blc_get_img_info_height(const hcl_altera_vvp_blc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_blc*)d, HCL_VVP_BLC_IMG_INFO_HEIGHT_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_blc_set_img_info_height(hcl_altera_vvp_blc* d, uint32_t height) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_BLC_IMG_INFO_HEIGHT_REG_ADDR, height);
}

bool hcl_altera_vvp_blc_get_reflect_around_zero(const hcl_altera_vvp_blc* d) {
    return (d != NULL) && d->reflect_around_zero;
}

hcl_status hcl_altera_vvp_blc_is_running(hcl_altera_vvp_blc* d, bool* out) {
    uint32_t status = 0u;
    hcl_status s;
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    s = read_reg(d, HCL_VVP_BLC_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out = (status & HCL_VVP_BLC_STATUS_RUNNING_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_blc_commit_is_pending(hcl_altera_vvp_blc* d, bool* out) {
    uint32_t status = 0u;
    hcl_status s;
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    s = read_reg(d, HCL_VVP_BLC_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out = (status & HCL_VVP_BLC_STATUS_COMMIT_PENDING_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_blc_get_status(hcl_altera_vvp_blc* d, uint32_t* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_VVP_BLC_STATUS_REG_ADDR, out);
}

hcl_status hcl_altera_vvp_blc_get_frame_stats(hcl_altera_vvp_blc* d, uint32_t* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_VVP_BLC_FRAME_STATS_REG_ADDR, out);
}

hcl_status hcl_altera_vvp_blc_commit(hcl_altera_vvp_blc* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_BLC_COMMIT_REG_ADDR, 1u);
}

bool hcl_altera_vvp_blc_get_bypass(const hcl_altera_vvp_blc* d) {
    return (d != NULL) && ((d->settings_reg & HCL_VVP_BLC_SETTINGS_BYPASS_MASK) != 0u);
}

hcl_status hcl_altera_vvp_blc_set_bypass(hcl_altera_vvp_blc* d, bool bypass) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (bypass) {
        d->settings_reg |= HCL_VVP_BLC_SETTINGS_BYPASS_MASK;
    } else {
        d->settings_reg &= ~HCL_VVP_BLC_SETTINGS_BYPASS_MASK;
    }
    return write_reg(d, HCL_VVP_BLC_SETTINGS_REG_ADDR, d->settings_reg);
}

uint8_t hcl_altera_vvp_blc_get_cfa_phase(const hcl_altera_vvp_blc* d) {
    return (d == NULL) ? 0u : (uint8_t)((d->settings_reg & HCL_VVP_BLC_SETTINGS_CFA_PHASE_MASK) >> HCL_VVP_BLC_SETTINGS_CFA_PHASE_SHIFT);
}

hcl_status hcl_altera_vvp_blc_set_cfa_phase(hcl_altera_vvp_blc* d, uint8_t cfa_phase) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (cfa_phase > HCL_VVP_BLC_CFA_PHASE_MAX) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    d->settings_reg = (d->settings_reg & ~HCL_VVP_BLC_SETTINGS_CFA_PHASE_MASK)
                    | (((uint32_t)cfa_phase << HCL_VVP_BLC_SETTINGS_CFA_PHASE_SHIFT) & HCL_VVP_BLC_SETTINGS_CFA_PHASE_MASK);
    return write_reg(d, HCL_VVP_BLC_SETTINGS_REG_ADDR, d->settings_reg);
}

bool hcl_altera_vvp_blc_get_clip_zero_en(const hcl_altera_vvp_blc* d) {
    return (d != NULL) && ((d->settings_reg & HCL_VVP_BLC_SETTINGS_CLIP_ZERO_EN_MASK) != 0u);
}

hcl_status hcl_altera_vvp_blc_set_clip_zero_en(hcl_altera_vvp_blc* d, bool clip_zero_en) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (clip_zero_en) {
        d->settings_reg |= HCL_VVP_BLC_SETTINGS_CLIP_ZERO_EN_MASK;
    } else {
        d->settings_reg &= ~HCL_VVP_BLC_SETTINGS_CLIP_ZERO_EN_MASK;
    }
    return write_reg(d, HCL_VVP_BLC_SETTINGS_REG_ADDR, d->settings_reg);
}

static hcl_status read_cfa_reg(hcl_altera_vvp_blc* d, uint32_t reg_addr, uint32_t mask, uint32_t* out) {
    uint32_t reg = 0u;
    hcl_status s;
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    s = read_reg(d, reg_addr, &reg);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out = reg & mask;
    return hcl_status_ok();
}

static hcl_status write_cfa_reg(hcl_altera_vvp_blc* d, uint32_t reg_addr, uint32_t mask, uint32_t value) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (value > mask) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return write_reg(d, reg_addr, value & mask);
}

hcl_status hcl_altera_vvp_blc_get_cfa_00_black_pedestal(hcl_altera_vvp_blc* d, uint32_t* out) {
    return read_cfa_reg(d, HCL_VVP_BLC_CFA_00_BLACK_REG_ADDR, HCL_VVP_BLC_BLACK_PEDESTAL_MASK, out);
}

hcl_status hcl_altera_vvp_blc_set_cfa_00_black_pedestal(hcl_altera_vvp_blc* d, uint32_t pedestal) {
    return write_cfa_reg(d, HCL_VVP_BLC_CFA_00_BLACK_REG_ADDR, HCL_VVP_BLC_BLACK_PEDESTAL_MASK, pedestal);
}

hcl_status hcl_altera_vvp_blc_get_cfa_00_color_scaler(hcl_altera_vvp_blc* d, uint32_t* out) {
    return read_cfa_reg(d, HCL_VVP_BLC_CFA_00_COLOR_REG_ADDR, HCL_VVP_BLC_COLOR_SCALER_MASK, out);
}

hcl_status hcl_altera_vvp_blc_set_cfa_00_color_scaler(hcl_altera_vvp_blc* d, uint32_t scaler) {
    return write_cfa_reg(d, HCL_VVP_BLC_CFA_00_COLOR_REG_ADDR, HCL_VVP_BLC_COLOR_SCALER_MASK, scaler);
}

hcl_status hcl_altera_vvp_blc_get_cfa_01_black_pedestal(hcl_altera_vvp_blc* d, uint32_t* out) {
    return read_cfa_reg(d, HCL_VVP_BLC_CFA_01_BLACK_REG_ADDR, HCL_VVP_BLC_BLACK_PEDESTAL_MASK, out);
}

hcl_status hcl_altera_vvp_blc_set_cfa_01_black_pedestal(hcl_altera_vvp_blc* d, uint32_t pedestal) {
    return write_cfa_reg(d, HCL_VVP_BLC_CFA_01_BLACK_REG_ADDR, HCL_VVP_BLC_BLACK_PEDESTAL_MASK, pedestal);
}

hcl_status hcl_altera_vvp_blc_get_cfa_01_color_scaler(hcl_altera_vvp_blc* d, uint32_t* out) {
    return read_cfa_reg(d, HCL_VVP_BLC_CFA_01_COLOR_REG_ADDR, HCL_VVP_BLC_COLOR_SCALER_MASK, out);
}

hcl_status hcl_altera_vvp_blc_set_cfa_01_color_scaler(hcl_altera_vvp_blc* d, uint32_t scaler) {
    return write_cfa_reg(d, HCL_VVP_BLC_CFA_01_COLOR_REG_ADDR, HCL_VVP_BLC_COLOR_SCALER_MASK, scaler);
}

hcl_status hcl_altera_vvp_blc_get_cfa_10_black_pedestal(hcl_altera_vvp_blc* d, uint32_t* out) {
    return read_cfa_reg(d, HCL_VVP_BLC_CFA_10_BLACK_REG_ADDR, HCL_VVP_BLC_BLACK_PEDESTAL_MASK, out);
}

hcl_status hcl_altera_vvp_blc_set_cfa_10_black_pedestal(hcl_altera_vvp_blc* d, uint32_t pedestal) {
    return write_cfa_reg(d, HCL_VVP_BLC_CFA_10_BLACK_REG_ADDR, HCL_VVP_BLC_BLACK_PEDESTAL_MASK, pedestal);
}

hcl_status hcl_altera_vvp_blc_get_cfa_10_color_scaler(hcl_altera_vvp_blc* d, uint32_t* out) {
    return read_cfa_reg(d, HCL_VVP_BLC_CFA_10_COLOR_REG_ADDR, HCL_VVP_BLC_COLOR_SCALER_MASK, out);
}

hcl_status hcl_altera_vvp_blc_set_cfa_10_color_scaler(hcl_altera_vvp_blc* d, uint32_t scaler) {
    return write_cfa_reg(d, HCL_VVP_BLC_CFA_10_COLOR_REG_ADDR, HCL_VVP_BLC_COLOR_SCALER_MASK, scaler);
}

hcl_status hcl_altera_vvp_blc_get_cfa_11_black_pedestal(hcl_altera_vvp_blc* d, uint32_t* out) {
    return read_cfa_reg(d, HCL_VVP_BLC_CFA_11_BLACK_REG_ADDR, HCL_VVP_BLC_BLACK_PEDESTAL_MASK, out);
}

hcl_status hcl_altera_vvp_blc_set_cfa_11_black_pedestal(hcl_altera_vvp_blc* d, uint32_t pedestal) {
    return write_cfa_reg(d, HCL_VVP_BLC_CFA_11_BLACK_REG_ADDR, HCL_VVP_BLC_BLACK_PEDESTAL_MASK, pedestal);
}

hcl_status hcl_altera_vvp_blc_get_cfa_11_color_scaler(hcl_altera_vvp_blc* d, uint32_t* out) {
    return read_cfa_reg(d, HCL_VVP_BLC_CFA_11_COLOR_REG_ADDR, HCL_VVP_BLC_COLOR_SCALER_MASK, out);
}

hcl_status hcl_altera_vvp_blc_set_cfa_11_color_scaler(hcl_altera_vvp_blc* d, uint32_t scaler) {
    return write_cfa_reg(d, HCL_VVP_BLC_CFA_11_COLOR_REG_ADDR, HCL_VVP_BLC_COLOR_SCALER_MASK, scaler);
}

hcl_status hcl_altera_vvp_blc_set_cfa_settings(hcl_altera_vvp_blc* d,
                                               const hcl_altera_vvp_blc_cfa_settings* settings) {
    if (d == NULL || settings == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (settings->cfa_00_black_pedestal > HCL_VVP_BLC_BLACK_PEDESTAL_MASK ||
        settings->cfa_00_color_scaler > HCL_VVP_BLC_COLOR_SCALER_MASK ||
        settings->cfa_01_black_pedestal > HCL_VVP_BLC_BLACK_PEDESTAL_MASK ||
        settings->cfa_01_color_scaler > HCL_VVP_BLC_COLOR_SCALER_MASK ||
        settings->cfa_10_black_pedestal > HCL_VVP_BLC_BLACK_PEDESTAL_MASK ||
        settings->cfa_10_color_scaler > HCL_VVP_BLC_COLOR_SCALER_MASK ||
        settings->cfa_11_black_pedestal > HCL_VVP_BLC_BLACK_PEDESTAL_MASK ||
        settings->cfa_11_color_scaler > HCL_VVP_BLC_COLOR_SCALER_MASK) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_BLC_CFA_00_BLACK_REG_ADDR, settings->cfa_00_black_pedestal & HCL_VVP_BLC_BLACK_PEDESTAL_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_BLC_CFA_00_COLOR_REG_ADDR, settings->cfa_00_color_scaler & HCL_VVP_BLC_COLOR_SCALER_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_BLC_CFA_01_BLACK_REG_ADDR, settings->cfa_01_black_pedestal & HCL_VVP_BLC_BLACK_PEDESTAL_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_BLC_CFA_01_COLOR_REG_ADDR, settings->cfa_01_color_scaler & HCL_VVP_BLC_COLOR_SCALER_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_BLC_CFA_10_BLACK_REG_ADDR, settings->cfa_10_black_pedestal & HCL_VVP_BLC_BLACK_PEDESTAL_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_BLC_CFA_10_COLOR_REG_ADDR, settings->cfa_10_color_scaler & HCL_VVP_BLC_COLOR_SCALER_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_BLC_CFA_11_BLACK_REG_ADDR, settings->cfa_11_black_pedestal & HCL_VVP_BLC_BLACK_PEDESTAL_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_BLC_CFA_11_COLOR_REG_ADDR, settings->cfa_11_color_scaler & HCL_VVP_BLC_COLOR_SCALER_MASK, 0u, HCL_STATUS_OK},
    };

    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}
