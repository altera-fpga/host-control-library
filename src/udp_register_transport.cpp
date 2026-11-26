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

#include "hcl/udp_register_transport.hpp"

#include <string>
#include <utility>

#include "hcl/register_interface_common.hpp"
#include "hcl/udp_register_transport.h"

namespace hcl {

UdpRegisterTransport::UdpRegisterTransport(std::string host, std::uint16_t port, std::uint32_t timeout_ms)
    : c_transport(nullptr) {
    c_transport = hcl_udp_register_transport_create(host.c_str(),
                                                    port,
                                                    timeout_ms);
}

UdpRegisterTransport::~UdpRegisterTransport() {
    if (c_transport != nullptr) {
        hcl_udp_register_transport_destroy(c_transport);
        c_transport = nullptr;
    }
}

Status UdpRegisterTransport::connect() {
    if (c_transport == nullptr || c_transport->context == nullptr) {
        return Status::error(StatusCode::InvalidArgument);
    }

    auto* concrete = static_cast<hcl_udp_register_transport*>(c_transport->context);
    const hcl_status_code status = hcl_udp_register_transport_connect(concrete);
    return StatusHelper::to_status(status);
}

void UdpRegisterTransport::disconnect() {
    if (c_transport != nullptr && c_transport->context != nullptr) {
        hcl_udp_register_transport_disconnect(static_cast<hcl_udp_register_transport*>(c_transport->context));
    }
}

bool UdpRegisterTransport::is_connected() const {
    return c_transport != nullptr && c_transport->context != nullptr &&
           hcl_udp_register_transport_is_connected(static_cast<const hcl_udp_register_transport*>(c_transport->context));
}

Result<std::uint32_t> UdpRegisterTransport::read_register(std::uint32_t base_address, std::uint32_t offset) {
    if (c_transport == nullptr || c_transport->context == nullptr) {
        return Result<std::uint32_t>::from_error(StatusCode::InvalidArgument);
    }

    std::uint32_t value = 0;
    auto* concrete = static_cast<hcl_udp_register_transport*>(c_transport->context);
    const hcl_status_code status = hcl_udp_register_transport_read32(concrete,
                                                                     base_address,
                                                                     offset,
                                                                     &value);
    if (status != HCL_STATUS_OK) {
        return Result<std::uint32_t>::from_error(StatusHelper::to_status_code(status),
                                                 RegisterContext{base_address, offset, value});
    }

    return Result<std::uint32_t>::from_value(value);
}

Status UdpRegisterTransport::write_register(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) {
    if (c_transport == nullptr || c_transport->context == nullptr) {
        return Status::error(StatusCode::InvalidArgument);
    }

    auto* concrete = static_cast<hcl_udp_register_transport*>(c_transport->context);
    const hcl_status_code status = hcl_udp_register_transport_write32(concrete,
                                                                      base_address,
                                                                      offset,
                                                                      value);
    if (status != HCL_STATUS_OK) {
        return StatusHelper::to_status(status, RegisterContext{base_address, offset, value});
    }
    return Status::success();
}

Status UdpRegisterTransport::execute(hcl_register_operation* operations, std::size_t operation_count) {
    if (c_transport == nullptr || c_transport->context == nullptr) {
        return Status::error(StatusCode::InvalidArgument);
    }
    if (operation_count > 0u && operations == nullptr) {
        return Status::error(StatusCode::InvalidArgument);
    }

    auto* concrete = static_cast<hcl_udp_register_transport*>(c_transport->context);
    const hcl_status_code status = hcl_udp_register_transport_execute(concrete,
                                                                      operations,
                                                                      operation_count);
    if (status != HCL_STATUS_OK && operation_count > 0u && operations != nullptr) {
        for (std::size_t index = 0u; index < operation_count; ++index) {
            if (operations[index].status != HCL_STATUS_OK) {
                return StatusHelper::to_status(status,
                                               RegisterContext{operations[index].base_address,
                                                               operations[index].offset,
                                                               operations[index].write_value});
            }
        }
    }
    return StatusHelper::to_status(status);
}

}  // namespace hcl
