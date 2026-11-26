/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>

#include "hcl/drivers/altera_vvp_tpg.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraVvpTpg {
public:
    PyAlteraVvpTpg(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_vvp_tpg_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create VVP TPG");
        }
    }

    ~PyAlteraVvpTpg() {
        if (handle_ != nullptr) {
            hcl_altera_vvp_tpg_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() { return to_py_status(hcl_altera_vvp_tpg_init(handle_)); }

    py::object set_width(std::uint32_t width) {
        return to_py_status(hcl_altera_vvp_tpg_set_img_info_width(handle_, width));
    }

    py::object set_height(std::uint32_t height) {
        return to_py_status(hcl_altera_vvp_tpg_set_img_info_height(handle_, height));
    }

    py::object set_pattern(std::uint8_t pattern) {
        return to_py_status(hcl_altera_vvp_tpg_set_pattern(handle_, pattern));
    }

    py::object enable(bool enabled) {
        return to_py_status(hcl_altera_vvp_tpg_enable(handle_, enabled));
    }

    py::object start() { return to_py_status(hcl_altera_vvp_tpg_start(handle_)); }
    py::object stop() { return to_py_status(hcl_altera_vvp_tpg_stop(handle_)); }

    py::object is_running() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_tpg_is_running(handle_, &value), py::cast(value));
    }

    py::object is_enabled() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_tpg_is_enabled(handle_, &value), py::cast(value));
    }

    py::object get_status() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_vvp_tpg_get_status(handle_, &value), value);
    }

    py::object commit() { return to_py_status(hcl_altera_vvp_tpg_commit(handle_)); }

    std::uint8_t get_num_patterns() const { return hcl_altera_vvp_tpg_get_num_patterns(handle_); }
    std::uint8_t get_bits_per_sample() const { return hcl_altera_vvp_tpg_get_bits_per_sample(handle_); }

private:
    hcl_altera_vvp_tpg* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_vvp_tpg, m) {
    m.doc() = "Altera VVP test pattern generator bindings";

    py::enum_<hcl_altera_vvp_tpg_pattern_type>(m, "PatternType")
        .value("BARS", HCL_VVP_TPG_PATTERN_TYPE_BARS)
        .value("UNIFORM", HCL_VVP_TPG_PATTERN_TYPE_UNIFORM)
        .value("PATHOLOGICAL", HCL_VVP_TPG_PATTERN_TYPE_PATHOLOGICAL)
        .value("ZONE_PLATE", HCL_VVP_TPG_PATTERN_TYPE_ZONE_PLATE)
        .value("DIGITAL_CLOCK", HCL_VVP_TPG_PATTERN_TYPE_DIGITAL_CLOCK)
        .value("SIGNALTAP", HCL_VVP_TPG_PATTERN_TYPE_SIGNALTAP);

    py::enum_<hcl_altera_vvp_tpg_bars_type>(m, "BarsType")
        .value("COLOR", HCL_VVP_TPG_BARS_TYPE_COLOR)
        .value("GREY", HCL_VVP_TPG_BARS_TYPE_GREY)
        .value("BLACK_WHITE", HCL_VVP_TPG_BARS_TYPE_BLACK_WHITE)
        .value("MIXED", HCL_VVP_TPG_BARS_TYPE_MIXED);

    py::class_<PyAlteraVvpTpg, std::shared_ptr<PyAlteraVvpTpg>>(m, "AlteraVvpTpg")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none() || !py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("invalid register interface");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraVvpTpg>(handle, base_address);
            }),
            py::arg("reg"), py::arg("base_address") = 0u, py::keep_alive<1, 2>())
        .def("init", &PyAlteraVvpTpg::init)
        .def("set_img_info_width", &PyAlteraVvpTpg::set_width)
        .def("set_img_info_height", &PyAlteraVvpTpg::set_height)
        .def("set_pattern", &PyAlteraVvpTpg::set_pattern)
        .def("enable", &PyAlteraVvpTpg::enable)
        .def("start", &PyAlteraVvpTpg::start)
        .def("stop", &PyAlteraVvpTpg::stop)
        .def("is_running", &PyAlteraVvpTpg::is_running)
        .def("is_enabled", &PyAlteraVvpTpg::is_enabled)
        .def("get_status", &PyAlteraVvpTpg::get_status)
        .def("commit", &PyAlteraVvpTpg::commit)
        .def("get_num_patterns", &PyAlteraVvpTpg::get_num_patterns)
        .def("get_bits_per_sample", &PyAlteraVvpTpg::get_bits_per_sample);
}
