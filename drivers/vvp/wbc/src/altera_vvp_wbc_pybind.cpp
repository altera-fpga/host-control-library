/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>

#include "hcl/drivers/altera_vvp_wbc.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraVvpWbc {
public:
    PyAlteraVvpWbc(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_vvp_wbc_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create VVP WBC");
        }
    }

    ~PyAlteraVvpWbc() {
        if (handle_ != nullptr) {
            hcl_altera_vvp_wbc_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() { return to_py_status(hcl_altera_vvp_wbc_init(handle_)); }

    py::object set_width(std::uint32_t width) {
        return to_py_status(hcl_altera_vvp_wbc_set_img_info_width(handle_, width));
    }

    py::object set_height(std::uint32_t height) {
        return to_py_status(hcl_altera_vvp_wbc_set_img_info_height(handle_, height));
    }

    py::object set_bypass(bool bypass) {
        return to_py_status(hcl_altera_vvp_wbc_set_bypass(handle_, bypass));
    }

    py::object set_phase(std::uint8_t phase) {
        return to_py_status(hcl_altera_vvp_wbc_set_cfa_phase(handle_, phase));
    }

    py::object set_scaler(std::uint32_t scaler, std::uint32_t value) {
        switch (scaler) {
            case 0u:
                return to_py_status(hcl_altera_vvp_wbc_set_cfa_00_color_scaler(handle_, value));
            case 1u:
                return to_py_status(hcl_altera_vvp_wbc_set_cfa_01_color_scaler(handle_, value));
            case 2u:
                return to_py_status(hcl_altera_vvp_wbc_set_cfa_10_color_scaler(handle_, value));
            default:
                return to_py_status(hcl_altera_vvp_wbc_set_cfa_11_color_scaler(handle_, value));
        }
    }

    py::object is_running() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_wbc_is_running(handle_, &value), py::cast(value));
    }

    py::object get_status() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_vvp_wbc_get_status(handle_, &value), value);
    }

    py::object commit() { return to_py_status(hcl_altera_vvp_wbc_commit(handle_)); }

private:
    hcl_altera_vvp_wbc* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_vvp_wbc, m) {
    m.doc() = "Altera VVP wide binning correction driver bindings";

    py::class_<PyAlteraVvpWbc, std::shared_ptr<PyAlteraVvpWbc>>(m, "AlteraVvpWbc")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none() || !py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("invalid register interface");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraVvpWbc>(handle, base_address);
            }),
            py::arg("reg"), py::arg("base_address") = 0u, py::keep_alive<1, 2>())
        .def("init", &PyAlteraVvpWbc::init)
        .def("set_img_info_width", &PyAlteraVvpWbc::set_width)
        .def("set_img_info_height", &PyAlteraVvpWbc::set_height)
        .def("set_bypass", &PyAlteraVvpWbc::set_bypass)
        .def("set_cfa_phase", &PyAlteraVvpWbc::set_phase)
        .def("set_cfa_color_scaler", &PyAlteraVvpWbc::set_scaler)
        .def("is_running", &PyAlteraVvpWbc::is_running)
        .def("get_status", &PyAlteraVvpWbc::get_status)
        .def("commit", &PyAlteraVvpWbc::commit);
}
