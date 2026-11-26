/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#include <cstdint>
#include <memory>
#include <stdexcept>

#include <pybind11/pybind11.h>

#include "hcl/drivers/altera_roce.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"

namespace py = pybind11;

namespace {

using hcl::pybind::to_py_status;

class PyAlteraRoCE {
public:
    struct SrcHdr {
        std::uint64_t src_mac{0u};
        std::uint64_t src_ip_hi{0u};
        std::uint64_t src_ip_lo{0u};
    };

    struct DstHdr {
        std::uint64_t dst_mac{0u};
        std::uint32_t vlan_vid{0u};
        std::uint32_t vlan_dei{0u};
        std::uint32_t vlan_pcp{0u};
        std::uint64_t dst_ip_hi{0u};
        std::uint64_t dst_ip_lo{0u};
        std::uint32_t dscp{0u};
        std::uint32_t udp_src_port{0u};
        std::uint32_t pkey{0u};
        std::uint32_t qkey{0u};
    };

    struct UcQpParams {
        std::uint32_t local_qp{0u};
        std::uint32_t remote_qp{0u};
        std::uint32_t src_hdr_entry{0u};
        std::uint32_t dst_hdr_entry{0u};
        std::uint32_t rqe_queue{0u};
        std::uint32_t initiator_psn{0u};
        std::uint32_t target_psn{0u};
        std::uint32_t timeout{0u};
        std::uint32_t rnr_en{0u};
        std::uint32_t vlan_valid{0u};
        std::uint32_t ipv6_valid{0u};
        std::uint32_t read_ops_allowed{0u};
        SrcHdr src{};
        DstHdr dst{};
    };

    PyAlteraRoCE(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_roce_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create Altera RoCE driver");
        }
    }

    ~PyAlteraRoCE() {
        if (handle_ != nullptr) {
            hcl_altera_roce_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init_global() {
        return to_py_status(hcl_altera_roce_init_global(handle_));
    }

    py::object configure_qp_uc(const UcQpParams& params) {
        hcl_roce_uc_qp_params c_params{};
        c_params.local_qp = params.local_qp;
        c_params.remote_qp = params.remote_qp;
        c_params.src_hdr_entry = params.src_hdr_entry;
        c_params.dst_hdr_entry = params.dst_hdr_entry;
        c_params.rqe_queue = params.rqe_queue;
        c_params.initiator_psn = params.initiator_psn;
        c_params.target_psn = params.target_psn;
        c_params.timeout = params.timeout;
        c_params.rnr_en = params.rnr_en;
        c_params.vlan_valid = params.vlan_valid;
        c_params.ipv6_valid = params.ipv6_valid;
        c_params.read_ops_allowed = params.read_ops_allowed;
        c_params.src.src_mac = params.src.src_mac;
        c_params.src.src_ip_hi = params.src.src_ip_hi;
        c_params.src.src_ip_lo = params.src.src_ip_lo;
        c_params.dst.dst_mac = params.dst.dst_mac;
        c_params.dst.vlan_vid = params.dst.vlan_vid;
        c_params.dst.vlan_dei = params.dst.vlan_dei;
        c_params.dst.vlan_pcp = params.dst.vlan_pcp;
        c_params.dst.dst_ip_hi = params.dst.dst_ip_hi;
        c_params.dst.dst_ip_lo = params.dst.dst_ip_lo;
        c_params.dst.dscp = params.dst.dscp;
        c_params.dst.udp_src_port = params.dst.udp_src_port;
        c_params.dst.pkey = params.dst.pkey;
        c_params.dst.qkey = params.dst.qkey;
        return to_py_status(hcl_altera_roce_configure_qp_uc(handle_, &c_params));
    }

    py::object configure_src_hdr(std::uint32_t entry, const SrcHdr& hdr) {
        hcl_roce_src_hdr c_hdr{};
        c_hdr.src_mac = hdr.src_mac;
        c_hdr.src_ip_hi = hdr.src_ip_hi;
        c_hdr.src_ip_lo = hdr.src_ip_lo;
        return to_py_status(hcl_altera_roce_configure_src_hdr(handle_, entry, &c_hdr));
    }

    py::object configure_dst_hdr(std::uint32_t entry, const DstHdr& hdr) {
        hcl_roce_dst_hdr c_hdr{};
        c_hdr.dst_mac = hdr.dst_mac;
        c_hdr.vlan_vid = hdr.vlan_vid;
        c_hdr.vlan_dei = hdr.vlan_dei;
        c_hdr.vlan_pcp = hdr.vlan_pcp;
        c_hdr.dst_ip_hi = hdr.dst_ip_hi;
        c_hdr.dst_ip_lo = hdr.dst_ip_lo;
        c_hdr.dscp = hdr.dscp;
        c_hdr.udp_src_port = hdr.udp_src_port;
        c_hdr.pkey = hdr.pkey;
        c_hdr.qkey = hdr.qkey;
        return to_py_status(hcl_altera_roce_configure_dst_hdr(handle_, entry, &c_hdr));
    }

    py::object init_qp_uc(const UcQpParams& params) {
        hcl_roce_uc_qp_params c_params{};
        c_params.local_qp = params.local_qp;
        c_params.remote_qp = params.remote_qp;
        c_params.src_hdr_entry = params.src_hdr_entry;
        c_params.dst_hdr_entry = params.dst_hdr_entry;
        c_params.rqe_queue = params.rqe_queue;
        c_params.initiator_psn = params.initiator_psn;
        c_params.target_psn = params.target_psn;
        c_params.timeout = params.timeout;
        c_params.rnr_en = params.rnr_en;
        c_params.vlan_valid = params.vlan_valid;
        c_params.ipv6_valid = params.ipv6_valid;
        c_params.read_ops_allowed = params.read_ops_allowed;
        c_params.src.src_mac = params.src.src_mac;
        c_params.src.src_ip_hi = params.src.src_ip_hi;
        c_params.src.src_ip_lo = params.src.src_ip_lo;
        c_params.dst.dst_mac = params.dst.dst_mac;
        c_params.dst.vlan_vid = params.dst.vlan_vid;
        c_params.dst.vlan_dei = params.dst.vlan_dei;
        c_params.dst.vlan_pcp = params.dst.vlan_pcp;
        c_params.dst.dst_ip_hi = params.dst.dst_ip_hi;
        c_params.dst.dst_ip_lo = params.dst.dst_ip_lo;
        c_params.dst.dscp = params.dst.dscp;
        c_params.dst.udp_src_port = params.dst.udp_src_port;
        c_params.dst.pkey = params.dst.pkey;
        c_params.dst.qkey = params.dst.qkey;
        return to_py_status(hcl_altera_roce_init_qp_uc(handle_, &c_params));
    }

private:
    hcl_altera_roce* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_roce, m) {
    m.doc() = "Altera RoCE driver bindings";

    py::class_<PyAlteraRoCE::SrcHdr>(m, "SrcHdr")
        .def(py::init<>())
        .def_readwrite("src_mac", &PyAlteraRoCE::SrcHdr::src_mac)
        .def_readwrite("src_ip_hi", &PyAlteraRoCE::SrcHdr::src_ip_hi)
        .def_readwrite("src_ip_lo", &PyAlteraRoCE::SrcHdr::src_ip_lo);

    py::class_<PyAlteraRoCE::DstHdr>(m, "DstHdr")
        .def(py::init<>())
        .def_readwrite("dst_mac", &PyAlteraRoCE::DstHdr::dst_mac)
        .def_readwrite("vlan_vid", &PyAlteraRoCE::DstHdr::vlan_vid)
        .def_readwrite("vlan_dei", &PyAlteraRoCE::DstHdr::vlan_dei)
        .def_readwrite("vlan_pcp", &PyAlteraRoCE::DstHdr::vlan_pcp)
        .def_readwrite("dst_ip_hi", &PyAlteraRoCE::DstHdr::dst_ip_hi)
        .def_readwrite("dst_ip_lo", &PyAlteraRoCE::DstHdr::dst_ip_lo)
        .def_readwrite("dscp", &PyAlteraRoCE::DstHdr::dscp)
        .def_readwrite("udp_src_port", &PyAlteraRoCE::DstHdr::udp_src_port)
        .def_readwrite("pkey", &PyAlteraRoCE::DstHdr::pkey)
        .def_readwrite("qkey", &PyAlteraRoCE::DstHdr::qkey);

    py::class_<PyAlteraRoCE::UcQpParams>(m, "UcQpParams")
        .def(py::init<>())
        .def_readwrite("local_qp", &PyAlteraRoCE::UcQpParams::local_qp)
        .def_readwrite("remote_qp", &PyAlteraRoCE::UcQpParams::remote_qp)
        .def_readwrite("src_hdr_entry", &PyAlteraRoCE::UcQpParams::src_hdr_entry)
        .def_readwrite("dst_hdr_entry", &PyAlteraRoCE::UcQpParams::dst_hdr_entry)
        .def_readwrite("rqe_queue", &PyAlteraRoCE::UcQpParams::rqe_queue)
        .def_readwrite("initiator_psn", &PyAlteraRoCE::UcQpParams::initiator_psn)
        .def_readwrite("target_psn", &PyAlteraRoCE::UcQpParams::target_psn)
        .def_readwrite("timeout", &PyAlteraRoCE::UcQpParams::timeout)
        .def_readwrite("rnr_en", &PyAlteraRoCE::UcQpParams::rnr_en)
        .def_readwrite("vlan_valid", &PyAlteraRoCE::UcQpParams::vlan_valid)
        .def_readwrite("ipv6_valid", &PyAlteraRoCE::UcQpParams::ipv6_valid)
        .def_readwrite("read_ops_allowed", &PyAlteraRoCE::UcQpParams::read_ops_allowed)
        .def_readwrite("src", &PyAlteraRoCE::UcQpParams::src)
        .def_readwrite("dst", &PyAlteraRoCE::UcQpParams::dst);

    py::class_<PyAlteraRoCE, std::shared_ptr<PyAlteraRoCE>>(m, "AlteraRoCE")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none()) {
                    throw std::invalid_argument("register interface must not be null");
                }
                if (!py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("register interface is invalid");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraRoCE>(handle, base_address);
            }),
            py::arg("reg"),
            py::arg("base_address") = 0u,
            py::keep_alive<1, 2>())
        .def("init_global", &PyAlteraRoCE::init_global)
        .def("configure_qp_uc", &PyAlteraRoCE::configure_qp_uc, py::arg("params"))
        .def("configure_src_hdr", &PyAlteraRoCE::configure_src_hdr,
             py::arg("entry"), py::arg("hdr"))
        .def("configure_dst_hdr", &PyAlteraRoCE::configure_dst_hdr,
             py::arg("entry"), py::arg("hdr"))
        .def("init_qp_uc", &PyAlteraRoCE::init_qp_uc, py::arg("params"));
}
