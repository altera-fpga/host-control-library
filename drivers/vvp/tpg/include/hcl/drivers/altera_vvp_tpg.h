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

#include <stdbool.h>
#include <stdint.h>

#include "hcl/drivers/altera_vvp_tpg_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_vvp_tpg hcl_altera_vvp_tpg;

typedef enum hcl_altera_vvp_tpg_pattern_type {
    HCL_VVP_TPG_PATTERN_TYPE_BARS,
    HCL_VVP_TPG_PATTERN_TYPE_UNIFORM,
    HCL_VVP_TPG_PATTERN_TYPE_PATHOLOGICAL,
    HCL_VVP_TPG_PATTERN_TYPE_ZONE_PLATE,
    HCL_VVP_TPG_PATTERN_TYPE_DIGITAL_CLOCK,
    HCL_VVP_TPG_PATTERN_TYPE_SIGNALTAP,
    HCL_VVP_TPG_PATTERN_TYPE_INVALID
} hcl_altera_vvp_tpg_pattern_type;

typedef enum hcl_altera_vvp_tpg_pattern_color {
    HCL_VVP_TPG_PATTERN_COLOR_RGB,
    HCL_VVP_TPG_PATTERN_COLOR_YCC_444,
    HCL_VVP_TPG_PATTERN_COLOR_YCC_422,
    HCL_VVP_TPG_PATTERN_COLOR_YCC_420,
    HCL_VVP_TPG_PATTERN_COLOR_MONO,
    HCL_VVP_TPG_PATTERN_COLOR_INVALID
} hcl_altera_vvp_tpg_pattern_color;

typedef struct hcl_altera_vvp_tpg_pattern {
    hcl_altera_vvp_tpg_pattern_type type;
    hcl_altera_vvp_tpg_pattern_color color;
} hcl_altera_vvp_tpg_pattern;

typedef enum hcl_altera_vvp_tpg_bars_type {
    HCL_VVP_TPG_BARS_TYPE_COLOR,
    HCL_VVP_TPG_BARS_TYPE_GREY,
    HCL_VVP_TPG_BARS_TYPE_BLACK_WHITE,
    HCL_VVP_TPG_BARS_TYPE_MIXED,
    HCL_VVP_TPG_BARS_TYPE_INVALID
} hcl_altera_vvp_tpg_bars_type;

hcl_altera_vvp_tpg* hcl_altera_vvp_tpg_create(hcl_register_interface* reg,
                                              uint32_t base_address);

void hcl_altera_vvp_tpg_destroy(hcl_altera_vvp_tpg* driver);

hcl_status hcl_altera_vvp_tpg_init(hcl_altera_vvp_tpg* driver);

bool hcl_altera_vvp_tpg_get_lite_mode(const hcl_altera_vvp_tpg* driver);
bool hcl_altera_vvp_tpg_get_debug_enabled(const hcl_altera_vvp_tpg* driver);
uint8_t hcl_altera_vvp_tpg_get_num_patterns(const hcl_altera_vvp_tpg* driver);
uint8_t hcl_altera_vvp_tpg_get_bits_per_sample(const hcl_altera_vvp_tpg* driver);
uint8_t hcl_altera_vvp_tpg_get_pixels_in_parallel(const hcl_altera_vvp_tpg* driver);
uint32_t hcl_altera_vvp_tpg_get_img_info_width(const hcl_altera_vvp_tpg* driver);
hcl_status hcl_altera_vvp_tpg_set_img_info_width(hcl_altera_vvp_tpg* driver, uint32_t width);
uint32_t hcl_altera_vvp_tpg_get_img_info_height(const hcl_altera_vvp_tpg* driver);
hcl_status hcl_altera_vvp_tpg_set_img_info_height(hcl_altera_vvp_tpg* driver, uint32_t height);
uint32_t hcl_altera_vvp_tpg_get_img_info_interlace(const hcl_altera_vvp_tpg* driver);
hcl_status hcl_altera_vvp_tpg_set_img_info_interlace(hcl_altera_vvp_tpg* driver, uint32_t interlace);

hcl_status hcl_altera_vvp_tpg_get_pattern_data(hcl_altera_vvp_tpg* driver,
                                               uint8_t pattern,
                                               hcl_altera_vvp_tpg_pattern* out_pattern_data);

hcl_status hcl_altera_vvp_tpg_is_running(hcl_altera_vvp_tpg* driver, bool* out_running);
hcl_status hcl_altera_vvp_tpg_commit_is_pending(hcl_altera_vvp_tpg* driver, bool* out_pending);
hcl_status hcl_altera_vvp_tpg_get_status(hcl_altera_vvp_tpg* driver, uint32_t* out_status);

hcl_status hcl_altera_vvp_tpg_enable(hcl_altera_vvp_tpg* driver, bool enabled);
hcl_status hcl_altera_vvp_tpg_start(hcl_altera_vvp_tpg* driver);
hcl_status hcl_altera_vvp_tpg_stop(hcl_altera_vvp_tpg* driver);
hcl_status hcl_altera_vvp_tpg_is_enabled(hcl_altera_vvp_tpg* driver, bool* out_enabled);

hcl_status hcl_altera_vvp_tpg_set_pattern(hcl_altera_vvp_tpg* driver, uint8_t pattern);
hcl_status hcl_altera_vvp_tpg_get_pattern(hcl_altera_vvp_tpg* driver, uint8_t* out_pattern);

hcl_status hcl_altera_vvp_tpg_get_fields_since_sequence_reset(hcl_altera_vvp_tpg* driver,
                                                              uint32_t* out_field_count);

hcl_status hcl_altera_vvp_tpg_set_bars_type(hcl_altera_vvp_tpg* driver,
                                            hcl_altera_vvp_tpg_bars_type bar_type);
hcl_status hcl_altera_vvp_tpg_get_bars_type(hcl_altera_vvp_tpg* driver,
                                            hcl_altera_vvp_tpg_bars_type* out_bar_type);

hcl_status hcl_altera_vvp_tpg_set_colors(hcl_altera_vvp_tpg* driver,
                                         uint16_t color1,
                                         uint16_t color2,
                                         uint16_t color3);
hcl_status hcl_altera_vvp_tpg_get_colors(hcl_altera_vvp_tpg* driver,
                                         uint16_t* out_color1,
                                         uint16_t* out_color2,
                                         uint16_t* out_color3);

hcl_status hcl_altera_vvp_tpg_set_zone_plate_origin(hcl_altera_vvp_tpg* driver,
                                                    uint16_t x_coord,
                                                    uint16_t y_coord);
hcl_status hcl_altera_vvp_tpg_get_zone_plate_origin(hcl_altera_vvp_tpg* driver,
                                                    uint16_t* out_x_coord,
                                                    uint16_t* out_y_coord);

hcl_status hcl_altera_vvp_tpg_set_zone_plate_factors(hcl_altera_vvp_tpg* driver,
                                                     uint16_t scaling_factor,
                                                     uint16_t fine_tune_factor);
hcl_status hcl_altera_vvp_tpg_get_zone_plate_factors(hcl_altera_vvp_tpg* driver,
                                                     uint16_t* out_scaling_factor,
                                                     uint16_t* out_fine_tune_factor);

hcl_status hcl_altera_vvp_tpg_set_digital_clock_background_color(hcl_altera_vvp_tpg* driver,
                                                                 uint16_t background_r,
                                                                 uint16_t background_g,
                                                                 uint16_t background_b);
hcl_status hcl_altera_vvp_tpg_get_digital_clock_background_color(hcl_altera_vvp_tpg* driver,
                                                                 uint16_t* out_background_r,
                                                                 uint16_t* out_background_g,
                                                                 uint16_t* out_background_b);

hcl_status hcl_altera_vvp_tpg_set_digital_clock_font_color(hcl_altera_vvp_tpg* driver,
                                                           uint16_t font_r,
                                                           uint16_t font_g,
                                                           uint16_t font_b);
hcl_status hcl_altera_vvp_tpg_get_digital_clock_font_color(hcl_altera_vvp_tpg* driver,
                                                           uint16_t* out_font_r,
                                                           uint16_t* out_font_g,
                                                           uint16_t* out_font_b);

hcl_status hcl_altera_vvp_tpg_set_digital_clock_location(hcl_altera_vvp_tpg* driver,
                                                         uint16_t location_x,
                                                         uint16_t location_y);
hcl_status hcl_altera_vvp_tpg_get_digital_clock_location(hcl_altera_vvp_tpg* driver,
                                                         uint16_t* out_location_x,
                                                         uint16_t* out_location_y);

hcl_status hcl_altera_vvp_tpg_set_digital_clock_scale_factor(hcl_altera_vvp_tpg* driver,
                                                             uint16_t scale_factor);
hcl_status hcl_altera_vvp_tpg_get_digital_clock_scale_factor(hcl_altera_vvp_tpg* driver,
                                                             uint16_t* out_scale_factor);

hcl_status hcl_altera_vvp_tpg_set_digital_clock_fps(hcl_altera_vvp_tpg* driver,
                                                    uint8_t fps);
hcl_status hcl_altera_vvp_tpg_get_digital_clock_fps(hcl_altera_vvp_tpg* driver,
                                                    uint8_t* out_fps);

hcl_status hcl_altera_vvp_tpg_set_signaltap_background_color(hcl_altera_vvp_tpg* driver,
                                                             uint16_t background_r,
                                                             uint16_t background_g,
                                                             uint16_t background_b);
hcl_status hcl_altera_vvp_tpg_get_signaltap_background_color(hcl_altera_vvp_tpg* driver,
                                                             uint16_t* out_background_r,
                                                             uint16_t* out_background_g,
                                                             uint16_t* out_background_b);

hcl_status hcl_altera_vvp_tpg_set_signaltap_font_color(hcl_altera_vvp_tpg* driver,
                                                       uint16_t font_r,
                                                       uint16_t font_g,
                                                       uint16_t font_b);
hcl_status hcl_altera_vvp_tpg_get_signaltap_font_color(hcl_altera_vvp_tpg* driver,
                                                       uint16_t* out_font_r,
                                                       uint16_t* out_font_g,
                                                       uint16_t* out_font_b);

hcl_status hcl_altera_vvp_tpg_set_signaltap_location(hcl_altera_vvp_tpg* driver,
                                                     uint16_t location_x,
                                                     uint16_t location_y);
hcl_status hcl_altera_vvp_tpg_get_signaltap_location(hcl_altera_vvp_tpg* driver,
                                                     uint16_t* out_location_x,
                                                     uint16_t* out_location_y);

hcl_status hcl_altera_vvp_tpg_set_signaltap_fps(hcl_altera_vvp_tpg* driver,
                                                uint8_t fps);
hcl_status hcl_altera_vvp_tpg_get_signaltap_fps(hcl_altera_vvp_tpg* driver,
                                                uint8_t* out_fps);

hcl_status hcl_altera_vvp_tpg_commit(hcl_altera_vvp_tpg* driver);

#ifdef __cplusplus
}
#endif
