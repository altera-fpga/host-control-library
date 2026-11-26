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

#include "hcl/nios_register_transport.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <string.h>

struct hcl_nios_register_transport {
    bool connected;
};

#if __has_include(<io.h>)
#include <io.h>

#define HCL_NIOS_HAS_IO_SUPPORT 1

#else

#define HCL_NIOS_HAS_IO_SUPPORT 0

#endif

static hcl_status_code read32_transport(const hcl_nios_register_transport* transport,
                                        uint32_t base_address,
                                        uint32_t offset,
                                        uint32_t* out_value) {
    if (transport == NULL || out_value == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (!transport->connected) {
        return HCL_STATUS_NOT_CONNECTED;
    }

#if HCL_NIOS_HAS_IO_SUPPORT
    *out_value = IORD_32DIRECT(base_address, offset);
#else
    (void)base_address;
    (void)offset;
    return HCL_STATUS_UNSUPPORTED;
#endif
    return HCL_STATUS_OK;
}

static hcl_status_code write32_transport(hcl_nios_register_transport* transport,
                                         uint32_t base_address,
                                         uint32_t offset,
                                         uint32_t value) {
    if (transport == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (!transport->connected) {
        return HCL_STATUS_NOT_CONNECTED;
    }

#if HCL_NIOS_HAS_IO_SUPPORT
    IOWR_32DIRECT(base_address, offset, value);
    return HCL_STATUS_OK;
#else
    (void)base_address;
    (void)offset;
    (void)value;
    return HCL_STATUS_UNSUPPORTED;
#endif
}

static hcl_status_code vtable_connect(hcl_register_transport* transport) {
    return hcl_nios_register_transport_connect((hcl_nios_register_transport*)transport->context);
}

static void vtable_disconnect(hcl_register_transport* transport) {
    hcl_nios_register_transport_disconnect((hcl_nios_register_transport*)transport->context);
}

static bool vtable_is_connected(const hcl_register_transport* transport) {
    return hcl_nios_register_transport_is_connected((const hcl_nios_register_transport*)transport->context);
}

static hcl_status vtable_read32(const hcl_register_transport* transport,
                                uint32_t base_address,
                                uint32_t offset,
                                uint32_t* out_value) {
    hcl_register_context context = hcl_make_register_context(base_address, offset, 0u);
    const hcl_status_code code = hcl_nios_register_transport_read32((hcl_nios_register_transport*)transport->context,
                                                                    base_address,
                                                                    offset,
                                                                    out_value);
    return hcl_status_error(code, &context);
}

static hcl_status vtable_write32(const hcl_register_transport* transport,
                                 uint32_t base_address,
                                 uint32_t offset,
                                 uint32_t value) {
    hcl_register_context context = hcl_make_register_context(base_address, offset, value);
    const hcl_status_code code = hcl_nios_register_transport_write32((hcl_nios_register_transport*)transport->context,
                                                                     base_address,
                                                                     offset,
                                                                     value);
    return hcl_status_error(code, &context);
}

static hcl_status vtable_execute(const hcl_register_transport* transport,
                                hcl_register_operation* operations,
                                size_t operation_count) {
    if (transport == NULL || transport->context == NULL ||
        (operation_count > 0u && operations == NULL)) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    const hcl_status_code code = hcl_nios_register_transport_execute((hcl_nios_register_transport*)transport->context,
                                                                     operations,
                                                                     operation_count);

    hcl_register_context context = {0u, 0u, 0u};

    if (code != HCL_STATUS_OK && operations != NULL) {
        for (size_t index = 0u; index < operation_count; ++index) {
            if (operations[index].status != HCL_STATUS_OK) {
                context = hcl_make_register_context(operations[index].base_address,
                                                    operations[index].offset,
                                                    operations[index].write_value);
                break;
            }
        }
    }

    return hcl_status_error(code, &context);
}

static const hcl_register_transport_interface hcl_nios_register_transport_vtable = {
    &vtable_connect,
    &vtable_disconnect,
    &vtable_is_connected,
    &vtable_read32,
    &vtable_write32,
    &vtable_execute,
};

hcl_register_transport* hcl_nios_register_transport_create(void) {
    hcl_nios_register_transport* transport =
        (hcl_nios_register_transport*)calloc(1u, sizeof(*transport));

    if (transport == NULL) {
        return NULL;
    }

    transport->connected = false;

    hcl_register_transport* handle = NULL;
    handle = (hcl_register_transport*)calloc(1u, sizeof(*handle));
    if (handle == NULL) {
        free(transport);
        return NULL;
    }

    handle->context = transport;
    handle->vtable = &hcl_nios_register_transport_vtable;
    return handle;
}

void hcl_nios_register_transport_destroy(hcl_register_transport* transport) {
    hcl_nios_register_transport* concrete = NULL;

    if (transport == NULL || transport->context == NULL) {
        return;
    }

    concrete = (hcl_nios_register_transport*)transport->context;
    free(concrete);
    free(transport);
}

hcl_status_code hcl_nios_register_transport_connect(hcl_nios_register_transport* transport) {
    if (transport == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    transport->connected = true;
    return HCL_STATUS_OK;
}

void hcl_nios_register_transport_disconnect(hcl_nios_register_transport* transport) {
    if (transport == NULL) {
        return;
    }

    transport->connected = false;
}

bool hcl_nios_register_transport_is_connected(const hcl_nios_register_transport* transport) {
    return transport != NULL && transport->connected;
}

hcl_status_code hcl_nios_register_transport_read32(hcl_nios_register_transport* transport,
                                                   uint32_t base_address,
                                                   uint32_t offset,
                                                   uint32_t* out_value) {
    return read32_transport(transport, base_address, offset, out_value);
}

hcl_status_code hcl_nios_register_transport_write32(hcl_nios_register_transport* transport,
                                                    uint32_t base_address,
                                                    uint32_t offset,
                                                    uint32_t value) {
    return write32_transport(transport, base_address, offset, value);
}

hcl_status_code hcl_nios_register_transport_execute(hcl_nios_register_transport* transport,
                                                    hcl_register_operation* operations,
                                                    size_t operation_count) {
    size_t index;

    if (transport == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (operation_count > 0u && operations == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (!transport->connected) {
        return HCL_STATUS_NOT_CONNECTED;
    }

    for (index = 0u; index < operation_count; ++index) {
        hcl_register_operation* operation = &operations[index];
        switch (operation->operation) {
            case HCL_REGISTER_OP_READ32:
                operation->status = read32_transport(transport,
                                                     operation->base_address,
                                                     operation->offset,
                                                     &operation->read_value);
                break;
            case HCL_REGISTER_OP_WRITE32:
                operation->status = write32_transport(transport,
                                                      operation->base_address,
                                                      operation->offset,
                                                      operation->write_value);
                break;
            default:
                operation->status = HCL_STATUS_UNSUPPORTED;
                break;
        }

        if (operation->status != HCL_STATUS_OK) {
            return operation->status;
        }
    }

    return HCL_STATUS_OK;
}

