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

#include "hcl/drivers/driveonchip.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using HclPyStatus = hcl::pybind::Status;
using HclPyResult = hcl::pybind::Result;
using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyDriveOnChip {
public:
    PyDriveOnChip(hcl_register_interface* reg, double scale_factor)
        : handle_(hcl_driveonchip_create(reg, scale_factor)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create DriveOnChip driver");
        }
    }

    ~PyDriveOnChip() {
        if (handle_ != nullptr) {
            hcl_driveonchip_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() {
        return to_py_status(hcl_driveonchip_init(handle_));
    }

    py::object set_speed(std::int32_t value) {
        return to_py_status(hcl_driveonchip_set_speed(handle_, value));
    }

    py::object set_and_get_speed(std::int32_t value) {
        std::int32_t result = 0;
        return to_py_result(hcl_driveonchip_set_and_get_speed(handle_, value, &result), result);
    }

    py::object get_speed() {
        std::int32_t value = 0;
        return to_py_result(hcl_driveonchip_get_speed(handle_, &value), value);
    }

    py::object get_speed_setpoint() {
        std::int32_t value = 0;
        return to_py_result(hcl_driveonchip_get_speed_setpoint(handle_, &value), value);
    }

    py::object set_position(std::int32_t value) {
        return to_py_status(hcl_driveonchip_set_position(handle_, value));
    }

    py::object set_and_get_position(std::int32_t value) {
        double result = 0.0;
        return to_py_result(hcl_driveonchip_set_and_get_position(handle_, value, &result), result);
    }

    py::object get_position() {
        double value = 0.0;
        return to_py_result(hcl_driveonchip_get_position(handle_, &value), value);
    }

    py::object get_position_setpoint() {
        std::int32_t value = 0;
        return to_py_result(hcl_driveonchip_get_position_setpoint(handle_, &value), value);
    }

    py::object set_position_speed_limit(std::int32_t value) {
        return to_py_status(hcl_driveonchip_set_position_speed_limit(handle_, value));
    }

    py::object get_position_speed_limit() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_driveonchip_get_position_speed_limit(handle_, &value), value);
    }

    py::object set_wave_demo_mode(std::uint32_t value) {
        return to_py_status(hcl_driveonchip_set_wave_demo_mode(handle_, value));
    }

    py::object get_wave_demo_mode() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_driveonchip_get_wave_demo_mode(handle_, &value), value);
    }

    py::object set_wave_demo_waveform(std::uint32_t value) {
        return to_py_status(hcl_driveonchip_set_wave_demo_waveform(handle_, value));
    }

    py::object get_wave_demo_waveform() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_driveonchip_get_wave_demo_waveform(handle_, &value), value);
    }

    py::object set_wave_demo_amplitude(std::int32_t value) {
        return to_py_status(hcl_driveonchip_set_wave_demo_amplitude(handle_, value));
    }

    py::object get_wave_demo_amplitude() {
        std::int32_t value = 0;
        return to_py_result(hcl_driveonchip_get_wave_demo_amplitude(handle_, &value), value);
    }

    py::object set_wave_demo_period(std::uint32_t value) {
        return to_py_status(hcl_driveonchip_set_wave_demo_period(handle_, value));
    }

    py::object get_wave_demo_period() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_driveonchip_get_wave_demo_period(handle_, &value), value);
    }

    py::object set_wave_demo_offset(std::int32_t value) {
        return to_py_status(hcl_driveonchip_set_wave_demo_offset(handle_, value));
    }

    py::object get_wave_demo_offset() {
        std::int32_t value = 0;
        return to_py_result(hcl_driveonchip_get_wave_demo_offset(handle_, &value), value);
    }

    py::object wave_demo_reset() {
        return to_py_status(hcl_driveonchip_wave_demo_reset(handle_));
    }

    py::object wave_demo_update() {
        return to_py_status(hcl_driveonchip_wave_demo_update(handle_));
    }

    py::object set_dump_mode(std::uint32_t value) {
        return to_py_status(hcl_driveonchip_set_dump_mode(handle_, value));
    }

    py::object get_dump_mode() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_driveonchip_get_dump_mode(handle_, &value), value);
    }

private:
    hcl_driveonchip* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(driveonchip, m) {
    m.doc() = "DriveOnChip driver bindings";

    py::class_<PyDriveOnChip, std::shared_ptr<PyDriveOnChip>>(m, "DriveOnChip")
        .def(py::init([](py::object reg, double scale_factor) {
                if (reg.is_none()) {
                    throw std::invalid_argument("register interface must not be null");
                }
                if (!py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("register interface is invalid");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyDriveOnChip>(handle, scale_factor);
            }),
            py::arg("reg"),
            py::arg("scale_factor") = 182.0,
            py::keep_alive<1, 2>())
        .def("init", &PyDriveOnChip::init)
        .def("set_speed", &PyDriveOnChip::set_speed, py::arg("value"))
        .def("set_and_get_speed", &PyDriveOnChip::set_and_get_speed, py::arg("value"))
        .def("get_speed", &PyDriveOnChip::get_speed)
        .def("get_speed_setpoint", &PyDriveOnChip::get_speed_setpoint)
        .def("set_position", &PyDriveOnChip::set_position, py::arg("value"))
        .def("set_and_get_position", &PyDriveOnChip::set_and_get_position, py::arg("value"))
        .def("get_position", &PyDriveOnChip::get_position)
        .def("get_position_setpoint", &PyDriveOnChip::get_position_setpoint)
        .def("set_position_speed_limit", &PyDriveOnChip::set_position_speed_limit, py::arg("value"))
        .def("get_position_speed_limit", &PyDriveOnChip::get_position_speed_limit)
        .def("set_wave_demo_mode", &PyDriveOnChip::set_wave_demo_mode, py::arg("value"))
        .def("get_wave_demo_mode", &PyDriveOnChip::get_wave_demo_mode)
        .def("set_wave_demo_waveform", &PyDriveOnChip::set_wave_demo_waveform, py::arg("value"))
        .def("get_wave_demo_waveform", &PyDriveOnChip::get_wave_demo_waveform)
        .def("set_wave_demo_amplitude", &PyDriveOnChip::set_wave_demo_amplitude, py::arg("value"))
        .def("get_wave_demo_amplitude", &PyDriveOnChip::get_wave_demo_amplitude)
        .def("set_wave_demo_period", &PyDriveOnChip::set_wave_demo_period, py::arg("value"))
        .def("get_wave_demo_period", &PyDriveOnChip::get_wave_demo_period)
        .def("set_wave_demo_offset", &PyDriveOnChip::set_wave_demo_offset, py::arg("value"))
        .def("get_wave_demo_offset", &PyDriveOnChip::get_wave_demo_offset)
        .def("wave_demo_reset", &PyDriveOnChip::wave_demo_reset)
        .def("wave_demo_update", &PyDriveOnChip::wave_demo_update)
        .def("set_dump_mode", &PyDriveOnChip::set_dump_mode, py::arg("value"))
        .def("get_dump_mode", &PyDriveOnChip::get_dump_mode);
}
