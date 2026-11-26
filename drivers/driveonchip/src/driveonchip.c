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

#include "hcl/drivers/driveonchip.h"

#include <stdlib.h>

struct hcl_driveonchip {
    hcl_register_interface* reg;
    double scale_factor;
};

static hcl_status read_register(const hcl_driveonchip* driver,
                                uint32_t register_index,
                                uint32_t* out_value) {
    if (driver == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return hcl_register_interface_read32(driver->reg, 0u, register_index, out_value);
}

static hcl_status write_register(const hcl_driveonchip* driver,
                                 uint32_t register_index,
                                 uint32_t value) {
    if (driver == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return hcl_register_interface_write32(driver->reg, 0u, register_index, value);
}

static int32_t twos_complement(uint32_t value) {
    if ((value & 0x80000000u) == 0u) {
        return (int32_t)value;
    }

    return -(int32_t)((~value + 1u) & 0xFFFFFFFFu);
}

hcl_driveonchip* hcl_driveonchip_create(hcl_register_interface* reg, double scale_factor) {
    hcl_driveonchip* driver;

    if (reg == NULL) {
        return NULL;
    }

    driver = (hcl_driveonchip*)calloc(1u, sizeof(*driver));
    if (driver == NULL) {
        return NULL;
    }

    driver->reg = reg;
    driver->scale_factor = scale_factor;
    return driver;
}

void hcl_driveonchip_destroy(hcl_driveonchip* driver) {
    free(driver);
}

hcl_status hcl_driveonchip_init(const hcl_driveonchip* driver) {
    if (driver == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return hcl_status_ok();
}

hcl_status hcl_driveonchip_set_speed(const hcl_driveonchip* driver, int32_t value) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_SPEED_SETP0, (uint32_t)value);
}

hcl_status hcl_driveonchip_set_and_get_speed(const hcl_driveonchip* driver,
                                             int32_t value,
                                             int32_t* out_value) {
    if (driver == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation operations[2u] = {
        {HCL_REGISTER_OP_WRITE32, 0u, HCL_DRIVEONCHIP_REG_SPEED_SETP0, (uint32_t)value, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_DRIVEONCHIP_REG_SPEED, 0u, 0u, HCL_STATUS_OK}
    };

    hcl_status status;
    uint32_t speed_value = 0u;

    status = hcl_register_interface_execute(driver->reg,
                                            operations,
                                            sizeof(operations) / sizeof(operations[0]));
    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    speed_value = operations[1].read_value;
    *out_value = twos_complement(speed_value);
    return hcl_status_ok();
}

hcl_status hcl_driveonchip_get_speed(const hcl_driveonchip* driver, int32_t* out_value) {
    uint32_t value = 0u;
    hcl_status status = read_register(driver, HCL_DRIVEONCHIP_REG_SPEED, &value);
    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    *out_value = twos_complement(value);
    return hcl_status_ok();
}

hcl_status hcl_driveonchip_get_speed_setpoint(const hcl_driveonchip* driver, int32_t* out_value) {
    uint32_t value = 0u;
    hcl_status status = read_register(driver, HCL_DRIVEONCHIP_REG_SPEED_SETP0, &value);
    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    *out_value = twos_complement(value);
    return hcl_status_ok();
}

hcl_status hcl_driveonchip_set_position(const hcl_driveonchip* driver, int32_t value) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_POS_SETP0, (uint32_t)value);
}

hcl_status hcl_driveonchip_set_and_get_position(const hcl_driveonchip* driver,
                                                int32_t value,
                                                double* out_value) {
    if (driver == NULL || out_value == NULL || driver->scale_factor == 0.0) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation operations[2u] = {
        {HCL_REGISTER_OP_WRITE32, 0u, HCL_DRIVEONCHIP_REG_POS_SETP0, (uint32_t)value, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, 0u, HCL_DRIVEONCHIP_REG_POSITION, 0u, 0u, HCL_STATUS_OK}
    };

    hcl_status status;
    uint32_t position_value = 0u;

    status = hcl_register_interface_execute(driver->reg,
                                            operations,
                                            sizeof(operations) / sizeof(operations[0]));

    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    position_value = operations[1].read_value;
    *out_value = (double)twos_complement(position_value) / driver->scale_factor;
    return hcl_status_ok();
}

hcl_status hcl_driveonchip_get_position(const hcl_driveonchip* driver, double* out_value) {
    if (driver == NULL || out_value == NULL || driver->scale_factor == 0.0) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    uint32_t value = 0u;
    hcl_status status;

    status = read_register(driver, HCL_DRIVEONCHIP_REG_POSITION, &value);
    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    *out_value = (double)twos_complement(value) / driver->scale_factor;
    return hcl_status_ok();
}

hcl_status hcl_driveonchip_get_position_setpoint(const hcl_driveonchip* driver, int32_t* out_value) {
    uint32_t value = 0u;
    hcl_status status = read_register(driver, HCL_DRIVEONCHIP_REG_POS_SETP0, &value);
    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    *out_value = twos_complement(value);
    return hcl_status_ok();
}

hcl_status hcl_driveonchip_set_position_speed_limit(const hcl_driveonchip* driver, int32_t value) {
    uint32_t magnitude;

    if (value < 0) {
        magnitude = (uint32_t)(-(int64_t)value);
    } else {
        magnitude = (uint32_t)value;
    }

    return write_register(driver, HCL_DRIVEONCHIP_REG_POS_SPEED_LIMIT, magnitude);
}

hcl_status hcl_driveonchip_get_position_speed_limit(const hcl_driveonchip* driver, uint32_t* out_value) {
    return read_register(driver, HCL_DRIVEONCHIP_REG_POS_SPEED_LIMIT, out_value);
}

hcl_status hcl_driveonchip_set_wave_demo_mode(const hcl_driveonchip* driver, uint32_t value) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_MODE, value);
}

hcl_status hcl_driveonchip_get_wave_demo_mode(const hcl_driveonchip* driver, uint32_t* out_value) {
    return read_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_MODE, out_value);
}

hcl_status hcl_driveonchip_set_wave_demo_waveform(const hcl_driveonchip* driver, uint32_t value) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_WAVEFORM, value);
}

hcl_status hcl_driveonchip_get_wave_demo_waveform(const hcl_driveonchip* driver, uint32_t* out_value) {
    return read_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_WAVEFORM, out_value);
}

hcl_status hcl_driveonchip_set_wave_demo_amplitude(const hcl_driveonchip* driver, int32_t value) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_AMP_F, (uint32_t)value);
}

hcl_status hcl_driveonchip_get_wave_demo_amplitude(const hcl_driveonchip* driver, int32_t* out_value) {
    uint32_t value = 0u;
    hcl_status status = read_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_AMP_F, &value);
    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    *out_value = twos_complement(value);
    return hcl_status_ok();
}

hcl_status hcl_driveonchip_set_wave_demo_period(const hcl_driveonchip* driver, uint32_t value) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_PERIOD, value);
}

hcl_status hcl_driveonchip_get_wave_demo_period(const hcl_driveonchip* driver, uint32_t* out_value) {
    return read_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_PERIOD, out_value);
}

hcl_status hcl_driveonchip_set_wave_demo_offset(const hcl_driveonchip* driver, int32_t value) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_OFFSET, (uint32_t)value);
}

hcl_status hcl_driveonchip_get_wave_demo_offset(const hcl_driveonchip* driver, int32_t* out_value) {
    uint32_t value = 0u;
    hcl_status status = read_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_OFFSET, &value);
    if (status.code != HCL_STATUS_OK) {
        return status;
    }

    *out_value = twos_complement(value);
    return hcl_status_ok();
}

hcl_status hcl_driveonchip_wave_demo_reset(const hcl_driveonchip* driver) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_BUTTON_DRIVE_RESET, 1u);
}

hcl_status hcl_driveonchip_wave_demo_update(const hcl_driveonchip* driver) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_WAVE_DEMO_UPDATE, 1u);
}

hcl_status hcl_driveonchip_set_dump_mode(const hcl_driveonchip* driver, uint32_t value) {
    return write_register(driver, HCL_DRIVEONCHIP_REG_DUMP_MODE, value);
}

hcl_status hcl_driveonchip_get_dump_mode(const hcl_driveonchip* driver, uint32_t* out_value) {
    return read_register(driver, HCL_DRIVEONCHIP_REG_DUMP_MODE, out_value);
}