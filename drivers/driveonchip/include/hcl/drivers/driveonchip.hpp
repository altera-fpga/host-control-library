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

#include "hcl/drivers/driveonchip.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class DriveOnChip final {
public:
    explicit DriveOnChip(std::shared_ptr<hcl::RegisterInterface> reg,
                         double scale_factor = 182.0);
    ~DriveOnChip();

    DriveOnChip(const DriveOnChip&) = delete;
    DriveOnChip& operator=(const DriveOnChip&) = delete;

    hcl::Status initialize();

    hcl::Status set_speed(std::int32_t value);
    hcl::Result<std::int32_t> set_and_get_speed(std::int32_t value);
    hcl::Result<std::int32_t> get_speed();
    hcl::Result<std::int32_t> get_speed_setpoint();

    hcl::Status set_position(std::int32_t value);
    hcl::Result<double> set_and_get_position(std::int32_t value);
    hcl::Result<double> get_position();
    hcl::Result<std::int32_t> get_position_setpoint();

    hcl::Status set_position_speed_limit(std::int32_t value);
    hcl::Result<std::uint32_t> get_position_speed_limit();

    hcl::Status set_wave_demo_mode(std::uint32_t value);
    hcl::Result<std::uint32_t> get_wave_demo_mode();

    hcl::Status set_wave_demo_waveform(std::uint32_t value);
    hcl::Result<std::uint32_t> get_wave_demo_waveform();

    hcl::Status set_wave_demo_amplitude(std::int32_t value);
    hcl::Result<std::int32_t> get_wave_demo_amplitude();

    hcl::Status set_wave_demo_period(std::uint32_t value);
    hcl::Result<std::uint32_t> get_wave_demo_period();

    hcl::Status set_wave_demo_offset(std::int32_t value);
    hcl::Result<std::int32_t> get_wave_demo_offset();

    hcl::Status wave_demo_reset();
    hcl::Status wave_demo_update();

    hcl::Status set_dump_mode(std::uint32_t value);
    hcl::Result<std::uint32_t> get_dump_mode();

private:
    std::shared_ptr<hcl::RegisterInterface> reg_;
    hcl_driveonchip* driver_{nullptr};
    double scale_factor_{182.0};
};

}  // namespace hcl::drivers::driveonchip