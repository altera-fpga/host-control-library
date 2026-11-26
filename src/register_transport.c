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

#include "hcl/register_transport.h"

#include <stdlib.h>

hcl_register_transport* hcl_register_transport_create(void* context,
                                                      const hcl_register_transport_interface* vtable) {
    if (vtable == NULL) {
        return NULL;
    }

    hcl_register_transport* transport = (hcl_register_transport*)calloc(1u, sizeof(*transport));
    if (transport == NULL) {
        return NULL;
    }

    transport->context = context;
    transport->vtable = vtable;
    return transport;
}

void hcl_register_transport_destroy(hcl_register_transport* transport) {
    free(transport);
}

hcl_status_code hcl_register_transport_connect(hcl_register_transport* transport) {
    if (transport == NULL || transport->vtable == NULL || transport->vtable->connect == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    return transport->vtable->connect(transport);
}

void hcl_register_transport_disconnect(hcl_register_transport* transport) {
    if (transport == NULL || transport->vtable == NULL || transport->vtable->disconnect == NULL) {
        return;
    }

    transport->vtable->disconnect(transport);
}

bool hcl_register_transport_is_connected(const hcl_register_transport* transport) {
    if (transport == NULL || transport->vtable == NULL || transport->vtable->is_connected == NULL) {
        return false;
    }

    return transport->vtable->is_connected(transport);
}

hcl_status hcl_register_transport_read32(const hcl_register_transport* transport,
                                         uint32_t base_address,
                                         uint32_t offset,
                                         uint32_t* out_value) {
    if (transport == NULL || transport->vtable == NULL || transport->vtable->read32 == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return transport->vtable->read32(transport, base_address, offset, out_value);
}

hcl_status hcl_register_transport_write32(const hcl_register_transport* transport,
                                          uint32_t base_address,
                                          uint32_t offset,
                                          uint32_t value) {
    if (transport == NULL || transport->vtable == NULL || transport->vtable->write32 == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return transport->vtable->write32(transport, base_address, offset, value);
}

hcl_status hcl_register_transport_execute(const hcl_register_transport* transport,
                                          hcl_register_operation* operations,
                                          size_t operation_count) {
    if (transport == NULL || transport->vtable == NULL || transport->vtable->execute == NULL) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    return transport->vtable->execute(transport, operations, operation_count);
}
