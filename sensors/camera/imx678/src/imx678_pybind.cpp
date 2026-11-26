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
#include "hcl/sensors/imx678.h"
#include "hcl/sensors/imx678_modes.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyIMX678 {
public:
    PyIMX678(hcl_i2c* i2c, std::uint8_t address)
        : handle_(hcl_imx678_create(i2c, address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create IMX678 sensor");
        }
    }

    ~PyIMX678() {
        if (handle_ != nullptr) {
            hcl_imx678_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init(const hcl_imx678_mode& mode) {
        return to_py_status(hcl_imx678_init(handle_, &mode));
    }

    py::object start() {
        return to_py_status(hcl_imx678_start(handle_));
    }

    py::object stop() {
        return to_py_status(hcl_imx678_stop(handle_));
    }

private:
    hcl_imx678* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(imx678, m) {
    py::class_<PyIMX678, std::shared_ptr<PyIMX678>>(m, "IMX678")
        .def(py::init([](py::object i2c, std::uint8_t address) {
                if (i2c.is_none()) {
                    throw std::invalid_argument("i2c must not be null");
                }
                if (!py::hasattr(i2c, "_handle")) {
                    throw std::invalid_argument("i2c is invalid");
                }
                const std::uintptr_t handle_address = i2c.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_i2c*>(handle_address);
                return std::make_shared<PyIMX678>(handle, address);
            }),
            py::arg("i2c"),
            py::arg("address"))
        .def("init", &PyIMX678::init,
             py::arg("mode") = py::cast(&IMX678_MODE_3840X2160_30FPS_12BIT,
                                        py::return_value_policy::reference))
        .def("start", &PyIMX678::start)
        .def("stop", &PyIMX678::stop);

    py::class_<hcl_imx678_mode>(m, "Mode")
        .def(py::init<>())
        .def("register_count", [](const hcl_imx678_mode& self) { return self.register_count; });

    m.attr("MODE_1280X720_30FPS_10BIT") = py::cast(&IMX678_MODE_1280X720_30FPS_10BIT, py::return_value_policy::reference);
    m.attr("MODE_1280X720_30FPS_12BIT") = py::cast(&IMX678_MODE_1280X720_30FPS_12BIT, py::return_value_policy::reference);
    m.attr("MODE_1280X720_60FPS_10BIT") = py::cast(&IMX678_MODE_1280X720_60FPS_10BIT, py::return_value_policy::reference);
    m.attr("MODE_1280X720_60FPS_12BIT") = py::cast(&IMX678_MODE_1280X720_60FPS_12BIT, py::return_value_policy::reference);
    m.attr("MODE_1920X1080_30FPS_10BIT") = py::cast(&IMX678_MODE_1920X1080_30FPS_10BIT, py::return_value_policy::reference);
    m.attr("MODE_1920X1080_30FPS_12BIT") = py::cast(&IMX678_MODE_1920X1080_30FPS_12BIT, py::return_value_policy::reference);
    m.attr("MODE_1920X1080_60FPS_10BIT") = py::cast(&IMX678_MODE_1920X1080_60FPS_10BIT, py::return_value_policy::reference);
    m.attr("MODE_1920X1080_60FPS_12BIT") = py::cast(&IMX678_MODE_1920X1080_60FPS_12BIT, py::return_value_policy::reference);
    m.attr("MODE_3840X2160_30FPS_10BIT") = py::cast(&IMX678_MODE_3840X2160_30FPS_10BIT, py::return_value_policy::reference);
    m.attr("MODE_3840X2160_30FPS_12BIT") = py::cast(&IMX678_MODE_3840X2160_30FPS_12BIT, py::return_value_policy::reference);
    m.attr("MODE_3840X2160_60FPS_10BIT") = py::cast(&IMX678_MODE_3840X2160_60FPS_10BIT, py::return_value_policy::reference);
    m.attr("MODE_3840X2160_60FPS_12BIT") = py::cast(&IMX678_MODE_3840X2160_60FPS_12BIT, py::return_value_policy::reference);
}
