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

#include "hcl/drivers/altera_i2c.h"
#include "hcl/drivers/altera_i2c_regs.h"
#include "hcl/register_interface_common.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct hcl_altera_i2c {
    hcl_register_interface* reg;
    uint32_t base_address;
    uint32_t sys_clk;
    uint32_t transaction_timeout_ms;
    bool reinit;
};

static hcl_status write_reg(hcl_altera_i2c* d, uint32_t offset, uint32_t value) {
    return hcl_register_interface_write32(d->reg, d->base_address, offset, value);
}

static hcl_status read_reg(hcl_altera_i2c* d, uint32_t offset, uint32_t* out) {
    return hcl_register_interface_read32(d->reg, d->base_address, offset, out);
}

static hcl_status execute(hcl_altera_i2c* d,
                          hcl_register_operation* ops,
                          size_t op_count) {
    return hcl_register_interface_execute(d->reg, ops, op_count);
}

static hcl_status wait_rx_data_ready(hcl_altera_i2c* d) {
    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_I2C_REG_RX_DATA_FIFO_LVL, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_I2C_REG_ISR, 0u, 0u, HCL_STATUS_OK},
    };
    uint32_t fill;
    uint32_t isr;

    hcl_deadline deadline;
    hcl_compute_deadline(d->transaction_timeout_ms, &deadline);
    while (!hcl_deadline_exceeded(&deadline)) {
        const hcl_status status = execute(d, ops, sizeof(ops) / sizeof(ops[0]));
        if (!hcl_status_is_ok(&status)) {
            return status;
        }

        fill = ops[0].read_value;
        isr = ops[1].read_value;

        if ((isr & HCL_I2C_ISR_ABN_DET_MASK) != 0u) {
            d->reinit = true;
            return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
        }

        if (fill != 0u) {
            return hcl_status_ok();
        }
        hcl_delay_ms(1u);
    }

    d->reinit = true;
    return hcl_status_error(HCL_STATUS_TIMEOUT, NULL);
}

static hcl_status vtable_init(hcl_i2c* i2c) {
    return hcl_altera_i2c_init((hcl_altera_i2c*)i2c->context);
}

static hcl_status vtable_write(hcl_i2c* i2c,
                               uint8_t address,
                               uint8_t reg,
                               uint8_t data) {
    return hcl_altera_i2c_write((hcl_altera_i2c*)i2c->context, address, reg, data);
}

static hcl_status vtable_read(hcl_i2c* i2c,
                              uint8_t address,
                              uint8_t reg,
                              uint8_t* out_value) {
    return hcl_altera_i2c_read((hcl_altera_i2c*)i2c->context, address, reg, out_value);
}

static hcl_status vtable_write_extended(hcl_i2c* i2c,
                                        uint8_t address,
                                        uint16_t reg,
                                        uint8_t data) {
    return hcl_altera_i2c_write_extended((hcl_altera_i2c*)i2c->context, address, reg, data);
}

static hcl_status vtable_read_extended(hcl_i2c* i2c,
                                       uint8_t address,
                                       uint16_t reg,
                                       uint8_t* out_value) {
    return hcl_altera_i2c_read_extended((hcl_altera_i2c*)i2c->context, address, reg, out_value);
}

static const hcl_i2c_interface vtable = {
    .init = vtable_init,
    .write = vtable_write,
    .read = vtable_read,
    .write_extended = vtable_write_extended,
    .read_extended = vtable_read_extended,
};

hcl_i2c* hcl_altera_i2c_create(hcl_register_interface* reg,
                               uint32_t base_address,
                               uint32_t sys_clk,
                               uint32_t transaction_timeout_ms) {
    if (reg == NULL) {
        return NULL;
    }

    hcl_altera_i2c* d = (hcl_altera_i2c*)calloc(1u, sizeof(*d));
    if (d == NULL) {
        return NULL;
    }

    d->reg = reg;
    d->base_address = base_address;
    d->sys_clk = sys_clk;
    d->transaction_timeout_ms = transaction_timeout_ms;
    d->reinit = false;

    hcl_i2c* i2c = (hcl_i2c*)calloc(1u, sizeof(*i2c));
    if (i2c == NULL) {
        free(d);
        return NULL;
    }

    i2c->context = d;
    i2c->vtable = &vtable;
    return i2c;
}

void hcl_altera_i2c_destroy(hcl_i2c* i) {
    if (i == NULL) {
        return;
    }

    free(i->context);
    free(i);
}

hcl_status hcl_altera_i2c_init(hcl_altera_i2c* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    const uint32_t scl_count = (d->sys_clk + 99000u) / 100000u;
    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_CTRL, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISER, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISR, HCL_I2C_ISR_ABN_DET_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_SCL_LOW, scl_count / 2u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_SCL_HIGH, (scl_count + 1u) / 2u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_SDA_HOLD, scl_count / 4u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISER, HCL_I2C_ISER_BURST_ENABLE, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_CTRL, 1u, 0u, HCL_STATUS_OK},
    };

    return execute(d, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_altera_i2c_enable(hcl_altera_i2c* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (d->reinit) {
        const uint32_t scl_count = (d->sys_clk + 99000u) / 100000u;
        hcl_register_operation reinit_ops[] = {
            {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_CTRL, 0x40u, 0u, HCL_STATUS_OK},
            {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISER, 0u, 0u, HCL_STATUS_OK},
            {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISR, HCL_I2C_ISR_ABN_DET_MASK, 0u, HCL_STATUS_OK},
            {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_SCL_LOW, scl_count / 2u, 0u, HCL_STATUS_OK},
            {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_SCL_HIGH, (scl_count + 1u) / 2u, 0u, HCL_STATUS_OK},
            {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_SDA_HOLD, scl_count / 4u, 0u, HCL_STATUS_OK},
            {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISER, HCL_I2C_ISER_BURST_ENABLE, 0u, HCL_STATUS_OK},
            {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_CTRL, 1u, 0u, HCL_STATUS_OK},
        };

        const hcl_status status = execute(d, reinit_ops, sizeof(reinit_ops) / sizeof(reinit_ops[0]));
        if (hcl_status_is_ok(&status)) {
            d->reinit = false;
        }

        return status;
    }

    hcl_register_operation enable_ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISR, HCL_I2C_ISR_ABN_DET_MASK, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISER, HCL_I2C_ISER_BURST_ENABLE, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_CTRL, 1u, 0u, HCL_STATUS_OK},
    };

    return execute(d, enable_ops, sizeof(enable_ops) / sizeof(enable_ops[0]));
}

hcl_status hcl_altera_i2c_disable(hcl_altera_i2c* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_CTRL, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISER, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_ISR, HCL_I2C_ISR_ABN_DET_MASK, 0u, HCL_STATUS_OK},
    };

    return execute(d, ops, sizeof(ops) / sizeof(ops[0]));
}

hcl_status hcl_altera_i2c_write(hcl_altera_i2c* d,
                                uint8_t address,
                                uint8_t reg,
                                uint8_t data) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_status status;

    if (d->reinit) {
        status = hcl_altera_i2c_enable(d);
        if (!hcl_status_is_ok(&status)) { return status; }
    }

    status = hcl_altera_i2c_wait_busy(d);
    if (!hcl_status_is_ok(&status)) { return status; }

    const uint32_t addr_word = ((uint32_t)address << 1u) | HCL_I2C_WRITE_FLAG | HCL_I2C_START_FLAG;
    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, addr_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, (uint32_t)reg, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, (uint32_t)data | HCL_I2C_STOP_FLAG, 0u, HCL_STATUS_OK},
    };
    status = execute(d, ops, sizeof(ops) / sizeof(ops[0]));
    if (!hcl_status_is_ok(&status)) { return status; }

    return hcl_altera_i2c_wait_busy(d);
}

hcl_status hcl_altera_i2c_read(hcl_altera_i2c* d,
                               uint8_t address,
                               uint8_t reg,
                               uint8_t* out_value) {
    if (d == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_status status;

    if (d->reinit) {
        status = hcl_altera_i2c_enable(d);
        if (!hcl_status_is_ok(&status)) { return status; }
    }

    status = hcl_altera_i2c_wait_busy(d);
    if (!hcl_status_is_ok(&status)) { return status; }

    const uint32_t addr_word = ((uint32_t)address << 1u) | HCL_I2C_WRITE_FLAG | HCL_I2C_START_FLAG;
    const uint32_t read_addr_word = ((uint32_t)address << 1u) | HCL_I2C_READ_FLAG | HCL_I2C_START_FLAG;
    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, addr_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, (uint32_t)reg, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, read_addr_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, HCL_I2C_STOP_FLAG, 0u, HCL_STATUS_OK},
    };
    status = execute(d, ops, sizeof(ops) / sizeof(ops[0]));
    if (!hcl_status_is_ok(&status)) { return status; }

    status = hcl_altera_i2c_wait_busy(d);
    if (!hcl_status_is_ok(&status)) { return status; }

    status = wait_rx_data_ready(d);
    if (!hcl_status_is_ok(&status)) { return status; }

    uint32_t rx = 0u;
    status = read_reg(d, HCL_I2C_REG_RX_DATA, &rx);
    if (!hcl_status_is_ok(&status)) { return status; }

    *out_value = (uint8_t)rx;
    return hcl_status_ok();
}

hcl_status hcl_altera_i2c_write_extended(hcl_altera_i2c* d,
                                         uint8_t address,
                                         uint16_t reg,
                                         uint8_t data) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_status status;

    if (d->reinit) {
        status = hcl_altera_i2c_enable(d);
        if (!hcl_status_is_ok(&status)) { return status; }
    }

    status = hcl_altera_i2c_wait_busy(d);
    if (!hcl_status_is_ok(&status)) { return status; }

    const uint32_t addr_word = ((uint32_t)address << 1u) | HCL_I2C_WRITE_FLAG | HCL_I2C_START_FLAG;
    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, addr_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, ((uint32_t)reg >> 8u) & 0xFFu, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, (uint32_t)reg & 0xFFu, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, (uint32_t)data | HCL_I2C_STOP_FLAG, 0u, HCL_STATUS_OK},
    };
    status = execute(d, ops, sizeof(ops) / sizeof(ops[0]));
    if (!hcl_status_is_ok(&status)) { return status; }

    return hcl_altera_i2c_wait_busy(d);
}

hcl_status hcl_altera_i2c_read_extended(hcl_altera_i2c* d,
                                        uint8_t address,
                                        uint16_t reg,
                                        uint8_t* out_value) {
    if (d == NULL || out_value == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_status status;

    if (d->reinit) {
        status = hcl_altera_i2c_enable(d);
        if (!hcl_status_is_ok(&status)) { return status; }
    }

    status = hcl_altera_i2c_wait_busy(d);
    if (!hcl_status_is_ok(&status)) { return status; }

    const uint32_t addr_word = ((uint32_t)address << 1u) | HCL_I2C_WRITE_FLAG | HCL_I2C_START_FLAG;
    const uint32_t read_addr_word = ((uint32_t)address << 1u) | HCL_I2C_READ_FLAG | HCL_I2C_START_FLAG;
    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, addr_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, ((uint32_t)reg >> 8u) & 0xFFu, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, (uint32_t)reg & 0xFFu, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, read_addr_word, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_WRITE32, d->base_address, HCL_I2C_REG_TFR_CMD, HCL_I2C_STOP_FLAG, 0u, HCL_STATUS_OK},
    };
    status = execute(d, ops, sizeof(ops) / sizeof(ops[0]));
    if (!hcl_status_is_ok(&status)) { return status; }

    status = hcl_altera_i2c_wait_busy(d);
    if (!hcl_status_is_ok(&status)) { return status; }

    status = wait_rx_data_ready(d);
    if (!hcl_status_is_ok(&status)) { return status; }

    uint32_t rx = 0u;
    status = read_reg(d, HCL_I2C_REG_RX_DATA, &rx);
    if (!hcl_status_is_ok(&status)) { return status; }

    *out_value = (uint8_t)rx;
    return hcl_status_ok();
}

hcl_status hcl_altera_i2c_wait_busy(hcl_altera_i2c* d) {
    if (d == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    hcl_register_operation ops[] = {
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_I2C_REG_TFR_CMD_FIFO_LVL, 0u, 0u, HCL_STATUS_OK},
        {HCL_REGISTER_OP_READ32, d->base_address, HCL_I2C_REG_ISR, 0u, 0u, HCL_STATUS_OK},
    };
    uint32_t level;
    uint32_t isr;

    hcl_deadline deadline;
    hcl_compute_deadline(d->transaction_timeout_ms, &deadline);
    while (!hcl_deadline_exceeded(&deadline)) {
        const hcl_status status = execute(d, ops, sizeof(ops) / sizeof(ops[0]));
        if (!hcl_status_is_ok(&status)) { return status; }

        level = ops[0].read_value;
        isr = ops[1].read_value;

        if ((isr & HCL_I2C_ISR_ABN_DET_MASK) != 0u) {
            d->reinit = true;
            return hcl_status_error(HCL_STATUS_PROTOCOL_ERROR, NULL);
        }

        if (level == 0u) { return hcl_status_ok(); }
        hcl_delay_ms(1u);
    }

    d->reinit = true;
    return hcl_status_error(HCL_STATUS_TIMEOUT, NULL);
}
