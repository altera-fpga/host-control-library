/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>

#include "hcl/drivers/altera_vvp_pip_conv.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraVvpPipConv {
public:
    PyAlteraVvpPipConv(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_vvp_pip_conv_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create VVP PIP converter");
        }
    }

    ~PyAlteraVvpPipConv() {
        if (handle_ != nullptr) {
            hcl_altera_vvp_pip_conv_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() { return to_py_status(hcl_altera_vvp_pip_conv_init(handle_)); }

    py::object set_width(std::uint32_t width) {
        return to_py_status(hcl_altera_vvp_pip_conv_set_img_info_width(handle_, width));
    }

    py::object set_height(std::uint32_t height) {
        return to_py_status(hcl_altera_vvp_pip_conv_set_img_info_height(handle_, height));
    }

    py::object is_running() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_pip_conv_is_running(handle_, &value), py::cast(value));
    }

    py::object get_status() {
        std::uint8_t value = 0u;
        return to_py_result(hcl_altera_vvp_pip_conv_get_status(handle_, &value), value);
    }

private:
    hcl_altera_vvp_pip_conv* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_vvp_pip_conv, m) {
    m.doc() = "Altera VVP PIP converter driver bindings";

    py::class_<PyAlteraVvpPipConv, std::shared_ptr<PyAlteraVvpPipConv>>(m, "AlteraVvpPipConv")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none() || !py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("invalid register interface");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraVvpPipConv>(handle, base_address);
            }),
            py::arg("reg"), py::arg("base_address") = 0u, py::keep_alive<1, 2>())
        .def("init", &PyAlteraVvpPipConv::init)
        .def("set_img_info_width", &PyAlteraVvpPipConv::set_width)
        .def("set_img_info_height", &PyAlteraVvpPipConv::set_height)
        .def("is_running", &PyAlteraVvpPipConv::is_running)
        .def("get_status", &PyAlteraVvpPipConv::get_status);
}
