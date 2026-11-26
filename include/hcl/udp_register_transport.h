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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "hcl/register_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_udp_register_transport hcl_udp_register_transport;

hcl_register_transport* hcl_udp_register_transport_create(const char* host,
                                                          uint16_t port,
                                                          uint32_t timeout_ms);

void hcl_udp_register_transport_destroy(hcl_register_transport* transport);

hcl_status_code hcl_udp_register_transport_connect(hcl_udp_register_transport* transport);

void hcl_udp_register_transport_disconnect(hcl_udp_register_transport* transport);

bool hcl_udp_register_transport_is_connected(const hcl_udp_register_transport* transport);

hcl_status_code hcl_udp_register_transport_read32(hcl_udp_register_transport* transport,
                                                  uint32_t base_address,
                                                  uint32_t offset,
                                                  uint32_t* out_value);

hcl_status_code hcl_udp_register_transport_write32(hcl_udp_register_transport* transport,
                                                   uint32_t base_address,
                                                   uint32_t offset,
                                                   uint32_t value);

hcl_status_code hcl_udp_register_transport_execute(hcl_udp_register_transport* transport,
                                                   hcl_register_operation* operations,
                                                   size_t operation_count);

#ifdef __cplusplus
}
#endif
