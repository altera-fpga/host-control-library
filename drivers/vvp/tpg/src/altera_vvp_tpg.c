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

#include "hcl/drivers/altera_vvp_tpg.h"

#include "hcl/drivers/altera_vvp_core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct hcl_altera_vvp_tpg {
    hcl_register_interface* reg;
    uint32_t base_address;
    hcl_altera_vvp_id id;
    hcl_altera_vvp_version version;
    uint8_t num_patterns;
    uint8_t bps;
    uint8_t pip;
    uint16_t max_color_value;
    bool debug_enabled;
    bool lite_mode;
};

static hcl_status write_reg(hcl_altera_vvp_tpg* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_altera_vvp_tpg* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, d->base_address, offset, out);
}

static hcl_status set_rgb_triplet(hcl_altera_vvp_tpg* d,
                                  uint32_t reg_b,
                                  uint32_t reg_g,
                                  uint32_t reg_r,
                                  uint16_t r,
                                  uint16_t g,
                                  uint16_t b) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, reg_b, b, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, reg_g, g, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, reg_r, r, 0u, HCL_STATUS_OK},
    };

    if (r > d->max_color_value || g > d->max_color_value || b > d->max_color_value) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}

static hcl_status get_rgb_triplet(hcl_altera_vvp_tpg* d,
                                  uint32_t reg_b,
                                  uint32_t reg_g,
                                  uint32_t reg_r,
                                  uint16_t* out_r,
                                  uint16_t* out_g,
                                  uint16_t* out_b) {
    if (d == NULL || out_r == NULL || out_g == NULL || out_b == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, reg_r, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, reg_g, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, reg_b, 0u, 0u, HCL_STATUS_OK},
    };
    hcl_status s;

    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_r = (uint16_t)ops[0].read_value;
    *out_g = (uint16_t)ops[1].read_value;
    *out_b = (uint16_t)ops[2].read_value;
    return hcl_status_ok();
}

hcl_altera_vvp_tpg* hcl_altera_vvp_tpg_create(hcl_register_interface* reg,
                                              uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }

    hcl_altera_vvp_tpg* d = (hcl_altera_vvp_tpg*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }

    d->reg = reg;
    d->base_address = base_address;

    return d;
}

void hcl_altera_vvp_tpg_destroy(hcl_altera_vvp_tpg* driver) {
    free(driver);
}

hcl_status hcl_altera_vvp_tpg_init(hcl_altera_vvp_tpg* d) {
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
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_VID_PID_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_VERSION_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_LITE_MODE_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_DEBUG_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_NUM_PATTERNS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_BPS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_PIP_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
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
    if (d->id.product_id != HCL_VVP_TPG_PRODUCT_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }
    regmap_version = d->version.regmap_version;
    if (regmap_version < HCL_VVP_TPG_MIN_SUPPORTED_REGMAP_VERSION ||
        regmap_version > HCL_VVP_TPG_MAX_SUPPORTED_REGMAP_VERSION) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->lite_mode = (ops[2].read_value != 0u);
    d->debug_enabled = (ops[3].read_value != 0u);
    d->num_patterns = (uint8_t)(ops[4].read_value & 0xFFu);
    d->bps = (uint8_t)(ops[5].read_value & 0x1Fu);
    d->pip = (uint8_t)(ops[6].read_value & 0xFFu);

    if (d->num_patterns > HCL_VVP_TPG_MAX_NUM_PATTERNS || d->bps == 0u || d->bps > 16u) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->max_color_value = (uint16_t)((1u << d->bps) - 1u);

    return hcl_altera_vvp_tpg_stop(d);
}

bool hcl_altera_vvp_tpg_get_lite_mode(const hcl_altera_vvp_tpg* d) {
    return (d != NULL) && d->lite_mode;
}

bool hcl_altera_vvp_tpg_get_debug_enabled(const hcl_altera_vvp_tpg* d) {
    return (d != NULL) && d->debug_enabled;
}

uint8_t hcl_altera_vvp_tpg_get_num_patterns(const hcl_altera_vvp_tpg* d) {
    return (d == NULL) ? 0u : d->num_patterns;
}

uint8_t hcl_altera_vvp_tpg_get_bits_per_sample(const hcl_altera_vvp_tpg* d) {
    return (d == NULL) ? 0u : d->bps;
}

uint8_t hcl_altera_vvp_tpg_get_pixels_in_parallel(const hcl_altera_vvp_tpg* d) {
    return (d == NULL) ? 0u : d->pip;
}

uint32_t hcl_altera_vvp_tpg_get_img_info_width(const hcl_altera_vvp_tpg* d) {
    uint32_t value = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_tpg*)d, HCL_VVP_TPG_IMG_INFO_WIDTH_REG_ADDR, &value).code != HCL_STATUS_OK) {
        return 0u;
    }
    return value;
}

hcl_status hcl_altera_vvp_tpg_set_img_info_width(hcl_altera_vvp_tpg* d, uint32_t width) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_TPG_IMG_INFO_WIDTH_REG_ADDR, width);
}

uint32_t hcl_altera_vvp_tpg_get_img_info_height(const hcl_altera_vvp_tpg* d) {
    uint32_t value = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_tpg*)d, HCL_VVP_TPG_IMG_INFO_HEIGHT_REG_ADDR, &value).code != HCL_STATUS_OK) {
        return 0u;
    }
    return value;
}

hcl_status hcl_altera_vvp_tpg_set_img_info_height(hcl_altera_vvp_tpg* d, uint32_t height) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_TPG_IMG_INFO_HEIGHT_REG_ADDR, height);
}

uint32_t hcl_altera_vvp_tpg_get_img_info_interlace(const hcl_altera_vvp_tpg* d) {
    uint32_t value = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_tpg*)d, HCL_VVP_TPG_IMG_INFO_INTERLACE_REG_ADDR, &value).code != HCL_STATUS_OK) {
        return 0u;
    }
    return value;
}

hcl_status hcl_altera_vvp_tpg_set_img_info_interlace(hcl_altera_vvp_tpg* d, uint32_t interlace) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_TPG_IMG_INFO_INTERLACE_REG_ADDR, interlace);
}

hcl_status hcl_altera_vvp_tpg_get_pattern_data(hcl_altera_vvp_tpg* d,
                                                    uint8_t pattern,
                                                    hcl_altera_vvp_tpg_pattern* out_pattern_data) {
    if (d == NULL || out_pattern_data == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_PATTERN_TYPE_REG_ADDR(pattern), 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_PATTERN_COLOR_REG_ADDR(pattern), 0u, 0u, HCL_STATUS_OK},
    };
    uint32_t pattern_type;
    uint32_t pattern_color;
    hcl_status s;

    if (pattern >= d->num_patterns) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    pattern_type = ops[0].read_value;

    switch (pattern_type) {
        case HCL_VVP_TPG_BARS_PATTERN:
            out_pattern_data->type = HCL_VVP_TPG_PATTERN_TYPE_BARS;
            break;
        case HCL_VVP_TPG_UNIFORM_PATTERN:
            out_pattern_data->type = HCL_VVP_TPG_PATTERN_TYPE_UNIFORM;
            break;
        case HCL_VVP_TPG_PATHOLOGICAL_PATTERN:
            out_pattern_data->type = HCL_VVP_TPG_PATTERN_TYPE_PATHOLOGICAL;
            break;
        case HCL_VVP_TPG_ZONE_PLATE_PATTERN:
            out_pattern_data->type = HCL_VVP_TPG_PATTERN_TYPE_ZONE_PLATE;
            break;
        case HCL_VVP_TPG_DIGITAL_CLOCK_PATTERN:
            out_pattern_data->type = HCL_VVP_TPG_PATTERN_TYPE_DIGITAL_CLOCK;
            break;
        case HCL_VVP_TPG_SIGNALTAP_COUNTER_PATTERN:
            out_pattern_data->type = HCL_VVP_TPG_PATTERN_TYPE_SIGNALTAP;
            break;
        default:
            out_pattern_data->type = HCL_VVP_TPG_PATTERN_TYPE_INVALID;
            break;
    }

    pattern_color = ops[1].read_value;

    switch (pattern_color) {
        case HCL_VVP_TPG_RGB:
            out_pattern_data->color = HCL_VVP_TPG_PATTERN_COLOR_RGB;
            break;
        case HCL_VVP_TPG_YCC_444:
            out_pattern_data->color = HCL_VVP_TPG_PATTERN_COLOR_YCC_444;
            break;
        case HCL_VVP_TPG_YCC_422:
            out_pattern_data->color = HCL_VVP_TPG_PATTERN_COLOR_YCC_422;
            break;
        case HCL_VVP_TPG_YCC_420:
            out_pattern_data->color = HCL_VVP_TPG_PATTERN_COLOR_YCC_420;
            break;
        case HCL_VVP_TPG_MONO:
            out_pattern_data->color = HCL_VVP_TPG_PATTERN_COLOR_MONO;
            break;
        default:
            out_pattern_data->color = HCL_VVP_TPG_PATTERN_COLOR_INVALID;
            break;
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_is_running(hcl_altera_vvp_tpg* d, bool* out_running) {
    uint32_t status_reg;
    hcl_status s;

    if (d == NULL || out_running == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_TPG_STATUS_REG_ADDR, &status_reg);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_running = (status_reg & HCL_VVP_TPG_STATUS_RUNNING_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_commit_is_pending(hcl_altera_vvp_tpg* d, bool* out_pending) {
    uint32_t status_reg;
    hcl_status s;

    if (d == NULL || out_pending == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    s = read_reg(d, HCL_VVP_TPG_STATUS_REG_ADDR, &status_reg);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_pending = (status_reg & HCL_VVP_TPG_STATUS_PENDING_COMMIT_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_get_status(hcl_altera_vvp_tpg* d, uint32_t* out_status) {
    if (d == NULL || out_status == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return read_reg(d, HCL_VVP_TPG_STATUS_REG_ADDR, out_status);
}

hcl_status hcl_altera_vvp_tpg_enable(hcl_altera_vvp_tpg* d, bool enabled) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_TPG_CONTROL_REG_ADDR, enabled ? HCL_VVP_TPG_CONTROL_GO_MASK : 0u);
}

hcl_status hcl_altera_vvp_tpg_start(hcl_altera_vvp_tpg* d) {
    return hcl_altera_vvp_tpg_enable(d, true);
}

hcl_status hcl_altera_vvp_tpg_stop(hcl_altera_vvp_tpg* d) {
    return hcl_altera_vvp_tpg_enable(d, false);
}

hcl_status hcl_altera_vvp_tpg_is_enabled(hcl_altera_vvp_tpg* d, bool* out_enabled) {
    uint32_t control_reg;
    hcl_status s;

    if (d == NULL || out_enabled == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_TPG_CONTROL_REG_ADDR, &control_reg);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_enabled = (control_reg & HCL_VVP_TPG_CONTROL_GO_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_set_pattern(hcl_altera_vvp_tpg* d, uint8_t pattern) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (pattern >= d->num_patterns) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return write_reg(d, HCL_VVP_TPG_PATTERN_SELECT_REG_ADDR, pattern);
}

hcl_status hcl_altera_vvp_tpg_get_pattern(hcl_altera_vvp_tpg* d, uint8_t* out_pattern) {
    uint32_t pattern;
    hcl_status s;

    if (d == NULL || out_pattern == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_TPG_PATTERN_SELECT_REG_ADDR, &pattern);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_pattern = (uint8_t)(pattern & 0xFFu);
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_get_fields_since_sequence_reset(hcl_altera_vvp_tpg* d,
                                                              uint32_t* out_field_count) {
    if (d == NULL || out_field_count == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return read_reg(d, HCL_VVP_TPG_FIELD_COUNT_REG_ADDR, out_field_count);
}

hcl_status hcl_altera_vvp_tpg_set_bars_type(hcl_altera_vvp_tpg* d,
                                            hcl_altera_vvp_tpg_bars_type bar_type) {
    uint32_t reg_value;

    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    switch (bar_type) {
        case HCL_VVP_TPG_BARS_TYPE_COLOR:
            reg_value = HCL_VVP_TPG_COLOR_BARS;
            break;
        case HCL_VVP_TPG_BARS_TYPE_GREY:
            reg_value = HCL_VVP_TPG_GREY_BARS;
            break;
        case HCL_VVP_TPG_BARS_TYPE_BLACK_WHITE:
            reg_value = HCL_VVP_TPG_BLACK_WHITE_BARS;
            break;
        case HCL_VVP_TPG_BARS_TYPE_MIXED:
            reg_value = HCL_VVP_TPG_MIXED_BARS;
            break;
        default:
            return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return write_reg(d, HCL_VVP_TPG_BARS_SELECT_REG_ADDR, reg_value);
}

hcl_status hcl_altera_vvp_tpg_get_bars_type(hcl_altera_vvp_tpg* d,
                                            hcl_altera_vvp_tpg_bars_type* out_bar_type) {
    uint32_t bars;
    hcl_status s;

    if (d == NULL || out_bar_type == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_TPG_BARS_SELECT_REG_ADDR, &bars);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    switch (bars) {
        case HCL_VVP_TPG_COLOR_BARS:
            *out_bar_type = HCL_VVP_TPG_BARS_TYPE_COLOR;
            break;
        case HCL_VVP_TPG_GREY_BARS:
            *out_bar_type = HCL_VVP_TPG_BARS_TYPE_GREY;
            break;
        case HCL_VVP_TPG_BLACK_WHITE_BARS:
            *out_bar_type = HCL_VVP_TPG_BARS_TYPE_BLACK_WHITE;
            break;
        case HCL_VVP_TPG_MIXED_BARS:
            *out_bar_type = HCL_VVP_TPG_BARS_TYPE_MIXED;
            break;
        default:
            *out_bar_type = HCL_VVP_TPG_BARS_TYPE_INVALID;
            break;
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_set_colors(hcl_altera_vvp_tpg* d,
                                         uint16_t color1,
                                         uint16_t color2,
                                         uint16_t color3) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_C0_REG_ADDR, color1, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_C1_REG_ADDR, color2, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_C2_REG_ADDR, color3, 0u, HCL_STATUS_OK},
    };

    if (color1 > d->max_color_value || color2 > d->max_color_value || color3 > d->max_color_value) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_altera_vvp_tpg_get_colors(hcl_altera_vvp_tpg* d,
                                         uint16_t* out_color1,
                                         uint16_t* out_color2,
                                         uint16_t* out_color3) {
    if (d == NULL || out_color1 == NULL || out_color2 == NULL || out_color3 == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_C0_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_C1_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_C2_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
    };
    hcl_status s;

    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_color1 = (uint16_t)ops[0].read_value;
    *out_color2 = (uint16_t)ops[1].read_value;
    *out_color3 = (uint16_t)ops[2].read_value;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_set_zone_plate_origin(hcl_altera_vvp_tpg* d,
                                                    uint16_t x_coord,
                                                    uint16_t y_coord) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_ZONE_X_ORIGIN_REG_ADDR, x_coord, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_ZONE_Y_ORIGIN_REG_ADDR, y_coord, 0u, HCL_STATUS_OK},
    };

    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_altera_vvp_tpg_get_zone_plate_origin(hcl_altera_vvp_tpg* d,
                                                    uint16_t* out_x_coord,
                                                    uint16_t* out_y_coord) {
    if (d == NULL || out_x_coord == NULL || out_y_coord == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_ZONE_X_ORIGIN_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_ZONE_Y_ORIGIN_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
    };
    hcl_status s;

    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_x_coord = (uint16_t)ops[0].read_value;
    *out_y_coord = (uint16_t)ops[1].read_value;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_set_zone_plate_factors(hcl_altera_vvp_tpg* d,
                                                     uint16_t scaling_factor,
                                                     uint16_t fine_tune_factor) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_ZONE_SCALING_FACTOR_REG_ADDR, scaling_factor, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_ZONE_FINE_TUNE_FACTOR_REG_ADDR, fine_tune_factor, 0u, HCL_STATUS_OK},
    };

    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_altera_vvp_tpg_get_zone_plate_factors(hcl_altera_vvp_tpg* d,
                                                     uint16_t* out_scaling_factor,
                                                     uint16_t* out_fine_tune_factor) {
    if (d == NULL || out_scaling_factor == NULL || out_fine_tune_factor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_ZONE_SCALING_FACTOR_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_ZONE_FINE_TUNE_FACTOR_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
    };
    hcl_status s;

    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_scaling_factor = (uint16_t)ops[0].read_value;
    *out_fine_tune_factor = (uint16_t)ops[1].read_value;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_set_digital_clock_background_color(hcl_altera_vvp_tpg* d,
                                                                 uint16_t background_r,
                                                                 uint16_t background_g,
                                                                 uint16_t background_b) {
    return set_rgb_triplet(d,
                           HCL_VVP_TPG_CLOCK_B_BACKGROUND_REG_ADDR,
                           HCL_VVP_TPG_CLOCK_G_BACKGROUND_REG_ADDR,
                           HCL_VVP_TPG_CLOCK_R_BACKGROUND_REG_ADDR,
                           background_r,
                           background_g,
                           background_b);
}

hcl_status hcl_altera_vvp_tpg_get_digital_clock_background_color(hcl_altera_vvp_tpg* d,
                                                                 uint16_t* out_background_r,
                                                                 uint16_t* out_background_g,
                                                                 uint16_t* out_background_b) {
    return get_rgb_triplet(d,
                           HCL_VVP_TPG_CLOCK_B_BACKGROUND_REG_ADDR,
                           HCL_VVP_TPG_CLOCK_G_BACKGROUND_REG_ADDR,
                           HCL_VVP_TPG_CLOCK_R_BACKGROUND_REG_ADDR,
                           out_background_r,
                           out_background_g,
                           out_background_b);
}

hcl_status hcl_altera_vvp_tpg_set_digital_clock_font_color(hcl_altera_vvp_tpg* d,
                                                           uint16_t font_r,
                                                           uint16_t font_g,
                                                           uint16_t font_b) {
    return set_rgb_triplet(d,
                           HCL_VVP_TPG_CLOCK_B_FONT_REG_ADDR,
                           HCL_VVP_TPG_CLOCK_G_FONT_REG_ADDR,
                           HCL_VVP_TPG_CLOCK_R_FONT_REG_ADDR,
                           font_r,
                           font_g,
                           font_b);
}

hcl_status hcl_altera_vvp_tpg_get_digital_clock_font_color(hcl_altera_vvp_tpg* d,
                                                           uint16_t* out_font_r,
                                                           uint16_t* out_font_g,
                                                           uint16_t* out_font_b) {
    return get_rgb_triplet(d,
                           HCL_VVP_TPG_CLOCK_B_FONT_REG_ADDR,
                           HCL_VVP_TPG_CLOCK_G_FONT_REG_ADDR,
                           HCL_VVP_TPG_CLOCK_R_FONT_REG_ADDR,
                           out_font_r,
                           out_font_g,
                           out_font_b);
}

hcl_status hcl_altera_vvp_tpg_set_digital_clock_location(hcl_altera_vvp_tpg* d,
                                                         uint16_t location_x,
                                                         uint16_t location_y) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_CLOCK_LOCATION_X_REG_ADDR, location_x, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_VVP_TPG_CLOCK_LOCATION_Y_REG_ADDR, location_y, 0u, HCL_STATUS_OK},
    };

    return hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_altera_vvp_tpg_get_digital_clock_location(hcl_altera_vvp_tpg* d,
                                                         uint16_t* out_location_x,
                                                         uint16_t* out_location_y) {
    if (d == NULL || out_location_x == NULL || out_location_y == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_CLOCK_LOCATION_X_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_TPG_CLOCK_LOCATION_Y_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
    };
    hcl_status s;

    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_location_x = (uint16_t)ops[0].read_value;
    *out_location_y = (uint16_t)ops[1].read_value;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_set_digital_clock_scale_factor(hcl_altera_vvp_tpg* d,
                                                             uint16_t scale_factor) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (scale_factor > 0x07FFu) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return write_reg(d, HCL_VVP_TPG_CLOCK_SCALE_FACTOR_REG_ADDR, scale_factor);
}

hcl_status hcl_altera_vvp_tpg_get_digital_clock_scale_factor(hcl_altera_vvp_tpg* d,
                                                             uint16_t* out_scale_factor) {
    uint32_t value;
    hcl_status s;

    if (d == NULL || out_scale_factor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_TPG_CLOCK_SCALE_FACTOR_REG_ADDR, &value);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_scale_factor = (uint16_t)value;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_set_digital_clock_fps(hcl_altera_vvp_tpg* d,
                                                    uint8_t fps) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (fps > 0x3Fu) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return write_reg(d, HCL_VVP_TPG_CLOCK_FPS_REG_ADDR, fps);
}

hcl_status hcl_altera_vvp_tpg_get_digital_clock_fps(hcl_altera_vvp_tpg* d,
                                                    uint8_t* out_fps) {
    uint32_t value;
    hcl_status s;

    if (d == NULL || out_fps == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_TPG_CLOCK_FPS_REG_ADDR, &value);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_fps = (uint8_t)(value & 0xFFu);
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_tpg_set_signaltap_background_color(hcl_altera_vvp_tpg* d,
                                                             uint16_t background_r,
                                                             uint16_t background_g,
                                                             uint16_t background_b) {
    return hcl_altera_vvp_tpg_set_digital_clock_background_color(d,
                                                                  background_r,
                                                                  background_g,
                                                                  background_b);
}

hcl_status hcl_altera_vvp_tpg_get_signaltap_background_color(hcl_altera_vvp_tpg* d,
                                                             uint16_t* out_background_r,
                                                             uint16_t* out_background_g,
                                                             uint16_t* out_background_b) {
    return hcl_altera_vvp_tpg_get_digital_clock_background_color(d,
                                                                 out_background_r,
                                                                 out_background_g,
                                                                 out_background_b);
}

hcl_status hcl_altera_vvp_tpg_set_signaltap_font_color(hcl_altera_vvp_tpg* d,
                                                       uint16_t font_r,
                                                       uint16_t font_g,
                                                       uint16_t font_b) {
    return hcl_altera_vvp_tpg_set_digital_clock_font_color(d, font_r, font_g, font_b);
}

hcl_status hcl_altera_vvp_tpg_get_signaltap_font_color(hcl_altera_vvp_tpg* d,
                                                       uint16_t* out_font_r,
                                                       uint16_t* out_font_g,
                                                       uint16_t* out_font_b) {
    return hcl_altera_vvp_tpg_get_digital_clock_font_color(d, out_font_r, out_font_g, out_font_b);
}

hcl_status hcl_altera_vvp_tpg_set_signaltap_location(hcl_altera_vvp_tpg* d,
                                                     uint16_t location_x,
                                                     uint16_t location_y) {
    return hcl_altera_vvp_tpg_set_digital_clock_location(d, location_x, location_y);
}

hcl_status hcl_altera_vvp_tpg_get_signaltap_location(hcl_altera_vvp_tpg* d,
                                                     uint16_t* out_location_x,
                                                     uint16_t* out_location_y) {
    return hcl_altera_vvp_tpg_get_digital_clock_location(d, out_location_x, out_location_y);
}

hcl_status hcl_altera_vvp_tpg_set_signaltap_fps(hcl_altera_vvp_tpg* d,
                                                uint8_t fps) {
    return hcl_altera_vvp_tpg_set_digital_clock_fps(d, fps);
}

hcl_status hcl_altera_vvp_tpg_get_signaltap_fps(hcl_altera_vvp_tpg* d,
                                                uint8_t* out_fps) {
    return hcl_altera_vvp_tpg_get_digital_clock_fps(d, out_fps);
}

hcl_status hcl_altera_vvp_tpg_commit(hcl_altera_vvp_tpg* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_TPG_COMMIT_REG_ADDR, 1u);
}
