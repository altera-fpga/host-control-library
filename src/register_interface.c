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

#include "hcl/register_interface.h"
#include "hcl/register_transport.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct hcl_register_interface {
    hcl_register_transport transport;
};

hcl_register_interface* hcl_register_interface_create(const hcl_register_transport* transport) {
    if (transport == NULL || transport->vtable == NULL ||
        transport->vtable->read32 == NULL || transport->vtable->write32 == NULL ||
        transport->vtable->execute == NULL) {
        return NULL;
    }

    hcl_register_interface* result = (hcl_register_interface*)calloc(1u, sizeof(*result));
    if (result == NULL) {
        return NULL;
    }

    result->transport = *transport;
    return result;
}

void hcl_register_interface_destroy(hcl_register_interface* handle) {
    free(handle);
}

hcl_status hcl_register_interface_read32(const hcl_register_interface* handle,
                                         uint32_t base_address,
                                         uint32_t offset,
                                         uint32_t* out_value) {
    if (handle == NULL || out_value == NULL || handle->transport.vtable == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (handle->transport.vtable->read32 == NULL) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    return handle->transport.vtable->read32(&handle->transport,
                                            base_address,
                                            offset,
                                            out_value);
}

hcl_status hcl_register_interface_write32(const hcl_register_interface* handle,
                                          uint32_t base_address,
                                          uint32_t offset,
                                          uint32_t value) {
    if (handle == NULL || handle->transport.vtable == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (handle->transport.vtable->write32 == NULL) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    return handle->transport.vtable->write32(&handle->transport,
                                             base_address,
                                             offset,
                                             value);
}

hcl_status hcl_register_interface_execute(hcl_register_interface* handle,
                                          hcl_register_operation* operations,
                                          size_t operation_count) {
    size_t index;

    if (handle == NULL || handle->transport.vtable == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (operation_count > 0u && operations == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    if (operations != NULL) {
        for (index = 0u; index < operation_count; ++index) {
            operations[index].status = HCL_STATUS_OK;
            operations[index].read_value = 0u;
        }
    }

    if (operation_count == 0u) {
        return hcl_status_ok();
    }

    if (handle->transport.vtable->execute == NULL) {
        return hcl_status_error(HCL_STATUS_UNSUPPORTED, NULL);
    }

    return handle->transport.vtable->execute(&handle->transport,
                                             operations,
                                             operation_count);
}
