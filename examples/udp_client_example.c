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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"
#include "hcl/udp_register_transport.h"

static const char* k_default_host = "127.0.0.1";
static const uint16_t k_default_port = 5005u;
static const uint32_t k_default_request_count = 2u;
static const uint32_t k_default_base_address = 0x00000000u;
static const uint32_t k_default_offset = 0x00000010u;
static const uint32_t k_initial_write_value = 0x12345678u;

typedef enum operation_mode {
    OPERATION_READ = 0,
    OPERATION_WRITE = 1
} operation_mode;

typedef struct client_config {
    const char* host;
    uint16_t port;
    uint32_t request_count;
    uint32_t base_address;
    uint32_t offset;
    operation_mode operation;
    uint32_t write_value;
} client_config;

static void print_usage(const char* program_name) {
    printf("Usage: %s"
           " [--host HOST] [--port PORT] [--count REQUEST_COUNT]"
           " [--base BASE_ADDRESS] [--offset OFFSET]"
           " [--operation read|write] [--value WRITE_VALUE]\n"
           "Defaults: --host %s --port %u --count %u"
           " --base 0x%08x --offset 0x%08x --operation read --value 0x%08x\n",
           program_name,
           k_default_host,
           (unsigned int)k_default_port,
           (unsigned int)k_default_request_count,
           (unsigned int)k_default_base_address,
           (unsigned int)k_default_offset,
           (unsigned int)k_initial_write_value);
}

static const char* operation_name(operation_mode operation) {
    return (operation == OPERATION_READ) ? "read" : "write";
}

static const char* status_name(hcl_status_code status) {
    switch (status) {
        case HCL_STATUS_OK:
            return "ok";
        case HCL_STATUS_TIMEOUT:
            return "timeout";
        case HCL_STATUS_TRANSPORT_ERROR:
            return "transport_error";
        case HCL_STATUS_PROTOCOL_ERROR:
            return "protocol_error";
        case HCL_STATUS_UNSUPPORTED:
            return "unsupported";
        case HCL_STATUS_INVALID_ARGUMENT:
            return "invalid_argument";
        case HCL_STATUS_NOT_CONNECTED:
            return "not_connected";
        case HCL_STATUS_OUT_OF_RANGE:
            return "out_of_range";
        case HCL_STATUS_ALLOCATION_FAILED:
            return "allocation_failed";
        case HCL_STATUS_INTERNAL_ERROR:
            return "internal_error";
        default:
            return "unknown";
    }
}

static uint16_t parse_port(const char* value) {
    char* end = NULL;
    long parsed = strtol(value, &end, 10);
    if (end == value || *end != '\0' || parsed <= 0 || parsed > 65535) {
        fprintf(stderr, "Invalid port: %s (using default %u)\n", value, (unsigned int)k_default_port);
        return k_default_port;
    }

    return (uint16_t)parsed;
}

static uint32_t parse_request_count(const char* value) {
    char* end = NULL;
    long parsed = strtol(value, &end, 10);
    if (end == value || *end != '\0' || parsed <= 0) {
        fprintf(stderr,
                "Invalid request count: %s (using default %u)\n",
                value,
                (unsigned int)k_default_request_count);
        return k_default_request_count;
    }

    return (uint32_t)parsed;
}

static uint32_t parse_u32_value(const char* value, const char* label, uint32_t fallback) {
    char* end = NULL;
    unsigned long parsed = strtoul(value, &end, 0);
    if (end == value || *end != '\0') {
        fprintf(stderr,
                "Invalid %s: %s (using default 0x%08x)\n",
                label,
                value,
                (unsigned int)fallback);
        return fallback;
    }

    return (uint32_t)parsed;
}

static operation_mode parse_operation(const char* value) {
    if (strcmp(value, "read") == 0) {
        return OPERATION_READ;
    }

    if (strcmp(value, "write") == 0) {
        return OPERATION_WRITE;
    }

    fprintf(stderr, "Invalid operation: %s (using default read)\n", value);
    return OPERATION_READ;
}

static client_config default_config(void) {
    client_config config;
    config.host = k_default_host;
    config.port = k_default_port;
    config.request_count = k_default_request_count;
    config.base_address = k_default_base_address;
    config.offset = k_default_offset;
    config.operation = OPERATION_READ;
    config.write_value = k_initial_write_value;
    return config;
}

static client_config parse_args(int argc, char** argv) {
    client_config config = default_config();
    int index;

    for (index = 1; index < argc; ++index) {
        const char* arg = argv[index];
        if ((strcmp(arg, "--help") == 0) || (strcmp(arg, "-h") == 0)) {
            print_usage(argv[0]);
            exit(EXIT_SUCCESS);
        }

        if (((strcmp(arg, "--host") == 0) || (strcmp(arg, "--port") == 0) ||
             (strcmp(arg, "--count") == 0) || (strcmp(arg, "--base") == 0) ||
             (strcmp(arg, "--offset") == 0) || (strcmp(arg, "--operation") == 0) ||
             (strcmp(arg, "--value") == 0)) &&
            ((index + 1) >= argc)) {
            fprintf(stderr, "Missing value for %s\n", arg);
            print_usage(argv[0]);
            exit(EXIT_FAILURE);
        }

        if (strcmp(arg, "--host") == 0) {
            config.host = argv[++index];
            continue;
        }

        if (strcmp(arg, "--port") == 0) {
            config.port = parse_port(argv[++index]);
            continue;
        }

        if (strcmp(arg, "--count") == 0) {
            config.request_count = parse_request_count(argv[++index]);
            continue;
        }

        if (strcmp(arg, "--base") == 0) {
            config.base_address = parse_u32_value(argv[++index], "base address", k_default_base_address);
            continue;
        }

        if (strcmp(arg, "--offset") == 0) {
            config.offset = parse_u32_value(argv[++index], "offset", k_default_offset);
            continue;
        }

        if (strcmp(arg, "--operation") == 0) {
            config.operation = parse_operation(argv[++index]);
            continue;
        }

        if (strcmp(arg, "--value") == 0) {
            config.write_value = parse_u32_value(argv[++index], "write value", k_initial_write_value);
            continue;
        }

        fprintf(stderr, "Unknown argument: %s\n", arg);
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return config;
}

int main(int argc, char** argv) {
    client_config config = parse_args(argc, argv);
    hcl_register_transport* transport =
        hcl_udp_register_transport_create(config.host, config.port, 500u);
    if (transport == NULL) {
        fprintf(stderr, "Transport create failed\n");
        return EXIT_FAILURE;
    }

    hcl_status_code status = hcl_register_transport_connect(transport);
    if (status != HCL_STATUS_OK) {
        fprintf(stderr, "Transport connect failed: %s\n", status_name(status));
        hcl_udp_register_transport_destroy(transport);
        return EXIT_FAILURE;
    }

    hcl_register_interface* access = hcl_register_interface_create(transport);
    if (access == NULL) {
        fprintf(stderr, "Interface create failed: %s\n", status_name(HCL_STATUS_ALLOCATION_FAILED));
        hcl_register_transport_disconnect(transport);
        hcl_udp_register_transport_destroy(transport);
        return EXIT_FAILURE;
    }

    printf("Client target: %s:%u requests=%u base=0x%08x offset=0x%08x operation=%s",
           config.host,
           (unsigned int)config.port,
           (unsigned int)config.request_count,
           (unsigned int)config.base_address,
           (unsigned int)config.offset,
           operation_name(config.operation));
    if (config.operation == OPERATION_WRITE) {
        printf(" value=0x%08x", (unsigned int)config.write_value);
    }
    printf("\n");

    for (uint32_t request_index = 0; request_index < config.request_count; ++request_index) {
        if (config.operation == OPERATION_WRITE) {
            hcl_status write_status = hcl_register_interface_write32(access,
                                                                     config.base_address,
                                                                     config.offset,
                                                                     config.write_value);
            if (write_status.code != HCL_STATUS_OK) {
                fprintf(stderr,
                        "Write failed at request %u: %s\n",
                        (unsigned int)request_index,
                        status_name(write_status.code));
                hcl_register_interface_destroy(access);
                hcl_register_transport_disconnect(transport);
                hcl_udp_register_transport_destroy(transport);
                return EXIT_FAILURE;
            }

            printf("WRITE request %u: register [base=0x%08x, offset=0x%08x] <- 0x%08x\n",
                   (unsigned int)(request_index + 1u),
                   (unsigned int)config.base_address,
                   (unsigned int)config.offset,
                   (unsigned int)config.write_value);
            continue;
        }

        uint32_t read_value = 0u;
        hcl_status read_status = hcl_register_interface_read32(access,
                                                               config.base_address,
                                                               config.offset,
                                                               &read_value);
        if (read_status.code != HCL_STATUS_OK) {
            fprintf(stderr,
                    "Read failed at request %u: %s\n",
                    (unsigned int)request_index,
                    status_name(read_status.code));
            hcl_register_interface_destroy(access);
            hcl_register_transport_disconnect(transport);
            hcl_udp_register_transport_destroy(transport);
            return EXIT_FAILURE;
        }

        printf("READ request %u: register [base=0x%08x, offset=0x%08x] = 0x%08x\n",
               (unsigned int)(request_index + 1u),
               (unsigned int)config.base_address,
               (unsigned int)config.offset,
               (unsigned int)read_value);
    }

    hcl_register_interface_destroy(access);
    hcl_register_transport_disconnect(transport);
    hcl_udp_register_transport_destroy(transport);

    return EXIT_SUCCESS;
}
