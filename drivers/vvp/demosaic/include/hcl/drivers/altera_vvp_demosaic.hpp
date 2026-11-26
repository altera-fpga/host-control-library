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
#include "hcl/drivers/altera_vvp_demosaic.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraVvpDemosaic {
public:
    AlteraVvpDemosaic(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address);
    ~AlteraVvpDemosaic();

    hcl::Status init();
    hcl::Result<AlteraVvp::Id> id() const;
    hcl::Result<AlteraVvp::Version> version() const;

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
    hcl::Result<std::uint32_t> get_status();
    hcl::Result<std::uint32_t> get_frame_stats();

    bool bypass() const;
    hcl::Status set_bypass(bool bypass);

    std::uint8_t cfa_phase() const;
    hcl::Status set_cfa_phase(std::uint8_t cfa_phase);

private:
    std::shared_ptr<hcl::RegisterInterface> reg_;
    hcl_altera_vvp_demosaic* driver_;
};

}  // namespace hcl::drivers
