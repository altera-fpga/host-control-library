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
#include <memory>
#include <vector>

#include "hcl/register_interface.h"
#include "hcl/register_interface_common.hpp"
#include "hcl/register_transport.hpp"

namespace hcl {

enum class OperationType {
    Read32 = HCL_REGISTER_OP_READ32,
    Write32 = HCL_REGISTER_OP_WRITE32
};

struct Operation {
    OperationType operation{OperationType::Read32};
    std::uint32_t base_address{0u};
    std::uint32_t offset{0u};
    std::uint32_t write_value{0u};
    std::uint32_t read_value{0u};
    StatusCode status{StatusCode::Ok};
};

class RegisterInterface {
public:
    explicit RegisterInterface(std::shared_ptr<RegisterTransport> transport);
    ~RegisterInterface() = default;

    /* Returns the underlying C ABI handle; lifetime is tied to this object. */
    [[nodiscard]] hcl_register_interface* c_handle() const;

    Result<std::uint32_t> read32(std::uint32_t base_address, std::uint32_t offset);
    Status write32(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value);

    Status execute(std::vector<Operation>& operations);

private:
    struct State;

    std::shared_ptr<State> state_;
};

}  // namespace hcl
