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

#include "hcl/drivers/abr_bridge.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_status;

class PyAbrBridge {
public:
    struct Configuration {
        std::uint8_t minor_version{0u};
        std::uint8_t major_version{0u};
        std::uint8_t num_channels{0u};
    };

    struct Control {
        std::uint8_t channel_enable{0u};
    };

    struct ChannelConfig {
        std::uint8_t channel_width{0u};
        std::uint8_t channel_type{0u};
    };

    struct ChannelParams {
        std::uint8_t channel_width{0u};
        std::uint8_t channel_type{0u};
        std::uint16_t num_lines{0u};
        std::uint8_t roce_qp{0u};
        std::uint8_t num_buffers{0u};
        std::uint32_t offset{0u};
        std::vector<std::uint32_t> roce_rkeys;
        std::vector<std::uint64_t> remote_addresses;
        std::uint64_t metadata_address{0u};
        std::uint32_t metadata_key{0u};

        ChannelParams()
            : roce_rkeys(HCL_ABR_BRIDGE_MAX_BUFFERS, 0u),
              remote_addresses(HCL_ABR_BRIDGE_MAX_BUFFERS, 0u) {}
    };

    PyAbrBridge(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_abr_bridge_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create ABR bridge driver");
        }
    }

    ~PyAbrBridge() {
        if (handle_ != nullptr) {
            hcl_abr_bridge_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object read_configuration() {
        hcl_abr_bridge_configuration c_value{};
        hcl_status status = hcl_abr_bridge_read_configuration(handle_, &c_value);
        Configuration value{
            c_value.minor_version,
            c_value.major_version,
            c_value.num_channels,
        };
        return hcl::pybind::to_py_result(status, py::cast(value));
    }

    py::object write_control(const Control& control) {
        hcl_abr_bridge_control c_control{control.channel_enable};
        return to_py_status(hcl_abr_bridge_write_control(handle_, &c_control));
    }

    py::object read_control() {
        hcl_abr_bridge_control c_value{};
        hcl_status status = hcl_abr_bridge_read_control(handle_, &c_value);
        Control value{c_value.channel_enable};
        return hcl::pybind::to_py_result(status, py::cast(value));
    }

    py::object read_status() {
        std::uint32_t value = 0u;
        return hcl::pybind::to_py_result(hcl_abr_bridge_read_status(handle_, &value), value);
    }

    py::object set_channel_enable(std::uint32_t channel) {
        return to_py_status(hcl_abr_bridge_set_channel_enable(handle_, channel));
    }

    py::object read_channel_config(std::uint32_t channel) {
        hcl_abr_bridge_channel_config c_value{};
        hcl_status status = hcl_abr_bridge_read_channel_config(handle_, channel, &c_value);
        ChannelConfig value{
            c_value.channel_width,
            c_value.channel_type,
        };
        return hcl::pybind::to_py_result(status, py::cast(value));
    }

    py::object write_channel_config(std::uint32_t channel, const ChannelConfig& config) {
        hcl_abr_bridge_channel_config c_config{config.channel_width, config.channel_type};
        return to_py_status(hcl_abr_bridge_write_channel_config(handle_, channel, &c_config));
    }

    py::object set_channel_num_lines(std::uint32_t channel, std::uint16_t num_lines) {
        return to_py_status(hcl_abr_bridge_set_channel_num_lines(handle_, channel, num_lines));
    }

    py::object get_channel_num_lines(std::uint32_t channel) {
        std::uint16_t value = 0u;
        return hcl::pybind::to_py_result(hcl_abr_bridge_get_channel_num_lines(handle_, channel, &value), value);
    }

    py::object set_channel_num_buffers(std::uint32_t channel, std::uint8_t num_buffers) {
        return to_py_status(hcl_abr_bridge_set_channel_num_buffers(handle_, channel, num_buffers));
    }

    py::object get_channel_num_buffers(std::uint32_t channel) {
        std::uint8_t value = 0u;
        hcl_status status = hcl_abr_bridge_get_channel_num_buffers(handle_, channel, &value);
        return hcl::pybind::to_py_result(status, value);
    }

    py::object set_channel_load_buffer(std::uint32_t channel, std::uint8_t load_buffer) {
        return to_py_status(hcl_abr_bridge_set_channel_load_buffer(handle_, channel, load_buffer));
    }

    py::object set_channel_roce_qp(std::uint32_t channel, std::uint8_t qp) {
        return to_py_status(hcl_abr_bridge_set_channel_roce_qp(handle_, channel, qp));
    }

    py::object set_channel_roce_rkey(std::uint32_t channel, std::uint8_t buffer, std::uint32_t rkey) {
        return to_py_status(hcl_abr_bridge_set_channel_roce_rkey(handle_, channel, buffer, rkey));
    }

    py::object set_channel_offset(std::uint32_t channel, std::uint32_t offset) {
        return to_py_status(hcl_abr_bridge_set_channel_offset(handle_, channel, offset));
    }

    py::object set_channel_remote_address(std::uint32_t channel, std::uint8_t buffer, std::uint64_t remote_address) {
        return to_py_status(hcl_abr_bridge_set_channel_remote_address(handle_, channel, buffer, remote_address));
    }

    py::object set_channel_metadata_address(std::uint32_t channel, std::uint64_t metadata_address) {
        return to_py_status(hcl_abr_bridge_set_channel_metadata_address(handle_, channel, metadata_address));
    }

    py::object set_channel_metadata_key(std::uint32_t channel, std::uint32_t metadata_key) {
        return to_py_status(hcl_abr_bridge_set_channel_metadata_key(handle_, channel, metadata_key));
    }

    py::object read_measured_vid_clk_hz() {
        std::uint32_t value = 0u;
        return hcl::pybind::to_py_result(hcl_abr_bridge_read_measured_vid_clk_hz(handle_, &value), value);
    }

    py::object read_measured_eth_clk_hz() {
        std::uint32_t value = 0u;
        return hcl::pybind::to_py_result(hcl_abr_bridge_read_measured_eth_clk_hz(handle_, &value), value);
    }

    py::object enable_channel(std::uint32_t channel) {
        return to_py_status(hcl_abr_bridge_enable_channel(handle_, channel));
    }

    py::object configure_channel(std::uint32_t channel, const ChannelParams& params) {
        hcl_abr_bridge_channel_params c_params{};
        c_params.channel_width = params.channel_width;
        c_params.channel_type = params.channel_type;
        c_params.num_lines = params.num_lines;
        c_params.roce_qp = params.roce_qp;
        c_params.num_buffers = params.num_buffers;
        c_params.offset = params.offset;
        c_params.metadata_address = params.metadata_address;
        c_params.metadata_key = params.metadata_key;

        for (std::size_t i = 0u; i < HCL_ABR_BRIDGE_MAX_BUFFERS; ++i) {
            c_params.roce_rkeys[i] = i < params.roce_rkeys.size() ? params.roce_rkeys[i] : 0u;
            c_params.remote_addresses[i] = i < params.remote_addresses.size() ? params.remote_addresses[i] : 0u;
        }

        return to_py_status(hcl_abr_bridge_configure_channel(handle_, channel, &c_params));
    }

private:
    hcl_abr_bridge* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(abr_bridge, m) {
    m.doc() = "ABR bridge driver bindings";

    py::class_<PyAbrBridge::Configuration>(m, "Configuration")
        .def(py::init<>())
        .def_readwrite("minor_version", &PyAbrBridge::Configuration::minor_version)
        .def_readwrite("major_version", &PyAbrBridge::Configuration::major_version)
        .def_readwrite("num_channels", &PyAbrBridge::Configuration::num_channels);

    py::class_<PyAbrBridge::Control>(m, "Control")
        .def(py::init<>())
        .def_readwrite("channel_enable", &PyAbrBridge::Control::channel_enable);

    py::class_<PyAbrBridge::ChannelConfig>(m, "ChannelConfig")
        .def(py::init<>())
        .def_readwrite("channel_width", &PyAbrBridge::ChannelConfig::channel_width)
        .def_readwrite("channel_type", &PyAbrBridge::ChannelConfig::channel_type);

    py::class_<PyAbrBridge::ChannelParams>(m, "ChannelParams")
        .def(py::init<>())
        .def_readwrite("channel_width", &PyAbrBridge::ChannelParams::channel_width)
        .def_readwrite("channel_type", &PyAbrBridge::ChannelParams::channel_type)
        .def_readwrite("num_lines", &PyAbrBridge::ChannelParams::num_lines)
        .def_readwrite("roce_qp", &PyAbrBridge::ChannelParams::roce_qp)
        .def_readwrite("num_buffers", &PyAbrBridge::ChannelParams::num_buffers)
        .def_readwrite("offset", &PyAbrBridge::ChannelParams::offset)
        .def_readwrite("roce_rkeys", &PyAbrBridge::ChannelParams::roce_rkeys)
        .def_readwrite("remote_addresses", &PyAbrBridge::ChannelParams::remote_addresses)
        .def_readwrite("metadata_address", &PyAbrBridge::ChannelParams::metadata_address)
        .def_readwrite("metadata_key", &PyAbrBridge::ChannelParams::metadata_key);

    py::class_<PyAbrBridge, std::shared_ptr<PyAbrBridge>>(m, "AbrBridge")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none()) {
                    throw std::invalid_argument("register interface must not be null");
                }
                if (!py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("register interface is invalid");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAbrBridge>(handle, base_address);
            }),
            py::arg("reg"),
            py::arg("base_address") = 0u,
            py::keep_alive<1, 2>())
        .def("read_configuration", &PyAbrBridge::read_configuration)
        .def("write_control", &PyAbrBridge::write_control, py::arg("control"))
        .def("read_control", &PyAbrBridge::read_control)
        .def("read_status", &PyAbrBridge::read_status)
        .def("set_channel_enable", &PyAbrBridge::set_channel_enable, py::arg("channel"))
        .def("read_channel_config", &PyAbrBridge::read_channel_config, py::arg("channel"))
        .def("write_channel_config", &PyAbrBridge::write_channel_config,
             py::arg("channel"), py::arg("config"))
        .def("set_channel_num_lines", &PyAbrBridge::set_channel_num_lines,
             py::arg("channel"), py::arg("num_lines"))
        .def("get_channel_num_lines", &PyAbrBridge::get_channel_num_lines, py::arg("channel"))
        .def("set_channel_num_buffers", &PyAbrBridge::set_channel_num_buffers,
             py::arg("channel"), py::arg("num_buffers"))
        .def("get_channel_num_buffers", &PyAbrBridge::get_channel_num_buffers, py::arg("channel"))
        .def("set_channel_load_buffer", &PyAbrBridge::set_channel_load_buffer,
             py::arg("channel"), py::arg("load_buffer"))
        .def("set_channel_roce_qp", &PyAbrBridge::set_channel_roce_qp,
             py::arg("channel"), py::arg("qp"))
        .def("set_channel_roce_rkey", &PyAbrBridge::set_channel_roce_rkey,
             py::arg("channel"), py::arg("buffer"), py::arg("rkey"))
        .def("set_channel_offset", &PyAbrBridge::set_channel_offset,
             py::arg("channel"), py::arg("offset"))
        .def("set_channel_remote_address", &PyAbrBridge::set_channel_remote_address,
             py::arg("channel"), py::arg("buffer"), py::arg("remote_address"))
        .def("set_channel_metadata_address", &PyAbrBridge::set_channel_metadata_address,
             py::arg("channel"), py::arg("metadata_address"))
        .def("set_channel_metadata_key", &PyAbrBridge::set_channel_metadata_key,
             py::arg("channel"), py::arg("metadata_key"))
        .def("read_measured_vid_clk_hz", &PyAbrBridge::read_measured_vid_clk_hz)
        .def("read_measured_eth_clk_hz", &PyAbrBridge::read_measured_eth_clk_hz)
        .def("enable_channel", &PyAbrBridge::enable_channel, py::arg("channel"))
        .def("configure_channel", &PyAbrBridge::configure_channel,
             py::arg("channel"), py::arg("params"));
}
