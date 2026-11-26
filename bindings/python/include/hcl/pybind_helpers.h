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

#include <pybind11/pybind11.h>

#include "hcl/register_interface.h"

namespace py = pybind11;

namespace hcl::pybind {

struct RegisterContext {
    std::uint32_t base_address{0u};
    std::uint32_t offset{0u};
    std::uint32_t value{0u};
};

struct Status {
    hcl_status_code code{HCL_STATUS_OK};
    RegisterContext ctx{};
    bool has_context{false};

    [[nodiscard]] bool ok() const { return code == HCL_STATUS_OK; }
};

struct Result {
    Status status{};
    std::uint32_t value{0u};

    [[nodiscard]] bool ok() const { return status.ok(); }
};

inline hcl_register_context to_hcl_register_context(const RegisterContext& context) {
    return hcl_register_context{context.base_address, context.offset, context.value};
}

inline hcl_status to_hcl_status(const Status& status) {
    return hcl_status{status.code, to_hcl_register_context(status.ctx), status.has_context};
}

inline RegisterContext to_py_register_context(const hcl_register_context& context) {
    return RegisterContext{context.base_address, context.offset, context.value};
}

inline RegisterContext to_pb_register_context(const hcl_register_context& context) {
    return to_py_register_context(context);
}

inline Status to_pb_status(const hcl_status& status) {
    RegisterContext context{};
    if (status.has_context) {
        context = to_py_register_context(status.ctx);
    }
    return Status{status.code, context, status.has_context};
}

inline py::module_ core_module() {
    return py::module_::import("altera_host_control");
}

inline py::object to_py_register_context(const RegisterContext& context) {
    py::module_ core = core_module();
    py::object py_context = core.attr("RegisterContext")();
    py_context.attr("base_address") = context.base_address;
    py_context.attr("offset") = context.offset;
    py_context.attr("value") = context.value;
    return py_context;
}

inline py::object to_py_status(const Status& status) {
    py::module_ core = core_module();
    if (status.has_context) {
        return core.attr("Status").attr("error")(status.code,
                                                 to_py_register_context(status.ctx));
    }
    return core.attr("Status").attr("error")(status.code, py::none());
}

inline py::object to_py_status(const hcl_status& status) {
    return to_py_status(to_pb_status(status));
}

inline py::object to_py_result(const Result& result) {
    py::module_ core = core_module();
    py::object py_result = core.attr("Result")();
    py_result.attr("value") = result.value;
    py_result.attr("status") = to_py_status(result.status);
    return py_result;
}

inline py::object to_py_result(const hcl_status& status, std::uint32_t value) {
    py::module_ core = core_module();
    py::object py_result = core.attr("Result")();
    py_result.attr("value") = value;
    py_result.attr("status") = to_py_status(status);
    return py_result;
}

inline py::object to_py_result(const hcl_status& status, py::object value) {
    py::module_ core = core_module();
    py::object py_result = core.attr("Result")();
    py_result.attr("value") = value;
    py_result.attr("status") = to_py_status(status);
    return py_result;
}

}  // namespace hcl::pybind
