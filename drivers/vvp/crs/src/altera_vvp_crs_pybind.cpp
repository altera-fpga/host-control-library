#include <cstdint>
#include <memory>
#include <stdexcept>
#include <pybind11/pybind11.h>
#include "hcl/drivers/altera_vvp_crs.h"
#include "hcl/pybind_helpers.h"
#include "hcl/register_interface.h"
namespace py = pybind11;

namespace {

using hcl::pybind::to_py_result;
using hcl::pybind::to_py_status;

class PyAlteraVvpCrs {
public:
    PyAlteraVvpCrs(hcl_register_interface* reg, std::uint32_t base_address)
        : handle_(hcl_altera_vvp_crs_create(reg, base_address)) {
        if (handle_ == nullptr) {
            throw std::runtime_error("failed to create VVP CRS");
        }
    }

    ~PyAlteraVvpCrs() {
        if (handle_ != nullptr) {
            hcl_altera_vvp_crs_destroy(handle_);
            handle_ = nullptr;
        }
    }

    py::object init() {
        return to_py_status(hcl_altera_vvp_crs_init(handle_));
    }

    py::object set_width(std::uint32_t width) {
        return to_py_status(hcl_altera_vvp_crs_set_img_info_width(handle_, width));
    }

    py::object set_height(std::uint32_t height) {
        return to_py_status(hcl_altera_vvp_crs_set_img_info_height(handle_, height));
    }

    py::object set_output_subsampling(std::uint32_t subsampling) {
        return to_py_status(hcl_altera_vvp_crs_set_output_subsampling(
            handle_, static_cast<hcl_altera_vvp_crs_subsampling>(subsampling)));
    }

    py::object commit() {
        return to_py_status(hcl_altera_vvp_crs_commit(handle_));
    }

    py::object is_running() {
        bool value = false;
        return to_py_result(hcl_altera_vvp_crs_is_running(handle_, &value), py::cast(value));
    }

    py::object get_status() {
        std::uint32_t value = 0u;
        return to_py_result(hcl_altera_vvp_crs_get_status(handle_, &value), value);
    }

private:
    hcl_altera_vvp_crs* handle_{nullptr};
};

}  // namespace

PYBIND11_MODULE(altera_vvp_crs, m) {
    py::enum_<hcl_altera_vvp_crs_subsampling>(m, "Subsampling")
        .value("420", HCL_VVP_CRS_SUBSAMPLING_MODE_420)
        .value("422", HCL_VVP_CRS_SUBSAMPLING_MODE_422)
        .value("444", HCL_VVP_CRS_SUBSAMPLING_MODE_444)
        .value("INVALID", HCL_VVP_CRS_SUBSAMPLING_MODE_INVALID);

    py::class_<PyAlteraVvpCrs, std::shared_ptr<PyAlteraVvpCrs>>(m, "AlteraVvpCrs")
        .def(py::init([](py::object reg, std::uint32_t base_address) {
                if (reg.is_none() || !py::hasattr(reg, "_handle")) {
                    throw std::invalid_argument("invalid register interface");
                }
                const std::uintptr_t handle_address = reg.attr("_handle")().cast<std::uintptr_t>();
                auto* handle = reinterpret_cast<hcl_register_interface*>(handle_address);
                return std::make_shared<PyAlteraVvpCrs>(handle, base_address);
            }),
            py::arg("reg"),
            py::arg("base_address") = 0u,
            py::keep_alive<1, 2>())
        .def("init", &PyAlteraVvpCrs::init)
        .def("set_img_info_width", &PyAlteraVvpCrs::set_width)
        .def("set_img_info_height", &PyAlteraVvpCrs::set_height)
        .def("set_output_subsampling", &PyAlteraVvpCrs::set_output_subsampling)
        .def("commit", &PyAlteraVvpCrs::commit)
        .def("is_running", &PyAlteraVvpCrs::is_running)
        .def("get_status", &PyAlteraVvpCrs::get_status);
}
