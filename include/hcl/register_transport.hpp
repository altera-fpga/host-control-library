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

#include <cstddef>
#include <cstdint>
#include <functional>

#include "hcl/register_transport.h"
#include "hcl/register_interface_common.hpp"

namespace hcl {

class RegisterTransport {
public:
    virtual ~RegisterTransport() = default;

    virtual Status connect() = 0;
    virtual void disconnect() = 0;
    [[nodiscard]] virtual bool is_connected() const = 0;

    virtual Result<std::uint32_t> read_register(std::uint32_t base_address, std::uint32_t offset) = 0;
    virtual Status write_register(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) = 0;
    virtual Status execute(hcl_register_operation* operations, std::size_t operation_count) = 0;
};

}  // namespace hcl
