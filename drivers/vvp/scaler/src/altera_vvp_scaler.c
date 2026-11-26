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

#include "hcl/drivers/altera_vvp_scaler.h"

#include "hcl/drivers/altera_vvp_core.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct hcl_altera_vvp_scaler_coeff_config {
    bool scaling_enabled;
    bool partial_scaling_enabled;
    uint8_t num_taps;
    uint16_t num_phases;
    uint8_t num_banks;
    bool coeffs_signed;
    uint8_t coeffs_int_bits;
    uint8_t coeffs_frac_bits;
} hcl_altera_vvp_scaler_coeff_config;

struct hcl_altera_vvp_scaler {
    hcl_register_interface* reg;
    uint32_t base_address;
    hcl_altera_vvp_id id;
    hcl_altera_vvp_version version;

    bool debug_enabled;
    bool lite_mode;
    uint8_t pixels_in_parallel;
    uint32_t max_input_width;
    uint32_t max_output_width;
    hcl_altera_vvp_scaler_algorithm scaling_algo;

    bool coeffs_runtime_load;
    bool coeffs_mem_init;

    hcl_altera_vvp_scaler_coeff_config v_coeffs;
    hcl_altera_vvp_scaler_coeff_config h_coeffs;
};

static hcl_status write_reg(hcl_altera_vvp_scaler* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_altera_vvp_scaler* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, d->base_address, offset, out);
}

static bool is_valid_scaling_algorithm(hcl_altera_vvp_scaler_algorithm algorithm) {
    return algorithm == HCL_VVP_SCALER_ALGORITHM_NEAREST_NEIGHBOR ||
           algorithm == HCL_VVP_SCALER_ALGORITHM_BILINEAR ||
           algorithm == HCL_VVP_SCALER_ALGORITHM_POLYPHASE;
}

static bool is_polyphase_scaling(const hcl_altera_vvp_scaler* d) {
    return d->scaling_algo == HCL_VVP_SCALER_ALGORITHM_POLYPHASE;
}

static hcl_status coeffs_config(
    const hcl_altera_vvp_scaler* d,
    bool h_coeffs,
    bool* out_coeffs_signed,
    uint8_t* out_coeffs_int_bits,
    uint8_t* out_coeffs_frac_bits,
    uint8_t* out_num_banks,
    uint16_t* out_num_phases,
    uint8_t* out_num_taps) {
    if (h_coeffs) {
        if (!d->h_coeffs.scaling_enabled) {
            return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
        }
        *out_coeffs_signed = d->h_coeffs.coeffs_signed;
        *out_coeffs_int_bits = d->h_coeffs.coeffs_int_bits;
        *out_coeffs_frac_bits = d->h_coeffs.coeffs_frac_bits;
        *out_num_banks = d->h_coeffs.num_banks;
        *out_num_phases = d->h_coeffs.num_phases;
        *out_num_taps = d->h_coeffs.num_taps;
    } else {
        if (!d->v_coeffs.scaling_enabled) {
            return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
        }
        *out_coeffs_signed = d->v_coeffs.coeffs_signed;
        *out_coeffs_int_bits = d->v_coeffs.coeffs_int_bits;
        *out_coeffs_frac_bits = d->v_coeffs.coeffs_frac_bits;
        *out_num_banks = d->v_coeffs.num_banks;
        *out_num_phases = d->v_coeffs.num_phases;
        *out_num_taps = d->v_coeffs.num_taps;
    }

    return hcl_status_ok();
}

static hcl_status check_set_coeffs(
    const hcl_altera_vvp_scaler* d,
    bool h_coeffs,
    uint8_t num_coeffs,
    uint8_t bank,
    uint8_t phase) {
    uint8_t num_banks = 0u;
    uint16_t num_phases = 0u;
    uint8_t num_taps = 0u;
    bool coeffs_signed = false;
    uint8_t coeffs_int_bits = 0u;
    uint8_t coeffs_frac_bits = 0u;
    hcl_status s;

    if (!d->coeffs_runtime_load) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = coeffs_config(d,
                      h_coeffs,
                      &coeffs_signed,
                      &coeffs_int_bits,
                      &coeffs_frac_bits,
                      &num_banks,
                      &num_phases,
                      &num_taps);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    if (bank >= num_banks || phase >= num_phases || num_coeffs != num_taps) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return hcl_status_ok();
}

static hcl_status write_coeffs(
    hcl_altera_vvp_scaler* d,
    bool h_coeffs,
    const int32_t* quantized_coeffs,
    uint8_t num_coeffs,
    uint8_t bank,
    uint8_t phase) {
    uint8_t c;
    size_t op_index = 0u;
    hcl_register_operation ops[HCL_VVP_SCALER_MAX_NUM_TAPS + 3u];

    for (c = 0u; c < num_coeffs; ++c) {
        ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
        ops[op_index].base_address = d->base_address;
        ops[op_index].offset = HCL_VVP_SCALER_RT_COEFF_LOAD_TAP_REG_ADDR(c);
        ops[op_index].write_value = (uint32_t)quantized_coeffs[c];
        ops[op_index].read_value = 0u;
        ops[op_index].status = HCL_STATUS_OK;
        ++op_index;
    }

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_RT_COEFF_LOAD_BANK_SELECT_REG_ADDR;
    ops[op_index].write_value = bank;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_RT_COEFF_LOAD_PHASE_SELECT_REG_ADDR;
    ops[op_index].write_value = phase;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_RT_COEFF_LOAD_COMMIT_REG_ADDR;
    ops[op_index].write_value = h_coeffs ? 1u : 0u;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    return hcl_register_interface_execute(d->reg, ops, op_index);
}

static int32_t quantize_coeff(float coeff,
                              bool coeffs_signed,
                              uint8_t coeffs_int_bits,
                              uint8_t coeffs_frac_bits,
                              bool* out_ok) {
    const float scale = (float)(1u << coeffs_frac_bits);
    const float quantized_f = roundf(coeff * scale);
    int32_t quantized = (int32_t)quantized_f;
    int32_t min_q = 0;
    int32_t max_q = 0;

    if (coeffs_signed) {
        const int32_t magnitude_bits = (int32_t)coeffs_int_bits + (int32_t)coeffs_frac_bits;
        min_q = -(1 << magnitude_bits);
        max_q = (1 << magnitude_bits) - 1;
    } else {
        const int32_t width = (int32_t)coeffs_int_bits + (int32_t)coeffs_frac_bits;
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

hcl_altera_vvp_scaler* hcl_altera_vvp_scaler_create(hcl_register_interface* reg,
                                                    uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }

    hcl_altera_vvp_scaler* d = (hcl_altera_vvp_scaler*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }

    d->reg = reg;
    d->base_address = base_address;

    return d;
}

void hcl_altera_vvp_scaler_destroy(hcl_altera_vvp_scaler* driver) {
    free(driver);
}

hcl_status hcl_altera_vvp_scaler_init(hcl_altera_vvp_scaler* d) {
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
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_VID_PID_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_VERSION_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_LITE_MODE_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_DEBUG_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_PIXELS_IN_PARALLEL_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_MAX_INPUT_WIDTH_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_MAX_OUTPUT_WIDTH_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_ALGORITHM_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_COEFFS_RUNTIME_LOAD_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_COEFFS_MEM_INIT_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_V_SCALING_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_V_PARTIAL_SCALING_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_V_NUM_TAPS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_V_NUM_PHASES_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_V_NUM_BANKS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_V_COEFFS_SIGNED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_V_COEFFS_INT_BITS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_V_COEFFS_FRAC_BITS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_H_SCALING_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_H_PARTIAL_SCALING_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_H_NUM_TAPS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_H_NUM_PHASES_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_H_NUM_BANKS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_H_COEFFS_SIGNED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_H_COEFFS_INT_BITS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_SCALER_H_COEFFS_FRAC_BITS_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
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
    if (d->id.product_id != HCL_VVP_SCALER_PRODUCT_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }
    regmap_version = d->version.regmap_version;
    if (regmap_version < HCL_VVP_SCALER_MIN_SUPPORTED_REGMAP_VERSION ||
        regmap_version > HCL_VVP_SCALER_MAX_SUPPORTED_REGMAP_VERSION) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->lite_mode = (ops[2].read_value != 0u);
    d->debug_enabled = (ops[3].read_value != 0u);
    d->pixels_in_parallel = (uint8_t)(ops[4].read_value & 0xFFu);
    d->max_input_width = ops[5].read_value;
    d->max_output_width = ops[6].read_value;
    d->scaling_algo = (hcl_altera_vvp_scaler_algorithm)(ops[7].read_value & 0xFFu);

    if (!is_valid_scaling_algorithm(d->scaling_algo)) {
        d->scaling_algo = HCL_VVP_SCALER_ALGORITHM_INVALID;
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->coeffs_runtime_load = (ops[8].read_value != 0u);
    d->coeffs_mem_init = (ops[9].read_value != 0u);

    d->v_coeffs.scaling_enabled = (ops[10].read_value != 0u);
    d->v_coeffs.partial_scaling_enabled = (ops[11].read_value != 0u);
    d->v_coeffs.num_taps = (uint8_t)(ops[12].read_value & 0xFFu);
    d->v_coeffs.num_phases = (uint16_t)(ops[13].read_value & 0xFFFFu);
    d->v_coeffs.num_banks = (uint8_t)(ops[14].read_value & 0xFFu);
    d->v_coeffs.coeffs_signed = (ops[15].read_value != 0u);
    d->v_coeffs.coeffs_int_bits = (uint8_t)(ops[16].read_value & 0xFFu);
    d->v_coeffs.coeffs_frac_bits = (uint8_t)(ops[17].read_value & 0xFFu);

    d->h_coeffs.scaling_enabled = (ops[18].read_value != 0u);
    d->h_coeffs.partial_scaling_enabled = (ops[19].read_value != 0u);
    d->h_coeffs.num_taps = (uint8_t)(ops[18].read_value & 0xFFu);
    d->h_coeffs.num_phases = (uint16_t)(ops[19].read_value & 0xFFFFu);
    d->h_coeffs.num_banks = (uint8_t)(ops[20].read_value & 0xFFu);
    d->h_coeffs.coeffs_signed = (ops[21].read_value != 0u);
    d->h_coeffs.coeffs_int_bits = (uint8_t)(ops[22].read_value & 0xFFu);
    d->h_coeffs.coeffs_frac_bits = (uint8_t)(ops[23].read_value & 0xFFu);

    if (is_polyphase_scaling(d)) {
        if ((d->v_coeffs.scaling_enabled &&
                    (d->v_coeffs.num_taps > HCL_VVP_SCALER_MAX_NUM_TAPS || d->v_coeffs.num_taps < 2u ||
                    ((uint16_t)d->v_coeffs.coeffs_int_bits + (uint16_t)d->v_coeffs.coeffs_frac_bits) > HCL_VVP_SCALER_MAX_COEFF_WIDTH)) ||
                (d->h_coeffs.scaling_enabled &&
                    (d->h_coeffs.num_taps > HCL_VVP_SCALER_MAX_NUM_TAPS || d->h_coeffs.num_taps < 2u ||
                    ((uint16_t)d->h_coeffs.coeffs_int_bits + (uint16_t)d->h_coeffs.coeffs_frac_bits) > HCL_VVP_SCALER_MAX_COEFF_WIDTH))) {
            return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
        }
    }

    return hcl_status_ok();
}

bool hcl_altera_vvp_scaler_get_lite_mode(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->lite_mode;
}

bool hcl_altera_vvp_scaler_get_debug_enabled(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->debug_enabled;
}

uint8_t hcl_altera_vvp_scaler_get_pixels_in_parallel(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->pixels_in_parallel;
}

uint32_t hcl_altera_vvp_scaler_get_max_input_width(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->max_input_width;
}

uint32_t hcl_altera_vvp_scaler_get_max_output_width(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->max_output_width;
}

uint32_t hcl_altera_vvp_scaler_get_img_info_width(const hcl_altera_vvp_scaler* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_scaler*)d, HCL_VVP_SCALER_IMG_INFO_WIDTH_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_scaler_set_img_info_width(hcl_altera_vvp_scaler* d, uint32_t width) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_IMG_INFO_WIDTH_REG_ADDR, width);
}

uint32_t hcl_altera_vvp_scaler_get_img_info_height(const hcl_altera_vvp_scaler* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_scaler*)d, HCL_VVP_SCALER_IMG_INFO_HEIGHT_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_scaler_set_img_info_height(hcl_altera_vvp_scaler* d, uint32_t height) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_IMG_INFO_HEIGHT_REG_ADDR, height);
}

uint32_t hcl_altera_vvp_scaler_get_img_info_interlace(const hcl_altera_vvp_scaler* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_scaler*)d, HCL_VVP_SCALER_IMG_INFO_INTERLACE_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_scaler_set_img_info_interlace(hcl_altera_vvp_scaler* d, uint32_t interlace) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_IMG_INFO_INTERLACE_REG_ADDR, interlace);
}

uint32_t hcl_altera_vvp_scaler_get_img_info_colorspace(const hcl_altera_vvp_scaler* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_scaler*)d, HCL_VVP_SCALER_IMG_INFO_COLORSPACE_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_scaler_set_img_info_colorspace(hcl_altera_vvp_scaler* d, uint32_t colorspace) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_IMG_INFO_COLORSPACE_REG_ADDR, colorspace);
}

uint32_t hcl_altera_vvp_scaler_get_img_info_subsampling(const hcl_altera_vvp_scaler* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_scaler*)d, HCL_VVP_SCALER_IMG_INFO_SUBSAMPLING_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_scaler_set_img_info_subsampling(hcl_altera_vvp_scaler* d, uint32_t subsampling) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_IMG_INFO_SUBSAMPLING_REG_ADDR, subsampling);
}

uint32_t hcl_altera_vvp_scaler_get_img_info_cositing(const hcl_altera_vvp_scaler* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_scaler*)d, HCL_VVP_SCALER_IMG_INFO_COSITING_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_scaler_set_img_info_cositing(hcl_altera_vvp_scaler* d, uint32_t cositing) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_IMG_INFO_COSITING_REG_ADDR, cositing);
}

uint32_t hcl_altera_vvp_scaler_get_img_info_field_count(const hcl_altera_vvp_scaler* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_scaler*)d, HCL_VVP_SCALER_IMG_INFO_FIELD_COUNT_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_altera_vvp_scaler_algorithm hcl_altera_vvp_scaler_get_scaling_algorithm(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? HCL_VVP_SCALER_ALGORITHM_INVALID : d->scaling_algo;
}

bool hcl_altera_vvp_scaler_is_coeffs_loading_enabled(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->coeffs_runtime_load;
}

bool hcl_altera_vvp_scaler_are_coeffs_preinitialized(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->coeffs_mem_init;
}

bool hcl_altera_vvp_scaler_is_vertical_scaling_enabled(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->v_coeffs.scaling_enabled;
}

bool hcl_altera_vvp_scaler_is_partial_vertical_scaling_enabled(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->v_coeffs.partial_scaling_enabled;
}

uint8_t hcl_altera_vvp_scaler_get_num_vertical_taps(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->v_coeffs.num_taps;
}

uint16_t hcl_altera_vvp_scaler_get_num_vertical_phases(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->v_coeffs.num_phases;
}

uint8_t hcl_altera_vvp_scaler_get_num_vertical_banks(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->v_coeffs.num_banks;
}

bool hcl_altera_vvp_scaler_are_vertical_coeffs_signed(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->v_coeffs.coeffs_signed;
}

uint8_t hcl_altera_vvp_scaler_get_vertical_coeffs_int_bits(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->v_coeffs.coeffs_int_bits;
}

uint8_t hcl_altera_vvp_scaler_get_vertical_coeffs_frac_bits(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->v_coeffs.coeffs_frac_bits;
}

bool hcl_altera_vvp_scaler_is_horizontal_scaling_enabled(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->h_coeffs.scaling_enabled;
}

bool hcl_altera_vvp_scaler_is_partial_horizontal_scaling_enabled(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->h_coeffs.partial_scaling_enabled;
}

uint8_t hcl_altera_vvp_scaler_get_num_horizontal_taps(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->h_coeffs.num_taps;
}

uint16_t hcl_altera_vvp_scaler_get_num_horizontal_phases(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->h_coeffs.num_phases;
}

uint8_t hcl_altera_vvp_scaler_get_num_horizontal_banks(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->h_coeffs.num_banks;
}

bool hcl_altera_vvp_scaler_are_horizontal_coeffs_signed(const hcl_altera_vvp_scaler* d) {
    return (d != NULL) && d->h_coeffs.coeffs_signed;
}

uint8_t hcl_altera_vvp_scaler_get_horizontal_coeffs_int_bits(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->h_coeffs.coeffs_int_bits;
}

uint8_t hcl_altera_vvp_scaler_get_horizontal_coeffs_frac_bits(const hcl_altera_vvp_scaler* d) {
    return (d == NULL) ? 0u : d->h_coeffs.coeffs_frac_bits;
}

hcl_status hcl_altera_vvp_scaler_is_running(hcl_altera_vvp_scaler* d, bool* out_running) {
    uint32_t status;
    hcl_status s;

    if (d == NULL || out_running == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_SCALER_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_running = (status & HCL_VVP_SCALER_STATUS_RUNNING_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_scaler_commit_is_pending(hcl_altera_vvp_scaler* d, bool* out_pending) {
    uint32_t status;
    hcl_status s;

    if (d == NULL || out_pending == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    s = read_reg(d, HCL_VVP_SCALER_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_pending = (status & HCL_VVP_SCALER_STATUS_PENDING_COMMIT_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_scaler_get_status(hcl_altera_vvp_scaler* d, uint32_t* out_status) {
    if (d == NULL || out_status == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_VVP_SCALER_STATUS_REG_ADDR, out_status);
}

hcl_status hcl_altera_vvp_scaler_set_h_bank(hcl_altera_vvp_scaler* d, uint8_t bank) {
    if (d == NULL || !is_polyphase_scaling(d) || !d->h_coeffs.scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (bank >= d->h_coeffs.num_banks) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_H_BANK_REG_ADDR, bank);
}

hcl_status hcl_altera_vvp_scaler_set_v_bank(hcl_altera_vvp_scaler* d, uint8_t bank) {
    if (d == NULL || !is_polyphase_scaling(d) || !d->v_coeffs.scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (bank >= d->v_coeffs.num_banks) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_V_BANK_REG_ADDR, bank);
}

hcl_status hcl_altera_vvp_scaler_set_output_width(hcl_altera_vvp_scaler* d, uint32_t output_width) {
    if (d == NULL || !d->h_coeffs.scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (output_width == 0u || output_width > d->max_output_width) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_OUTPUT_WIDTH_REG_ADDR, output_width);
}

hcl_status hcl_altera_vvp_scaler_set_output_height(hcl_altera_vvp_scaler* d, uint32_t output_height) {
    if (d == NULL || !d->v_coeffs.scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (output_height == 0u) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_OUTPUT_HEIGHT_REG_ADDR, output_height);
}

hcl_status hcl_altera_vvp_scaler_set_total_input_width(hcl_altera_vvp_scaler* d, uint32_t total_input_width) {
    if (d == NULL || !d->h_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (total_input_width == 0u) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_TOTAL_IN_WIDTH_REG_ADDR, total_input_width);
}

hcl_status hcl_altera_vvp_scaler_set_total_output_width(hcl_altera_vvp_scaler* d, uint32_t total_output_width) {
    if (d == NULL || !d->h_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (total_output_width == 0u) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_TOTAL_OUT_WIDTH_REG_ADDR, total_output_width);
}

hcl_status hcl_altera_vvp_scaler_set_total_input_height(hcl_altera_vvp_scaler* d, uint32_t total_input_height) {
    if (d == NULL || !d->v_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (total_input_height == 0u) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_TOTAL_IN_HEIGHT_REG_ADDR, total_input_height);
}

hcl_status hcl_altera_vvp_scaler_set_total_output_height(hcl_altera_vvp_scaler* d, uint32_t total_output_height) {
    if (d == NULL || !d->v_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (total_output_height == 0u) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_TOTAL_OUT_HEIGHT_REG_ADDR, total_output_height);
}

hcl_status hcl_altera_vvp_scaler_set_h_pos_error(hcl_altera_vvp_scaler* d, uint32_t h_pos_err) {
    if (d == NULL || !d->h_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_H_POS_ERROR_REG_ADDR, h_pos_err);
}

hcl_status hcl_altera_vvp_scaler_set_v_pos_error(hcl_altera_vvp_scaler* d, uint32_t v_pos_err) {
    if (d == NULL || !d->v_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_V_POS_ERROR_REG_ADDR, v_pos_err);
}

hcl_status hcl_altera_vvp_scaler_set_h_extended_fill(hcl_altera_vvp_scaler* d, bool extended_fill) {
    if (d == NULL || !d->h_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_H_EXTENDED_FILL_REG_ADDR, extended_fill ? 1u : 0u);
}

hcl_status hcl_altera_vvp_scaler_set_v_extended_fill(hcl_altera_vvp_scaler* d, bool extended_fill) {
    if (d == NULL || !d->v_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    return write_reg(d, HCL_VVP_SCALER_V_EXTENDED_FILL_REG_ADDR, extended_fill ? 1u : 0u);
}

hcl_status hcl_altera_vvp_scaler_set_h_phase_offset_error(
    hcl_altera_vvp_scaler* d,
    uint32_t phase_offset,
    uint32_t phase_error) {
    hcl_status s;

    if (d == NULL || !d->h_coeffs.partial_scaling_enabled || !is_polyphase_scaling(d)) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = write_reg(d, HCL_VVP_SCALER_H_PHASE_OFFSET_REG_ADDR, phase_offset);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return write_reg(d, HCL_VVP_SCALER_H_PHASE_ERROR_REG_ADDR, phase_error);
}

hcl_status hcl_altera_vvp_scaler_set_v_phase_offset_error(
    hcl_altera_vvp_scaler* d,
    uint32_t phase_offset,
    uint32_t phase_error) {
    hcl_status s;

    if (d == NULL || !d->v_coeffs.partial_scaling_enabled || !is_polyphase_scaling(d)) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = write_reg(d, HCL_VVP_SCALER_V_PHASE_OFFSET_REG_ADDR, phase_offset);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return write_reg(d, HCL_VVP_SCALER_V_PHASE_ERROR_REG_ADDR, phase_error);
}

hcl_status hcl_altera_vvp_scaler_set_split_h_scaling(
    hcl_altera_vvp_scaler* d,
    uint32_t total_input_width,
    uint32_t total_output_width,
    uint32_t split_output_width,
    uint32_t split_output_left_offset,
    bool extended_fill,
    uint32_t* split_input_width,
    uint32_t* split_input_left_offset) {
    uint32_t left_window = 0u;
    uint32_t right_window = 0u;
    uint32_t input_error;
    uint32_t input_phase;
    uint32_t input_phase_error;
    uint32_t last_output_pix;
    uint32_t input_left_offset;
    uint32_t last_input_pix;
    uint32_t input_width;
    uint32_t total_split_input_width;
    uint8_t pip;
    size_t op_index = 0u;
    hcl_register_operation ops[7u];

    if (d == NULL || !d->h_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (total_input_width == 0u || total_output_width == 0u || split_output_width == 0u) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }
    if (split_output_width > d->max_output_width) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    last_output_pix = (split_output_width + split_output_left_offset) - 1u;
    if (last_output_pix >= total_output_width) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    input_left_offset = (split_output_left_offset * total_input_width) / total_output_width;
    last_input_pix = (last_output_pix * total_input_width) / total_output_width;
    input_width = (last_input_pix + 1u) - input_left_offset;

    if (extended_fill) {
        pip = d->pixels_in_parallel;
        left_window = (d->h_coeffs.num_taps - 1u) / 2u;
        if (pip != 0u) {
            left_window = ((left_window + pip - 1u) / pip) * pip;
        }
        if (input_left_offset < left_window) {
            left_window = 0u;
        }

        right_window = d->h_coeffs.num_taps / 2u;
        if ((input_left_offset + input_width + right_window) > total_input_width) {
            right_window = total_input_width - (input_left_offset + input_width);
        }
    }

    total_split_input_width = input_width + left_window + right_window;

    if (split_input_left_offset != NULL) {
        *split_input_left_offset = input_left_offset - left_window;
    }
    if (split_input_width != NULL) {
        *split_input_width = total_split_input_width;
    }

    input_error = (split_output_left_offset * total_input_width) % total_output_width;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_OUTPUT_WIDTH_REG_ADDR;
    ops[op_index].write_value = split_output_width;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_TOTAL_IN_WIDTH_REG_ADDR;
    ops[op_index].write_value = total_input_width;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_TOTAL_OUT_WIDTH_REG_ADDR;
    ops[op_index].write_value = total_output_width;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_H_POS_ERROR_REG_ADDR;
    ops[op_index].write_value = input_error;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_H_EXTENDED_FILL_REG_ADDR;
    ops[op_index].write_value = (left_window != 0u) ? 1u : 0u;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    if (is_polyphase_scaling(d)) {
        input_phase = (split_output_left_offset * total_input_width * d->h_coeffs.num_phases) / total_output_width;
        input_phase_error = (split_output_left_offset * total_input_width * d->h_coeffs.num_phases) % total_output_width;

        ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
        ops[op_index].base_address = d->base_address;
        ops[op_index].offset = HCL_VVP_SCALER_H_PHASE_OFFSET_REG_ADDR;
        ops[op_index].write_value = input_phase;
        ops[op_index].read_value = 0u;
        ops[op_index].status = HCL_STATUS_OK;
        ++op_index;

        ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
        ops[op_index].base_address = d->base_address;
        ops[op_index].offset = HCL_VVP_SCALER_H_PHASE_ERROR_REG_ADDR;
        ops[op_index].write_value = input_phase_error;
        ops[op_index].read_value = 0u;
        ops[op_index].status = HCL_STATUS_OK;
        ++op_index;
    }

    {
        hcl_status s = hcl_register_interface_execute(d->reg, ops, op_index);
        if (s.code != HCL_STATUS_OK) {
            return s;
        }
    }

    if (total_split_input_width > d->max_input_width) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_scaler_set_split_v_scaling(
    hcl_altera_vvp_scaler* d,
    uint32_t total_input_height,
    uint32_t total_output_height,
    uint32_t split_output_height,
    uint32_t split_output_top_offset,
    bool extended_fill,
    uint32_t* split_input_height,
    uint32_t* split_input_top_offset) {
    uint32_t top_window = 0u;
    uint32_t bottom_window = 0u;
    uint32_t input_error;
    uint32_t input_phase;
    uint32_t input_phase_error;
    uint32_t last_output_line;
    uint32_t last_input_line;
    uint32_t input_height;
    uint32_t input_top_offset;
    size_t op_index = 0u;
    hcl_register_operation ops[7u];

    if (d == NULL || !d->v_coeffs.partial_scaling_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    if (total_input_height == 0u || total_output_height == 0u || split_output_height == 0u) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    last_output_line = (split_output_height + split_output_top_offset) - 1u;
    if (last_output_line >= total_output_height) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    input_top_offset = (split_output_top_offset * total_input_height) / total_output_height;
    last_input_line = (last_output_line * total_input_height) / total_output_height;
    input_height = (last_input_line + 1u) - input_top_offset;

    if (extended_fill) {
        top_window = (d->v_coeffs.num_taps - 1u) / 2u;
        if (input_top_offset < top_window) {
            top_window = 0u;
        }

        bottom_window = d->v_coeffs.num_taps / 2u;
        if ((input_top_offset + input_height + bottom_window) > total_input_height) {
            bottom_window = total_input_height - (input_top_offset + input_height);
        }
    }

    if (split_input_top_offset != NULL) {
        *split_input_top_offset = input_top_offset - top_window;
    }
    if (split_input_height != NULL) {
        *split_input_height = input_height + top_window + bottom_window;
    }

    input_error = (split_output_top_offset * total_input_height) % total_output_height;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_OUTPUT_HEIGHT_REG_ADDR;
    ops[op_index].write_value = split_output_height;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_TOTAL_IN_HEIGHT_REG_ADDR;
    ops[op_index].write_value = total_input_height;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_TOTAL_OUT_HEIGHT_REG_ADDR;
    ops[op_index].write_value = total_output_height;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_V_POS_ERROR_REG_ADDR;
    ops[op_index].write_value = input_error;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
    ops[op_index].base_address = d->base_address;
    ops[op_index].offset = HCL_VVP_SCALER_V_EXTENDED_FILL_REG_ADDR;
    ops[op_index].write_value = (top_window != 0u) ? 1u : 0u;
    ops[op_index].read_value = 0u;
    ops[op_index].status = HCL_STATUS_OK;
    ++op_index;

    if (is_polyphase_scaling(d)) {
        input_phase = (split_output_top_offset * total_input_height * d->v_coeffs.num_phases) / total_output_height;
        input_phase_error = (split_output_top_offset * total_input_height * d->v_coeffs.num_phases) % total_output_height;

        ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
        ops[op_index].base_address = d->base_address;
        ops[op_index].offset = HCL_VVP_SCALER_V_PHASE_OFFSET_REG_ADDR;
        ops[op_index].write_value = input_phase;
        ops[op_index].read_value = 0u;
        ops[op_index].status = HCL_STATUS_OK;
        ++op_index;

        ops[op_index].operation = HCL_REGISTER_OP_WRITE32;
        ops[op_index].base_address = d->base_address;
        ops[op_index].offset = HCL_VVP_SCALER_V_PHASE_ERROR_REG_ADDR;
        ops[op_index].write_value = input_phase_error;
        ops[op_index].read_value = 0u;
        ops[op_index].status = HCL_STATUS_OK;
        ++op_index;
    }

    return hcl_register_interface_execute(d->reg, ops, op_index);
}

static hcl_status checked_read_u32(hcl_altera_vvp_scaler* d,
                                        bool condition,
                                        uint32_t reg_addr,
                                        uint32_t* out) {
    if (d == NULL || out == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!condition || !d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }
    return read_reg(d, reg_addr, out);
}

hcl_status hcl_altera_vvp_scaler_get_h_bank(hcl_altera_vvp_scaler* d, uint8_t* out_h_bank) {
    uint32_t out = 0u;
    hcl_status s = checked_read_u32(d,
                                         is_polyphase_scaling(d) && d->h_coeffs.scaling_enabled,
                                         HCL_VVP_SCALER_H_BANK_REG_ADDR,
                                         &out);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out_h_bank = (uint8_t)(out & 0xFFu);
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_scaler_get_v_bank(hcl_altera_vvp_scaler* d, uint8_t* out_v_bank) {
    uint32_t out = 0u;
    hcl_status s = checked_read_u32(d,
                                         is_polyphase_scaling(d) && d->v_coeffs.scaling_enabled,
                                         HCL_VVP_SCALER_V_BANK_REG_ADDR,
                                         &out);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out_v_bank = (uint8_t)(out & 0xFFu);
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_scaler_get_output_width(hcl_altera_vvp_scaler* d, uint32_t* out_output_width) {
    return checked_read_u32(d, d->h_coeffs.scaling_enabled, HCL_VVP_SCALER_OUTPUT_WIDTH_REG_ADDR, out_output_width);
}

hcl_status hcl_altera_vvp_scaler_get_output_height(hcl_altera_vvp_scaler* d, uint32_t* out_output_height) {
    return checked_read_u32(d, d->v_coeffs.scaling_enabled, HCL_VVP_SCALER_OUTPUT_HEIGHT_REG_ADDR, out_output_height);
}

hcl_status hcl_altera_vvp_scaler_get_total_input_width(hcl_altera_vvp_scaler* d, uint32_t* out_total_input_width) {
    return checked_read_u32(d, d->h_coeffs.partial_scaling_enabled, HCL_VVP_SCALER_TOTAL_IN_WIDTH_REG_ADDR, out_total_input_width);
}

hcl_status hcl_altera_vvp_scaler_get_total_output_width(hcl_altera_vvp_scaler* d, uint32_t* out_total_output_width) {
    return checked_read_u32(d, d->h_coeffs.partial_scaling_enabled, HCL_VVP_SCALER_TOTAL_OUT_WIDTH_REG_ADDR, out_total_output_width);
}

hcl_status hcl_altera_vvp_scaler_get_total_input_height(hcl_altera_vvp_scaler* d, uint32_t* out_total_input_height) {
    return checked_read_u32(d, d->v_coeffs.partial_scaling_enabled, HCL_VVP_SCALER_TOTAL_IN_HEIGHT_REG_ADDR, out_total_input_height);
}

hcl_status hcl_altera_vvp_scaler_get_total_output_height(hcl_altera_vvp_scaler* d, uint32_t* out_total_output_height) {
    return checked_read_u32(d, d->v_coeffs.partial_scaling_enabled, HCL_VVP_SCALER_TOTAL_OUT_HEIGHT_REG_ADDR, out_total_output_height);
}

hcl_status hcl_altera_vvp_scaler_get_h_pos_error(hcl_altera_vvp_scaler* d, uint32_t* out_h_pos_error) {
    return checked_read_u32(d, d->h_coeffs.partial_scaling_enabled, HCL_VVP_SCALER_H_POS_ERROR_REG_ADDR, out_h_pos_error);
}

hcl_status hcl_altera_vvp_scaler_get_v_pos_error(hcl_altera_vvp_scaler* d, uint32_t* out_v_pos_error) {
    return checked_read_u32(d, d->v_coeffs.partial_scaling_enabled, HCL_VVP_SCALER_V_POS_ERROR_REG_ADDR, out_v_pos_error);
}

hcl_status hcl_altera_vvp_scaler_get_h_extended_fill(hcl_altera_vvp_scaler* d, bool* out_extended_fill) {
    uint32_t out = 0u;
    hcl_status s;

    if (d == NULL || out_extended_fill == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = checked_read_u32(d,
                         d->h_coeffs.partial_scaling_enabled,
                         HCL_VVP_SCALER_H_EXTENDED_FILL_REG_ADDR,
                         &out);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out_extended_fill = (out != 0u);
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_scaler_get_v_extended_fill(hcl_altera_vvp_scaler* d, bool* out_extended_fill) {
    uint32_t out = 0u;
    hcl_status s;

    if (d == NULL || out_extended_fill == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = checked_read_u32(d,
                         d->v_coeffs.partial_scaling_enabled,
                         HCL_VVP_SCALER_V_EXTENDED_FILL_REG_ADDR,
                         &out);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    *out_extended_fill = (out != 0u);
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_scaler_get_h_phase_offset_error(hcl_altera_vvp_scaler* d,
                                                          uint32_t* out_phase_offset,
                                                          uint32_t* out_phase_error) {
    hcl_status s;

    if (d == NULL || out_phase_offset == NULL || out_phase_error == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->h_coeffs.partial_scaling_enabled || !is_polyphase_scaling(d) || !d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_SCALER_H_PHASE_OFFSET_REG_ADDR, out_phase_offset);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return read_reg(d, HCL_VVP_SCALER_H_PHASE_ERROR_REG_ADDR, out_phase_error);
}

hcl_status hcl_altera_vvp_scaler_get_v_phase_offset_error(hcl_altera_vvp_scaler* d,
                                                          uint32_t* out_phase_offset,
                                                          uint32_t* out_phase_error) {
    hcl_status s;

    if (d == NULL || out_phase_offset == NULL || out_phase_error == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (!d->v_coeffs.partial_scaling_enabled || !is_polyphase_scaling(d) || !d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_SCALER_V_PHASE_OFFSET_REG_ADDR, out_phase_offset);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }
    return read_reg(d, HCL_VVP_SCALER_V_PHASE_ERROR_REG_ADDR, out_phase_error);
}

hcl_status hcl_altera_vvp_scaler_set_quantized_coeffs(
    hcl_altera_vvp_scaler* d,
    bool h_coeffs,
    const int32_t* quantized_coeffs,
    uint8_t num_coeffs,
    uint8_t bank,
    uint8_t phase) {
    bool coeffs_signed;
    uint8_t coeffs_int_bits;
    uint8_t coeffs_frac_bits;
    uint8_t num_banks;
    uint16_t num_phases;
    uint8_t num_taps;
    uint16_t nb_bits;
    uint32_t sign_mask;
    uint32_t mask;
    uint32_t masked_coeff;
    uint8_t c;
    hcl_status s;
    hcl_status write_status = hcl_status_ok();
    hcl_status write_ret;

    if (d == NULL || quantized_coeffs == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = check_set_coeffs(d, h_coeffs, num_coeffs, bank, phase);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    s = coeffs_config(d,
                      h_coeffs,
                      &coeffs_signed,
                      &coeffs_int_bits,
                      &coeffs_frac_bits,
                      &num_banks,
                      &num_phases,
                      &num_taps);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    nb_bits = (uint16_t)coeffs_int_bits + (uint16_t)coeffs_frac_bits;
    if (nb_bits > 31u) {
        (void)write_coeffs(d, h_coeffs, quantized_coeffs, num_coeffs, bank, phase);
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    sign_mask = 1u << nb_bits;
    mask = ~(sign_mask - 1u);

    for (c = 0u; c < num_coeffs; ++c) {
        masked_coeff = ((uint32_t)quantized_coeffs[c]) & mask;
        if (masked_coeff != 0u &&
            (!coeffs_signed || (masked_coeff != sign_mask && masked_coeff != mask))) {
            write_status = hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
        }
    }

    write_ret = write_coeffs(d, h_coeffs, quantized_coeffs, num_coeffs, bank, phase);
    if (write_ret .code != HCL_STATUS_OK) {
        return write_ret;
    }

    return write_status;
}

hcl_status hcl_altera_vvp_scaler_set_coeffs(
    hcl_altera_vvp_scaler* d,
    bool h_coeffs,
    const float* coeffs,
    uint8_t num_coeffs,
    uint8_t bank,
    uint8_t phase) {
    bool coeffs_signed;
    uint8_t coeffs_int_bits;
    uint8_t coeffs_frac_bits;
    uint8_t num_banks;
    uint16_t num_phases;
    uint8_t num_taps;
    int32_t quantized_coeffs[HCL_VVP_SCALER_MAX_NUM_TAPS];
    uint8_t c;
    bool ok = true;
    hcl_status s;

    if (d == NULL || coeffs == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    if (num_coeffs > HCL_VVP_SCALER_MAX_NUM_TAPS) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    s = check_set_coeffs(d, h_coeffs, num_coeffs, bank, phase);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    s = coeffs_config(d,
                      h_coeffs,
                      &coeffs_signed,
                      &coeffs_int_bits,
                      &coeffs_frac_bits,
                      &num_banks,
                      &num_phases,
                      &num_taps);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    for (c = 0u; c < num_coeffs; ++c) {
        quantized_coeffs[c] = quantize_coeff(coeffs[c],
                                             coeffs_signed,
                                             coeffs_int_bits,
                                             coeffs_frac_bits,
                                             &ok);
        
        if (!ok) { return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL); }
    }

    return write_coeffs(d, h_coeffs, quantized_coeffs, num_coeffs, bank, phase);
}

hcl_status hcl_altera_vvp_scaler_commit(hcl_altera_vvp_scaler* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_SCALER_COMMIT_REG_ADDR, 1u);
}
