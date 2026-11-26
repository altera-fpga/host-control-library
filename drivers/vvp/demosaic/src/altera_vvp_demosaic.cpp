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

#include "hcl/drivers/altera_vvp_demosaic.hpp"

namespace hcl::drivers {

AlteraVvpDemosaic::AlteraVvpDemosaic(std::shared_ptr<hcl::RegisterInterface> reg,
                                     std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_demosaic_create(reg_ ? reg_->c_handle() : nullptr,
                                             base_address)) {}

AlteraVvpDemosaic::~AlteraVvpDemosaic() {
    hcl_altera_vvp_demosaic_destroy(driver_);
}

hcl::Status AlteraVvpDemosaic::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_demosaic_init(driver_));
}

hcl::Result<AlteraVvp::Id> AlteraVvpDemosaic::id() const {
    AlteraVvp::Id out{};
    return hcl::StatusHelper::to_result(hcl_altera_vvp_demosaic_get_id(driver_, &out), out);
}

hcl::Result<AlteraVvp::Version> AlteraVvpDemosaic::version() const {
    AlteraVvp::Version out{};
    return hcl::StatusHelper::to_result(hcl_altera_vvp_demosaic_get_version(driver_, &out), out);
}

bool AlteraVvpDemosaic::lite_mode() const {
    return hcl_altera_vvp_demosaic_get_lite_mode(driver_);
}

bool AlteraVvpDemosaic::debug_enabled() const {
    return hcl_altera_vvp_demosaic_get_debug_enabled(driver_);
}

std::uint8_t AlteraVvpDemosaic::bits_per_sample_in() const {
    return hcl_altera_vvp_demosaic_get_bits_per_sample_in(driver_);
}

std::uint8_t AlteraVvpDemosaic::bits_per_sample_out() const {
    return hcl_altera_vvp_demosaic_get_bits_per_sample_out(driver_);
}

std::uint8_t AlteraVvpDemosaic::num_color_planes_in() const {
    return hcl_altera_vvp_demosaic_get_num_color_planes_in(driver_);
}

std::uint8_t AlteraVvpDemosaic::num_color_planes_out() const {
    return hcl_altera_vvp_demosaic_get_num_color_planes_out(driver_);
}

std::uint8_t AlteraVvpDemosaic::pixels_in_parallel() const {
    return hcl_altera_vvp_demosaic_get_pixels_in_parallel(driver_);
}

std::uint32_t AlteraVvpDemosaic::max_width() const {
    return hcl_altera_vvp_demosaic_get_max_width(driver_);
}

std::uint32_t AlteraVvpDemosaic::max_height() const {
    return hcl_altera_vvp_demosaic_get_max_height(driver_);
}

std::uint32_t AlteraVvpDemosaic::img_info_width() const {
    return hcl_altera_vvp_demosaic_get_img_info_width(driver_);
}

hcl::Status AlteraVvpDemosaic::set_img_info_width(std::uint32_t width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_demosaic_set_img_info_width(driver_, width));
}

std::uint32_t AlteraVvpDemosaic::img_info_height() const {
    return hcl_altera_vvp_demosaic_get_img_info_height(driver_);
}

hcl::Status AlteraVvpDemosaic::set_img_info_height(std::uint32_t height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_demosaic_set_img_info_height(driver_, height));
}

hcl::Result<bool> AlteraVvpDemosaic::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_demosaic_is_running(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpDemosaic::get_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_demosaic_get_status(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpDemosaic::get_frame_stats() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_demosaic_get_frame_stats(driver_, &out), out);
}

bool AlteraVvpDemosaic::bypass() const {
    return hcl_altera_vvp_demosaic_get_bypass(driver_);
}

hcl::Status AlteraVvpDemosaic::set_bypass(bool bypass) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_demosaic_set_bypass(driver_, bypass));
}

std::uint8_t AlteraVvpDemosaic::cfa_phase() const {
    return hcl_altera_vvp_demosaic_get_cfa_phase(driver_);
}

hcl::Status AlteraVvpDemosaic::set_cfa_phase(std::uint8_t cfa_phase) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_demosaic_set_cfa_phase(driver_, cfa_phase));
}

}  // namespace hcl::drivers
