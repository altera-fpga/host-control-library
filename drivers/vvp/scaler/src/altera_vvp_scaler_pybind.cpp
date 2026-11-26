/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hcl/drivers/altera_vvp_scaler.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraVvpScaler {
public:
    PyAlteraVvpScaler(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_vvp_scaler_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create VVP scaler");
        }
    }

    ~PyAlteraVvpScaler() {
        if (handle_ != nullptr) {
            hcl_altera_vvp_scaler_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() { return to_py_status(hcl_altera_vvp_scaler_init(handle_)); }

    py::object set_output_width(std::uint32_t width) {
        return to_py_status(hcl_altera_vvp_scaler_set_output_width(handle_, width));
    }

    py::object set_output_height(std::uint32_t height) {
        return to_py_status(hcl_altera_vvp_scaler_set_output_height(handle_, height));
    }

    py::object set_quantized_coeffs(bool horizontal,
                                    const std::vector<std::int32_t>& coefficients,
                                    std::uint8_t bank,
                                    std::uint8_t phase) {
        return to_py_status(hcl_altera_vvp_scaler_set_quantized_coeffs(
            handle_, horizontal, coefficients.data(),
            static_cast<std::uint8_t>(coefficients.size()), bank, phase));
    }

    py::object set_coeffs(bool horizontal,
                          const std::vector<float>& coefficients,
                          std::uint8_t bank,
                          std::uint8_t phase) {
        return to_py_status(hcl_altera_vvp_scaler_set_coeffs(
            handle_, horizontal, coefficients.data(),
            static_cast<std::uint8_t>(coefficients.size()), bank, phase));
    }

    py::object is_running() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_scaler_is_running(handle_, &value), py::cast(value));
    }

    py::object commit_is_pending() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_scaler_commit_is_pending(handle_, &value), py::cast(value));
    }

    py::object get_status() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_vvp_scaler_get_status(handle_, &value), value);
    }

    py::object commit() { return to_py_status(hcl_altera_vvp_scaler_commit(handle_)); }

    std::uint32_t get_max_input_width() const {
        return hcl_altera_vvp_scaler_get_max_input_width(handle_);
    }

    std::uint32_t get_max_output_width() const {
        return hcl_altera_vvp_scaler_get_max_output_width(handle_);
    }

private:
    hcl_altera_vvp_scaler* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_vvp_scaler, m) {
    m.doc() = "Altera VVP scaler driver bindings";

    py::enum_<hcl_altera_vvp_scaler_algorithm>(m, "ScalingAlgorithm")
        .value("NEAREST_NEIGHBOR", HCL_VVP_SCALER_ALGORITHM_NEAREST_NEIGHBOR)
        .value("BILINEAR", HCL_VVP_SCALER_ALGORITHM_BILINEAR)
        .value("POLYPHASE", HCL_VVP_SCALER_ALGORITHM_POLYPHASE);

    py::class_<PyAlteraVvpScaler, std::shared_ptr<PyAlteraVvpScaler>>(m, "AlteraVvpScaler")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none() || !py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("invalid register interface");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraVvpScaler>(handle, base_address);
            }),
            py::arg("reg"), py::arg("base_address") = 0u, py::keep_alive<1, 2>())
        .def("init", &PyAlteraVvpScaler::init)
        .def("set_output_width", &PyAlteraVvpScaler::set_output_width)
        .def("set_output_height", &PyAlteraVvpScaler::set_output_height)
        .def("set_quantized_coeffs", &PyAlteraVvpScaler::set_quantized_coeffs)
        .def("set_coeffs", &PyAlteraVvpScaler::set_coeffs)
        .def("is_running", &PyAlteraVvpScaler::is_running)
        .def("commit_is_pending", &PyAlteraVvpScaler::commit_is_pending)
        .def("get_status", &PyAlteraVvpScaler::get_status)
        .def("commit", &PyAlteraVvpScaler::commit)
        .def("get_max_input_width", &PyAlteraVvpScaler::get_max_input_width)
        .def("get_max_output_width", &PyAlteraVvpScaler::get_max_output_width);
}
