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

#pragma once

#include <cstdint>
#include <memory>

#include "hcl/drivers/altera_vvp_core.hpp"
#include "hcl/drivers/altera_vvp_tpg.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraVvpTpg {
public:
    using BarsType = hcl_altera_vvp_tpg_bars_type;
    using Pattern = hcl_altera_vvp_tpg_pattern;
    using PatternColor = hcl_altera_vvp_tpg_pattern_color;
    using PatternType = hcl_altera_vvp_tpg_pattern_type;

    AlteraVvpTpg(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address);
    ~AlteraVvpTpg();

    hcl::Status init();

    bool lite_mode() const;
    bool debug_enabled() const;
    std::uint8_t num_patterns() const;
    std::uint8_t bits_per_sample() const;
    std::uint8_t pixels_in_parallel() const;
    std::uint32_t img_info_width() const;
    hcl::Status set_img_info_width(std::uint32_t width);
    std::uint32_t img_info_height() const;
    hcl::Status set_img_info_height(std::uint32_t height);
    std::uint32_t img_info_interlace() const;
    hcl::Status set_img_info_interlace(std::uint32_t interlace);

    hcl::Result<Pattern> get_pattern_data(std::uint8_t pattern);

    hcl::Result<bool> is_running();
    hcl::Result<bool> commit_is_pending();
    hcl::Result<std::uint32_t> get_status();

    hcl::Status enable(bool enabled);
    hcl::Status start();
    hcl::Status stop();
    hcl::Result<bool> is_enabled();

    hcl::Status set_pattern(std::uint8_t pattern);
    hcl::Result<std::uint8_t> get_pattern();
    hcl::Result<std::uint32_t> get_fields_since_sequence_reset();

    hcl::Status set_bars_type(BarsType bars_type);
    hcl::Result<BarsType> get_bars_type();

    hcl::Status set_colors(std::uint16_t color1, std::uint16_t color2, std::uint16_t color3);
    hcl::Result<std::uint16_t> get_color1();
    hcl::Result<std::uint16_t> get_color2();
    hcl::Result<std::uint16_t> get_color3();

    hcl::Status set_zone_plate_origin(std::uint16_t x_coord, std::uint16_t y_coord);
    hcl::Result<std::uint16_t> get_zone_plate_origin_x();
    hcl::Result<std::uint16_t> get_zone_plate_origin_y();

    hcl::Status set_zone_plate_factors(std::uint16_t scaling_factor, std::uint16_t fine_tune_factor);
    hcl::Result<std::uint16_t> get_zone_plate_scaling_factor();
    hcl::Result<std::uint16_t> get_zone_plate_fine_tune_factor();

    hcl::Status set_digital_clock_background_color(std::uint16_t background_r,
                                                   std::uint16_t background_g,
                                                   std::uint16_t background_b);
    hcl::Result<std::uint16_t> get_digital_clock_background_r();
    hcl::Result<std::uint16_t> get_digital_clock_background_g();
    hcl::Result<std::uint16_t> get_digital_clock_background_b();

    hcl::Status set_digital_clock_font_color(std::uint16_t font_r,
                                             std::uint16_t font_g,
                                             std::uint16_t font_b);
    hcl::Result<std::uint16_t> get_digital_clock_font_r();
    hcl::Result<std::uint16_t> get_digital_clock_font_g();
    hcl::Result<std::uint16_t> get_digital_clock_font_b();

    hcl::Status set_digital_clock_location(std::uint16_t x, std::uint16_t y);
    hcl::Result<std::uint16_t> get_digital_clock_location_x();
    hcl::Result<std::uint16_t> get_digital_clock_location_y();

    hcl::Status set_digital_clock_scale_factor(std::uint16_t scale_factor);
    hcl::Result<std::uint16_t> get_digital_clock_scale_factor();

    hcl::Status set_digital_clock_fps(std::uint8_t fps);
    hcl::Result<std::uint8_t> get_digital_clock_fps();

    hcl::Status set_signaltap_background_color(std::uint16_t background_r,
                                               std::uint16_t background_g,
                                               std::uint16_t background_b);
    hcl::Result<std::uint16_t> get_signaltap_background_r();
    hcl::Result<std::uint16_t> get_signaltap_background_g();
    hcl::Result<std::uint16_t> get_signaltap_background_b();

    hcl::Status set_signaltap_font_color(std::uint16_t font_r,
                                         std::uint16_t font_g,
                                         std::uint16_t font_b);
    hcl::Result<std::uint16_t> get_signaltap_font_r();
    hcl::Result<std::uint16_t> get_signaltap_font_g();
    hcl::Result<std::uint16_t> get_signaltap_font_b();

    hcl::Status set_signaltap_location(std::uint16_t x, std::uint16_t y);
    hcl::Result<std::uint16_t> get_signaltap_location_x();
    hcl::Result<std::uint16_t> get_signaltap_location_y();

    hcl::Status set_signaltap_fps(std::uint8_t fps);
    hcl::Result<std::uint8_t> get_signaltap_fps();

    hcl::Status commit();

private:
    std::shared_ptr<hcl::RegisterInterface> reg_;
    hcl_altera_vvp_tpg* driver_;
};

}  // namespace hcl::drivers