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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hcl/interfaces/i2c.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_i2c hcl_altera_i2c;

/* The driver borrows reg; the caller must keep it alive for the driver's lifetime. */
hcl_i2c* hcl_altera_i2c_create(hcl_register_interface* reg,
                               uint32_t base_address,
                               uint32_t sys_clk,
                               uint32_t transaction_timeout_ms);

void hcl_altera_i2c_destroy(hcl_i2c* i);

hcl_status hcl_altera_i2c_init(hcl_altera_i2c* d);

hcl_status hcl_altera_i2c_enable(hcl_altera_i2c* d);

hcl_status hcl_altera_i2c_disable(hcl_altera_i2c* d);

/* 16-bit big-endian register-addressed transfers. */
hcl_status hcl_altera_i2c_write(hcl_altera_i2c* d,
                                uint8_t address,
                                uint8_t reg,
                                uint8_t data);

hcl_status hcl_altera_i2c_read(hcl_altera_i2c* d,
                               uint8_t address,
                               uint8_t reg,
                               uint8_t* out_value);

/* 16-bit register-addressed transfers. */
hcl_status hcl_altera_i2c_write_extended(hcl_altera_i2c* d,
                                         uint8_t address,
                                         uint16_t reg,
                                         uint8_t data);

hcl_status hcl_altera_i2c_read_extended(hcl_altera_i2c* d,
                                        uint8_t address,
                                        uint16_t reg,
                                        uint8_t* out_value);

hcl_status hcl_altera_i2c_wait_busy(hcl_altera_i2c* d);

#ifdef __cplusplus
}
#endif
