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

#include <cstdint>
#include <string>

#include "hcl/register_transport.hpp"

struct hcl_udp_register_transport;

namespace hcl {

class UdpRegisterTransport final : public RegisterTransport {
public:
    UdpRegisterTransport(std::string host, std::uint16_t port = 5005, std::uint32_t timeout_ms = 200);
    ~UdpRegisterTransport() override;

    Status connect() override;
    void disconnect() override;
    [[nodiscard]] bool is_connected() const override;

    Result<std::uint32_t> read_register(std::uint32_t base_address, std::uint32_t offset) override;
    Status write_register(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) override;
    Status execute(hcl_register_operation* operations, std::size_t operation_count) override;

    hcl_register_transport* c_transport{nullptr};
};

}  // namespace hcl
