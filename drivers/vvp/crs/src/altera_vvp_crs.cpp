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

#include "hcl/drivers/altera_vvp_crs.hpp"

namespace hcl::drivers {

AlteraVvpCrs::AlteraVvpCrs(std::shared_ptr<hcl::RegisterInterface> reg,
                           std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_crs_create(reg_ ? reg_->c_handle() : nullptr,
                                        base_address)) {}

AlteraVvpCrs::~AlteraVvpCrs() {
    hcl_altera_vvp_crs_destroy(driver_);
}

hcl::Status AlteraVvpCrs::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_crs_init(driver_));
}

bool AlteraVvpCrs::lite_mode() const {
    return hcl_altera_vvp_crs_get_lite_mode(driver_);
}

bool AlteraVvpCrs::debug_enabled() const {
    return hcl_altera_vvp_crs_get_debug_enabled(driver_);
}

std::uint32_t AlteraVvpCrs::img_info_width() const { return hcl_altera_vvp_crs_get_img_info_width(driver_); }
hcl::Status AlteraVvpCrs::set_img_info_width(std::uint32_t width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_crs_set_img_info_width(driver_, width));
}
std::uint32_t AlteraVvpCrs::img_info_height() const { return hcl_altera_vvp_crs_get_img_info_height(driver_); }
hcl::Status AlteraVvpCrs::set_img_info_height(std::uint32_t height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_crs_set_img_info_height(driver_, height));
}
std::uint32_t AlteraVvpCrs::img_info_interlace() const { return hcl_altera_vvp_crs_get_img_info_interlace(driver_); }
hcl::Status AlteraVvpCrs::set_img_info_interlace(std::uint32_t interlace) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_crs_set_img_info_interlace(driver_, interlace));
}
std::uint32_t AlteraVvpCrs::img_info_colorspace() const { return hcl_altera_vvp_crs_get_img_info_colorspace(driver_); }
hcl::Status AlteraVvpCrs::set_img_info_colorspace(std::uint32_t colorspace) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_crs_set_img_info_colorspace(driver_, colorspace));
}
std::uint32_t AlteraVvpCrs::img_info_subsampling() const { return hcl_altera_vvp_crs_get_img_info_subsampling(driver_); }
hcl::Status AlteraVvpCrs::set_img_info_subsampling(std::uint32_t subsampling) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_crs_set_img_info_subsampling(driver_, subsampling));
}
std::uint32_t AlteraVvpCrs::img_info_cositing() const { return hcl_altera_vvp_crs_get_img_info_cositing(driver_); }
hcl::Status AlteraVvpCrs::set_img_info_cositing(std::uint32_t cositing) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_crs_set_img_info_cositing(driver_, cositing));
}
std::uint32_t AlteraVvpCrs::img_info_field_count() const { return hcl_altera_vvp_crs_get_img_info_field_count(driver_); }

hcl::Result<bool> AlteraVvpCrs::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_crs_is_running(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpCrs::commit_is_pending() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_crs_commit_is_pending(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpCrs::get_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_crs_get_status(driver_, &out), out);
}

bool AlteraVvpCrs::is_conversion_supported(AlteraVvpCrs::Subsampling input_subsampling,
                                           AlteraVvpCrs::Subsampling output_subsampling) const {
    return hcl_altera_vvp_crs_is_conversion_supported(driver_, input_subsampling, output_subsampling);
}

hcl::Result<AlteraVvpCrs::Subsampling> AlteraVvpCrs::get_output_subsampling() {
    AlteraVvpCrs::Subsampling out = HCL_VVP_CRS_SUBSAMPLING_MODE_INVALID;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_crs_get_output_subsampling(driver_, &out), out);
}

hcl::Status AlteraVvpCrs::set_output_subsampling(AlteraVvpCrs::Subsampling output_subsampling) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_crs_set_output_subsampling(driver_, output_subsampling));
}

hcl::Status AlteraVvpCrs::commit() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_crs_commit(driver_));
}

}  // namespace hcl::drivers
