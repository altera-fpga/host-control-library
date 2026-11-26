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

#include "hcl/drivers/altera_avalon_pio.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

struct hcl_altera_avalon_pio {
    hcl_register_interface* reg;
    uint32_t base_address;
};

static hcl_status write_reg(hcl_altera_avalon_pio* d, uint32_t offset, uint32_t value) {
    const hcl_status status = hcl_register_interface_write32(d->reg, d->base_address, offset, value);
    return status;
}

static hcl_status read_reg(hcl_altera_avalon_pio* d, uint32_t offset, uint32_t* out) {
    const hcl_status status = hcl_register_interface_read32(d->reg, d->base_address, offset, out);
    return status;
}

hcl_altera_avalon_pio* hcl_altera_avalon_pio_create(hcl_register_interface* reg,
                                                    uint32_t base_address) {
    if (reg == NULL) {
        return NULL;
    }

    hcl_altera_avalon_pio* d = (hcl_altera_avalon_pio*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }

    d->reg = reg;
    d->base_address = base_address;
    return d;
}

void hcl_altera_avalon_pio_destroy(hcl_altera_avalon_pio* driver) {
    free(driver);
}

hcl_status hcl_altera_avalon_pio_read_data(hcl_altera_avalon_pio* d,
                                           uint32_t* out_value) {
    if (d == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_PIO_REG_DATA, out_value);
}

hcl_status hcl_altera_avalon_pio_write_data(hcl_altera_avalon_pio* d,
                                            uint32_t value) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_PIO_REG_DATA, value);
}

hcl_status hcl_altera_avalon_pio_read_direction(hcl_altera_avalon_pio* d,
                                                uint32_t* out_value) {
    if (d == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_PIO_REG_DIRECTION, out_value);
}

hcl_status hcl_altera_avalon_pio_write_direction(hcl_altera_avalon_pio* d,
                                                 uint32_t value) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_PIO_REG_DIRECTION, value);
}

hcl_status hcl_altera_avalon_pio_read_irq_mask(hcl_altera_avalon_pio* d,
                                               uint32_t* out_value) {
    if (d == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_PIO_REG_IRQ_MASK, out_value);
}

hcl_status hcl_altera_avalon_pio_write_irq_mask(hcl_altera_avalon_pio* d,
                                                uint32_t value) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_PIO_REG_IRQ_MASK, value);
}

hcl_status hcl_altera_avalon_pio_read_edge_cap(hcl_altera_avalon_pio* d,
                                               uint32_t* out_value) {
    if (d == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return read_reg(d, HCL_PIO_REG_EDGE_CAP, out_value);
}

hcl_status hcl_altera_avalon_pio_clear_edge_cap(hcl_altera_avalon_pio* d,
                                                uint32_t value) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_PIO_REG_EDGE_CAP, value);
}

hcl_status hcl_altera_avalon_pio_set_bits(hcl_altera_avalon_pio* d,
                                          uint32_t value) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_PIO_REG_SET_BITS, value);
}

hcl_status hcl_altera_avalon_pio_clear_bits(hcl_altera_avalon_pio* d,
                                            uint32_t value) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }
    return write_reg(d, HCL_PIO_REG_CLEAR_BITS, value);
}
