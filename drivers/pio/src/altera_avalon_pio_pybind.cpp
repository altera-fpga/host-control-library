/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>

#include "hcl/drivers/altera_avalon_pio.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraAvalonPIO {
public:
    PyAlteraAvalonPIO(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_avalon_pio_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create Altera Avalon PIO driver");
        }
    }

    ~PyAlteraAvalonPIO() {
        if (handle_ != nullptr) {
            hcl_altera_avalon_pio_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object read_data() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_avalon_pio_read_data(handle_, &value), value);
    }

    py::object write_data(std::uint32_t value) {
        return to_py_status(hcl_altera_avalon_pio_write_data(handle_, value));
    }

    py::object read_direction() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_avalon_pio_read_direction(handle_, &value), value);
    }

    py::object write_direction(std::uint32_t value) {
        return to_py_status(hcl_altera_avalon_pio_write_direction(handle_, value));
    }

    py::object read_irq_mask() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_avalon_pio_read_irq_mask(handle_, &value), value);
    }

    py::object write_irq_mask(std::uint32_t value) {
        return to_py_status(hcl_altera_avalon_pio_write_irq_mask(handle_, value));
    }

    py::object read_edge_cap() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_avalon_pio_read_edge_cap(handle_, &value), value);
    }

    py::object clear_edge_cap(std::uint32_t value) {
        return to_py_status(hcl_altera_avalon_pio_clear_edge_cap(handle_, value));
    }

    py::object set_bits(std::uint32_t value) {
        return to_py_status(hcl_altera_avalon_pio_set_bits(handle_, value));
    }

    py::object clear_bits(std::uint32_t value) {
        return to_py_status(hcl_altera_avalon_pio_clear_bits(handle_, value));
    }

private:
    hcl_altera_avalon_pio* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_avalon_pio, m) {
    m.doc() = "Altera Avalon PIO driver bindings";

    py::class_<PyAlteraAvalonPIO, std::shared_ptr<PyAlteraAvalonPIO>>(m, "AlteraAvalonPIO")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none()) {
                    throw std::invalid_argument("register interface must not be null");
                }
                if (!py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("register interface is invalid");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraAvalonPIO>(handle, base_address);
            }),
            py::arg("reg"),
            py::arg("base_address") = 0u,
            py::keep_alive<1, 2>())
        .def("read_data", &PyAlteraAvalonPIO::read_data)
        .def("write_data", &PyAlteraAvalonPIO::write_data, py::arg("value"))
        .def("read_direction", &PyAlteraAvalonPIO::read_direction)
        .def("write_direction", &PyAlteraAvalonPIO::write_direction, py::arg("value"))
        .def("read_irq_mask", &PyAlteraAvalonPIO::read_irq_mask)
        .def("write_irq_mask", &PyAlteraAvalonPIO::write_irq_mask, py::arg("value"))
        .def("read_edge_cap", &PyAlteraAvalonPIO::read_edge_cap)
        .def("clear_edge_cap", &PyAlteraAvalonPIO::clear_edge_cap, py::arg("value"))
        .def("set_bits", &PyAlteraAvalonPIO::set_bits, py::arg("value"))
        .def("clear_bits", &PyAlteraAvalonPIO::clear_bits, py::arg("value"));
}
