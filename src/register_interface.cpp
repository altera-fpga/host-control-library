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

#include "hcl/register_interface.hpp"

#include <utility>

#include "hcl/register_interface.h"
#include "hcl/register_interface_common.hpp"

namespace hcl {

struct RegisterInterface::State {
    static const hcl_register_transport_interface vtable;

    explicit State(std::shared_ptr<RegisterTransport> transport)
        : transport(std::move(transport)) {
        c_transport.context = this;
        c_transport.vtable = &vtable;

        handle = hcl_register_interface_create(&c_transport);
    }

    ~State() {
        if (handle != nullptr) {
            hcl_register_interface_destroy(handle);
            handle = nullptr;
        }
    }

    static hcl_status read32_vtable(const hcl_register_transport* transport,
                                    std::uint32_t base_address,
                                    std::uint32_t offset,
                                    std::uint32_t* out_value) {
        auto* self = static_cast<State*>(transport != nullptr ? transport->context : nullptr);
        if (self == nullptr || !self->transport || out_value == nullptr) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }

        const auto result = self->transport->read_register(base_address, offset);
        if (!result.ok()) {
            return StatusHelper::to_hcl_status(result.status);
        }

        *out_value = result.value;
        return hcl_status_ok();
    }

    static hcl_status write32_vtable(const hcl_register_transport* transport,
                                     std::uint32_t base_address,
                                     std::uint32_t offset,
                                     std::uint32_t value) {
        auto* self = static_cast<State*>(transport != nullptr ? transport->context : nullptr);
        if (self == nullptr || !self->transport) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }

        const Status status = self->transport->write_register(base_address, offset, value);
        return StatusHelper::to_hcl_status(status);
    }

    static hcl_status execute_vtable(const hcl_register_transport* transport,
                                     hcl_register_operation* operations,
                                     std::size_t operation_count) {
        auto* self = static_cast<State*>(transport != nullptr ? transport->context : nullptr);

        if (self == nullptr || !self->transport || (operation_count > 0u && operations == nullptr)) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }

        const Status status = self->transport->execute(operations, operation_count);
        return StatusHelper::to_hcl_status(status);
    }

    std::shared_ptr<RegisterTransport> transport;
    hcl_register_transport c_transport{};
    hcl_register_interface* handle{nullptr};
};

const hcl_register_transport_interface RegisterInterface::State::vtable = {
    nullptr,
    nullptr,
    nullptr,
    &RegisterInterface::State::read32_vtable,
    &RegisterInterface::State::write32_vtable,
    &RegisterInterface::State::execute_vtable,
};

RegisterInterface::RegisterInterface(std::shared_ptr<RegisterTransport> transport)
    : state_(std::make_shared<State>(std::move(transport))) {}

hcl_register_interface* RegisterInterface::c_handle() const {
    return state_ ? state_->handle : nullptr;
}

Result<std::uint32_t> RegisterInterface::read32(std::uint32_t base_address, std::uint32_t offset) {
    if (!state_ || state_->handle == nullptr) {
        return Result<std::uint32_t>::from_error(StatusCode::InvalidArgument);
    }

    std::uint32_t value = 0;
    const hcl_status status = hcl_register_interface_read32(state_->handle,
                                                            base_address,
                                                            offset,
                                                            &value);
    if (status.code != HCL_STATUS_OK) {
        return Result<std::uint32_t>{StatusHelper::to_status(status), value};
    }
    return Result<std::uint32_t>::from_value(value);
}

Status RegisterInterface::write32(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) {
    if (!state_ || state_->handle == nullptr) {
        return Status::error(StatusCode::InvalidArgument);
    }

    const hcl_status status = hcl_register_interface_write32(state_->handle,
                                                             base_address,
                                                             offset,
                                                             value);
    return StatusHelper::to_status(status);
}

Status RegisterInterface::execute(std::vector<Operation>& operations) {
    if (!state_ || state_->handle == nullptr) {
        return Status::error(StatusCode::InvalidArgument);
    }

    std::vector<hcl_register_operation> c_operations;
    c_operations.reserve(operations.size());
    for (const auto& operation : operations) {
        hcl_register_operation c_operation{};
        c_operation.operation = static_cast<hcl_register_operation_type>(operation.operation);
        c_operation.base_address = operation.base_address;
        c_operation.offset = operation.offset;
        c_operation.write_value = operation.write_value;
        c_operations.push_back(c_operation);
    }

    const hcl_status status = hcl_register_interface_execute(state_->handle,
                                                             c_operations.data(),
                                                             c_operations.size());

    for (std::size_t index = 0u; index < operations.size(); ++index) {
        operations[index].read_value = c_operations[index].read_value;
        operations[index].status = StatusHelper::to_status_code(c_operations[index].status);
    }

    return StatusHelper::to_status(status);
}

}  // namespace hcl
