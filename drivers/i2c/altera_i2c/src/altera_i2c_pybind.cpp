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

#include <pybind11/pybind11.h>

#include "hcl/drivers/altera_i2c.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using HclPyStatus = hcl::pybind::Status;
using HclPyResult = hcl::pybind::Result;
using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraI2C {
public:
    PyAlteraI2C(hcl_register_interface* reg,
                std::uint32_t base_address,
                std::uint32_t sys_clk,
                std::uint32_t transaction_timeout_ms)
        : handle_(hcl_altera_i2c_create(reg, base_address, sys_clk, transaction_timeout_ms)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create Altera I2C driver");
        }
    }

    ~PyAlteraI2C() {
        if (handle_ != nullptr) {
            hcl_altera_i2c_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() {
        return to_py_status(hcl_i2c_init(handle_));
    }

    py::object write(std::uint8_t address, std::uint8_t reg, std::uint8_t data) {
        return to_py_status(hcl_i2c_write(handle_, address, reg, data));
    }

    py::object read(std::uint8_t address, std::uint8_t reg) {
        std::uint8_t value = 0u;
        return to_py_result(hcl_i2c_read(handle_, address, reg, &value), value);
    }

    py::object write_extended(std::uint8_t address, std::uint16_t reg, std::uint8_t data) {
        return to_py_status(hcl_i2c_write_extended(handle_, address, reg, data));
    }

    py::object read_extended(std::uint8_t address, std::uint16_t reg) {
        std::uint8_t value = 0u;
        return to_py_result(hcl_i2c_read_extended(handle_, address, reg, &value), value);
    }

private:
    hcl_i2c* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_i2c, m) {
    m.doc() = "Altera-backed I2C adapter";

    py::class_<PyAlteraI2C, std::shared_ptr<PyAlteraI2C>>(m, "AlteraI2C")
        .def(py::init([](py::object reg,
                         std::uint32_t base_address,
                         std::uint32_t sys_clk,
                         std::uint32_t transaction_timeout_ms) {
                if (reg.is_none()) {
                    throw std::invalid_argument("register interface must not be null");
                }
                if (!py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("register interface is invalid");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraI2C>(handle,
                                                     base_address,
                                                     sys_clk,
                                                     transaction_timeout_ms);
            }),
            py::arg("reg"),
            py::arg("base_address"),
            py::arg("sys_clk"),
            py::arg("transaction_timeout_ms") = 1000u,
            py::keep_alive<1, 2>())
        .def("init", &PyAlteraI2C::init)
        .def("write", &PyAlteraI2C::write,
            py::arg("address"),
            py::arg("reg"),
            py::arg("data"))
        .def("read", &PyAlteraI2C::read,
            py::arg("address"),
            py::arg("reg"))
        .def("write_extended", &PyAlteraI2C::write_extended,
            py::arg("address"),
            py::arg("reg"),
            py::arg("data"))
        .def("read_extended", &PyAlteraI2C::read_extended,
            py::arg("address"),
            py::arg("reg"));
}
