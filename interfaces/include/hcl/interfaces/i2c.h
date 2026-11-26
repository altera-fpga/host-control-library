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

#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_i2c hcl_i2c;
typedef struct hcl_i2c_interface hcl_i2c_interface;

typedef struct hcl_i2c_interface {
    hcl_status (*init)(hcl_i2c* i2c);
    hcl_status (*write)(hcl_i2c* i2c,
                        uint8_t address,
                        uint8_t reg,
                        uint8_t data);
    hcl_status (*read)(hcl_i2c* i2c,
                       uint8_t address,
                       uint8_t reg,
                       uint8_t* out_value);
    hcl_status (*write_extended)(hcl_i2c* i2c,
                                 uint8_t address,
                                 uint16_t reg,
                                 uint8_t data);
    hcl_status (*read_extended)(hcl_i2c* i2c,
                                uint8_t address,
                                uint16_t reg,
                                uint8_t* out_value);
} hcl_i2c_interface;

struct hcl_i2c {
    void* context;
    const hcl_i2c_interface* vtable;
};

hcl_i2c* hcl_i2c_create(void* context,
                        const hcl_i2c_interface* vtable);
void hcl_i2c_destroy(hcl_i2c* i2c);

hcl_status hcl_i2c_init(hcl_i2c* i2c);
hcl_status hcl_i2c_write(hcl_i2c* i2c,
                         uint8_t address,
                         uint8_t reg,
                         uint8_t data);
hcl_status hcl_i2c_read(hcl_i2c* i2c,
                        uint8_t address,
                        uint8_t reg,
                        uint8_t* out_value);
hcl_status hcl_i2c_write_extended(hcl_i2c* i2c,
                                  uint8_t address,
                                  uint16_t reg,
                                  uint8_t data);
hcl_status hcl_i2c_read_extended(hcl_i2c* i2c,
                                 uint8_t address,
                                 uint16_t reg,
                                 uint8_t* out_value);

#ifdef __cplusplus
}
#endif
