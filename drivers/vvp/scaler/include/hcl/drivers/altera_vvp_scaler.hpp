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

#include <array>
#include <cstdint>
#include <memory>

#include "hcl/drivers/altera_vvp_core.hpp"
#include "hcl/drivers/altera_vvp_scaler.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraVvpScaler {
public:
    using Algorithm = hcl_altera_vvp_scaler_algorithm;

    AlteraVvpScaler(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address);
    ~AlteraVvpScaler();

    hcl::Status init();

    bool lite_mode() const;
    bool debug_enabled() const;
    std::uint8_t pixels_in_parallel() const;
    std::uint32_t max_input_width() const;
    std::uint32_t max_output_width() const;
    std::uint32_t img_info_width() const;
    hcl::Status set_img_info_width(std::uint32_t width);
    std::uint32_t img_info_height() const;
    hcl::Status set_img_info_height(std::uint32_t height);
    std::uint32_t img_info_interlace() const;
    hcl::Status set_img_info_interlace(std::uint32_t interlace);
    std::uint32_t img_info_colorspace() const;
    hcl::Status set_img_info_colorspace(std::uint32_t colorspace);
    std::uint32_t img_info_subsampling() const;
    hcl::Status set_img_info_subsampling(std::uint32_t subsampling);
    std::uint32_t img_info_cositing() const;
    hcl::Status set_img_info_cositing(std::uint32_t cositing);
    std::uint32_t img_info_field_count() const;
    Algorithm scaling_algorithm() const;
    bool coeffs_loading_enabled() const;
    bool coeffs_preinitialized() const;

    bool vertical_scaling_enabled() const;
    bool partial_vertical_scaling_enabled() const;
    std::uint8_t num_vertical_taps() const;
    std::uint16_t num_vertical_phases() const;
    std::uint8_t num_vertical_banks() const;
    bool vertical_coeffs_signed() const;
    std::uint8_t vertical_coeffs_int_bits() const;
    std::uint8_t vertical_coeffs_frac_bits() const;

    bool horizontal_scaling_enabled() const;
    bool partial_horizontal_scaling_enabled() const;
    std::uint8_t num_horizontal_taps() const;
    std::uint16_t num_horizontal_phases() const;
    std::uint8_t num_horizontal_banks() const;
    bool horizontal_coeffs_signed() const;
    std::uint8_t horizontal_coeffs_int_bits() const;
    std::uint8_t horizontal_coeffs_frac_bits() const;

    hcl::Result<bool> is_running();
    hcl::Result<bool> commit_is_pending();
    hcl::Result<std::uint32_t> get_status();

    hcl::Status set_h_bank(std::uint8_t bank);
    hcl::Status set_v_bank(std::uint8_t bank);
    hcl::Status set_output_width(std::uint32_t output_width);
    hcl::Status set_output_height(std::uint32_t output_height);
    hcl::Status set_total_input_width(std::uint32_t total_input_width);
    hcl::Status set_total_output_width(std::uint32_t total_output_width);
    hcl::Status set_total_input_height(std::uint32_t total_input_height);
    hcl::Status set_total_output_height(std::uint32_t total_output_height);
    hcl::Status set_h_pos_error(std::uint32_t h_pos_err);
    hcl::Status set_v_pos_error(std::uint32_t v_pos_err);
    hcl::Status set_h_extended_fill(bool extended_fill);
    hcl::Status set_v_extended_fill(bool extended_fill);
    hcl::Status set_h_phase_offset_error(std::uint32_t phase_offset, std::uint32_t phase_error);
    hcl::Status set_v_phase_offset_error(std::uint32_t phase_offset, std::uint32_t phase_error);

    hcl::Status set_split_h_scaling(
        std::uint32_t total_input_width,
        std::uint32_t total_output_width,
        std::uint32_t split_output_width,
        std::uint32_t split_output_left_offset,
        bool extended_fill,
        std::uint32_t* split_input_width,
        std::uint32_t* split_input_left_offset);

    hcl::Status set_split_v_scaling(
        std::uint32_t total_input_height,
        std::uint32_t total_output_height,
        std::uint32_t split_output_height,
        std::uint32_t split_output_top_offset,
        bool extended_fill,
        std::uint32_t* split_input_height,
        std::uint32_t* split_input_top_offset);

    hcl::Result<std::uint8_t> get_h_bank();
    hcl::Result<std::uint8_t> get_v_bank();
    hcl::Result<std::uint32_t> get_output_width();
    hcl::Result<std::uint32_t> get_output_height();
    hcl::Result<std::uint32_t> get_total_input_width();
    hcl::Result<std::uint32_t> get_total_output_width();
    hcl::Result<std::uint32_t> get_total_input_height();
    hcl::Result<std::uint32_t> get_total_output_height();
    hcl::Result<std::uint32_t> get_h_pos_error();
    hcl::Result<std::uint32_t> get_v_pos_error();
    hcl::Result<bool> get_h_extended_fill();
    hcl::Result<bool> get_v_extended_fill();

    struct PhaseOffsetError {
        std::uint32_t phase_offset;
        std::uint32_t phase_error;
    };

    hcl::Result<PhaseOffsetError> get_h_phase_offset_error();
    hcl::Result<PhaseOffsetError> get_v_phase_offset_error();

    hcl::Status set_quantized_coeffs(bool h_coeffs,
                                     const std::int32_t* quantized_coeffs,
                                     std::uint8_t num_coeffs,
                                     std::uint8_t bank,
                                     std::uint8_t phase);
    hcl::Status set_coeffs(bool h_coeffs,
                           const float* coeffs,
                           std::uint8_t num_coeffs,
                           std::uint8_t bank,
                           std::uint8_t phase);

    hcl::Status commit();

private:
    std::shared_ptr<hcl::RegisterInterface> reg_;
    hcl_altera_vvp_scaler* driver_;
};

}  // namespace hcl::drivers
