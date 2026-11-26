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

#include "hcl/drivers/altera_vvp_blc.hpp"

namespace hcl::drivers {

AlteraVvpBlc::AlteraVvpBlc(std::shared_ptr<hcl::RegisterInterface> reg,
                           std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_blc_create(reg_ ? reg_->c_handle() : nullptr,
                                        base_address)) {}

AlteraVvpBlc::~AlteraVvpBlc() {
    hcl_altera_vvp_blc_destroy(driver_);
}

hcl::Status AlteraVvpBlc::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_init(driver_));
}

bool AlteraVvpBlc::lite_mode() const { return hcl_altera_vvp_blc_get_lite_mode(driver_); }
bool AlteraVvpBlc::debug_enabled() const { return hcl_altera_vvp_blc_get_debug_enabled(driver_); }
std::uint8_t AlteraVvpBlc::bps_in() const { return hcl_altera_vvp_blc_get_bps_in(driver_); }
std::uint8_t AlteraVvpBlc::bps_out() const { return hcl_altera_vvp_blc_get_bps_out(driver_); }
std::uint8_t AlteraVvpBlc::num_color_in() const { return hcl_altera_vvp_blc_get_num_color_in(driver_); }
std::uint8_t AlteraVvpBlc::num_color_out() const { return hcl_altera_vvp_blc_get_num_color_out(driver_); }
std::uint8_t AlteraVvpBlc::pip() const { return hcl_altera_vvp_blc_get_pip(driver_); }
std::uint32_t AlteraVvpBlc::max_width() const { return hcl_altera_vvp_blc_get_max_width(driver_); }
std::uint32_t AlteraVvpBlc::max_height() const { return hcl_altera_vvp_blc_get_max_height(driver_); }
std::uint32_t AlteraVvpBlc::img_info_width() const { return hcl_altera_vvp_blc_get_img_info_width(driver_); }
hcl::Status AlteraVvpBlc::set_img_info_width(std::uint32_t width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_img_info_width(driver_, width));
}
std::uint32_t AlteraVvpBlc::img_info_height() const { return hcl_altera_vvp_blc_get_img_info_height(driver_); }
hcl::Status AlteraVvpBlc::set_img_info_height(std::uint32_t height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_img_info_height(driver_, height));
}
bool AlteraVvpBlc::reflect_around_zero() const { return hcl_altera_vvp_blc_get_reflect_around_zero(driver_); }

hcl::Result<bool> AlteraVvpBlc::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_is_running(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpBlc::commit_is_pending() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_commit_is_pending(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_status(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_frame_stats() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_frame_stats(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::commit() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_commit(driver_));
}

bool AlteraVvpBlc::bypass() const {
    return hcl_altera_vvp_blc_get_bypass(driver_);
}

hcl::Status AlteraVvpBlc::set_bypass(bool bypass) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_bypass(driver_, bypass));
}

std::uint8_t AlteraVvpBlc::cfa_phase() const {
    return hcl_altera_vvp_blc_get_cfa_phase(driver_);
}

hcl::Status AlteraVvpBlc::set_cfa_phase(std::uint8_t cfa_phase) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_phase(driver_, cfa_phase));
}

bool AlteraVvpBlc::clip_zero_en() const {
    return hcl_altera_vvp_blc_get_clip_zero_en(driver_);
}

hcl::Status AlteraVvpBlc::set_clip_zero_en(bool clip_zero_en) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_clip_zero_en(driver_, clip_zero_en));
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_cfa_00_black_pedestal() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_cfa_00_black_pedestal(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::set_cfa_00_black_pedestal(std::uint32_t pedestal) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_00_black_pedestal(driver_, pedestal));
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_cfa_00_color_scaler() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_cfa_00_color_scaler(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::set_cfa_00_color_scaler(std::uint32_t scaler) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_00_color_scaler(driver_, scaler));
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_cfa_01_black_pedestal() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_cfa_01_black_pedestal(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::set_cfa_01_black_pedestal(std::uint32_t pedestal) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_01_black_pedestal(driver_, pedestal));
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_cfa_01_color_scaler() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_cfa_01_color_scaler(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::set_cfa_01_color_scaler(std::uint32_t scaler) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_01_color_scaler(driver_, scaler));
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_cfa_10_black_pedestal() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_cfa_10_black_pedestal(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::set_cfa_10_black_pedestal(std::uint32_t pedestal) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_10_black_pedestal(driver_, pedestal));
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_cfa_10_color_scaler() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_cfa_10_color_scaler(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::set_cfa_10_color_scaler(std::uint32_t scaler) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_10_color_scaler(driver_, scaler));
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_cfa_11_black_pedestal() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_cfa_11_black_pedestal(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::set_cfa_11_black_pedestal(std::uint32_t pedestal) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_11_black_pedestal(driver_, pedestal));
}

hcl::Result<std::uint32_t> AlteraVvpBlc::get_cfa_11_color_scaler() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_blc_get_cfa_11_color_scaler(driver_, &out), out);
}

hcl::Status AlteraVvpBlc::set_cfa_11_color_scaler(std::uint32_t scaler) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_11_color_scaler(driver_, scaler));
}

hcl::Status AlteraVvpBlc::set_cfa_settings(const hcl_altera_vvp_blc_cfa_settings& settings) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_blc_set_cfa_settings(driver_, &settings));
}

}  // namespace hcl::drivers
