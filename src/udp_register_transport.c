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

#include "hcl/udp_register_transport.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    kProtocolMagic = 0x41434C4Bu,
    kProtocolVersion = 1u,
    kOpRead = 1u,
    kOpWrite = 2u,
    kMaxOperationsPerMessage = 32u,
    kMaxHostLength = 255u
};

#pragma pack(push, 1)
typedef struct hcl_udp_request {
    uint32_t magic;
    uint8_t version;
    uint8_t op;
    uint16_t reserved;
    uint32_t sequence_id;
    uint32_t base_address;
    uint32_t offset;
    uint32_t value;
} hcl_udp_request;

typedef struct hcl_udp_response {
    uint32_t magic;
    uint8_t version;
    uint8_t op;
    uint16_t status;
    uint32_t sequence_id;
    uint32_t base_address;
    uint32_t offset;
    uint32_t value;
} hcl_udp_response;
#pragma pack(pop)

struct hcl_udp_register_transport {
    char* host;
    uint16_t port;
    uint32_t timeout_ms;
    bool connected;
#ifdef _WIN32
    uintptr_t socket_handle;
#else
    int socket_handle;
#endif
    uint32_t next_sequence_id;
};

#ifdef _WIN32
static const uintptr_t kInvalidSocket = (uintptr_t)(~(uintptr_t)0);
#else
static const int kInvalidSocket = -1;
#endif

static void close_socket_handle(
#ifdef _WIN32
    uintptr_t* socket_handle
#else
    int* socket_handle
#endif
) {
    if (socket_handle == NULL || *socket_handle == kInvalidSocket) {
        return;
    }

#ifdef _WIN32
    closesocket((SOCKET)*socket_handle);
#else
    close(*socket_handle);
#endif
    *socket_handle = kInvalidSocket;
}

static bool format_port_string(char* buffer, size_t buffer_size, unsigned int port) {
    int written;

    if (buffer == NULL || buffer_size == 0u) {
        return false;
    }

    written = snprintf(buffer, buffer_size, "%u", port);
    return written >= 0 && (size_t)written < buffer_size;
}

static hcl_status_code validate_config(const hcl_udp_register_transport* transport) {
    if (transport == NULL || transport->host == NULL || transport->host[0] == '\0') {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    return HCL_STATUS_OK;
}

#ifdef _WIN32
static bool init_registry_lock(CRITICAL_SECTION* lock) {
    return lock != NULL && InitializeCriticalSectionEx(lock, 0u, 0u) != 0;
}
#endif

static uint32_t allocate_sequence_id(hcl_udp_register_transport* transport) {
    const uint32_t sequence_id = transport->next_sequence_id;
    transport->next_sequence_id++;
    return sequence_id;
}

static hcl_status_code send_read_request(hcl_udp_register_transport* transport,
                                         uint32_t base_address,
                                         uint32_t offset,
                                         uint32_t* out_value) {
    uint32_t sequence_id;
    hcl_udp_request request;
    hcl_udp_response response;
#ifdef _WIN32
    int sent;
    int received;
#else
    ssize_t sent;
    ssize_t received;
#endif

    if (transport == NULL || out_value == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (!transport->connected) {
        return HCL_STATUS_NOT_CONNECTED;
    }

    sequence_id = allocate_sequence_id(transport);
    memset(&request, 0, sizeof(request));
    request.magic = htonl(kProtocolMagic);
    request.version = kProtocolVersion;
    request.op = kOpRead;
    request.sequence_id = htonl(sequence_id);
    request.base_address = htonl(base_address);
    request.offset = htonl(offset);

#ifdef _WIN32
    sent = send((SOCKET)transport->socket_handle, (const char*)&request, (int)sizeof(request), 0);
#else
    sent = send(transport->socket_handle, (const char*)&request, sizeof(request), 0);
#endif
    if (sent != (int)sizeof(request)) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    memset(&response, 0, sizeof(response));
#ifdef _WIN32
    received = recv((SOCKET)transport->socket_handle, (char*)&response, (int)sizeof(response), 0);
#else
    received = recv(transport->socket_handle, (char*)&response, sizeof(response), 0);
#endif
    if (received != (int)sizeof(response)) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    if (ntohl(response.magic) != kProtocolMagic || response.version != kProtocolVersion || response.op != kOpRead) {
        return HCL_STATUS_PROTOCOL_ERROR;
    }

    if (ntohl(response.sequence_id) != sequence_id) {
        return HCL_STATUS_PROTOCOL_ERROR;
    }

    if (ntohl(response.base_address) != base_address || ntohl(response.offset) != offset) {
        return HCL_STATUS_PROTOCOL_ERROR;
    }

    if (ntohs(response.status) != HCL_STATUS_OK) {
        return ntohs(response.status);
    }

    *out_value = ntohl(response.value);
    return HCL_STATUS_OK;
}

static hcl_status_code send_write_request(hcl_udp_register_transport* transport,
                                          uint32_t base_address,
                                          uint32_t offset,
                                          uint32_t value) {
    uint32_t sequence_id;
    hcl_udp_request request;
    hcl_udp_response response;
#ifdef _WIN32
    int sent;
    int received;
#else
    ssize_t sent;
    ssize_t received;
#endif

    if (transport == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (!transport->connected) {
        return HCL_STATUS_NOT_CONNECTED;
    }

    sequence_id = allocate_sequence_id(transport);
    memset(&request, 0, sizeof(request));
    request.magic = htonl(kProtocolMagic);
    request.version = kProtocolVersion;
    request.op = kOpWrite;
    request.sequence_id = htonl(sequence_id);
    request.base_address = htonl(base_address);
    request.offset = htonl(offset);
    request.value = htonl(value);

#ifdef _WIN32
    sent = send((SOCKET)transport->socket_handle, (const char*)&request, (int)sizeof(request), 0);
#else
    sent = send(transport->socket_handle, (const char*)&request, sizeof(request), 0);
#endif
    if (sent != (int)sizeof(request)) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    memset(&response, 0, sizeof(response));
#ifdef _WIN32
    received = recv((SOCKET)transport->socket_handle, (char*)&response, (int)sizeof(response), 0);
#else
    received = recv(transport->socket_handle, (char*)&response, sizeof(response), 0);
#endif
    if (received != (int)sizeof(response)) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    if (ntohl(response.magic) != kProtocolMagic || response.version != kProtocolVersion || response.op != kOpWrite) {
        return HCL_STATUS_PROTOCOL_ERROR;
    }

    if (ntohl(response.sequence_id) != sequence_id) {
        return HCL_STATUS_PROTOCOL_ERROR;
    }

    if (ntohl(response.base_address) != base_address || ntohl(response.offset) != offset) {
        return HCL_STATUS_PROTOCOL_ERROR;
    }

    return ntohs(response.status);
}

static hcl_status_code send_execute_requests(hcl_udp_register_transport* transport,
                                             hcl_register_operation* operations,
                                             size_t operation_count) {
    size_t processed = 0u;
    hcl_status_code aggregate_status = HCL_STATUS_OK;
    hcl_udp_request requests[kMaxOperationsPerMessage];
    hcl_udp_response responses[kMaxOperationsPerMessage];
#ifdef _WIN32
    int sent;
    int received;
#else
    ssize_t sent;
    ssize_t received;
#endif

    if (transport == NULL || operations == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    if (!transport->connected) {
        return HCL_STATUS_NOT_CONNECTED;
    }

    if (operation_count == 0u) {
        return HCL_STATUS_OK;
    }

    while (processed < operation_count) {
        size_t index;
        size_t chunk_count = operation_count - processed;
        size_t total_bytes;
        int expected_bytes;

        if (chunk_count > kMaxOperationsPerMessage) {
            chunk_count = kMaxOperationsPerMessage;
        }

        total_bytes = chunk_count * sizeof(hcl_udp_request);
        expected_bytes = (int)total_bytes;

        memset(requests, 0, total_bytes);
        memset(responses, 0, total_bytes);

        for (index = 0u; index < chunk_count; ++index) {
            const hcl_register_operation* operation = &operations[processed + index];
            requests[index].magic = htonl(kProtocolMagic);
            requests[index].version = kProtocolVersion;
            requests[index].op = (uint8_t)operation->operation;
            requests[index].sequence_id = htonl(allocate_sequence_id(transport));
            requests[index].base_address = htonl(operation->base_address);
            requests[index].offset = htonl(operation->offset);
            requests[index].value = htonl(operation->write_value);
        }

#ifdef _WIN32
        sent = send((SOCKET)transport->socket_handle, (const char*)requests, expected_bytes, 0);
#else
        sent = send(transport->socket_handle, (const char*)requests, total_bytes, 0);
#endif
        if (sent != expected_bytes) {
            return HCL_STATUS_TRANSPORT_ERROR;
        }

#ifdef _WIN32
        received = recv((SOCKET)transport->socket_handle, (char*)responses, expected_bytes, 0);
#else
        received = recv(transport->socket_handle, (char*)responses, total_bytes, 0);
#endif
        if (received != expected_bytes) {
            return HCL_STATUS_TRANSPORT_ERROR;
        }

        for (index = 0u; index < chunk_count; ++index) {
            hcl_status_code status = HCL_STATUS_OK;

            if (ntohl(responses[index].magic) != kProtocolMagic ||
                responses[index].version != kProtocolVersion ||
                responses[index].op != requests[index].op ||
                ntohl(responses[index].sequence_id) != ntohl(requests[index].sequence_id) ||
                ntohl(responses[index].base_address) != ntohl(requests[index].base_address) ||
                ntohl(responses[index].offset) != ntohl(requests[index].offset)) {
                status = HCL_STATUS_PROTOCOL_ERROR;
            } else {
                status = ntohs(responses[index].status);
            }

            operations[processed + index].status = status;
            operations[processed + index].read_value = ntohl(responses[index].value);
            if (status != HCL_STATUS_OK && aggregate_status == HCL_STATUS_OK) {
                aggregate_status = status;
            }
        }

        processed += chunk_count;
    }

    return aggregate_status;
}

static hcl_status_code vtable_connect(hcl_register_transport* transport) {
    return hcl_udp_register_transport_connect((hcl_udp_register_transport*)transport->context);
}

static void vtable_disconnect(hcl_register_transport* transport) {
    hcl_udp_register_transport_disconnect((hcl_udp_register_transport*)transport->context);
}

static bool vtable_is_connected(const hcl_register_transport* transport) {
    return hcl_udp_register_transport_is_connected((const hcl_udp_register_transport*)transport->context);
}

static hcl_status vtable_read32(const hcl_register_transport* transport,
                                uint32_t base_address,
                                uint32_t offset,
                                uint32_t* out_value) {
    hcl_register_context context = hcl_make_register_context(base_address, offset, 0u);
    const hcl_status_code code = hcl_udp_register_transport_read32((hcl_udp_register_transport*)transport->context,
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
    const hcl_status_code code = hcl_udp_register_transport_write32((hcl_udp_register_transport*)transport->context,
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

    const hcl_status_code code = hcl_udp_register_transport_execute((hcl_udp_register_transport*)transport->context,
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

static const hcl_register_transport_interface hcl_udp_register_transport_vtable = {
    &vtable_connect,
    &vtable_disconnect,
    &vtable_is_connected,
    &vtable_read32,
    &vtable_write32,
    &vtable_execute,
};

hcl_register_transport* hcl_udp_register_transport_create(const char* host,
                                                          uint16_t port,
                                                          uint32_t timeout_ms) {
    hcl_udp_register_transport* transport;
    size_t host_len;

    if (host == NULL || host[0] == '\0') {
        return NULL;
    }

    transport = (hcl_udp_register_transport*)calloc(1u, sizeof(*transport));
    if (transport == NULL) {
        return NULL;
    }

    host_len = strnlen(host, kMaxHostLength + 1u);
    if (host_len == 0u || host_len > kMaxHostLength) {
        free(transport);
        return NULL;
    }

    transport->host = (char*)malloc(host_len + 1u);
    if (transport->host == NULL) {
        free(transport);
        return NULL;
    }

    memcpy(transport->host, host, host_len + 1u);
    transport->port = port;
    transport->timeout_ms = timeout_ms;
    transport->connected = false;
    transport->socket_handle = kInvalidSocket;
    transport->next_sequence_id = 1u;

    hcl_register_transport* handle = (hcl_register_transport*)calloc(1u, sizeof(*handle));
    if (handle == NULL) {
        free(transport->host);
        free(transport);
        return NULL;
    }

    handle->context = transport;
    handle->vtable = &hcl_udp_register_transport_vtable;
    return handle;
}

void hcl_udp_register_transport_destroy(hcl_register_transport* transport) {
    hcl_udp_register_transport* concrete = NULL;

    if (transport == NULL || transport->context == NULL) {
        return;
    }

    concrete = (hcl_udp_register_transport*)transport->context;
    hcl_udp_register_transport_disconnect(concrete);
    free(concrete->host);
    concrete->host = NULL;
    free(concrete);
    free(transport);
}

hcl_status_code hcl_udp_register_transport_connect(hcl_udp_register_transport* transport) {
    char service[16];
    struct addrinfo hints;
    struct addrinfo* result = NULL;
#ifdef _WIN32
    DWORD timeout;
#else
    struct timeval timeout;
#endif

    if (transport == NULL) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

#ifdef _WIN32
    {
        hcl_status_code ws = ensure_winsock();
        if (ws != HCL_STATUS_OK) {
            return ws;
        }
    }
#endif

    {
        hcl_status_code config_status = validate_config(transport);
        if (config_status != HCL_STATUS_OK) {
            return config_status;
        }
    }

    if (transport->connected) {
        return HCL_STATUS_OK;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    if (!format_port_string(service, sizeof(service), (unsigned int)transport->port)) {
        return HCL_STATUS_OUT_OF_RANGE;
    }

    if (getaddrinfo(transport->host, service, &hints, &result) != 0 || result == NULL) {
        return HCL_STATUS_TRANSPORT_ERROR;
    }

#ifdef _WIN32
    transport->socket_handle = (uintptr_t)socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (transport->socket_handle == kInvalidSocket) {
        freeaddrinfo(result);
        return HCL_STATUS_TRANSPORT_ERROR;
    }
    if (connect((SOCKET)transport->socket_handle, result->ai_addr, (int)result->ai_addrlen) != 0) {
#else
    transport->socket_handle = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (transport->socket_handle == kInvalidSocket) {
        freeaddrinfo(result);
        return HCL_STATUS_TRANSPORT_ERROR;
    }
    if (connect(transport->socket_handle, result->ai_addr, (int)result->ai_addrlen) != 0) {
#endif
        freeaddrinfo(result);
        hcl_udp_register_transport_disconnect(transport);
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    freeaddrinfo(result);

#ifdef _WIN32
    timeout = transport->timeout_ms;
    if (setsockopt((SOCKET)transport->socket_handle,
                   SOL_SOCKET,
                   SO_RCVTIMEO,
                   (const char*)&timeout,
                   sizeof(timeout)) != 0 ||
        setsockopt((SOCKET)transport->socket_handle,
                   SOL_SOCKET,
                   SO_SNDTIMEO,
                   (const char*)&timeout,
                   sizeof(timeout)) != 0) {
#else
    timeout.tv_sec = (time_t)(transport->timeout_ms / 1000u);
    timeout.tv_usec = (suseconds_t)((transport->timeout_ms % 1000u) * 1000u);
    if (setsockopt(transport->socket_handle,
                   SOL_SOCKET,
                   SO_RCVTIMEO,
                   &timeout,
                   sizeof(timeout)) != 0 ||
        setsockopt(transport->socket_handle,
                   SOL_SOCKET,
                   SO_SNDTIMEO,
                   &timeout,
                   sizeof(timeout)) != 0) {
#endif
        hcl_udp_register_transport_disconnect(transport);
        return HCL_STATUS_TRANSPORT_ERROR;
    }

    transport->connected = true;
    return HCL_STATUS_OK;
}

void hcl_udp_register_transport_disconnect(hcl_udp_register_transport* transport) {
    if (transport == NULL) {
        return;
    }
    close_socket_handle(&transport->socket_handle);

    transport->connected = false;
}

bool hcl_udp_register_transport_is_connected(const hcl_udp_register_transport* transport) {
    return transport != NULL && transport->connected;
}

hcl_status_code hcl_udp_register_transport_read32(hcl_udp_register_transport* transport,
                                                  uint32_t base_address,
                                                  uint32_t offset,
                                                  uint32_t* out_value) {
    return send_read_request(transport, base_address, offset, out_value);
}

hcl_status_code hcl_udp_register_transport_write32(hcl_udp_register_transport* transport,
                                                   uint32_t base_address,
                                                   uint32_t offset,
                                                   uint32_t value) {
    return send_write_request(transport, base_address, offset, value);
}

hcl_status_code hcl_udp_register_transport_execute(hcl_udp_register_transport* transport,
                                                   hcl_register_operation* operations,
                                                   size_t operation_count) {
    if (transport == NULL || (operation_count > 0u && operations == NULL)) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    return send_execute_requests(transport, operations, operation_count);
}

