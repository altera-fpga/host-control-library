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

#include "hcl/pybind_helpers.h"
#include "hcl/sensors/framos_gmsl.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyFramosGmsl {
public:
    PyFramosGmsl(hcl_i2c* i2c,
                 std::uint8_t serializer_address,
                 std::uint8_t deserializer_address,
                 std::uint8_t gpio_address,
                 std::uint8_t sensor_address)
        : handle_(hcl_framos_gmsl_create(i2c,
                                         serializer_address,
                                         deserializer_address,
                                         gpio_address,
                                         sensor_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create FRAMOS GMSL sensor");
        }
    }

    ~PyFramosGmsl() {
        if (handle_ != nullptr) {
            hcl_framos_gmsl_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() {
        return to_py_status(hcl_framos_gmsl_init(handle_));
    }

    py::object read_register(std::uint8_t device_address, std::uint16_t reg) {
        std::uint8_t value = 0u;
        const hcl_status status = hcl_framos_gmsl_read_register(handle_, device_address, reg, &value);
        return to_py_result(status, static_cast<std::uint32_t>(value));
    }

    py::object write_register(std::uint8_t device_address, std::uint16_t reg, std::uint8_t data) {
        return to_py_status(hcl_framos_gmsl_write_register(handle_, device_address, reg, data));
    }

    py::object write_gpio_register(std::uint8_t reg, std::uint8_t data) {
        return to_py_status(hcl_framos_gmsl_write_gpio_register(handle_, reg, data));
    }

private:
    hcl_framos_gmsl* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(framos_gmsl, m) {
    py::class_<PyFramosGmsl, std::shared_ptr<PyFramosGmsl>>(m, "FramosGmsl")
        .def(py::init([](py::object i2c,
                         std::uint8_t serializer_address,
                         std::uint8_t deserializer_address,
                         std::uint8_t gpio_address,
                         std::uint8_t sensor_address) {
                if (i2c.is_none()) {
                    throw std::invalid_argument("i2c must not be null");
                }
                if (!py::hasattr(i2c, "_handle")) {
                    throw std::invalid_argument("i2c must expose a _handle() method");
                }
                const std::uintptr_t handle_address = i2c.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_i2c*>(handle_address);
                return std::make_shared<PyFramosGmsl>(handle,
                                                      serializer_address,
                                                      deserializer_address,
                                                      gpio_address,
                                                      sensor_address);
            }),
            py::arg("i2c"),
            py::arg("serializer_address"),
            py::arg("deserializer_address"),
            py::arg("gpio_address"),
            py::arg("sensor_address"))
        .def("init", &PyFramosGmsl::init)
        .def("read_register", &PyFramosGmsl::read_register,
            py::arg("device_address"),
            py::arg("reg"))
        .def("write_register", &PyFramosGmsl::write_register,
            py::arg("device_address"),
            py::arg("reg"),
            py::arg("data"))
        .def("write_gpio_register", &PyFramosGmsl::write_gpio_register,
            py::arg("reg"),
            py::arg("data"));
}
