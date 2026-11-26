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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "hcl/sensors/imx678.h"
#include "hcl/sensors/imx678_regs.h"
#include "hcl/sensors/imx678_modes.h"


#define IMX678_RETRY_ATTEMPTS 3u
#define IMX678_MAX_GAIN_DEC 240u
#define IMX678_MAX_GAIN_DB 72u
#define IMX678_GAIN_RATIO (IMX678_MAX_GAIN_DB * 10u / IMX678_MAX_GAIN_DEC)
#define IMX678_HIGH_GAIN_REG_MIN 10u
#define IMX678_MAX_CLEAR_HDR_GAIN 80u

struct hcl_imx678 {
    hcl_i2c* i2c;
    uint8_t address;
};

static hcl_status read_register(hcl_imx678* sensor,
                                uint16_t reg,
                                uint8_t* out_value) {
    hcl_status status = hcl_status_ok();
    uint32_t attempt = 0u;

    if (sensor == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    for (attempt = 0u; attempt < IMX678_RETRY_ATTEMPTS; ++attempt) {
        status = hcl_i2c_read_extended(sensor->i2c, sensor->address, reg, out_value);
        if (status.code == HCL_STATUS_OK) {
            return status;
        }

        hcl_delay_ms(100u);
    }

    return status;
}

static hcl_status write_register(hcl_imx678* sensor,
                                 uint16_t reg,
                                 uint8_t data) {
    hcl_status status = hcl_status_ok();
    uint32_t attempt = 0u;

    if (sensor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    for (attempt = 0u; attempt < IMX678_RETRY_ATTEMPTS; ++attempt) {
        status = hcl_i2c_write_extended(sensor->i2c, sensor->address, reg, data);
        if (status.code == HCL_STATUS_OK) {
            return status;
        }

        hcl_delay_ms(100u);
    }

    return status;
}

static hcl_status write_registers(hcl_imx678* sensor,
                                  const hcl_imx678_reg_value_t* registers,
                                  size_t register_count) {
    if (sensor == NULL || (registers == NULL && register_count != 0u)) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    for (size_t i = 0u; i < register_count; ++i) {
        const hcl_imx678_reg_value_t* entry = &registers[i];

        if (entry->reg == HCL_IMX678_TABLE_WAIT_MS) {
            hcl_delay_ms(entry->value);
            continue;
        }

        hcl_status status = write_register(sensor, entry->reg, entry->value);

        if (status.code != HCL_STATUS_OK) {
            return status;
        }
    }

    return hcl_status_ok();
}

hcl_imx678* hcl_imx678_create(hcl_i2c* i2c, uint8_t address) {
    if (i2c == NULL) {
        return NULL;
    }
    hcl_imx678* s = (hcl_imx678*)calloc(1u, sizeof(*s));
    if (s == NULL) {
        return NULL;
    }
    s->i2c = i2c;
    s->address = address;
    return s;
}

void hcl_imx678_destroy(hcl_imx678* sensor) {
    free(sensor);
}



hcl_status hcl_imx678_init(hcl_imx678* sensor,
                           const hcl_imx678_mode* mode) {
    if (sensor == NULL || mode == NULL ||
        (mode->registers == NULL && mode->register_count != 0u)) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_status s;

    s = hcl_imx678_stop(sensor);
    if (s.code != HCL_STATUS_OK) { return s; }

    // Write base settings
    s = write_registers(sensor,
                        imx678_init_setting,
                        sizeof(imx678_init_setting) / sizeof(imx678_init_setting[0]));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    // Write mode settings
    s = write_registers(sensor,
                        mode->registers,
                        mode->register_count);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    s = hcl_imx678_start(sensor);
    if (s.code != HCL_STATUS_OK) { return s; }

    uint8_t result = 0u;
    return read_register(sensor, HCL_IMX678_STANDBY, &result);
}

hcl_status hcl_imx678_start(hcl_imx678* sensor) {
    if (sensor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_registers(sensor,
                           imx678_start,
                           sizeof(imx678_start) / sizeof(imx678_start[0]));
}

hcl_status hcl_imx678_stop(hcl_imx678* sensor) {
    if (sensor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return write_registers(sensor,
                           imx678_stop,
                           sizeof(imx678_stop) / sizeof(imx678_stop[0]));
}

hcl_status hcl_imx678_set_gain(hcl_imx678* sensor,
                               uint32_t gain) {
    if (sensor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    uint32_t gain_reg = 0u;
    hcl_status s;

    gain_reg = gain * 10 / IMX678_GAIN_RATIO;

    s = write_register(sensor, HCL_IMX678_REGHOLD, 0x01u);
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    s = write_register(sensor, HCL_IMX678_GAIN_0_HIGH,
        (uint8_t)((gain_reg >> 8u) & 0xFFu));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    s = write_register(sensor, HCL_IMX678_GAIN_0_LOW,
        (uint8_t)(gain_reg & 0xFFu));
    if (s.code != HCL_STATUS_OK) {
        return s;
    }

    return write_register(sensor, HCL_IMX678_REGHOLD, 0x00u);
}

