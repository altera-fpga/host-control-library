/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>

#include "hcl/drivers/altera_vvp_blc.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraVvpBlc {
public:
    PyAlteraVvpBlc(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_vvp_blc_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create VVP BLC");
        }
    }

    ~PyAlteraVvpBlc() {
        if (handle_ != nullptr) {
            hcl_altera_vvp_blc_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() { return to_py_status(hcl_altera_vvp_blc_init(handle_)); }

    py::object is_running() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_blc_is_running(handle_, &value), py::cast(value));
    }

    py::object commit_is_pending() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_blc_commit_is_pending(handle_, &value), py::cast(value));
    }

    py::object get_status() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_vvp_blc_get_status(handle_, &value), value);
    }

    py::object get_frame_stats() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_vvp_blc_get_frame_stats(handle_, &value), value);
    }

    py::object commit() { return to_py_status(hcl_altera_vvp_blc_commit(handle_)); }
    py::object set_bypass(bool bypass) { return to_py_status(hcl_altera_vvp_blc_set_bypass(handle_, bypass)); }

    py::object set_width(std::uint32_t width) {
        return to_py_status(hcl_altera_vvp_blc_set_img_info_width(handle_, width));
    }

    py::object set_height(std::uint32_t height) {
        return to_py_status(hcl_altera_vvp_blc_set_img_info_height(handle_, height));
    }

    bool get_lite_mode() const { return hcl_altera_vvp_blc_get_lite_mode(handle_); }
    bool get_debug_enabled() const { return hcl_altera_vvp_blc_get_debug_enabled(handle_); }
    bool get_bypass() const { return hcl_altera_vvp_blc_get_bypass(handle_); }

private:
    hcl_altera_vvp_blc* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_vvp_blc, m) {
    m.doc() = "Altera VVP black level correction driver bindings";

    py::class_<PyAlteraVvpBlc, std::shared_ptr<PyAlteraVvpBlc>>(m, "AlteraVvpBlc")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none() || !py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("invalid register interface");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraVvpBlc>(handle, base_address);
            }),
            py::arg("reg"), py::arg("base_address") = 0u, py::keep_alive<1, 2>())
        .def("init", &PyAlteraVvpBlc::init)
        .def("is_running", &PyAlteraVvpBlc::is_running)
        .def("commit_is_pending", &PyAlteraVvpBlc::commit_is_pending)
        .def("get_status", &PyAlteraVvpBlc::get_status)
        .def("get_frame_stats", &PyAlteraVvpBlc::get_frame_stats)
        .def("commit", &PyAlteraVvpBlc::commit)
        .def("set_bypass", &PyAlteraVvpBlc::set_bypass, py::arg("bypass"))
        .def("set_img_info_width", &PyAlteraVvpBlc::set_width, py::arg("width"))
        .def("set_img_info_height", &PyAlteraVvpBlc::set_height, py::arg("height"))
        .def("get_lite_mode", &PyAlteraVvpBlc::get_lite_mode)
        .def("get_debug_enabled", &PyAlteraVvpBlc::get_debug_enabled)
        .def("get_bypass", &PyAlteraVvpBlc::get_bypass);
}
