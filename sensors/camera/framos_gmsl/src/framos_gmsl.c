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

#include "hcl/sensors/framos_gmsl.h"

#include <stdint.h>
#include <stdlib.h>

#define HCL_FRAMOS_GMSL_INIT_DELAY 200u
#define HCL_FRAMOS_GMSL_RETRY_ATTEMPTS 3u

struct hcl_framos_gmsl {
    hcl_i2c* i2c;
    uint8_t serializer_address;
    uint8_t deserializer_address;
    uint8_t gpio_address;
    uint8_t sensor_address;
};

hcl_framos_gmsl* hcl_framos_gmsl_create(hcl_i2c* i2c,
                                        uint8_t serializer_address,
                                        uint8_t deserializer_address,
                                        uint8_t gpio_address,
                                        uint8_t sensor_address) {
    hcl_framos_gmsl* sensor = NULL;

    if (i2c == NULL) {
        return NULL;
    }

    sensor = (hcl_framos_gmsl*)calloc(1u, sizeof(*sensor));
    if (sensor == NULL) {
        return NULL;
    }

    sensor->i2c = i2c;
    sensor->serializer_address = serializer_address;
    sensor->deserializer_address = deserializer_address;
    sensor->gpio_address = gpio_address;
    sensor->sensor_address = sensor_address;
    return sensor;
}

void hcl_framos_gmsl_destroy(hcl_framos_gmsl* sensor) {
    free(sensor);
}

hcl_status hcl_framos_gmsl_read_register(hcl_framos_gmsl* sensor,
                                         uint8_t device_address,
                                         uint16_t reg,
                                         uint8_t* out_value) {
    hcl_status status = hcl_status_ok();
    uint32_t attempt = 0u;

    if (sensor == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    for (attempt = 0u; attempt < HCL_FRAMOS_GMSL_RETRY_ATTEMPTS; ++attempt) {
        status = hcl_i2c_read_extended(sensor->i2c, device_address, reg, out_value);
        if (status.code == HCL_STATUS_OK) {
            return status;
        }

        hcl_delay_ms(HCL_FRAMOS_GMSL_INIT_DELAY);
    }

    return status;
}

hcl_status hcl_framos_gmsl_write_register(hcl_framos_gmsl* sensor,
                                          uint8_t device_address,
                                          uint16_t reg,
                                          uint8_t data) {
    hcl_status status = hcl_status_ok();
    uint32_t attempt = 0u;

    if (sensor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    for (attempt = 0u; attempt < HCL_FRAMOS_GMSL_RETRY_ATTEMPTS; ++attempt) {
        status = hcl_i2c_write_extended(sensor->i2c, device_address, reg, data);
        if (status.code == HCL_STATUS_OK) {
            return status;
        }

        hcl_delay_ms(HCL_FRAMOS_GMSL_INIT_DELAY);
    }

    return status;
}

hcl_status hcl_framos_gmsl_write_gpio_register(hcl_framos_gmsl* sensor,
                                               uint8_t reg,
                                               uint8_t data) {
    hcl_status status = hcl_status_ok();
    uint32_t attempt = 0u;

    if (sensor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    for (attempt = 0u; attempt < HCL_FRAMOS_GMSL_RETRY_ATTEMPTS; ++attempt) {
        status = hcl_i2c_write(sensor->i2c, sensor->gpio_address, reg, data);
        if (status.code == HCL_STATUS_OK) {
            return status;
        }

        hcl_delay_ms(HCL_FRAMOS_GMSL_INIT_DELAY);
    }

    return status;
}

hcl_status hcl_framos_gmsl_init(hcl_framos_gmsl* sensor) {
    typedef struct hcl_framos_gmsl_register_write {
        uint16_t reg;
        uint8_t value;
    } hcl_framos_gmsl_register_write;

    static const hcl_framos_gmsl_register_write deserializer_init[] = {
        {0x0001u, 0x03u},
        {0x0004u, 0xC3u},
        {0x0028u, 0x62u},
        {0x2001u, 0x01u},
        {0x2101u, 0x01u},
        {0x0443u, 0x81u},
        {0x0444u, 0x81u},
    };
    static const hcl_framos_gmsl_register_write serializer_init[] = {
        {0x14CEu, 0x19u},
        {0x0001u, 0x0Cu},
        {0x0006u, 0x11u},
        {0x0028u, 0x62u},
    };
    static const hcl_framos_gmsl_register_write serializer_link_setup[] = {
        {0x0040u, 0x16u},
        {0x02BEu, 0x84u},
        {0x02C0u, 0x4Fu},
        {0x02D6u, 0x90u},
        {0x02D0u, 0x80u},
    };
    static const hcl_framos_gmsl_register_write deserializer_link_setup[] = {
        {0x0161u, 0x01u},
        {0x1D00u, 0xF4u},
        {0x0320u, 0x38u},
        {0x1D00u, 0xF5u},
        {0x0040u, 0x16u},
        {0x02C5u, 0x83u},
        {0x02C6u, 0x6Fu},
        {0x02C8u, 0x83u},
    };
    static const hcl_framos_gmsl_register_write serializer_streaming[] = {
        {0x0330u, 0x08u},
        {0x0330u, 0x00u},
        {0x0331u, 0x70u},
        {0x031Eu, 0x2Cu},
        {0x0111u, 0x4Cu},
        {0x0110u, 0x28u},
        {0x005Bu, 0x01u},
        {0x0383u, 0x80u},
    };
    hcl_status status;
    size_t index = 0u;

    if (sensor == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    status = hcl_framos_gmsl_write_register(sensor, sensor->deserializer_address, 0x0010u, 0x81u);
    if (status.code != HCL_STATUS_OK) { return status; }

    hcl_delay_ms(HCL_FRAMOS_GMSL_INIT_DELAY);

    for (index = 0u; index < sizeof(deserializer_init) / sizeof(deserializer_init[0]); ++index) {
        status = hcl_framos_gmsl_write_register(sensor,
                                                sensor->deserializer_address,
                                                deserializer_init[index].reg,
                                                deserializer_init[index].value);
        if (status.code != HCL_STATUS_OK) { return status; }
    }

    for (index = 0u; index < sizeof(serializer_init) / sizeof(serializer_init[0]); ++index) {
        status = hcl_framos_gmsl_write_register(sensor,
                                                sensor->serializer_address,
                                                serializer_init[index].reg,
                                                serializer_init[index].value);
        if (status.code != HCL_STATUS_OK) { return status; }
    }

    hcl_delay_ms(HCL_FRAMOS_GMSL_INIT_DELAY);

    status = hcl_framos_gmsl_write_register(sensor, sensor->serializer_address, 0x0010u, 0x21u);
    if (status.code != HCL_STATUS_OK) { return status; }

    hcl_delay_ms(HCL_FRAMOS_GMSL_INIT_DELAY);

    status = hcl_framos_gmsl_write_register(sensor, sensor->deserializer_address, 0x0010u, 0x21u);
    if (status.code != HCL_STATUS_OK) { return status; }

    hcl_delay_ms(HCL_FRAMOS_GMSL_INIT_DELAY);

    for (index = 0u; index < sizeof(serializer_link_setup) / sizeof(serializer_link_setup[0]); ++index) {
        status = hcl_framos_gmsl_write_register(sensor,
                                                sensor->serializer_address,
                                                serializer_link_setup[index].reg,
                                                serializer_link_setup[index].value);
        if (status.code != HCL_STATUS_OK) { return status; }
    }

    for (index = 0u; index < sizeof(deserializer_link_setup) / sizeof(deserializer_link_setup[0]); ++index) {
        status = hcl_framos_gmsl_write_register(sensor,
                                                sensor->deserializer_address,
                                                deserializer_link_setup[index].reg,
                                                deserializer_link_setup[index].value);
        if (status.code != HCL_STATUS_OK) { return status; }
    }

    for (index = 0u; index < sizeof(serializer_streaming) / sizeof(serializer_streaming[0]); ++index) {
        status = hcl_framos_gmsl_write_register(sensor,
                                                sensor->serializer_address,
                                                serializer_streaming[index].reg,
                                                serializer_streaming[index].value);
        if (status.code != HCL_STATUS_OK) { return status; }
    }

    status = hcl_framos_gmsl_write_register(sensor, sensor->deserializer_address, 0x0474u, 0x19u);
    if (status.code != HCL_STATUS_OK) { return status; }
    status = hcl_framos_gmsl_write_register(sensor, sensor->deserializer_address, 0x0112u, 0x30u);
    if (status.code != HCL_STATUS_OK) { return status; }

    hcl_delay_ms(HCL_FRAMOS_GMSL_INIT_DELAY);

    status = hcl_framos_gmsl_write_register(sensor, sensor->deserializer_address, 0x0112u, 0x31u);
    if (status.code != HCL_STATUS_OK) { return status; }

    // Configure GPIO expander with desired sensor address
    status = hcl_framos_gmsl_write_gpio_register(sensor, 0x01u, sensor->sensor_address);
    if (status.code != HCL_STATUS_OK) { return status; }
    status = hcl_framos_gmsl_write_gpio_register(sensor, 0x03u, 0xFCu);
    if (status.code != HCL_STATUS_OK) { return status; }

    return hcl_status_ok();
}
