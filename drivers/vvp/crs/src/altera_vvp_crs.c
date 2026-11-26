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

#include "hcl/drivers/altera_vvp_crs.h"

#include "hcl/drivers/altera_vvp_core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct hcl_altera_vvp_crs {
    hcl_register_interface* reg;
    uint32_t base_address;
    hcl_altera_vvp_id id;
    hcl_altera_vvp_version version;
    bool lite_mode;
    bool debug_enabled;
    bool supported_modes[HCL_VVP_CRS_NUM_SUBSAMPLINGS][HCL_VVP_CRS_NUM_SUBSAMPLINGS];
};

static hcl_status write_reg(hcl_altera_vvp_crs* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_altera_vvp_crs* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, d->base_address, offset, out);
}

static bool is_valid_subsampling(hcl_altera_vvp_crs_subsampling subsampling) {
    return subsampling == HCL_VVP_CRS_SUBSAMPLING_MODE_420 ||
           subsampling == HCL_VVP_CRS_SUBSAMPLING_MODE_422 ||
           subsampling == HCL_VVP_CRS_SUBSAMPLING_MODE_444;
}

hcl_altera_vvp_crs* hcl_altera_vvp_crs_create(hcl_register_interface* reg,
                                              uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }

    hcl_altera_vvp_crs* d = (hcl_altera_vvp_crs*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }

    d->reg = reg;
    d->base_address = base_address;

    return d;
}

void hcl_altera_vvp_crs_destroy(hcl_altera_vvp_crs* driver) {
    free(driver);
}

hcl_status hcl_altera_vvp_crs_init(hcl_altera_vvp_crs* d) {
    uint8_t regmap_version;
    uint32_t s;
    uint32_t vid_pid = 0u;
    uint32_t version = 0u;
    hcl_altera_vvp_id parsed_id = {0u};
    hcl_altera_vvp_version parsed_version = {0u};
    hcl_status status;

    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_VID_PID_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_VERSION_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_LITE_MODE_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_DEBUG_ENABLED_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_444_444_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_444_422_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_444_420_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_422_444_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_422_422_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_422_420_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_420_444_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_420_422_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_VVP_CRS_ENABLE_420_420_REG_ADDR, 0u, 0u, HCL_STATUS_OK},
    };

    status = hcl_register_interface_execute(d->reg, ops, sizeof(ops) / sizeof(ops[0]));
    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    vid_pid = ops[0].read_value;
    version = ops[1].read_value;

    status = hcl_altera_vvp_core_parse_id(vid_pid, &parsed_id);
    if (status .code != HCL_STATUS_OK) {
        return status;
    }

    status = hcl_altera_vvp_core_parse_version(version, &parsed_version);
    if (status .code != HCL_STATUS_OK) {
        return status;
    }

    d->id = parsed_id;
    d->version = parsed_version;

    if (d->id.vendor_id != HCL_VVP_VENDOR_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }
    if (d->id.product_id != HCL_VVP_CRS_PRODUCT_ID) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }
    regmap_version = d->version.regmap_version;

    if (regmap_version < HCL_VVP_CRS_MIN_SUPPORTED_REGMAP_VERSION ||
        regmap_version > HCL_VVP_CRS_MAX_SUPPORTED_REGMAP_VERSION) {
        return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
    }

    d->lite_mode = (ops[2].read_value != 0u);
    d->debug_enabled = (ops[3].read_value != 0u);

    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_444][HCL_VVP_CRS_SUBSAMPLING_MODE_444] =
        (ops[4].read_value != 0u);
    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_444][HCL_VVP_CRS_SUBSAMPLING_MODE_422] =
        (ops[5].read_value != 0u);
    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_444][HCL_VVP_CRS_SUBSAMPLING_MODE_420] =
        (ops[6].read_value != 0u);

    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_422][HCL_VVP_CRS_SUBSAMPLING_MODE_444] =
        (ops[7].read_value != 0u);
    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_422][HCL_VVP_CRS_SUBSAMPLING_MODE_422] =
        (ops[8].read_value != 0u);
    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_422][HCL_VVP_CRS_SUBSAMPLING_MODE_420] =
        (ops[9].read_value != 0u);

    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_420][HCL_VVP_CRS_SUBSAMPLING_MODE_444] =
        (ops[10].read_value != 0u);
    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_420][HCL_VVP_CRS_SUBSAMPLING_MODE_422] =
        (ops[11].read_value != 0u);
    d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_420][HCL_VVP_CRS_SUBSAMPLING_MODE_420] =
        (ops[12].read_value != 0u);

    for (s = 0u; s < HCL_VVP_CRS_NUM_SUBSAMPLINGS; ++s) {
        d->supported_modes[s][HCL_VVP_CRS_SUBSAMPLING_MODE_INVALID] = false;
        d->supported_modes[HCL_VVP_CRS_SUBSAMPLING_MODE_INVALID][s] = false;
    }

    return hcl_status_ok();
}

bool hcl_altera_vvp_crs_get_lite_mode(const hcl_altera_vvp_crs* d) {
    return (d != NULL) && d->lite_mode;
}

bool hcl_altera_vvp_crs_get_debug_enabled(const hcl_altera_vvp_crs* d) {
    return (d != NULL) && d->debug_enabled;
}

uint32_t hcl_altera_vvp_crs_get_img_info_width(const hcl_altera_vvp_crs* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_crs*)d, HCL_VVP_CRS_IMG_INFO_WIDTH_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_crs_set_img_info_width(hcl_altera_vvp_crs* d, uint32_t width) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_IMG_INFO_WIDTH_REG_ADDR, width);
}

uint32_t hcl_altera_vvp_crs_get_img_info_height(const hcl_altera_vvp_crs* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_crs*)d, HCL_VVP_CRS_IMG_INFO_HEIGHT_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_crs_set_img_info_height(hcl_altera_vvp_crs* d, uint32_t height) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_IMG_INFO_HEIGHT_REG_ADDR, height);
}

uint32_t hcl_altera_vvp_crs_get_img_info_interlace(const hcl_altera_vvp_crs* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_crs*)d, HCL_VVP_CRS_IMG_INFO_INTERLACE_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_crs_set_img_info_interlace(hcl_altera_vvp_crs* d, uint32_t interlace) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_IMG_INFO_INTERLACE_REG_ADDR, interlace);
}

uint32_t hcl_altera_vvp_crs_get_img_info_colorspace(const hcl_altera_vvp_crs* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_crs*)d, HCL_VVP_CRS_IMG_INFO_COLORSPACE_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_crs_set_img_info_colorspace(hcl_altera_vvp_crs* d, uint32_t colorspace) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_IMG_INFO_COLORSPACE_REG_ADDR, colorspace);
}

uint32_t hcl_altera_vvp_crs_get_img_info_subsampling(const hcl_altera_vvp_crs* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_crs*)d, HCL_VVP_CRS_IMG_INFO_SUBSAMPLING_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_crs_set_img_info_subsampling(hcl_altera_vvp_crs* d, uint32_t subsampling) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_IMG_INFO_SUBSAMPLING_REG_ADDR, subsampling);
}

uint32_t hcl_altera_vvp_crs_get_img_info_cositing(const hcl_altera_vvp_crs* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_crs*)d, HCL_VVP_CRS_IMG_INFO_COSITING_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_crs_set_img_info_cositing(hcl_altera_vvp_crs* d, uint32_t cositing) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_IMG_INFO_COSITING_REG_ADDR, cositing);
}

uint32_t hcl_altera_vvp_crs_get_img_info_field_count(const hcl_altera_vvp_crs* d) {
    uint32_t out = 0u;
    if (d == NULL) {
        return 0u;
    }
    if (read_reg((hcl_altera_vvp_crs*)d, HCL_VVP_CRS_IMG_INFO_FIELD_COUNT_REG_ADDR, &out).code != HCL_STATUS_OK) {
        return 0u;
    }
    return out;
}

hcl_status hcl_altera_vvp_crs_set_img_info_field_count(hcl_altera_vvp_crs* d, uint32_t field_count) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_IMG_INFO_FIELD_COUNT_REG_ADDR, field_count);
}

hcl_status hcl_altera_vvp_crs_is_running(hcl_altera_vvp_crs* d, bool* out_running) {
    uint32_t status = 0u;
    hcl_status s;

    if (d == NULL || out_running == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_CRS_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_running = (status & HCL_VVP_CRS_STATUS_RUNNING_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_crs_commit_is_pending(hcl_altera_vvp_crs* d, bool* out_pending) {
    uint32_t status = 0u;
    hcl_status s;

    if (d == NULL || out_pending == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    s = read_reg(d, HCL_VVP_CRS_STATUS_REG_ADDR, &status);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    *out_pending = (status & HCL_VVP_CRS_STATUS_PENDING_COMMIT_MASK) != 0u;
    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_crs_get_status(hcl_altera_vvp_crs* d, uint32_t* out_status) {
    if (d == NULL || out_status == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return read_reg(d, HCL_VVP_CRS_STATUS_REG_ADDR, out_status);
}

bool hcl_altera_vvp_crs_is_conversion_supported(
    const hcl_altera_vvp_crs* d,
    hcl_altera_vvp_crs_subsampling input_subsampling,
    hcl_altera_vvp_crs_subsampling output_subsampling) {
    if (d == NULL) {
        return false;
    }

    if (!is_valid_subsampling(input_subsampling) || !is_valid_subsampling(output_subsampling)) {
        return false;
    }

    return d->supported_modes[input_subsampling][output_subsampling];
}

hcl_status hcl_altera_vvp_crs_get_output_subsampling(
    hcl_altera_vvp_crs* d,
    hcl_altera_vvp_crs_subsampling* out_output_subsampling) {
    uint32_t output_subsampling = 0u;
    hcl_status s;

    if (d == NULL || out_output_subsampling == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (!d->debug_enabled) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    s = read_reg(d, HCL_VVP_CRS_OUTPUT_MODE_REG_ADDR, &output_subsampling);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    switch ((uint8_t)output_subsampling) {
        case HCL_VVP_CRS_SUBSAMPLING_444:
            *out_output_subsampling = HCL_VVP_CRS_SUBSAMPLING_MODE_444;
            break;
        case HCL_VVP_CRS_SUBSAMPLING_422:
            *out_output_subsampling = HCL_VVP_CRS_SUBSAMPLING_MODE_422;
            break;
        case HCL_VVP_CRS_SUBSAMPLING_420:
            *out_output_subsampling = HCL_VVP_CRS_SUBSAMPLING_MODE_420;
            break;
        default:
            *out_output_subsampling = HCL_VVP_CRS_SUBSAMPLING_MODE_INVALID;
            break;
    }

    return hcl_status_ok();
}

hcl_status hcl_altera_vvp_crs_set_output_subsampling(
    hcl_altera_vvp_crs* d,
    hcl_altera_vvp_crs_subsampling output_subsampling) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (!is_valid_subsampling(output_subsampling)) {
        return hcl_status_error(HCL_STATUS_OUT_OF_RANGE, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_OUTPUT_MODE_REG_ADDR, (uint32_t)output_subsampling);
}

hcl_status hcl_altera_vvp_crs_commit(hcl_altera_vvp_crs* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_reg(d, HCL_VVP_CRS_COMMIT_REG_ADDR, 1u);
}
