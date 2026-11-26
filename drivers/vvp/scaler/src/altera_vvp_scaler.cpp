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

#include "hcl/drivers/altera_vvp_scaler.hpp"

namespace hcl::drivers {

AlteraVvpScaler::AlteraVvpScaler(std::shared_ptr<hcl::RegisterInterface> reg,
                                 std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_scaler_create(reg_ ? reg_->c_handle() : nullptr,
                                           base_address)) {}

AlteraVvpScaler::~AlteraVvpScaler() {
    hcl_altera_vvp_scaler_destroy(driver_);
}

hcl::Status AlteraVvpScaler::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_init(driver_));
}

bool AlteraVvpScaler::lite_mode() const { return hcl_altera_vvp_scaler_get_lite_mode(driver_); }
bool AlteraVvpScaler::debug_enabled() const { return hcl_altera_vvp_scaler_get_debug_enabled(driver_); }
std::uint8_t AlteraVvpScaler::pixels_in_parallel() const { return hcl_altera_vvp_scaler_get_pixels_in_parallel(driver_); }
std::uint32_t AlteraVvpScaler::max_input_width() const { return hcl_altera_vvp_scaler_get_max_input_width(driver_); }
std::uint32_t AlteraVvpScaler::max_output_width() const { return hcl_altera_vvp_scaler_get_max_output_width(driver_); }
std::uint32_t AlteraVvpScaler::img_info_width() const { return hcl_altera_vvp_scaler_get_img_info_width(driver_); }
hcl::Status AlteraVvpScaler::set_img_info_width(std::uint32_t width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_img_info_width(driver_, width));
}
std::uint32_t AlteraVvpScaler::img_info_height() const { return hcl_altera_vvp_scaler_get_img_info_height(driver_); }
hcl::Status AlteraVvpScaler::set_img_info_height(std::uint32_t height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_img_info_height(driver_, height));
}
std::uint32_t AlteraVvpScaler::img_info_interlace() const { return hcl_altera_vvp_scaler_get_img_info_interlace(driver_); }
hcl::Status AlteraVvpScaler::set_img_info_interlace(std::uint32_t interlace) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_img_info_interlace(driver_, interlace));
}
std::uint32_t AlteraVvpScaler::img_info_colorspace() const { return hcl_altera_vvp_scaler_get_img_info_colorspace(driver_); }
hcl::Status AlteraVvpScaler::set_img_info_colorspace(std::uint32_t colorspace) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_img_info_colorspace(driver_, colorspace));
}
std::uint32_t AlteraVvpScaler::img_info_subsampling() const { return hcl_altera_vvp_scaler_get_img_info_subsampling(driver_); }
hcl::Status AlteraVvpScaler::set_img_info_subsampling(std::uint32_t subsampling) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_img_info_subsampling(driver_, subsampling));
}
std::uint32_t AlteraVvpScaler::img_info_cositing() const { return hcl_altera_vvp_scaler_get_img_info_cositing(driver_); }
hcl::Status AlteraVvpScaler::set_img_info_cositing(std::uint32_t cositing) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_img_info_cositing(driver_, cositing));
}
std::uint32_t AlteraVvpScaler::img_info_field_count() const { return hcl_altera_vvp_scaler_get_img_info_field_count(driver_); }
AlteraVvpScaler::Algorithm AlteraVvpScaler::scaling_algorithm() const {
    return hcl_altera_vvp_scaler_get_scaling_algorithm(driver_);
}
bool AlteraVvpScaler::coeffs_loading_enabled() const { return hcl_altera_vvp_scaler_is_coeffs_loading_enabled(driver_); }
bool AlteraVvpScaler::coeffs_preinitialized() const { return hcl_altera_vvp_scaler_are_coeffs_preinitialized(driver_); }

bool AlteraVvpScaler::vertical_scaling_enabled() const { return hcl_altera_vvp_scaler_is_vertical_scaling_enabled(driver_); }
bool AlteraVvpScaler::partial_vertical_scaling_enabled() const { return hcl_altera_vvp_scaler_is_partial_vertical_scaling_enabled(driver_); }
std::uint8_t AlteraVvpScaler::num_vertical_taps() const { return hcl_altera_vvp_scaler_get_num_vertical_taps(driver_); }
std::uint16_t AlteraVvpScaler::num_vertical_phases() const { return hcl_altera_vvp_scaler_get_num_vertical_phases(driver_); }
std::uint8_t AlteraVvpScaler::num_vertical_banks() const { return hcl_altera_vvp_scaler_get_num_vertical_banks(driver_); }
bool AlteraVvpScaler::vertical_coeffs_signed() const { return hcl_altera_vvp_scaler_are_vertical_coeffs_signed(driver_); }
std::uint8_t AlteraVvpScaler::vertical_coeffs_int_bits() const { return hcl_altera_vvp_scaler_get_vertical_coeffs_int_bits(driver_); }
std::uint8_t AlteraVvpScaler::vertical_coeffs_frac_bits() const { return hcl_altera_vvp_scaler_get_vertical_coeffs_frac_bits(driver_); }

bool AlteraVvpScaler::horizontal_scaling_enabled() const { return hcl_altera_vvp_scaler_is_horizontal_scaling_enabled(driver_); }
bool AlteraVvpScaler::partial_horizontal_scaling_enabled() const { return hcl_altera_vvp_scaler_is_partial_horizontal_scaling_enabled(driver_); }
std::uint8_t AlteraVvpScaler::num_horizontal_taps() const { return hcl_altera_vvp_scaler_get_num_horizontal_taps(driver_); }
std::uint16_t AlteraVvpScaler::num_horizontal_phases() const { return hcl_altera_vvp_scaler_get_num_horizontal_phases(driver_); }
std::uint8_t AlteraVvpScaler::num_horizontal_banks() const { return hcl_altera_vvp_scaler_get_num_horizontal_banks(driver_); }
bool AlteraVvpScaler::horizontal_coeffs_signed() const { return hcl_altera_vvp_scaler_are_horizontal_coeffs_signed(driver_); }
std::uint8_t AlteraVvpScaler::horizontal_coeffs_int_bits() const { return hcl_altera_vvp_scaler_get_horizontal_coeffs_int_bits(driver_); }
std::uint8_t AlteraVvpScaler::horizontal_coeffs_frac_bits() const { return hcl_altera_vvp_scaler_get_horizontal_coeffs_frac_bits(driver_); }

hcl::Result<bool> AlteraVvpScaler::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_is_running(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpScaler::commit_is_pending() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_commit_is_pending(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_status(driver_, &out), out);
}

hcl::Status AlteraVvpScaler::set_h_bank(std::uint8_t bank) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_h_bank(driver_, bank));
}

hcl::Status AlteraVvpScaler::set_v_bank(std::uint8_t bank) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_v_bank(driver_, bank));
}

hcl::Status AlteraVvpScaler::set_output_width(std::uint32_t output_width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_output_width(driver_, output_width));
}

hcl::Status AlteraVvpScaler::set_output_height(std::uint32_t output_height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_output_height(driver_, output_height));
}

hcl::Status AlteraVvpScaler::set_total_input_width(std::uint32_t total_input_width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_total_input_width(driver_, total_input_width));
}

hcl::Status AlteraVvpScaler::set_total_output_width(std::uint32_t total_output_width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_total_output_width(driver_, total_output_width));
}

hcl::Status AlteraVvpScaler::set_total_input_height(std::uint32_t total_input_height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_total_input_height(driver_, total_input_height));
}

hcl::Status AlteraVvpScaler::set_total_output_height(std::uint32_t total_output_height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_total_output_height(driver_, total_output_height));
}

hcl::Status AlteraVvpScaler::set_h_pos_error(std::uint32_t h_pos_err) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_h_pos_error(driver_, h_pos_err));
}

hcl::Status AlteraVvpScaler::set_v_pos_error(std::uint32_t v_pos_err) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_v_pos_error(driver_, v_pos_err));
}

hcl::Status AlteraVvpScaler::set_h_extended_fill(bool extended_fill) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_h_extended_fill(driver_, extended_fill));
}

hcl::Status AlteraVvpScaler::set_v_extended_fill(bool extended_fill) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_v_extended_fill(driver_, extended_fill));
}

hcl::Status AlteraVvpScaler::set_h_phase_offset_error(std::uint32_t phase_offset, std::uint32_t phase_error) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_scaler_set_h_phase_offset_error(driver_, phase_offset, phase_error));
}

hcl::Status AlteraVvpScaler::set_v_phase_offset_error(std::uint32_t phase_offset, std::uint32_t phase_error) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_scaler_set_v_phase_offset_error(driver_, phase_offset, phase_error));
}

hcl::Status AlteraVvpScaler::set_split_h_scaling(
    std::uint32_t total_input_width,
    std::uint32_t total_output_width,
    std::uint32_t split_output_width,
    std::uint32_t split_output_left_offset,
    bool extended_fill,
    std::uint32_t* split_input_width,
    std::uint32_t* split_input_left_offset) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_split_h_scaling(
        driver_, total_input_width, total_output_width, split_output_width, split_output_left_offset,
        extended_fill, split_input_width, split_input_left_offset));
}

hcl::Status AlteraVvpScaler::set_split_v_scaling(
    std::uint32_t total_input_height,
    std::uint32_t total_output_height,
    std::uint32_t split_output_height,
    std::uint32_t split_output_top_offset,
    bool extended_fill,
    std::uint32_t* split_input_height,
    std::uint32_t* split_input_top_offset) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_set_split_v_scaling(
        driver_, total_input_height, total_output_height, split_output_height, split_output_top_offset,
        extended_fill, split_input_height, split_input_top_offset));
}

hcl::Result<std::uint8_t> AlteraVvpScaler::get_h_bank() {
    std::uint8_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_h_bank(driver_, &out), out);
}

hcl::Result<std::uint8_t> AlteraVvpScaler::get_v_bank() {
    std::uint8_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_v_bank(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_output_width() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_output_width(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_output_height() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_output_height(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_total_input_width() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_total_input_width(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_total_output_width() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_total_output_width(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_total_input_height() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_total_input_height(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_total_output_height() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_total_output_height(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_h_pos_error() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_h_pos_error(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpScaler::get_v_pos_error() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_v_pos_error(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpScaler::get_h_extended_fill() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_h_extended_fill(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpScaler::get_v_extended_fill() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_scaler_get_v_extended_fill(driver_, &out), out);
}

hcl::Result<AlteraVvpScaler::PhaseOffsetError> AlteraVvpScaler::get_h_phase_offset_error() {
    PhaseOffsetError out{0u, 0u};
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_scaler_get_h_phase_offset_error(driver_, &out.phase_offset, &out.phase_error),
        out);
}

hcl::Result<AlteraVvpScaler::PhaseOffsetError> AlteraVvpScaler::get_v_phase_offset_error() {
    PhaseOffsetError out{0u, 0u};
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_scaler_get_v_phase_offset_error(driver_, &out.phase_offset, &out.phase_error),
        out);
}

hcl::Status AlteraVvpScaler::set_quantized_coeffs(bool h_coeffs,
                                                   const std::int32_t* quantized_coeffs,
                                                   std::uint8_t num_coeffs,
                                                   std::uint8_t bank,
                                                   std::uint8_t phase) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_scaler_set_quantized_coeffs(driver_, h_coeffs, quantized_coeffs, num_coeffs, bank, phase));
}

hcl::Status AlteraVvpScaler::set_coeffs(bool h_coeffs,
                                        const float* coeffs,
                                        std::uint8_t num_coeffs,
                                        std::uint8_t bank,
                                        std::uint8_t phase) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_scaler_set_coeffs(driver_, h_coeffs, coeffs, num_coeffs, bank, phase));
}

hcl::Status AlteraVvpScaler::commit() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_scaler_commit(driver_));
}

}  // namespace hcl::drivers
