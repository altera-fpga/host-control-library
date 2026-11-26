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

#include <stddef.h>
#include <stdint.h>

#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_register_transport hcl_register_transport;
typedef struct hcl_register_interface hcl_register_interface;
typedef struct hcl_register_operation hcl_register_operation;

typedef enum hcl_register_operation_type {
    HCL_REGISTER_OP_READ32 = 1,
    HCL_REGISTER_OP_WRITE32 = 2
} hcl_register_operation_type;

struct hcl_register_operation {
    hcl_register_operation_type operation;
    uint32_t base_address;
    uint32_t offset;
    uint32_t write_value;
    uint32_t read_value;
    hcl_status_code status;
};

hcl_register_interface* hcl_register_interface_create(const hcl_register_transport* transport);

void hcl_register_interface_destroy(hcl_register_interface* handle);

hcl_status hcl_register_interface_read32(const hcl_register_interface* handle,
                                         uint32_t base_address,
                                         uint32_t offset,
                                         uint32_t* out_value);

hcl_status hcl_register_interface_write32(const hcl_register_interface* handle,
                                          uint32_t base_address,
                                          uint32_t offset,
                                          uint32_t value);

hcl_status hcl_register_interface_execute(hcl_register_interface* handle,
                                          hcl_register_operation* operations,
                                          size_t operation_count);

#ifdef __cplusplus
}
#endif
