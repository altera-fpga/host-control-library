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
#include "hcl/drivers/altera_vvp_wbc.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraVvpWbc {
public:
    AlteraVvpWbc(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address);
    ~AlteraVvpWbc();

    hcl::Status init();

    bool lite_mode() const;
    bool debug_enabled() const;
    std::uint8_t bits_per_sample_in() const;
    std::uint8_t bits_per_sample_out() const;
    std::uint8_t num_color_planes_in() const;
    std::uint8_t num_color_planes_out() const;
    std::uint8_t pixels_in_parallel() const;
    std::uint32_t max_width() const;
    std::uint32_t max_height() const;
    std::uint32_t img_info_width() const;
    hcl::Status set_img_info_width(std::uint32_t width);
    std::uint32_t img_info_height() const;
    hcl::Status set_img_info_height(std::uint32_t height);

    hcl::Result<bool> is_running();
    hcl::Result<bool> commit_is_pending();
    hcl::Result<std::uint32_t> get_status();
    hcl::Result<std::uint32_t> get_frame_stats();
    hcl::Status commit();

    bool bypass() const;
    hcl::Status set_bypass(bool bypass);

    std::uint8_t cfa_phase() const;
    hcl::Status set_cfa_phase(std::uint8_t cfa_phase);

    hcl::Result<std::uint32_t> get_cfa_00_color_scaler();
    hcl::Status set_cfa_00_color_scaler(std::uint32_t scaler);

    hcl::Result<std::uint32_t> get_cfa_01_color_scaler();
    hcl::Status set_cfa_01_color_scaler(std::uint32_t scaler);

    hcl::Result<std::uint32_t> get_cfa_10_color_scaler();
    hcl::Status set_cfa_10_color_scaler(std::uint32_t scaler);

    hcl::Result<std::uint32_t> get_cfa_11_color_scaler();
    hcl::Status set_cfa_11_color_scaler(std::uint32_t scaler);

    hcl::Status set_cfa_settings(const hcl_altera_vvp_wbc_cfa_settings& settings);

private:
    std::shared_ptr<hcl::RegisterInterface> reg_;
    hcl_altera_vvp_wbc* driver_;
};

}  // namespace hcl::drivers
