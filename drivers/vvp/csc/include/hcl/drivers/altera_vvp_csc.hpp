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

#include "hcl/drivers/altera_vvp_csc.h"
#include "hcl/drivers/altera_vvp_core.hpp"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraVvpCsc {
public:
    using Coefficients = hcl_altera_vvp_csc_coefficients;
    using QuantizedCoefficients = hcl_altera_vvp_csc_quantized_coefficients;
    using ColorSpace = hcl_altera_vvp_csc_color_space;
    using Rounding = hcl_altera_vvp_csc_rounding;

    AlteraVvpCsc(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address);
    ~AlteraVvpCsc();

    hcl::Status init();

    bool lite_mode() const;
    bool debug_enabled() const;
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
    std::uint8_t bits_per_sample_in() const;
    std::uint8_t bits_per_sample_out() const;
    std::uint8_t coeffs_frac_bits() const;
    bool coeffs_signed() const;
    std::uint8_t coeffs_int_bits() const;
    bool summands_signed() const;
    std::uint8_t summands_int_bits() const;
    std::int8_t binary_point_right_move() const;
    Rounding rounding_method() const;

    hcl::Result<bool> is_running();
    hcl::Result<bool> commit_is_pending();
    hcl::Result<std::uint32_t> get_status();

    hcl::Status set_quantized_coeff_data(const QuantizedCoefficients& quantized_coeffs);
    hcl::Result<QuantizedCoefficients> get_quantized_coeff_data();

    hcl::Status set_coeff_data(const Coefficients& coeffs, std::int32_t summand_rescale);
    hcl::Result<Coefficients> get_coeff_data();

    hcl::Status set_output_color_space(ColorSpace output_color_space);
    hcl::Result<ColorSpace> get_output_color_space();

    hcl::Status commit();

private:
    std::shared_ptr<hcl::RegisterInterface> reg_;
    hcl_altera_vvp_csc* driver_;
};

}  // namespace hcl::drivers
