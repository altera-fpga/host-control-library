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

#include "hcl/drivers/altera_vvp_csc.h"

#include "hcl/drivers/altera_vvp_core.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct hcl_altera_vvp_csc {
    hcl_register_interface* reg;
    uint32_t base_address;
    hcl_altera_vvp_id id;
    hcl_altera_vvp_version version;
    bool debug_enabled;
    bool lite_mode;
    uint8_t bps_in;
    uint8_t bps_out;
    uint8_t frac_bits;
    bool coeffs_signed;
    uint8_t coeffs_int_bits;
    bool summands_signed;
    uint8_t summands_int_bits;
    int8_t binary_point_right_move;
    hcl_altera_vvp_csc_rounding rounding_method;
};

static hcl_status write_reg(hcl_altera_vvp_csc* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_altera_vvp_csc* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, -d->base_address, offset, out);
}

static bool is_valid_rounding(uint32_t rounding) {
    return rounding == HCL_VVP_CSC_ROUND_UP ||
           rounding == HCL_VVP_CSC_ROUND_HALF_EVEN ||
           rounding == HCL_VVP_CSC_ROUND_TRUNC;
}

static bool is_valid_color_space(hcl_altera_vvp_csc_color_space output_color_space) {
    return output_color_space == HCL_VVP_CSC_COLOR_SPACE_RGB ||
           output_color_space == HCL_VVP_CSC_COLOR_SPACE_YCC ||
           output_color_space == HCL_VVP_CSC_COLOR_SPACE_YCCSD ||
           output_color_space == HCL_VVP_CSC_COLOR_SPACE_YCCHD ||
           output_color_space == HCL_VVP_CSC_COLOR_SPACE_MONO ||
           output_color_space == HCL_VVP_CSC_COLOR_SPACE_RAW ||
           output_color_space == HCL_VVP_CSC_COLOR_SPACE_RGB565;
}

static int32_t quantize_value(float value,
                              bool is_signed,
                              uint8_t int_bits,
                              uint8_t frac_bits,
                              bool* out_ok) {
    const float scale = (float)(1u << frac_bits);
    const float quantized_f = roundf(value * scale);
    int32_t quantized = (int32_t)quantized_f;
    int32_t min_q = 0;
    int32_t max_q = 0;

    if (is_signed) {
        const int32_t magnitude_bits = (int32_t)int_bits + (int32_t)frac_bits;
        min_q = -(1 << magnitude_bits);
        max_q = (1 << magnitude_bits) - 1;
    } else {
        const int32_t width = (int32_t)int_bits + (int32_t)frac_bits;
        min_q = 0;
        max_q = (1 << width) - 1;
    }

    if (quantized < min_q) {
        *out_ok = false;
        quantized = min_q;
    } else if (quantized > max_q) {
        *out_ok = false;
        quantized = max_q;
    }

    return quantized;
}

static int32_t decode_quantized(int32_t raw,
                                bool is_signed,
                                uint8_t int_bits,
                                uint8_t frac_bits) {
    const uint32_t width = (uint32_t)int_bits + (uint32_t)frac_bits + (is_signed ? 1u : 0u);
    uint32_t mask;
    uint32_t value;

    if (width == 0u || width >= 32u) {
        return raw;
    }

    mask = (1u << width) - 1u;
    value = ((uint32_t)raw) & mask;

    if (is_signed) {
        const uint32_t sign_bit = 1u << (width - 1u);
        if ((value & sign_bit) != 0u) {
            value |= ~mask;
        }
    }

    return (int32_t)value;
}

hcl_altera_vvp_csc* hcl_altera_vvp_csc_create(hcl_register_interface* reg,
                                              uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }

    hcl_altera_vvp_csc* d = (hcl_altera_vvp_csc*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }

    d->reg = reg;
    d->base_address = base_address;

    return d;
}

void hcl_altera_vvp_csc_destroy(hcl_altera_vvp_csc* driver) {
    free(driver);
}

hcl_status hcl_altera_vvp_csc_init(hcl_altera_vvp_csc* d) {
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
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_VID_PID_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_VERSION_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_LITE_MODE_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_DEBUG_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_BPS_IN_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_BPS_OUT_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_FRAC_BITS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_COEFFS_SIGNED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_COEFFS_INT_BITS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_SUMMANDS_SIGNED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_SUMMANDS_INT_BITS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_BINARY_POINT_RIGHT_MOVE_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CSC_ROUND_METHOD_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
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
    if (d->id.product_id != HCL_VVP_CSC_PRODUCT_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }
    regmap_version = d->version.regmap_version;
    if (regmap_version < HCL_VVP_CSC_MIN_SUPPORTED_REGMAP_VERSION ||
        regmap_version > HCL_VVP_CSC_MAX_SUPPORTED_REGMAP_VERSION) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->lite_mode = (ops[2].read_value != 0u);
    d->debug_enabled = (ops[3].read_value != 0u);
    d->bps_in = (uint8_t)(ops[4].read_value & 0xFFu);
    d->bps_out = (uint8_t)(ops[5].read_value & 0xFFu);
    d->frac_bits = (uint8_t)(ops[6].read_value & 0xFFu);
    d->coeffs_signed = (ops[7].read_value != 0u);
    d->coeffs_int_bits = (uint8_t)(ops[8].read_value & 0xFFu);
    d->summands_signed = (ops[9].read_value != 0u);
    d->summands_int_bits = (uint8_t)(ops[10].read_value & 0xFFu);
    d->binary_point_right_move = (int8_t)(ops[11].read_value & 0xFFu);

    if (!is_valid_rounding(ops[12].read_value)) {
        d->rounding_method = HCL_VVP_CSC_ROUNDING_INVALID;
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->rounding_method = (hcl_altera_vvp_csc_rounding)(int32_t)ops[12].read_value;

    if (((uint16_t)d->coeffs_int_bits + (uint16_t)d->frac_bits) > 31u ||
        ((uint16_t)d->summands_int_bits + (uint16_t)d->frac_bits) > 31u) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    return hcl_status_ok();
}

bool hcl_altera_vvp_csc_get_lite_mode(const hcl_altera_vvp_csc* d) {
    return (d != NULL) && d->lite_mode;
}

bool hcl_altera_vvp_csc_get_debug_enabled(const hcl_altera_vvp_csc* d) {
    return (d != NULL) && d->debug_enabled;
}

uint32_t hcl_altera_vvp_csc_get_img_info_width(const hcl_altera_vvp_csc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_csc*)d, HCL_VVP_CSC_IMG_INFO_WIDTH_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_csc_set_img_info_width(hcl_altera_vvp_csc* d, uint32_t width) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CSC_IMG_INFO_WIDTH_REG_ADDR, width);
}

uint32_t hcl_altera_vvp_csc_get_img_info_height(const hcl_altera_vvp_csc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_csc*)d, HCL_VVP_CSC_IMG_INFO_HEIGHT_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_csc_set_img_info_height(hcl_altera_vvp_csc* d, uint32_t height) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CSC_IMG_INFO_HEIGHT_REG_ADDR, height);
}

uint32_t hcl_altera_vvp_csc_get_img_info_interlace(const hcl_altera_vvp_csc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_csc*)d, HCL_VVP_CSC_IMG_INFO_INTERLACE_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_csc_set_img_info_interlace(hcl_altera_vvp_csc* d, uint32_t interlace) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CSC_IMG_INFO_INTERLACE_REG_ADDR, interlace);
}

uint32_t hcl_altera_vvp_csc_get_img_info_colorspace(const hcl_altera_vvp_csc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_csc*)d, HCL_VVP_CSC_IMG_INFO_COLORSPACE_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_csc_set_img_info_colorspace(hcl_altera_vvp_csc* d, uint32_t colorspace) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CSC_IMG_INFO_COLORSPACE_REG_ADDR, colorspace);
}

uint32_t hcl_altera_vvp_csc_get_img_info_subsampling(const hcl_altera_vvp_csc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_csc*)d, HCL_VVP_CSC_IMG_INFO_SUBSAMPLING_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_csc_set_img_info_subsampling(hcl_altera_vvp_csc* d, uint32_t subsampling) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CSC_IMG_INFO_SUBSAMPLING_REG_ADDR, subsampling);
}

uint32_t hcl_altera_vvp_csc_get_img_info_cositing(const hcl_altera_vvp_csc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_csc*)d, HCL_VVP_CSC_IMG_INFO_COSITING_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_csc_set_img_info_cositing(hcl_altera_vvp_csc* d, uint32_t cositing) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CSC_IMG_INFO_COSITING_REG_ADDR, cositing);
}

uint32_t hcl_altera_vvp_csc_get_img_info_field_count(const hcl_altera_vvp_csc* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_csc*)d, HCL_VVP_CSC_IMG_INFO_FIELD_COUNT_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

uint8_t hcl_altera_vvp_csc_get_bits_per_sample_in(const hcl_altera_vvp_csc* d) {
    return (d == NULL) ? 0u : d->bps_in;
}

uint8_t hcl_altera_vvp_csc_get_bits_per_sample_out(const hcl_altera_vvp_csc* d) {
    return (d == NULL) ? 0u : d->bps_out;
}

uint8_t hcl_altera_vvp_csc_get_coeffs_frac_bits(const hcl_altera_vvp_csc* d) {
    return (d == NULL) ? 0u : d->frac_bits;
}

bool hcl_altera_vvp_csc_are_coeffs_signed(const hcl_altera_vvp_csc* d) {
    return (d != NULL) && d->coeffs_signed;
}

uint8_t hcl_altera_vvp_csc_get_coeffs_int_bits(const hcl_altera_vvp_csc* d) {
    return (d == NULL) ? 0u : d->coeffs_int_bits;
}

bool hcl_altera_vvp_csc_are_summands_signed(const hcl_altera_vvp_csc* d) {
    return (d != NULL) && d->summands_signed;
}

uint8_t hcl_altera_vvp_csc_get_summands_int_bits(const hcl_altera_vvp_csc* d) {
    return (d == NULL) ? 0u : d->summands_int_bits;
}

int8_t hcl_altera_vvp_csc_get_binary_point_right_move(const hcl_altera_vvp_csc* d) {
    return (d == NULL) ? 0 : d->binary_point_right_move;
}

hcl_altera_vvp_csc_rounding hcl_altera_vvp_csc_get_rounding_method(const hcl_altera_vvp_csc* d) {
    return (d == NULL) ? HCL_VVP_CSC_ROUNDING_INVALID : d->rounding_method;
}

hcl_status hcl_altera_vvp_csc_is_running(hcl_altera_vvp_csc* d, bool* out_running) {
    uint32_t status = 0u;
    hcl_status s;

    if (d == NULL || out_running == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_CSC_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_running = (status & HCL_VVP_CSC_STATUS_RUNNING_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_csc_commit_is_pending(hcl_altera_vvp_csc* d, bool* out_pending) {
    uint32_t status = 0u;
    hcl_status s;

    if (d == NULL || out_pending == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    s = read_reg(d, HCL_VVP_CSC_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_pending = (status & HCL_VVP_CSC_STATUS_PENDING_COMMIT_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_csc_get_status(hcl_altera_vvp_csc* d, uint32_t* out_status) {
    if (d == NULL || out_status == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return read_reg(d, HCL_VVP_CSC_STATUS_REG_ADDR, out_status);
}

hcl_status hcl_altera_vvp_csc_set_quantized_coeff_data(
    hcl_altera_vvp_csc* d,
    const hcl_altera_vvp_csc_quantized_coefficients* quantized_coeffs) {
    uint16_t nb_bits;
    uint32_t sign_mask;
    uint32_t mask;
    uint32_t masked_value;
    uint8_t i;
    hcl_status write_status = hcl_status_ok();
    hcl_register_operation ops[HCL_VVP_CSC_NUM_COEFFS + HCL_VVP_CSC_NUM_SUMMANDS];

    if (d == NULL || quantized_coeffs == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    nb_bits = (uint16_t)d->coeffs_int_bits + (uint16_t)d->frac_bits;
    sign_mask = 1u << nb_bits;
    mask = ~(sign_mask - 1u);
    for (i = 0u; i < HCL_VVP_CSC_NUM_COEFFS; ++i) {
        masked_value = ((uint32_t)quantized_coeffs->coeffs[i]) & mask;
        if (masked_value != 0u &&
            (!d->coeffs_signed || (masked_value != sign_mask && masked_value != mask))) {
            write_status = hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
        }

        ops[i].operation = HCL_REGISTER_OP_WRITE32;
        ops[i].base_address = d->base_address;
        ops[i].offset = HCL_VVP_CSC_COEFF_REG_ADDR(i);
        ops[i].write_value = (uint32_t)quantized_coeffs->coeffs[i];
        ops[i].read_value = 0u;
        ops[i].status = HCL_STATUS_OK;
    }

    nb_bits = (uint16_t)d->summands_int_bits + (uint16_t)d->frac_bits;
    sign_mask = 1u << nb_bits;
    mask = ~(sign_mask - 1u);
    for (i = 0u; i < HCL_VVP_CSC_NUM_SUMMANDS; ++i) {
        masked_value = ((uint32_t)quantized_coeffs->summands[i]) & mask;
        if (masked_value != 0u &&
            (!d->summands_signed || (masked_value != sign_mask && masked_value != mask))) {
            write_status = hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
        }

        ops[HCL_VVP_CSC_NUM_COEFFS + i].operation = HCL_REGISTER_OP_WRITE32;
        ops[HCL_VVP_CSC_NUM_COEFFS + i].base_address = d->base_address;
        ops[HCL_VVP_CSC_NUM_COEFFS + i].offset = HCL_VVP_CSC_SUMMAND_REG_ADDR(i);
        ops[HCL_VVP_CSC_NUM_COEFFS + i].write_value = (uint32_t)quantized_coeffs->summands[i];
        ops[HCL_VVP_CSC_NUM_COEFFS + i].read_value = 0u;
        ops[HCL_VVP_CSC_NUM_COEFFS + i].status = HCL_STATUS_OK;
    }

    {
        hcl_status s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
        if (s.code != HCL_STATUS_OK) {
            return s;
        }
    }

    return write_status;
}

hcl_status hcl_altera_vvp_csc_get_quantized_coeff_data(
    hcl_altera_vvp_csc* d,
    hcl_altera_vvp_csc_quantized_coefficients* out_quantized_coeffs) {
    uint8_t i;
    hcl_register_operation ops[HCL_VVP_CSC_NUM_COEFFS + HCL_VVP_CSC_NUM_SUMMANDS];
    hcl_status s;

    if (d == NULL || out_quantized_coeffs == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    for (i = 0u; i < HCL_VVP_CSC_NUM_COEFFS; ++i) {
        ops[i].operation = HCL_REGISTER_OP_READ32;
        ops[i].base_address = d->base_address;
        ops[i].offset = HCL_VVP_CSC_COEFF_REG_ADDR(i);
        ops[i].write_value = 0u;
        ops[i].read_value = 0u;
        ops[i].status = HCL_STATUS_OK;
    }

    for (i = 0u; i < HCL_VVP_CSC_NUM_SUMMANDS; ++i) {
        ops[HCL_VVP_CSC_NUM_COEFFS + i].operation = HCL_REGISTER_OP_READ32;
        ops[HCL_VVP_CSC_NUM_COEFFS + i].base_address = d->base_address;
        ops[HCL_VVP_CSC_NUM_COEFFS + i].offset = HCL_VVP_CSC_SUMMAND_REG_ADDR(i);
        ops[HCL_VVP_CSC_NUM_COEFFS + i].write_value = 0u;
        ops[HCL_VVP_CSC_NUM_COEFFS + i].read_value = 0u;
        ops[HCL_VVP_CSC_NUM_COEFFS + i].status = HCL_STATUS_OK;
    }

    s = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    for (i = 0u; i < HCL_VVP_CSC_NUM_COEFFS; ++i) {
        out_quantized_coeffs->coeffs[i] = (int32_t)ops[i].read_value;
    }

    for (i = 0u; i < HCL_VVP_CSC_NUM_SUMMANDS; ++i) {
        out_quantized_coeffs->summands[i] = (int32_t)ops[HCL_VVP_CSC_NUM_COEFFS + i].read_value;
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_csc_set_coeff_data(
    hcl_altera_vvp_csc* d,
    const hcl_altera_vvp_csc_coefficients* coeffs,
    int32_t summand_rescale) {
    hcl_altera_vvp_csc_quantized_coefficients quantized = {0};
    uint8_t i;
    bool ok = true;
    hcl_status s;

    if (d == NULL || coeffs == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    for (i = 0u; i < HCL_VVP_CSC_NUM_COEFFS; ++i) {
        quantized.coeffs[i] = quantize_value(coeffs->coeffs[i],
                                             d->coeffs_signed,
                                             d->coeffs_int_bits,
                                             d->frac_bits,
                                             &ok);
    }

    for (i = 0u; i < HCL_VVP_CSC_NUM_SUMMANDS; ++i) {
        const float scaled_summand = ldexpf(coeffs->summands[i], summand_rescale);
        quantized.summands[i] = quantize_value(scaled_summand,
                                               d->summands_signed,
                                               d->summands_int_bits,
                                               d->frac_bits,
                                               &ok);
    }

    s = hcl_altera_vvp_csc_set_quantized_coeff_data(d, &quantized);
    if (s.code != HCL_STATUS_OK && s.code != HCL_STATUS_OUT_OF_RANGE) {
        return s;
    }

    if (!ok || s.code == HCL_STATUS_OUT_OF_RANGE) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_csc_get_coeff_data(
    hcl_altera_vvp_csc* d,
    hcl_altera_vvp_csc_coefficients* out_coeffs) {
    if (d == NULL || out_coeffs == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_altera_vvp_csc_quantized_coefficients quantized = {0};
    const float divider = 1.0f / (float)(1u << d->frac_bits);
    uint8_t i;
    hcl_status s;

    s = hcl_altera_vvp_csc_get_quantized_coeff_data(d, &quantized);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    for (i = 0u; i < HCL_VVP_CSC_NUM_COEFFS; ++i) {
        const int32_t decoded = decode_quantized(quantized.coeffs[i],
                                                 d->coeffs_signed,
                                                 d->coeffs_int_bits,
                                                 d->frac_bits);
        out_coeffs->coeffs[i] = (float)decoded * divider;
    }

    for (i = 0u; i < HCL_VVP_CSC_NUM_SUMMANDS; ++i) {
        const int32_t decoded = decode_quantized(quantized.summands[i],
                                                 d->summands_signed,
                                                 d->summands_int_bits,
                                                 d->frac_bits);
        out_coeffs->summands[i] = (float)decoded * divider;
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_csc_set_output_color_space(
    hcl_altera_vvp_csc* d,
    hcl_altera_vvp_csc_color_space output_color_space) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (d->lite_mode) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (!is_valid_color_space(output_color_space)) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return write_reg(d, HCL_VVP_CSC_OUTPUT_CS_REG_ADDR, (uint32_t)output_color_space);
}

hcl_status hcl_altera_vvp_csc_get_output_color_space(
    hcl_altera_vvp_csc* d,
    hcl_altera_vvp_csc_color_space* out_output_color_space) {
    uint32_t output_color_space = 0u;
    hcl_status s;

    if (d == NULL || out_output_color_space == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (d->lite_mode || !d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_CSC_OUTPUT_CS_REG_ADDR, &output_color_space);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    switch ((int32_t)output_color_space) {
        case HCL_VVP_CSC_COLOR_SPACE_RGB:
            *out_output_color_space = HCL_VVP_CSC_COLOR_SPACE_RGB;
            break;
        case HCL_VVP_CSC_COLOR_SPACE_YCC:
            *out_output_color_space = HCL_VVP_CSC_COLOR_SPACE_YCC;
            break;
        case HCL_VVP_CSC_COLOR_SPACE_YCCSD:
            *out_output_color_space = HCL_VVP_CSC_COLOR_SPACE_YCCSD;
            break;
        case HCL_VVP_CSC_COLOR_SPACE_YCCHD:
            *out_output_color_space = HCL_VVP_CSC_COLOR_SPACE_YCCHD;
            break;
        case HCL_VVP_CSC_COLOR_SPACE_MONO:
            *out_output_color_space = HCL_VVP_CSC_COLOR_SPACE_MONO;
            break;
        case HCL_VVP_CSC_COLOR_SPACE_RAW:
            *out_output_color_space = HCL_VVP_CSC_COLOR_SPACE_RAW;
            break;
        case HCL_VVP_CSC_COLOR_SPACE_RGB565:
            *out_output_color_space = HCL_VVP_CSC_COLOR_SPACE_RGB565;
            break;
        default:
            *out_output_color_space = HCL_VVP_CSC_COLOR_SPACE_INVALID;
            break;
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_csc_commit(hcl_altera_vvp_csc* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CSC_COMMIT_REG_ADDR, 1u);
}
