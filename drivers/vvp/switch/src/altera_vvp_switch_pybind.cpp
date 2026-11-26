/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>

#include "hcl/drivers/altera_vvp_switch.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraVvpSwitch {
public:
    PyAlteraVvpSwitch(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_vvp_switch_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create VVP switch");
        }
    }

    ~PyAlteraVvpSwitch() {
        if (handle_ != nullptr) {
            hcl_altera_vvp_switch_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() { return to_py_status(hcl_altera_vvp_switch_init(handle_)); }

    py::object set_input(std::uint8_t input, std::uint32_t config) {
        return to_py_status(hcl_altera_vvp_switch_set_input_config(
            handle_, input, static_cast<hcl_altera_vvp_switch_input_config>(config)));
    }

    py::object set_output(std::uint8_t output, bool enabled, std::uint8_t input) {
        return to_py_status(hcl_altera_vvp_switch_set_output_config(handle_, output, enabled, input));
    }

    py::object commit() { return to_py_status(hcl_altera_vvp_switch_commit(handle_)); }

    py::object is_running() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_switch_is_running(handle_, &value), py::cast(value));
    }

    py::object commit_is_pending() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_switch_commit_is_pending(handle_, &value), py::cast(value));
    }

    py::object get_status() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_vvp_switch_get_status(handle_, &value), value);
    }

    std::uint8_t get_num_inputs() const { return hcl_altera_vvp_switch_get_num_inputs(handle_); }
    std::uint8_t get_num_outputs() const { return hcl_altera_vvp_switch_get_num_outputs(handle_); }

private:
    hcl_altera_vvp_switch* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_vvp_switch, m) {
    m.doc() = "Altera VVP switch driver bindings";

    py::enum_<hcl_altera_vvp_switch_input_config>(m, "InputConfig")
        .value("DISABLED", HCL_VVP_SWITCH_INPUT_DISABLED)
        .value("ENABLED", HCL_VVP_SWITCH_INPUT_ENABLED)
        .value("CONSUMED", HCL_VVP_SWITCH_INPUT_CONSUMED);

    py::class_<PyAlteraVvpSwitch, std::shared_ptr<PyAlteraVvpSwitch>>(m, "AlteraVvpSwitch")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none() || !py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("invalid register interface");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraVvpSwitch>(handle, base_address);
            }),
            py::arg("reg"), py::arg("base_address") = 0u, py::keep_alive<1, 2>())
        .def("init", &PyAlteraVvpSwitch::init)
        .def("set_input_config", &PyAlteraVvpSwitch::set_input)
        .def("set_output_config", &PyAlteraVvpSwitch::set_output)
        .def("commit", &PyAlteraVvpSwitch::commit)
        .def("is_running", &PyAlteraVvpSwitch::is_running)
        .def("commit_is_pending", &PyAlteraVvpSwitch::commit_is_pending)
        .def("get_status", &PyAlteraVvpSwitch::get_status)
        .def("get_num_inputs", &PyAlteraVvpSwitch::get_num_inputs)
        .def("get_num_outputs", &PyAlteraVvpSwitch::get_num_outputs);
}
