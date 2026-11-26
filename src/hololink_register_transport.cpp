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

#include "hcl/hololink_register_transport.hpp"

#include <cstdint>
#include <exception>
#include <memory>
#include <utility>

#include "hcl/register_interface_common.hpp"

#if __has_include(<hololink/core/hololink.hpp>)
#include <hololink/core/hololink.hpp>

namespace hcl {

namespace {

static hcl_status_code status_from_exception(const std::exception& error) {
    if (dynamic_cast<const hololink::TimeoutError*>(&error) != nullptr) {
        return HCL_STATUS_TIMEOUT;
    }
    if (dynamic_cast<const hololink::TransactionError*>(&error) != nullptr) {
        return HCL_STATUS_PROTOCOL_ERROR;
    }
    if (dynamic_cast<const std::bad_alloc*>(&error) != nullptr) {
        return HCL_STATUS_ALLOCATION_FAILED;
    }
    return HCL_STATUS_INTERNAL_ERROR;
}

static hcl_status_code read32_transport(hololink::Hololink& hololink,
                                        std::uint32_t base_address,
                                        std::uint32_t offset,
                                        std::uint32_t* out_value) {
    if (out_value == nullptr) {
        return HCL_STATUS_INVALID_ARGUMENT;
    }

    const std::uint64_t address = static_cast<std::uint64_t>(base_address) + static_cast<std::uint64_t>(offset);
    if (address > UINT32_MAX) {
        return HCL_STATUS_OUT_OF_RANGE;
    }

    try {
        *out_value = hololink.read_uint32(static_cast<std::uint32_t>(address));
        return HCL_STATUS_OK;
    } catch (const std::exception& error) {
        return status_from_exception(error);
    } catch (...) {
        return HCL_STATUS_INTERNAL_ERROR;
    }
}

static hcl_status_code write32_transport(hololink::Hololink& hololink,
                                         std::uint32_t base_address,
                                         std::uint32_t offset,
                                         std::uint32_t value) {
    const std::uint64_t address = static_cast<std::uint64_t>(base_address) + static_cast<std::uint64_t>(offset);
    if (address > UINT32_MAX) {
        return HCL_STATUS_OUT_OF_RANGE;
    }

    try {
        if (!hololink.write_uint32(static_cast<std::uint32_t>(address), value)) {
            return HCL_STATUS_TRANSPORT_ERROR;
        }
        return HCL_STATUS_OK;
    } catch (const std::exception& error) {
        return status_from_exception(error);
    } catch (...) {
        return HCL_STATUS_INTERNAL_ERROR;
    }
}

}  // namespace

HololinkRegisterTransport::HololinkRegisterTransport(std::shared_ptr<hololink::Hololink> hololink)
    : hololink_(std::move(hololink)) {}

HololinkRegisterTransport::~HololinkRegisterTransport() = default;

Status HololinkRegisterTransport::connect() {
    if (!hololink_) {
        return Status::error(StatusCode::InvalidArgument);
    }
    if (connected_) {
        return Status::success();
    }

    try {
        hololink_->start();
    } catch (const std::exception&) {
        return Status::error(StatusCode::TransportError);
    }

    connected_ = true;
    return Status::success();
}

void HololinkRegisterTransport::disconnect() {
    if (!connected_) {
        return;
    }

    try {
        hololink_->stop();
    } catch (const std::exception&) {}

    connected_ = false;
}

bool HololinkRegisterTransport::is_connected() const {
    return connected_ && static_cast<bool>(hololink_);
}

Result<std::uint32_t> HololinkRegisterTransport::read_register(std::uint32_t base_address,
                                                               std::uint32_t offset) {
    if (!is_connected()) {
        return Result<std::uint32_t>::from_error(StatusCode::NotConnected);
    }

    std::uint32_t value = 0;
    const hcl_status_code status = read32_transport(*hololink_, base_address, offset, &value);
    if (status != HCL_STATUS_OK) {
        return Result<std::uint32_t>::from_error(StatusHelper::to_status_code(status),
                                                 RegisterContext{base_address, offset, value});
    }

    return Result<std::uint32_t>::from_value(value);
}

Status HololinkRegisterTransport::write_register(std::uint32_t base_address,
                                                 std::uint32_t offset,
                                                 std::uint32_t value) {
    if (!is_connected()) {
        return Status::error(StatusCode::NotConnected);
    }

    const hcl_status_code status = write32_transport(*hololink_, base_address, offset, value);
    if (status != HCL_STATUS_OK) {
        return StatusHelper::to_status(status, RegisterContext{base_address, offset, value});
    }
    return Status::success();
}

Status HololinkRegisterTransport::execute(hcl_register_operation* operations,
                                          std::size_t operation_count) {
    if (!is_connected()) {
        return Status::error(StatusCode::NotConnected);
    }
    if (operation_count > 0u && operations == nullptr) {
        return Status::error(StatusCode::InvalidArgument);
    }

    for (std::size_t index = 0u; index < operation_count; ++index) {
        hcl_register_operation& operation = operations[index];
        switch (operation.operation) {
            case HCL_REGISTER_OP_READ32:
                operation.status = read32_transport(
                    *hololink_, operation.base_address, operation.offset, &operation.read_value);
                break;
            case HCL_REGISTER_OP_WRITE32:
                operation.status = write32_transport(
                    *hololink_, operation.base_address, operation.offset, operation.write_value);
                break;
            default:
                operation.status = HCL_STATUS_UNSUPPORTED;
                break;
        }

        if (operation.status != HCL_STATUS_OK) {
            const std::uint32_t context_value = operation.operation == HCL_REGISTER_OP_READ32
                                                    ? 0u
                                                    : operation.write_value;
            return StatusHelper::to_status(operation.status,
                                           RegisterContext{operation.base_address,
                                                           operation.offset,
                                                           context_value});
        }
    }

    return Status::success();
}

}  // namespace hcl

#endif