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

#include "hcl/drivers/altera_vvp_wbc.hpp"

namespace hcl::drivers {

AlteraVvpWbc::AlteraVvpWbc(std::shared_ptr<hcl::RegisterInterface> reg,
                           std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_wbc_create(reg_ ? reg_->c_handle() : nullptr,
                                        base_address)) {}

AlteraVvpWbc::~AlteraVvpWbc() {
    hcl_altera_vvp_wbc_destroy(driver_);
}

hcl::Status AlteraVvpWbc::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_init(driver_));
}

bool AlteraVvpWbc::lite_mode() const {
    return hcl_altera_vvp_wbc_get_lite_mode(driver_);
}

bool AlteraVvpWbc::debug_enabled() const {
    return hcl_altera_vvp_wbc_get_debug_enabled(driver_);
}

std::uint8_t AlteraVvpWbc::bits_per_sample_in() const {
    return hcl_altera_vvp_wbc_get_bits_per_sample_in(driver_);
}

std::uint8_t AlteraVvpWbc::bits_per_sample_out() const {
    return hcl_altera_vvp_wbc_get_bits_per_sample_out(driver_);
}

std::uint8_t AlteraVvpWbc::num_color_planes_in() const {
    return hcl_altera_vvp_wbc_get_num_color_planes_in(driver_);
}

std::uint8_t AlteraVvpWbc::num_color_planes_out() const {
    return hcl_altera_vvp_wbc_get_num_color_planes_out(driver_);
}

std::uint8_t AlteraVvpWbc::pixels_in_parallel() const {
    return hcl_altera_vvp_wbc_get_pixels_in_parallel(driver_);
}

std::uint32_t AlteraVvpWbc::max_width() const {
    return hcl_altera_vvp_wbc_get_max_width(driver_);
}

std::uint32_t AlteraVvpWbc::max_height() const {
    return hcl_altera_vvp_wbc_get_max_height(driver_);
}
std::uint32_t AlteraVvpWbc::img_info_width() const { return hcl_altera_vvp_wbc_get_img_info_width(driver_); }
hcl::Status AlteraVvpWbc::set_img_info_width(std::uint32_t width) { return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_img_info_width(driver_, width)); }
std::uint32_t AlteraVvpWbc::img_info_height() const { return hcl_altera_vvp_wbc_get_img_info_height(driver_); }
hcl::Status AlteraVvpWbc::set_img_info_height(std::uint32_t height) { return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_img_info_height(driver_, height)); }

hcl::Result<bool> AlteraVvpWbc::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_wbc_is_running(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpWbc::commit_is_pending() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_wbc_commit_is_pending(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpWbc::get_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_wbc_get_status(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpWbc::get_frame_stats() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_wbc_get_frame_stats(driver_, &out), out);
}

hcl::Status AlteraVvpWbc::commit() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_commit(driver_));
}

bool AlteraVvpWbc::bypass() const {
    return hcl_altera_vvp_wbc_get_bypass(driver_);
}

hcl::Status AlteraVvpWbc::set_bypass(bool bypass) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_bypass(driver_, bypass));
}

std::uint8_t AlteraVvpWbc::cfa_phase() const {
    return hcl_altera_vvp_wbc_get_cfa_phase(driver_);
}

hcl::Status AlteraVvpWbc::set_cfa_phase(std::uint8_t cfa_phase) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_cfa_phase(driver_, cfa_phase));
}

hcl::Result<std::uint32_t> AlteraVvpWbc::get_cfa_00_color_scaler() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_wbc_get_cfa_00_color_scaler(driver_, &out), out);
}

hcl::Status AlteraVvpWbc::set_cfa_00_color_scaler(std::uint32_t scaler) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_cfa_00_color_scaler(driver_, scaler));
}

hcl::Result<std::uint32_t> AlteraVvpWbc::get_cfa_01_color_scaler() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_wbc_get_cfa_01_color_scaler(driver_, &out), out);
}

hcl::Status AlteraVvpWbc::set_cfa_01_color_scaler(std::uint32_t scaler) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_cfa_01_color_scaler(driver_, scaler));
}

hcl::Result<std::uint32_t> AlteraVvpWbc::get_cfa_10_color_scaler() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_wbc_get_cfa_10_color_scaler(driver_, &out), out);
}

hcl::Status AlteraVvpWbc::set_cfa_10_color_scaler(std::uint32_t scaler) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_cfa_10_color_scaler(driver_, scaler));
}

hcl::Result<std::uint32_t> AlteraVvpWbc::get_cfa_11_color_scaler() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_wbc_get_cfa_11_color_scaler(driver_, &out), out);
}

hcl::Status AlteraVvpWbc::set_cfa_11_color_scaler(std::uint32_t scaler) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_cfa_11_color_scaler(driver_, scaler));
}

hcl::Status AlteraVvpWbc::set_cfa_settings(const hcl_altera_vvp_wbc_cfa_settings& settings) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_wbc_set_cfa_settings(driver_, &settings));
}

}  // namespace hcl::drivers
