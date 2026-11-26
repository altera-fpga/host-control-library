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

#pragma once

#include <stdint.h>

#include "hcl/interfaces/i2c.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_framos_gmsl hcl_framos_gmsl;

hcl_framos_gmsl* hcl_framos_gmsl_create(hcl_i2c* i2c,
                                        uint8_t serializer_address,
                                        uint8_t deserializer_address,
                                        uint8_t gpio_address,
                                        uint8_t sensor_address);

void hcl_framos_gmsl_destroy(hcl_framos_gmsl* sensor);

hcl_status hcl_framos_gmsl_init(hcl_framos_gmsl* sensor);

hcl_status hcl_framos_gmsl_read_register(hcl_framos_gmsl* sensor,
                                         uint8_t device_address,
                                         uint16_t reg,
                                         uint8_t* out_value);

hcl_status hcl_framos_gmsl_write_register(hcl_framos_gmsl* sensor,
                                          uint8_t device_address,
                                          uint16_t reg,
                                          uint8_t data);

hcl_status hcl_framos_gmsl_write_gpio_register(hcl_framos_gmsl* sensor,
                                               uint8_t reg,
                                               uint8_t data);

#ifdef __cplusplus
}
#endif
