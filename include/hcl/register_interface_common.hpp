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
#include <utility>

#include "hcl/register_interface_common.h"

namespace hcl {

enum class StatusCode {
    Ok = HCL_STATUS_OK,
    Timeout = HCL_STATUS_TIMEOUT,
    TransportError = HCL_STATUS_TRANSPORT_ERROR,
    ProtocolError = HCL_STATUS_PROTOCOL_ERROR,
    Unsupported = HCL_STATUS_UNSUPPORTED,
    InvalidArgument = HCL_STATUS_INVALID_ARGUMENT,
    NotConnected = HCL_STATUS_NOT_CONNECTED,
    OutOfRange = HCL_STATUS_OUT_OF_RANGE,
    AllocationFailed = HCL_STATUS_ALLOCATION_FAILED,
    InternalError = HCL_STATUS_INTERNAL_ERROR
};

struct RegisterContext {
    std::uint32_t base_address{0u};
    std::uint32_t offset{0u};
    std::uint32_t value{0u};
};

struct Status {
    StatusCode code{StatusCode::Ok};
    RegisterContext ctx{};
    bool has_context{false};

    [[nodiscard]] bool ok() const { return code == StatusCode::Ok; }

    static Status success() {
        Status status{};
        status.code = StatusCode::Ok;
        status.has_context = false;
        return status;
    }

    static Status error(StatusCode error_code) {
        Status status{};
        status.code = error_code;
        status.has_context = false;
        return status;
    }

    static Status error(StatusCode error_code, const RegisterContext& context) {
        Status status{};
        status.code = error_code;
        status.ctx = context;
        status.has_context = true;
        return status;
    }
};

static inline hcl_status to_hcl_status(const Status& status) {
    if (status.code == StatusCode::Ok) {
        return hcl_status_ok();
    }

    if (status.has_context) {
        const hcl_register_context context = hcl_make_register_context(status.ctx.base_address,
                                                                       status.ctx.offset,
                                                                       status.ctx.value);
        return hcl_status_error(static_cast<hcl_status_code>(status.code), &context);
    }

    return hcl_status_error(static_cast<hcl_status_code>(status.code), nullptr);
}

static inline Status to_status(const hcl_status& status) {
    if (status.code == HCL_STATUS_OK) {
        return Status::success();
    }

    if (status.has_context) {
        const RegisterContext context{status.ctx.base_address,
                                      status.ctx.offset,
                                      status.ctx.value};
        return Status::error(static_cast<StatusCode>(status.code), context);
    }

    return Status::error(static_cast<StatusCode>(status.code));
}

template <typename T>
struct Result {
    Status status{};
    T value{};

    [[nodiscard]] bool ok() const { return status.ok(); }

    static Result<T> from_error(StatusCode code) {
        return Result<T>{Status::error(code), T{}};
    }

    static Result<T> from_error(StatusCode code, const RegisterContext& context) {
        return Result<T>{Status::error(code, context), T{}};
    }

    static Result<T> from_value(T out_value) {
        return Result<T>{Status::success(), out_value};
    }
};

class StatusHelper final {
public:
    [[nodiscard]] static StatusCode to_status_code(hcl_status_code code) {
        switch (code) {
            case HCL_STATUS_OK:
                return StatusCode::Ok;
            case HCL_STATUS_TIMEOUT:
                return StatusCode::Timeout;
            case HCL_STATUS_TRANSPORT_ERROR:
                return StatusCode::TransportError;
            case HCL_STATUS_PROTOCOL_ERROR:
                return StatusCode::ProtocolError;
            case HCL_STATUS_UNSUPPORTED:
                return StatusCode::Unsupported;
            case HCL_STATUS_INVALID_ARGUMENT:
                return StatusCode::InvalidArgument;
            case HCL_STATUS_NOT_CONNECTED:
                return StatusCode::NotConnected;
            case HCL_STATUS_OUT_OF_RANGE:
                return StatusCode::OutOfRange;
            case HCL_STATUS_ALLOCATION_FAILED:
                return StatusCode::AllocationFailed;
            case HCL_STATUS_INTERNAL_ERROR:
                return StatusCode::InternalError;
            default:
                return StatusCode::InternalError;
        }
    }

    [[nodiscard]] static Status to_status(hcl_status_code code) {
        if (code == HCL_STATUS_OK) {
            return Status::success();
        }
        return Status::error(to_status_code(code));
    }

    [[nodiscard]] static Status to_status(const hcl_status& status) {
        return hcl::to_status(status);
    }

    [[nodiscard]] static hcl_status to_hcl_status(const Status& status) {
        return hcl::to_hcl_status(status);
    }

    [[nodiscard]] static Status to_status(hcl_status_code code, const RegisterContext& context) {
        if (code == HCL_STATUS_OK) {
            return Status::success();
        }
        return Status::error(to_status_code(code), context);
    }

    template <typename T>
    [[nodiscard]] static Result<T> to_result(const hcl_status& status, T value) {
        if (status.code == HCL_STATUS_OK) {
            return Result<T>::from_value(std::move(value));
        }
        if (status.has_context) {
            return Result<T>::from_error(to_status_code(status.code),
                                         RegisterContext{status.ctx.base_address,
                                                         status.ctx.offset,
                                                         status.ctx.value});
        }
        return Result<T>::from_error(to_status_code(status.code));
    }

    template <typename T>
    [[nodiscard]] static Result<T> to_result(hcl_status_code code, T value) {
        if (code == HCL_STATUS_OK) {
            return Result<T>::from_value(std::move(value));
        }
        return Result<T>::from_error(to_status_code(code));
    }

    template <typename T>
    [[nodiscard]] static Result<T> to_result(hcl_status_code code,
                                             T value,
                                             const RegisterContext& context) {
        if (code == HCL_STATUS_OK) {
            return Result<T>::from_value(std::move(value));
        }
        return Result<T>::from_error(to_status_code(code), context);
    }
};

}  // namespace hcl