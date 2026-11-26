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

#include "hcl/drivers/altera_avalon_pio_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_avalon_pio hcl_altera_avalon_pio;

/* The driver borrows reg; the caller must keep it alive for the driver's lifetime. */
hcl_altera_avalon_pio* hcl_altera_avalon_pio_create(hcl_register_interface* reg,
                                                    uint32_t base_address);

void hcl_altera_avalon_pio_destroy(hcl_altera_avalon_pio* driver);

hcl_status hcl_altera_avalon_pio_read_data(hcl_altera_avalon_pio* driver,
                                           uint32_t* out_value);

hcl_status hcl_altera_avalon_pio_write_data(hcl_altera_avalon_pio* driver,
                                            uint32_t value);

hcl_status hcl_altera_avalon_pio_read_direction(hcl_altera_avalon_pio* driver,
                                                uint32_t* out_value);

hcl_status hcl_altera_avalon_pio_write_direction(hcl_altera_avalon_pio* driver,
                                                 uint32_t value);

hcl_status hcl_altera_avalon_pio_read_irq_mask(hcl_altera_avalon_pio* driver,
                                               uint32_t* out_value);

hcl_status hcl_altera_avalon_pio_write_irq_mask(hcl_altera_avalon_pio* driver,
                                                uint32_t value);

hcl_status hcl_altera_avalon_pio_read_edge_cap(hcl_altera_avalon_pio* driver,
                                               uint32_t* out_value);

hcl_status hcl_altera_avalon_pio_clear_edge_cap(hcl_altera_avalon_pio* driver,
                                                uint32_t value);

hcl_status hcl_altera_avalon_pio_set_bits(hcl_altera_avalon_pio* driver,
                                          uint32_t value);

hcl_status hcl_altera_avalon_pio_clear_bits(hcl_altera_avalon_pio* driver,
                                            uint32_t value);

#ifdef __cplusplus
}
#endif
