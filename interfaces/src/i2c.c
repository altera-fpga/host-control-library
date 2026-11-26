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

#include "hcl/interfaces/i2c.h"

#include <stdlib.h>

hcl_i2c* hcl_i2c_create(void* context,
                        const hcl_i2c_interface* vtable) {
    if (vtable == NULL) {
        return NULL;
    }

    hcl_i2c* i2c = (hcl_i2c*)calloc(1u, sizeof(*i2c));
    if (i2c == NULL) {
        return NULL;
    }

    i2c->context = context;
    i2c->vtable = vtable;
    return i2c;
}

void hcl_i2c_destroy(hcl_i2c* i2c) {
    free(i2c);
}

hcl_status hcl_i2c_init(hcl_i2c* i2c) {
    if (i2c == NULL || i2c->vtable == NULL || i2c->vtable->init == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return i2c->vtable->init(i2c);
}

hcl_status hcl_i2c_write(hcl_i2c* i2c,
                         uint8_t address,
                         uint8_t reg,
                         uint8_t data) {
    if (i2c == NULL || i2c->vtable == NULL || i2c->vtable->write == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return i2c->vtable->write(i2c, address, reg, data);
}

hcl_status hcl_i2c_read(hcl_i2c* i2c,
                        uint8_t address,
                        uint8_t reg,
                        uint8_t* out_value) {
    if (i2c == NULL || i2c->vtable == NULL || i2c->vtable->read == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return i2c->vtable->read(i2c, address, reg, out_value);
}

hcl_status hcl_i2c_write_extended(hcl_i2c* i2c,
                                  uint8_t address,
                                  uint16_t reg,
                                  uint8_t data) {
    if (i2c == NULL || i2c->vtable == NULL || i2c->vtable->write_extended == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return i2c->vtable->write_extended(i2c, address, reg, data);
}

hcl_status hcl_i2c_read_extended(hcl_i2c* i2c,
                                 uint8_t address,
                                 uint16_t reg,
                                 uint8_t* out_value) {
    if (i2c == NULL || i2c->vtable == NULL || i2c->vtable->read_extended == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return i2c->vtable->read_extended(i2c, address, reg, out_value);
}
