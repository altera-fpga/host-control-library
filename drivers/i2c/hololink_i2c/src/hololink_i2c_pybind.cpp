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

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include <pybind11/pybind11.h>

#include "hcl/interfaces/i2c.h"
#include "hcl/pybind_helpers.h"

namespace py = pybind11;

namespace {

using HclPyStatus = hcl::pybind::Status;
using HclPyResult = hcl::pybind::Result;
using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

struct PyHololinkI2CState {
    py::object i2c;
    bool started{false};
};

static hcl_status i2c_status_error(hcl_status_code code,
                                   std::uint8_t address,
                                   std::uint32_t reg,
                                   std::uint32_t data) {
    const hcl_register_context context = {
        static_cast<std::uint32_t>(address),
        static_cast<std::uint32_t>(reg),
        static_cast<std::uint32_t>(data),
    };
    return hcl_status_error(code, &context);
}

static hcl_status vtable_write(void* context,
                               std::uint8_t address,
                               std::uint8_t reg,
                               std::uint8_t data) {
    auto* state = static_cast<PyHololinkI2CState*>(context);
    if (state == nullptr || state->i2c.is_none()) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    try {
        const std::vector<std::uint8_t> write_bytes{reg, data};
        const auto read_bytes = state->i2c.attr("i2c_transaction")(address, py::cast(write_bytes), 0u);
        return read_bytes.cast<std::vector<std::uint8_t>>().empty()
                   ? hcl_status_ok()
                   : i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, data);
    } catch (const std::exception&) {
        return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, data);
    }
}

static hcl_status vtable_read(void* context,
                              std::uint8_t address,
                              std::uint8_t reg,
                              std::uint8_t* out_value) {
    if (out_value == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    auto* state = static_cast<PyHololinkI2CState*>(context);
    if (state == nullptr || state->i2c.is_none()) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    try {
        const std::vector<std::uint8_t> write_bytes{reg};
        const auto read_bytes = state->i2c.attr("i2c_transaction")(address, py::cast(write_bytes), 1u);
        const auto bytes = read_bytes.cast<std::vector<std::uint8_t>>();
        if (bytes.size() != 1u) {
            return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
        }
        *out_value = bytes.front();
        return hcl_status_ok();
    } catch (const std::exception&) {
        return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
    }
}

static hcl_status vtable_write_extended(void* context,
                                        std::uint8_t address,
                                        std::uint16_t reg,
                                        std::uint8_t data) {
    auto* state = static_cast<PyHololinkI2CState*>(context);
    if (state == nullptr || state->i2c.is_none()) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    try {
        std::vector<std::uint8_t> write_bytes(3u);
        write_bytes[0] = static_cast<std::uint8_t>((reg >> 8u) & 0xFFu);
        write_bytes[1] = static_cast<std::uint8_t>(reg & 0xFFu);
        write_bytes[2] = data;
        const auto read_bytes = state->i2c.attr("i2c_transaction")(address, py::cast(write_bytes), 0u);
        return read_bytes.cast<std::vector<std::uint8_t>>().empty()
                   ? hcl_status_ok()
                   : i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, data);
    } catch (const std::exception&) {
        return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, data);
    }
}

static hcl_status vtable_read_extended(void* context,
                                      std::uint8_t address,
                                      std::uint16_t reg,
                                      std::uint8_t* out_value) {
    if (out_value == nullptr) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    auto* state = static_cast<PyHololinkI2CState*>(context);
    if (state == nullptr || state->i2c.is_none()) {
        return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
    }

    try {
        std::vector<std::uint8_t> write_bytes(2u);
        write_bytes[0] = static_cast<std::uint8_t>((reg >> 8u) & 0xFFu);
        write_bytes[1] = static_cast<std::uint8_t>(reg & 0xFFu);
        const auto read_bytes = state->i2c.attr("i2c_transaction")(address, py::cast(write_bytes), 1u);
        const auto bytes = read_bytes.cast<std::vector<std::uint8_t>>();
        if (bytes.size() != 1u) {
            return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
        }
        *out_value = bytes.front();
        return hcl_status_ok();
    } catch (const std::exception&) {
        return i2c_status_error(HCL_STATUS_TRANSPORT_ERROR, address, reg, 0u);
    }
}

static const hcl_i2c_interface vtable = {
    .init = [](hcl_i2c* i2c) {
        auto* state = static_cast<PyHololinkI2CState*>(i2c != nullptr ? i2c->context : nullptr);
        if (state == nullptr || state->i2c.is_none()) {
            return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL);
        }
        if (!state->started) {
            state->started = true;
        }
        return hcl_status_ok();
    },
    .write = [](hcl_i2c* i2c, std::uint8_t address, std::uint8_t reg, std::uint8_t data) {
        auto* state = static_cast<PyHololinkI2CState*>(i2c != nullptr ? i2c->context : nullptr);
        return state == nullptr ? hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL)
                               : vtable_write(state, address, reg, data);
    },
    .read = [](hcl_i2c* i2c, std::uint8_t address, std::uint8_t reg, std::uint8_t* out_value) {
        auto* state = static_cast<PyHololinkI2CState*>(i2c != nullptr ? i2c->context : nullptr);
        return state == nullptr ? hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL)
                               : vtable_read(state, address, reg, out_value);
    },
    .write_extended = [](hcl_i2c* i2c, std::uint8_t address, std::uint16_t reg, std::uint8_t data) {
        auto* state = static_cast<PyHololinkI2CState*>(i2c != nullptr ? i2c->context : nullptr);
        return state == nullptr ? hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL)
                               : vtable_write_extended(state, address, reg, data);
    },
    .read_extended = [](hcl_i2c* i2c, std::uint8_t address, std::uint16_t reg, std::uint8_t* out_value) {
        auto* state = static_cast<PyHololinkI2CState*>(i2c != nullptr ? i2c->context : nullptr);
        return state == nullptr ? hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, NULL)
                               : vtable_read_extended(state, address, reg, out_value);
    },
};

class PyHololinkI2C {
public:
    explicit PyHololinkI2C(py::object i2c)
        : state_(std::make_unique<PyHololinkI2CState>()) {
        if (i2c.is_none()) {
            throw std::invalid_argument("hololink i2c instance must not be None");
        }
        if (!py::hasattr(i2c, "i2c_transaction")) {
            throw std::invalid_argument(
                "hololink i2c is invalid");
        }

        state_->i2c = std::move(i2c);
        state_->started = false;
        handle_ = hcl_i2c_create(state_.get(), &vtable);
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create Hololink I2C driver");
        }
    }

    ~PyHololinkI2C() {
        if (handle_ != nullptr) {
            hcl_i2c_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() {
        if (handle_ == nullptr || state_ == nullptr || state_->i2c.is_none()) {
            return to_py_status(hcl_status{HCL_STATUS_INVALID_ARGUMENT, {}, false});
        }
        state_->started = true;
        return to_py_status(hcl_i2c_init(handle_));
    }

    py::object write(std::uint8_t address, std::uint8_t reg, std::uint8_t data) {
        return to_py_status(hcl_i2c_write(handle_, address, reg, data));
    }

    py::object read(std::uint8_t address, std::uint8_t reg) {
        std::uint8_t value = 0u;
        const hcl_status status = hcl_i2c_read(handle_, address, reg, &value);
        return to_py_result(status, static_cast<std::uint32_t>(value));
    }

    py::object write_extended(std::uint8_t address, std::uint16_t reg, std::uint8_t data) {
        return to_py_status(hcl_i2c_write_extended(handle_, address, reg, data));
    }

    py::object read_extended(std::uint8_t address, std::uint16_t reg) {
        std::uint8_t value = 0u;
        const hcl_status status = hcl_i2c_read_extended(handle_, address, reg, &value);
        return to_py_result(status, static_cast<std::uint32_t>(value));
    }

private:
    std::unique_ptr<PyHololinkI2CState> state_;
    hcl_i2c* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(hololink_i2c, m) {
    m.doc() = "Hololink-backed I2C adapter";

    py::class_<PyHololinkI2C, std::shared_ptr<PyHololinkI2C>>(m, "HololinkI2C")
        .def(py::init<py::object>(), py::arg("i2c"))
        .def("init", &PyHololinkI2C::init)
        .def("write", &PyHololinkI2C::write,
             py::arg("address"),
             py::arg("reg"),
             py::arg("data"))
        .def("read", &PyHololinkI2C::read,
             py::arg("address"),
             py::arg("reg"))
        .def("write_extended", &PyHololinkI2C::write_extended,
             py::arg("address"),
             py::arg("reg"),
             py::arg("data"))
        .def("read_extended", &PyHololinkI2C::read_extended,
             py::arg("address"),
             py::arg("reg"));
}
