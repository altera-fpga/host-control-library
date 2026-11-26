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

#include "hcl/uio_register_transport.h"

#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hcl_uio_register_transport {
    char* device_path;
    unsigned int map_index;
    int fd;
    void* map_base;
    size_t map_size;
    pthread_mutex_t lock;
};

enum {
    kPathBufferSize = 256,
    kMaxDevicePathLength = 4096
};

#ifdef O_NOFOLLOW
#define HCL_OPEN_NOFOLLOW O_NOFOLLOW
#else
#define HCL_OPEN_NOFOLLOW 0
#endif

static const char* device_name_from_path(const char* path) {
    const char* slash = strrchr(path, '/');
    return slash == NULL ? path : (slash + 1);
}

static bool build_sysfs_size_path(const hcl_uio_register_transport* transport,
                                  char* out_path,
                                  size_t out_path_size) {
    int written;

    if (transport == NULL || out_path == NULL || out_path_size == 0u) {
        return false;
    }

    written = snprintf(out_path,
                       out_path_size,
                       "/sys/class/uio/%s/maps/map%u/size",
                       device_name_from_path(transport->device_path),
                       transport->map_index);
    return written >= 0 && (size_t)written < out_path_size;
}

static hcl_status_code read_map_size(const hcl_uio_register_transport* transport,
                                     size_t* out_size) {
    char path_buffer[kPathBufferSize];
    FILE* file = NULL;
    char line[64];
    char* end_ptr = NULL;
    unsigned long long parsed_size = 0ull;

    if (transport == NULL || out_size == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (!build_sysfs_size_path(transport, path_buffer, sizeof(path_buffer))) {
        return HCL_STATUS_OUT_OF_RANGE;
    }

    file = fopen(path_buffer, "r");
    if (file == NULL) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return HCL_STATUS_TRANSPORT_ERROR;
    }
    fclose(file);

    errno = 0;
    parsed_size = strtoull(line, &end_ptr, 0);
    if (errno != 0 || end_ptr == line) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    if (parsed_size == 0ull) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    *out_size = (size_t)parsed_size;
    return HCL_STATUS_OK;
}

static hcl_status_code vtable_connect(hcl_register_transport* transport) {
    return hcl_uio_register_transport_connect((hcl_uio_register_transport*)transport->context);
}

static void vtable_disconnect(hcl_register_transport* transport) {
    hcl_uio_register_transport_disconnect((hcl_uio_register_transport*)transport->context);
}

static bool vtable_is_connected(const hcl_register_transport* transport) {
    return hcl_uio_register_transport_is_connected((const hcl_uio_register_transport*)transport->context);
}

static hcl_status vtable_read32(const hcl_register_transport* transport,
                                uint32_t base_address,
                                uint32_t offset,
                                uint32_t* out_value) {
    hcl_register_context context = hcl_make_register_context(base_address, offset, 0u);
    const hcl_status_code code = hcl_uio_register_transport_read32((hcl_uio_register_transport*)transport->context,
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
    const hcl_status_code code = hcl_uio_register_transport_write32((hcl_uio_register_transport*)transport->context,
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

    const hcl_status_code code = hcl_uio_register_transport_execute((hcl_uio_register_transport*)transport->context,
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

static const hcl_register_transport_interface hcl_uio_register_transport_vtable = {
    &vtable_connect,
    &vtable_disconnect,
    &vtable_is_connected,
    &vtable_read32,
    &vtable_write32,
    &vtable_execute,
};

hcl_register_transport* hcl_uio_register_transport_create(const char* device_path,
                                                          unsigned int map_index) {
    hcl_uio_register_transport* transport = NULL;
    size_t path_len = 0u;
    if (device_path == NULL || device_path[0] == '\0') {
        return NULL;
    }

    transport = (hcl_uio_register_transport*)calloc(1u, sizeof(*transport));
    if (transport == NULL) {
        return NULL;
    }

    path_len = strnlen(device_path, kMaxDevicePathLength + 1u);
    if (path_len == 0u || path_len > kMaxDevicePathLength) {
        free(transport);
        return NULL;
    }

    transport->device_path = (char*)malloc(path_len + 1u);
    if (transport->device_path == NULL) {
        free(transport);
        return NULL;
    }

    if (pthread_mutex_init(&transport->lock, NULL) != 0) {
        free(transport->device_path);
        free(transport);
        return NULL;
    }

    memcpy(transport->device_path, device_path, path_len + 1u);
    transport->map_index = map_index;
    transport->fd = -1;
    transport->map_base = NULL;
    transport->map_size = 0u;

    hcl_register_transport* handle = (hcl_register_transport*)calloc(1u, sizeof(*handle));
    if (handle == NULL) {
        pthread_mutex_destroy(&transport->lock);
        free(transport->device_path);
        free(transport);
        return NULL;
    }

    handle->context = transport;
    handle->vtable = &hcl_uio_register_transport_vtable;
    return handle;
}

void hcl_uio_register_transport_destroy(hcl_register_transport* transport) {
    hcl_uio_register_transport* concrete = NULL;

    if (transport == NULL || transport->context == NULL) {
        return;
    }

    concrete = (hcl_uio_register_transport*)transport->context;
    hcl_uio_register_transport_disconnect(concrete);
    pthread_mutex_destroy(&concrete->lock);
    free(concrete->device_path);
    concrete->device_path = NULL;
    free(concrete);
    free(transport);
}

hcl_status_code hcl_uio_register_transport_connect(hcl_uio_register_transport* transport) {
    size_t map_size = 0u;
    hcl_status_code size_status;
    int fd;
    long page_size;
    off_t file_offset;
    void* addr;

    if (transport == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (transport->fd != -1) {
        return HCL_STATUS_OK;
    }

    size_status = read_map_size(transport, &map_size);
    if (size_status != HCL_STATUS_OK) {
        return size_status;
    }

    fd = open(transport->device_path, O_RDWR | O_CLOEXEC | HCL_OPEN_NOFOLLOW);
    if (fd == -1) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        close(fd);
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    file_offset = (off_t)transport->map_index * (off_t)page_size;
    addr = mmap(NULL, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, file_offset);
    if (addr == MAP_FAILED) {
        close(fd);
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    transport->fd = fd;
    transport->map_base = addr;
    transport->map_size = map_size;
    return HCL_STATUS_OK;
}

void hcl_uio_register_transport_disconnect(hcl_uio_register_transport* transport) {
    if (transport == NULL) {
        return;
    }

    if (transport->map_base != NULL) {
        munmap(transport->map_base, transport->map_size);
        transport->map_base = NULL;
        transport->map_size = 0u;
    }

    if (transport->fd != -1) {
        close(transport->fd);
        transport->fd = -1;
    }
}

bool hcl_uio_register_transport_is_connected(const hcl_uio_register_transport* transport) {
    return transport != NULL && transport->fd != -1;
}

hcl_status_code hcl_uio_register_transport_read32(hcl_uio_register_transport* transport,
                                                  uint32_t base_address,
                                                  uint32_t offset,
                                                  uint32_t* out_value) {
    uint64_t byte_offset;
    const volatile uint32_t* reg;

    if (transport == NULL || out_value == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (transport->map_base == NULL) {
        return HCL_STATUS_NOT_CONNECTED;
    }

    byte_offset = (uint64_t)base_address + (uint64_t)offset;
    if (byte_offset + sizeof(uint32_t) > transport->map_size) {
        return HCL_STATUS_OUT_OF_RANGE;
    }

    reg = (const volatile uint32_t*)((const uint8_t*)transport->map_base + byte_offset);
    *out_value = *reg;
    return HCL_STATUS_OK;
}

hcl_status_code hcl_uio_register_transport_write32(hcl_uio_register_transport* transport,
                                                   uint32_t base_address,
                                                   uint32_t offset,
                                                   uint32_t value) {
    uint64_t byte_offset;
    volatile uint32_t* reg;

    if (transport == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (transport->map_base == NULL) {
        return HCL_STATUS_NOT_CONNECTED;
    }

    byte_offset = (uint64_t)base_address + (uint64_t)offset;
    if (byte_offset + sizeof(uint32_t) > transport->map_size) {
        return HCL_STATUS_OUT_OF_RANGE;
    }

    reg = (volatile uint32_t*)((uint8_t*)transport->map_base + byte_offset);
    *reg = value;
    return HCL_STATUS_OK;
}

hcl_status_code hcl_uio_register_transport_execute(hcl_uio_register_transport* transport,
                                                   hcl_register_operation* operations,
                                                   size_t operation_count) {
    size_t index;
    hcl_status_code aggregate_status = HCL_STATUS_OK;

    if (transport == NULL || (operation_count > 0u && operations == NULL)) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0u; index < operation_count; ++index) {
        hcl_status_code status = HCL_STATUS_INVALID_ARGUMENT;

        if (operations[index].operation == HCL_REGISTER_OP_READ32) {
            status = hcl_uio_register_transport_read32(transport,
                                                       operations[index].base_address,
                                                       operations[index].offset,
                                                       &operations[index].read_value);
        } else if (operations[index].operation == HCL_REGISTER_OP_WRITE32) {
            status = hcl_uio_register_transport_write32(transport,
                                                        operations[index].base_address,
                                                        operations[index].offset,
                                                        operations[index].write_value);
        } else {
            status = HCL_STATUS_INVALID_ARGUMENT;
        }

        operations[index].status = status;

        if (status != HCL_STATUS_OK && aggregate_status == HCL_STATUS_OK) {
            aggregate_status = status;
        }
    }

    return aggregate_status;
}

