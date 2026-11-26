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

#include <cstring>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_transport.h"

#include "hcl/interfaces/i2c.h"

#if defined(HCL_PYTHON_HAS_UDP)
#include "hcl/udp_register_transport.h"
#endif

#if defined(HCL_PYTHON_HAS_UIO)
#include "hcl/uio_register_transport.h"
#endif

#if defined(HCL_PYTHON_HAS_HOLOLINK)
#include <pybind11/pytypes.h>
#endif


namespace py = pybind11;

namespace {

using HclPyRegisterContext = hcl::pybind::RegisterContext;
using HclPyStatus = hcl::pybind::Status;
using HclPyResult = hcl::pybind::Result;
using hcl::pybind::to_hcl_register_context;
using hcl::pybind::to_hcl_status;
using hcl::pybind::to_pb_register_context;
using hcl::pybind::to_pb_status;

struct HclPyOperation {
    hcl_register_operation_type operation{HCL_REGISTER_OP_READ32};
    std::uint32_t base_address{0u};
    std::uint32_t offset{0u};
    std::uint32_t write_value{0u};
    std::uint32_t read_value{0u};
    HclPyStatus status{};
};

class PyRegisterTransport {
public:
    virtual ~PyRegisterTransport() = default;

    [[nodiscard]] virtual hcl_register_transport transport() const = 0;
    [[nodiscard]] virtual void* context() const = 0;

    virtual HclPyStatus connect() {
        auto transport = this->transport();
        if (transport.context == nullptr) {
            return HclPyStatus{HCL_STATUS_INVALID_ARGUMENT};
        }
        const hcl_status_code code = hcl_register_transport_connect(&transport);
        return HclPyStatus{code};
    }

    virtual void disconnect() {
        auto transport = this->transport();
        if (transport.context != nullptr) {
            hcl_register_transport_disconnect(&transport);
        }
    }

    [[nodiscard]] virtual bool is_connected() const {
        const auto transport = this->transport();
        if (transport.context == nullptr) {
            return false;
        }
        return hcl_register_transport_is_connected(&transport);
    }

    virtual HclPyResult read_register(std::uint32_t base_address, std::uint32_t offset) {
        auto transport = this->transport();
        if (transport.context == nullptr) {
            return HclPyResult{{HCL_STATUS_INVALID_ARGUMENT}, 0u};
        }
        HclPyResult result{};
        const hcl_status status = hcl_register_transport_read32(&transport,
                                                                base_address,
                                                                offset,
                                                                &result.value);
        result.status = to_pb_status(status);
        return result;
    }

    virtual HclPyStatus write_register(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) {
        auto transport = this->transport();
        if (transport.context == nullptr) {
            return HclPyStatus{HCL_STATUS_INVALID_ARGUMENT};
        }
        const hcl_status status = hcl_register_transport_write32(&transport,
                                                                 base_address,
                                                                 offset,
                                                                 value);
        return to_pb_status(status);
    }

    virtual HclPyStatus execute(std::vector<HclPyOperation>& operations) {
        std::vector<hcl_register_operation> c_operations;
        c_operations.reserve(operations.size());
        for (const auto& operation : operations) {
            hcl_register_operation c_operation{};
            c_operation.operation = operation.operation;
            c_operation.base_address = operation.base_address;
            c_operation.offset = operation.offset;
            c_operation.write_value = operation.write_value;
            c_operations.push_back(c_operation);
        }

        auto transport = this->transport();
        if (transport.context == nullptr) {
            return HclPyStatus{HCL_STATUS_INVALID_ARGUMENT};
        }

        const hcl_status status = hcl_register_transport_execute(&transport,
                                                                 c_operations.data(),
                                                                 c_operations.size());
        for (std::size_t index = 0u; index < operations.size(); ++index) {
            operations[index].read_value = c_operations[index].read_value;
            operations[index].status.code = c_operations[index].status;
        }

        return to_pb_status(status);
    }
};

#if defined(HCL_PYTHON_HAS_UDP)
class PyUdpRegisterTransport final : public PyRegisterTransport {
public:
    PyUdpRegisterTransport(std::string host, std::uint16_t port, std::uint32_t timeout_ms)
        : transport_(nullptr) {
        transport_ = hcl_udp_register_transport_create(host.c_str(),
                                                       port,
                                                       timeout_ms);
        if (transport_ == nullptr) {
            throw std::runtime_error("Failed to create UDP transport");
        }
    }

    ~PyUdpRegisterTransport() override {
        if (transport_ != nullptr) {
            hcl_udp_register_transport_destroy(transport_);
            transport_ = nullptr;
        }
    }

    [[nodiscard]] hcl_register_transport transport() const override { return *transport_; }
    [[nodiscard]] void* context() const override { return transport_ == nullptr ? nullptr : transport_->context; }

private:
    hcl_register_transport* transport_;
};
#endif

#if defined(HCL_PYTHON_HAS_UIO)
class PyUioRegisterTransport final : public PyRegisterTransport {
public:
    PyUioRegisterTransport(const std::string& device_path, unsigned int map_index)
        : transport_(nullptr) {
        transport_ = hcl_uio_register_transport_create(device_path.c_str(),
                                                       map_index);
        if (transport_ == nullptr) {
            throw std::runtime_error("Failed to create UIO transport");
        }
    }

    ~PyUioRegisterTransport() override {
        if (transport_ != nullptr) {
            hcl_uio_register_transport_destroy(transport_);
            transport_ = nullptr;
        }
    }

    [[nodiscard]] hcl_register_transport transport() const override { return *transport_; }
    [[nodiscard]] void* context() const override { return transport_ == nullptr ? nullptr : transport_->context; }

private:
    hcl_register_transport* transport_;
};
#endif

class PyRegisterInterface {
public:
    explicit PyRegisterInterface(std::shared_ptr<PyRegisterTransport> transport)
        : transport_(std::move(transport)),
          handle_(nullptr) {
        if (!transport_) {
            throw std::invalid_argument("transport must not be null");
        }

        const hcl_register_transport c_transport = transport_->transport();
        handle_ = hcl_register_interface_create(&c_transport);
        if (handle_ == nullptr) {
            throw std::runtime_error("register interface create failed");
        }
    }

    ~PyRegisterInterface() {
        if (handle_ != nullptr) {
            hcl_register_interface_destroy(handle_);
            handle_ = nullptr;
        }
    }

    [[nodiscard]] std::uintptr_t _handle_address() const {
        return reinterpret_cast<std::uintptr_t>(handle_);
    }

    HclPyResult read32(std::uint32_t base_address, std::uint32_t offset) {
        HclPyResult result{};
        const hcl_status status = hcl_register_interface_read32(handle_,
                                                                base_address,
                                                                offset,
                                                                &result.value);
        result.status = to_pb_status(status);
        return result;
    }

    HclPyStatus write32(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) {
        const hcl_status status = hcl_register_interface_write32(handle_,
                                                                 base_address,
                                                                 offset,
                                                                 value);
        return to_pb_status(status);
    }

    HclPyStatus execute(std::vector<HclPyOperation>& operations) {
        std::vector<hcl_register_operation> c_operations;
        c_operations.reserve(operations.size());

        for (const auto& operation : operations) {
            hcl_register_operation c_operation{};
            c_operation.operation = operation.operation;
            c_operation.base_address = operation.base_address;
            c_operation.offset = operation.offset;
            c_operation.write_value = operation.write_value;
            c_operations.push_back(c_operation);
        }

        const hcl_status status = hcl_register_interface_execute(handle_,
                                                                 c_operations.data(),
                                                                 c_operations.size());

        for (std::size_t index = 0u; index < operations.size(); ++index) {
            operations[index].read_value = c_operations[index].read_value;
            operations[index].status.code = c_operations[index].status;
        }

        return to_pb_status(status);
    }

private:
    std::shared_ptr<PyRegisterTransport> transport_;
    hcl_register_interface* handle_;
};

class PyI2C {
public:
    explicit PyI2C(hcl_i2c* handle) : handle_(handle) {
        if (handle_ == nullptr) {
            throw std::invalid_argument("i2c handle must not be null");
        }
    }

    ~PyI2C() {
        if (handle_ != nullptr) {
            hcl_i2c_destroy(handle_);
            handle_ = nullptr;
        }
    }

    [[nodiscard]] std::uintptr_t _handle_address() const {
        return reinterpret_cast<std::uintptr_t>(handle_);
    }

    HclPyStatus init() {
        return to_pb_status(hcl_i2c_init(handle_));
    }

    HclPyStatus write(std::uint8_t address, std::uint8_t reg, std::uint8_t data) {
        return to_pb_status(hcl_i2c_write(handle_, address, reg, data));
    }

    HclPyResult read(std::uint8_t address, std::uint8_t reg) {
        HclPyResult result{};
        std::uint8_t value = 0u;
        const hcl_status status = hcl_i2c_read(handle_, address, reg, &value);
        result.status = to_pb_status(status);
        result.value = static_cast<std::uint32_t>(value);
        return result;
    }

    HclPyStatus write_extended(std::uint8_t address, std::uint16_t reg, std::uint8_t data) {
        return to_pb_status(hcl_i2c_write_extended(handle_, address, reg, data));
    }

    HclPyResult read_extended(std::uint8_t address, std::uint16_t reg) {
        HclPyResult result{};
        std::uint8_t value = 0u;
        const hcl_status status = hcl_i2c_read_extended(handle_, address, reg, &value);
        result.status = to_pb_status(status);
        result.value = static_cast<std::uint32_t>(value);
        return result;
    }

private:
    hcl_i2c* handle_{nullptr};
};

#if defined(HCL_PYTHON_HAS_HOLOLINK)
class PyHololinkRegisterTransport final : public PyRegisterTransport {
public:
    explicit PyHololinkRegisterTransport(py::object hololink)
        : hololink_(std::move(hololink)), started_(false) {
        if (hololink_.is_none()) {
            throw std::invalid_argument("hololink must not be None");
        }
        if (!py::hasattr(hololink_, "start") || !py::hasattr(hololink_, "stop") ||
            !py::hasattr(hololink_, "read_uint32") || !py::hasattr(hololink_, "write_uint32")) {
            throw std::invalid_argument("hololink object must provide start(), stop(), read_uint32(), and write_uint32()");
        }
    }

    [[nodiscard]] hcl_register_transport transport() const override {
        static const hcl_register_transport_interface vtable = {
            [](hcl_register_transport* transport) {
                auto* self = static_cast<PyHololinkRegisterTransport*>(transport != nullptr ? transport->context : nullptr);
                if (self == nullptr) {
                    return HCL_STATUS_INVALID_ARGUMENT;
                }
                return self->connect().code;
            },
            [](hcl_register_transport* transport) {
                auto* self = static_cast<PyHololinkRegisterTransport*>(transport != nullptr ? transport->context : nullptr);
                if (self != nullptr) {
                    self->disconnect();
                }
            },
            [](const hcl_register_transport* transport) {
                auto* self = static_cast<const PyHololinkRegisterTransport*>(transport != nullptr ? transport->context : nullptr);
                return self != nullptr && self->is_connected();
            },
            [](const hcl_register_transport* transport, std::uint32_t base_address, std::uint32_t offset, std::uint32_t* out_value) -> hcl_status {
                auto* self = static_cast<PyHololinkRegisterTransport*>(transport != nullptr ? transport->context : nullptr);
                if (self == nullptr || out_value == nullptr) {
                    return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, nullptr);
                }
                const auto result = self->read_register(base_address, offset);
                if (!result.ok()) {
                    return to_hcl_status(result.status);
                }
                *out_value = result.value;
                return hcl_status_ok();
            },
            [](const hcl_register_transport* transport, std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) -> hcl_status {
                auto* self = static_cast<PyHololinkRegisterTransport*>(transport != nullptr ? transport->context : nullptr);
                if (self == nullptr) {
                    return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, nullptr);
                }
                return to_hcl_status(self->write_register(base_address, offset, value));
            },
            [](const hcl_register_transport* transport, hcl_register_operation* operations, std::size_t operation_count) -> hcl_status {
                auto* self = static_cast<PyHololinkRegisterTransport*>(transport != nullptr ? transport->context : nullptr);
                if (self == nullptr || (operation_count > 0u && operations == nullptr)) {
                    return hcl_status_error(HCL_STATUS_INVALID_ARGUMENT, nullptr);
                }

                std::vector<HclPyOperation> ops;
                ops.reserve(operation_count);
                for (std::size_t index = 0u; index < operation_count; ++index) {
                    HclPyOperation op{};
                    op.operation = operations[index].operation;
                    op.base_address = operations[index].base_address;
                    op.offset = operations[index].offset;
                    op.write_value = operations[index].write_value;
                    ops.push_back(op);
                }

                const auto status = self->execute(ops);
                for (std::size_t index = 0u; index < operation_count; ++index) {
                    operations[index].read_value = ops[index].read_value;
                    operations[index].status = ops[index].status.code;
                }
                return to_hcl_status(status);
            }
        };

        hcl_register_transport transport{};
        transport.context = const_cast<PyHololinkRegisterTransport*>(this);
        transport.vtable = &vtable;
        return transport;
    }

    [[nodiscard]] void* context() const override {
        return const_cast<PyHololinkRegisterTransport*>(this);
    }

    HclPyStatus connect() override {
        if (started_) {
            return HclPyStatus{HCL_STATUS_OK};
        }
        hololink_.attr("start")();
        started_ = true;
        return HclPyStatus{HCL_STATUS_OK};
    }

    void disconnect() override {
        if (!started_) {
            return;
        }
        if (py::hasattr(hololink_, "stop")) {
            hololink_.attr("stop")();
        }
        started_ = false;
    }

    [[nodiscard]] bool is_connected() const override {
        return started_ && !hololink_.is_none();
    }

    HclPyResult read_register(std::uint32_t base_address, std::uint32_t offset) override {
        const std::uint32_t address = base_address + offset;
        try {
            const std::uint32_t value = hololink_.attr("read_uint32")(address).cast<std::uint32_t>();
            return HclPyResult{HclPyStatus{HCL_STATUS_OK}, value};
        } catch (const py::error_already_set&) {
            HclPyStatus status{HCL_STATUS_TRANSPORT_ERROR};
            status.ctx = HclPyRegisterContext{base_address, offset, 0u};
            status.has_context = true;
            return HclPyResult{status, 0u};
        }
    }

    HclPyStatus write_register(std::uint32_t base_address, std::uint32_t offset, std::uint32_t value) override {
        const std::uint32_t address = base_address + offset;
        try {
            hololink_.attr("write_uint32")(address, value);
            return HclPyStatus{HCL_STATUS_OK};
        } catch (const py::error_already_set&) {
            HclPyStatus status{HCL_STATUS_TRANSPORT_ERROR};
            status.ctx = HclPyRegisterContext{base_address, offset, value};
            status.has_context = true;
            return status;
        }
    }

    HclPyStatus execute(std::vector<HclPyOperation>& operations) override {
        for (auto& operation : operations) {
            const std::uint32_t address = operation.base_address + operation.offset;
            try {
                switch (operation.operation) {
                    case HCL_REGISTER_OP_READ32:
                        operation.read_value = hololink_.attr("read_uint32")(address).cast<std::uint32_t>();
                        operation.status = HclPyStatus{HCL_STATUS_OK};
                        break;
                    case HCL_REGISTER_OP_WRITE32:
                        hololink_.attr("write_uint32")(address, operation.write_value);
                        operation.status = HclPyStatus{HCL_STATUS_OK};
                        break;
                    default: {
                        operation.status = HclPyStatus{HCL_STATUS_UNSUPPORTED};
                        break;
                    }
                }
            } catch (const py::error_already_set&) {
                HclPyStatus status{HCL_STATUS_TRANSPORT_ERROR};
                status.ctx = HclPyRegisterContext{operation.base_address, operation.offset, operation.write_value};
                status.has_context = true;
                operation.status = status;
            }
        }
        return HclPyStatus{HCL_STATUS_OK};
    }

private:
    py::object hololink_;
    bool started_;
};
#endif  // defined(HCL_PYTHON_HAS_HOLOLINK)


}  // namespace

PYBIND11_MODULE(core, m) {
    m.doc() = "Python bindings for altera_host_control";

    py::enum_<hcl_status_code>(m, "StatusCode")
        .value("OK", HCL_STATUS_OK)
        .value("TIMEOUT", HCL_STATUS_TIMEOUT)
        .value("TRANSPORT_ERROR", HCL_STATUS_TRANSPORT_ERROR)
        .value("PROTOCOL_ERROR", HCL_STATUS_PROTOCOL_ERROR)
        .value("UNSUPPORTED", HCL_STATUS_UNSUPPORTED)
        .value("INVALID_ARGUMENT", HCL_STATUS_INVALID_ARGUMENT)
        .value("NOT_CONNECTED", HCL_STATUS_NOT_CONNECTED)
        .value("OUT_OF_RANGE", HCL_STATUS_OUT_OF_RANGE)
        .value("ALLOCATION_FAILED", HCL_STATUS_ALLOCATION_FAILED)
        .value("INTERNAL_ERROR", HCL_STATUS_INTERNAL_ERROR)
        .export_values();

    py::enum_<hcl_register_operation_type>(m, "OperationType")
        .value("READ32", HCL_REGISTER_OP_READ32)
        .value("WRITE32", HCL_REGISTER_OP_WRITE32)
        .export_values();

    py::class_<HclPyRegisterContext>(m, "RegisterContext")
        .def(py::init<>())
        .def_readwrite("base_address", &HclPyRegisterContext::base_address)
        .def_readwrite("offset", &HclPyRegisterContext::offset)
        .def_readwrite("value", &HclPyRegisterContext::value);

    py::class_<HclPyStatus>(m, "Status")
        .def(py::init<>())
        .def_readwrite("code", &HclPyStatus::code)
        .def_readwrite("ctx", &HclPyStatus::ctx)
        .def_readwrite("has_context", &HclPyStatus::has_context)
        .def("ok", &HclPyStatus::ok)
        .def_static("success", []() {
            return HclPyStatus{};
        })
        .def_static("error", [](hcl_status_code code, const HclPyRegisterContext* context = nullptr) {
            HclPyStatus status{code};
            if (context != nullptr) {
                status.ctx = *context;
                status.has_context = true;
            }
            return status;
        }, py::arg("code"), py::arg("context") = py::none());

    py::class_<HclPyResult>(m, "Result")
        .def(py::init<>())
        .def_readwrite("status", &HclPyResult::status)
        .def_readwrite("value", &HclPyResult::value)
        .def("ok", &HclPyResult::ok);

    py::class_<HclPyOperation>(m, "Operation")
        .def(py::init<>())
        .def_readwrite("operation", &HclPyOperation::operation)
        .def_readwrite("base_address", &HclPyOperation::base_address)
        .def_readwrite("offset", &HclPyOperation::offset)
        .def_readwrite("write_value", &HclPyOperation::write_value)
        .def_readwrite("read_value", &HclPyOperation::read_value)
        .def_readwrite("status", &HclPyOperation::status);

    py::class_<PyRegisterTransport, std::shared_ptr<PyRegisterTransport>>(m, "RegisterTransport")
        .def("connect", &PyRegisterTransport::connect)
        .def("disconnect", &PyRegisterTransport::disconnect)
        .def("is_connected", &PyRegisterTransport::is_connected)
        .def("read_register", &PyRegisterTransport::read_register)
        .def("write_register", &PyRegisterTransport::write_register);

#if defined(HCL_PYTHON_HAS_UDP)
    py::class_<PyUdpRegisterTransport, PyRegisterTransport, std::shared_ptr<PyUdpRegisterTransport>>(
        m, "UdpRegisterTransport")
        .def(py::init<std::string, std::uint16_t, std::uint32_t>(),
            py::arg("host"),
            py::arg("port") = 5005u,
            py::arg("timeout_ms") = 200u);
#endif

#if defined(HCL_PYTHON_HAS_UIO)
    py::class_<PyUioRegisterTransport, PyRegisterTransport, std::shared_ptr<PyUioRegisterTransport>>(
        m, "UioRegisterTransport")
        .def(py::init<std::string, unsigned int>(),
            py::arg("device_path"),
            py::arg("map_index") = 0u);
#endif

#if defined(HCL_PYTHON_HAS_HOLOLINK)
    py::class_<PyHololinkRegisterTransport,
               PyRegisterTransport,
               std::shared_ptr<PyHololinkRegisterTransport>>(m, "HololinkRegisterTransport")
        .def(py::init<py::object>(), py::arg("hololink"))
        .def("connect", &PyHololinkRegisterTransport::connect)
        .def("disconnect", &PyHololinkRegisterTransport::disconnect)
        .def("is_connected", &PyHololinkRegisterTransport::is_connected)
        .def("read_register", &PyHololinkRegisterTransport::read_register,
            py::arg("base_address"),
            py::arg("offset"))
        .def("write_register", &PyHololinkRegisterTransport::write_register,
            py::arg("base_address"),
            py::arg("offset"),
            py::arg("value"))
        .def("execute", &PyHololinkRegisterTransport::execute, py::arg("operations"));
#endif

    py::class_<PyI2C, std::shared_ptr<PyI2C>>(m, "I2C")
        .def(py::init([](std::uintptr_t handle) {
                if (handle == 0u) {
                    throw std::invalid_argument("i2c handle must not be null");
                }
                return std::make_shared<PyI2C>(reinterpret_cast<hcl_i2c*>(handle));
            }),
            py::arg("handle"))
        .def("_handle", &PyI2C::_handle_address)
        .def("init", &PyI2C::init)
        .def("write", &PyI2C::write,
            py::arg("address"),
            py::arg("reg"),
            py::arg("data"))
        .def("read", &PyI2C::read,
            py::arg("address"),
            py::arg("reg"))
        .def("write_extended", &PyI2C::write_extended,
            py::arg("address"),
            py::arg("reg"),
            py::arg("data"))
        .def("read_extended", &PyI2C::read_extended,
            py::arg("address"),
            py::arg("reg"));

    py::class_<PyRegisterInterface, std::shared_ptr<PyRegisterInterface>>(m, "RegisterInterface")
        .def("_handle", &PyRegisterInterface::_handle_address)
        .def(py::init<std::shared_ptr<PyRegisterTransport>>(), py::arg("transport"))
        .def("read32", &PyRegisterInterface::read32)
        .def("write32", &PyRegisterInterface::write32)
        .def("execute", &PyRegisterInterface::execute, py::arg("operations"));
}
