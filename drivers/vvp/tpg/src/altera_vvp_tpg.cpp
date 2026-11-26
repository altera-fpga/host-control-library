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

#include "hcl/drivers/altera_vvp_tpg.hpp"

namespace hcl::drivers {

AlteraVvpTpg::AlteraVvpTpg(std::shared_ptr<hcl::RegisterInterface> reg,
                           std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_tpg_create(reg_ ? reg_->c_handle() : nullptr,
                                        base_address)) {}

AlteraVvpTpg::~AlteraVvpTpg() {
    hcl_altera_vvp_tpg_destroy(driver_);
}

hcl::Status AlteraVvpTpg::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_init(driver_));
}

bool AlteraVvpTpg::lite_mode() const { return hcl_altera_vvp_tpg_get_lite_mode(driver_); }
bool AlteraVvpTpg::debug_enabled() const { return hcl_altera_vvp_tpg_get_debug_enabled(driver_); }
std::uint8_t AlteraVvpTpg::num_patterns() const { return hcl_altera_vvp_tpg_get_num_patterns(driver_); }
std::uint8_t AlteraVvpTpg::bits_per_sample() const { return hcl_altera_vvp_tpg_get_bits_per_sample(driver_); }
std::uint8_t AlteraVvpTpg::pixels_in_parallel() const { return hcl_altera_vvp_tpg_get_pixels_in_parallel(driver_); }
std::uint32_t AlteraVvpTpg::img_info_width() const { return hcl_altera_vvp_tpg_get_img_info_width(driver_); }
hcl::Status AlteraVvpTpg::set_img_info_width(std::uint32_t width) { return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_img_info_width(driver_, width)); }
std::uint32_t AlteraVvpTpg::img_info_height() const { return hcl_altera_vvp_tpg_get_img_info_height(driver_); }
hcl::Status AlteraVvpTpg::set_img_info_height(std::uint32_t height) { return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_img_info_height(driver_, height)); }
std::uint32_t AlteraVvpTpg::img_info_interlace() const { return hcl_altera_vvp_tpg_get_img_info_interlace(driver_); }
hcl::Status AlteraVvpTpg::set_img_info_interlace(std::uint32_t interlace) { return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_img_info_interlace(driver_, interlace)); }

hcl::Result<AlteraVvpTpg::Pattern> AlteraVvpTpg::get_pattern_data(std::uint8_t pattern) {
    AlteraVvpTpg::Pattern out = {HCL_VVP_TPG_PATTERN_TYPE_INVALID, HCL_VVP_TPG_PATTERN_COLOR_INVALID};
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_pattern_data(driver_, pattern, &out), out);
}

hcl::Result<bool> AlteraVvpTpg::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_is_running(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpTpg::commit_is_pending() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_commit_is_pending(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpTpg::get_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_status(driver_, &out), out);
}

hcl::Status AlteraVvpTpg::enable(bool enabled) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_enable(driver_, enabled));
}

hcl::Status AlteraVvpTpg::start() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_start(driver_));
}

hcl::Status AlteraVvpTpg::stop() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_stop(driver_));
}

hcl::Result<bool> AlteraVvpTpg::is_enabled() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_is_enabled(driver_, &out), out);
}

hcl::Status AlteraVvpTpg::set_pattern(std::uint8_t pattern) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_pattern(driver_, pattern));
}

hcl::Result<std::uint8_t> AlteraVvpTpg::get_pattern() {
    std::uint8_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_pattern(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpTpg::get_fields_since_sequence_reset() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_fields_since_sequence_reset(driver_, &out), out);
}

hcl::Status AlteraVvpTpg::set_bars_type(AlteraVvpTpg::BarsType bars_type) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_bars_type(driver_, bars_type));
}

hcl::Result<AlteraVvpTpg::BarsType> AlteraVvpTpg::get_bars_type() {
    AlteraVvpTpg::BarsType out = HCL_VVP_TPG_BARS_TYPE_INVALID;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_bars_type(driver_, &out), out);
}

hcl::Status AlteraVvpTpg::set_colors(std::uint16_t color1, std::uint16_t color2, std::uint16_t color3) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_colors(driver_, color1, color2, color3));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_color1() {
    std::uint16_t c1 = 0u;
    std::uint16_t c2 = 0u;
    std::uint16_t c3 = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_colors(driver_, &c1, &c2, &c3), c1);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_color2() {
    std::uint16_t c1 = 0u;
    std::uint16_t c2 = 0u;
    std::uint16_t c3 = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_colors(driver_, &c1, &c2, &c3), c2);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_color3() {
    std::uint16_t c1 = 0u;
    std::uint16_t c2 = 0u;
    std::uint16_t c3 = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_colors(driver_, &c1, &c2, &c3), c3);
}

hcl::Status AlteraVvpTpg::set_zone_plate_origin(std::uint16_t x_coord, std::uint16_t y_coord) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_zone_plate_origin(driver_, x_coord, y_coord));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_zone_plate_origin_x() {
    std::uint16_t x = 0u;
    std::uint16_t y = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_zone_plate_origin(driver_, &x, &y), x);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_zone_plate_origin_y() {
    std::uint16_t x = 0u;
    std::uint16_t y = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_zone_plate_origin(driver_, &x, &y), y);
}

hcl::Status AlteraVvpTpg::set_zone_plate_factors(std::uint16_t scaling_factor, std::uint16_t fine_tune_factor) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_tpg_set_zone_plate_factors(driver_, scaling_factor, fine_tune_factor));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_zone_plate_scaling_factor() {
    std::uint16_t scaling = 0u;
    std::uint16_t fine = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_zone_plate_factors(driver_, &scaling, &fine), scaling);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_zone_plate_fine_tune_factor() {
    std::uint16_t scaling = 0u;
    std::uint16_t fine = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_zone_plate_factors(driver_, &scaling, &fine), fine);
}

hcl::Status AlteraVvpTpg::set_digital_clock_background_color(std::uint16_t background_r,
                                                             std::uint16_t background_g,
                                                             std::uint16_t background_b) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_digital_clock_background_color(
        driver_, background_r, background_g, background_b));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_background_r() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_digital_clock_background_color(driver_, &r, &g, &b), r);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_background_g() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_digital_clock_background_color(driver_, &r, &g, &b), g);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_background_b() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_digital_clock_background_color(driver_, &r, &g, &b), b);
}

hcl::Status AlteraVvpTpg::set_digital_clock_font_color(std::uint16_t font_r,
                                                       std::uint16_t font_g,
                                                       std::uint16_t font_b) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_tpg_set_digital_clock_font_color(driver_, font_r, font_g, font_b));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_font_r() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_digital_clock_font_color(driver_, &r, &g, &b), r);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_font_g() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_digital_clock_font_color(driver_, &r, &g, &b), g);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_font_b() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_digital_clock_font_color(driver_, &r, &g, &b), b);
}

hcl::Status AlteraVvpTpg::set_digital_clock_location(std::uint16_t x, std::uint16_t y) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_digital_clock_location(driver_, x, y));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_location_x() {
    std::uint16_t x = 0u;
    std::uint16_t y = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_digital_clock_location(driver_, &x, &y), x);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_location_y() {
    std::uint16_t x = 0u;
    std::uint16_t y = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_digital_clock_location(driver_, &x, &y), y);
}

hcl::Status AlteraVvpTpg::set_digital_clock_scale_factor(std::uint16_t scale_factor) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_tpg_set_digital_clock_scale_factor(driver_, scale_factor));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_digital_clock_scale_factor() {
    std::uint16_t out = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_digital_clock_scale_factor(driver_, &out), out);
}

hcl::Status AlteraVvpTpg::set_digital_clock_fps(std::uint8_t fps) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_digital_clock_fps(driver_, fps));
}

hcl::Result<std::uint8_t> AlteraVvpTpg::get_digital_clock_fps() {
    std::uint8_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_digital_clock_fps(driver_, &out), out);
}

hcl::Status AlteraVvpTpg::set_signaltap_background_color(std::uint16_t background_r,
                                                         std::uint16_t background_g,
                                                         std::uint16_t background_b) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_signaltap_background_color(
        driver_, background_r, background_g, background_b));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_signaltap_background_r() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_signaltap_background_color(driver_, &r, &g, &b), r);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_signaltap_background_g() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_signaltap_background_color(driver_, &r, &g, &b), g);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_signaltap_background_b() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_signaltap_background_color(driver_, &r, &g, &b), b);
}

hcl::Status AlteraVvpTpg::set_signaltap_font_color(std::uint16_t font_r,
                                                   std::uint16_t font_g,
                                                   std::uint16_t font_b) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_tpg_set_signaltap_font_color(driver_, font_r, font_g, font_b));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_signaltap_font_r() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_signaltap_font_color(driver_, &r, &g, &b), r);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_signaltap_font_g() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_signaltap_font_color(driver_, &r, &g, &b), g);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_signaltap_font_b() {
    std::uint16_t r = 0u;
    std::uint16_t g = 0u;
    std::uint16_t b = 0u;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_tpg_get_signaltap_font_color(driver_, &r, &g, &b), b);
}

hcl::Status AlteraVvpTpg::set_signaltap_location(std::uint16_t x, std::uint16_t y) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_signaltap_location(driver_, x, y));
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_signaltap_location_x() {
    std::uint16_t x = 0u;
    std::uint16_t y = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_signaltap_location(driver_, &x, &y), x);
}

hcl::Result<std::uint16_t> AlteraVvpTpg::get_signaltap_location_y() {
    std::uint16_t x = 0u;
    std::uint16_t y = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_signaltap_location(driver_, &x, &y), y);
}

hcl::Status AlteraVvpTpg::set_signaltap_fps(std::uint8_t fps) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_set_signaltap_fps(driver_, fps));
}

hcl::Result<std::uint8_t> AlteraVvpTpg::get_signaltap_fps() {
    std::uint8_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_tpg_get_signaltap_fps(driver_, &out), out);
}

hcl::Status AlteraVvpTpg::commit() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_tpg_commit(driver_));
}

}  // namespace hcl::drivers
