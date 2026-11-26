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

#include "hcl/drivers/altera_vvp_csc.hpp"

namespace hcl::drivers {

AlteraVvpCsc::AlteraVvpCsc(std::shared_ptr<hcl::RegisterInterface> reg,
                           std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_csc_create(reg_ ? reg_->c_handle() : nullptr,
                                        base_address)) {}

AlteraVvpCsc::~AlteraVvpCsc() {
    hcl_altera_vvp_csc_destroy(driver_);
}

hcl::Status AlteraVvpCsc::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_csc_init(driver_));
}

bool AlteraVvpCsc::lite_mode() const {
    return hcl_altera_vvp_csc_get_lite_mode(driver_);
}

bool AlteraVvpCsc::debug_enabled() const {
    return hcl_altera_vvp_csc_get_debug_enabled(driver_);
}

std::uint32_t AlteraVvpCsc::img_info_width() const {
    return hcl_altera_vvp_csc_get_img_info_width(driver_);
}

hcl::Status AlteraVvpCsc::set_img_info_width(std::uint32_t width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_csc_set_img_info_width(driver_, width));
}

std::uint32_t AlteraVvpCsc::img_info_height() const {
    return hcl_altera_vvp_csc_get_img_info_height(driver_);
}

hcl::Status AlteraVvpCsc::set_img_info_height(std::uint32_t height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_csc_set_img_info_height(driver_, height));
}

std::uint32_t AlteraVvpCsc::img_info_interlace() const {
    return hcl_altera_vvp_csc_get_img_info_interlace(driver_);
}

hcl::Status AlteraVvpCsc::set_img_info_interlace(std::uint32_t interlace) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_csc_set_img_info_interlace(driver_, interlace));
}

std::uint32_t AlteraVvpCsc::img_info_colorspace() const {
    return hcl_altera_vvp_csc_get_img_info_colorspace(driver_);
}

hcl::Status AlteraVvpCsc::set_img_info_colorspace(std::uint32_t colorspace) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_csc_set_img_info_colorspace(driver_, colorspace));
}

std::uint32_t AlteraVvpCsc::img_info_subsampling() const {
    return hcl_altera_vvp_csc_get_img_info_subsampling(driver_);
}

hcl::Status AlteraVvpCsc::set_img_info_subsampling(std::uint32_t subsampling) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_csc_set_img_info_subsampling(driver_, subsampling));
}

std::uint32_t AlteraVvpCsc::img_info_cositing() const {
    return hcl_altera_vvp_csc_get_img_info_cositing(driver_);
}

hcl::Status AlteraVvpCsc::set_img_info_cositing(std::uint32_t cositing) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_csc_set_img_info_cositing(driver_, cositing));
}

std::uint32_t AlteraVvpCsc::img_info_field_count() const {
    return hcl_altera_vvp_csc_get_img_info_field_count(driver_);
}

std::uint8_t AlteraVvpCsc::bits_per_sample_in() const {
    return hcl_altera_vvp_csc_get_bits_per_sample_in(driver_);
}

std::uint8_t AlteraVvpCsc::bits_per_sample_out() const {
    return hcl_altera_vvp_csc_get_bits_per_sample_out(driver_);
}

std::uint8_t AlteraVvpCsc::coeffs_frac_bits() const {
    return hcl_altera_vvp_csc_get_coeffs_frac_bits(driver_);
}

bool AlteraVvpCsc::coeffs_signed() const {
    return hcl_altera_vvp_csc_are_coeffs_signed(driver_);
}

std::uint8_t AlteraVvpCsc::coeffs_int_bits() const {
    return hcl_altera_vvp_csc_get_coeffs_int_bits(driver_);
}

bool AlteraVvpCsc::summands_signed() const {
    return hcl_altera_vvp_csc_are_summands_signed(driver_);
}

std::uint8_t AlteraVvpCsc::summands_int_bits() const {
    return hcl_altera_vvp_csc_get_summands_int_bits(driver_);
}

std::int8_t AlteraVvpCsc::binary_point_right_move() const {
    return hcl_altera_vvp_csc_get_binary_point_right_move(driver_);
}

AlteraVvpCsc::Rounding AlteraVvpCsc::rounding_method() const {
    return hcl_altera_vvp_csc_get_rounding_method(driver_);
}

hcl::Result<bool> AlteraVvpCsc::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_csc_is_running(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpCsc::commit_is_pending() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_csc_commit_is_pending(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpCsc::get_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_csc_get_status(driver_, &out), out);
}

hcl::Status AlteraVvpCsc::set_quantized_coeff_data(const AlteraVvpCsc::QuantizedCoefficients& quantized_coeffs) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_csc_set_quantized_coeff_data(driver_, &quantized_coeffs));
}

hcl::Result<AlteraVvpCsc::QuantizedCoefficients> AlteraVvpCsc::get_quantized_coeff_data() {
    AlteraVvpCsc::QuantizedCoefficients out = {};
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_csc_get_quantized_coeff_data(driver_, &out), out);
}

hcl::Status AlteraVvpCsc::set_coeff_data(const AlteraVvpCsc::Coefficients& coeffs, std::int32_t summand_rescale) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_csc_set_coeff_data(driver_, &coeffs, summand_rescale));
}

hcl::Result<AlteraVvpCsc::Coefficients> AlteraVvpCsc::get_coeff_data() {
    AlteraVvpCsc::Coefficients out = {};
    return hcl::StatusHelper::to_result(hcl_altera_vvp_csc_get_coeff_data(driver_, &out), out);
}

hcl::Status AlteraVvpCsc::set_output_color_space(AlteraVvpCsc::ColorSpace output_color_space) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_csc_set_output_color_space(driver_, output_color_space));
}

hcl::Result<AlteraVvpCsc::ColorSpace> AlteraVvpCsc::get_output_color_space() {
    AlteraVvpCsc::ColorSpace out = HCL_VVP_CSC_COLOR_SPACE_INVALID;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_csc_get_output_color_space(driver_, &out), out);
}

hcl::Status AlteraVvpCsc::commit() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_csc_commit(driver_));
}

}  // namespace hcl::drivers
