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

#include "hcl/drivers/driveonchip.hpp"

#include <utility>

namespace hcl::drivers {

DriveOnChip::DriveOnChip(std::shared_ptr<hcl::RegisterInterface> reg, double scale_factor)
    : reg_(std::move(reg)),
      driver_(hcl_driveonchip_create(reg_ ? reg_->c_handle() : nullptr, scale_factor)),
      scale_factor_(scale_factor) {}

DriveOnChip::~DriveOnChip() {
    hcl_driveonchip_destroy(driver_);
    driver_ = nullptr;
}

hcl::Status DriveOnChip::initialize() {
    return hcl::StatusHelper::to_status(hcl_driveonchip_init(driver_));
}

hcl::Status DriveOnChip::set_speed(std::int32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_speed(driver_, value));
}

hcl::Result<std::int32_t> DriveOnChip::set_and_get_speed(std::int32_t value) {
    std::int32_t result = 0;
    return hcl::StatusHelper::to_result(hcl_driveonchip_set_and_get_speed(driver_, value, &result), result);
}

hcl::Result<std::int32_t> DriveOnChip::get_speed() {
    std::int32_t value = 0;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_speed(driver_, &value), value);
}

hcl::Result<std::int32_t> DriveOnChip::get_speed_setpoint() {
    std::int32_t value = 0;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_speed_setpoint(driver_, &value), value);
}

hcl::Status DriveOnChip::set_position(std::int32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_position(driver_, value));
}

hcl::Result<double> DriveOnChip::set_and_get_position(std::int32_t value) {
    double result = 0.0;
    return hcl::StatusHelper::to_result(hcl_driveonchip_set_and_get_position(driver_, value, &result), result);
}

hcl::Result<double> DriveOnChip::get_position() {
    double value = 0.0;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_position(driver_, &value), value);
}

hcl::Result<std::int32_t> DriveOnChip::get_position_setpoint() {
    std::int32_t value = 0;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_position_setpoint(driver_, &value), value);
}

hcl::Status DriveOnChip::set_position_speed_limit(std::int32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_position_speed_limit(driver_, value));
}

hcl::Result<std::uint32_t> DriveOnChip::get_position_speed_limit() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_position_speed_limit(driver_, &value), value);
}

hcl::Status DriveOnChip::set_wave_demo_mode(std::uint32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_wave_demo_mode(driver_, value));
}

hcl::Result<std::uint32_t> DriveOnChip::get_wave_demo_mode() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_wave_demo_mode(driver_, &value), value);
}

hcl::Status DriveOnChip::set_wave_demo_waveform(std::uint32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_wave_demo_waveform(driver_, value));
}

hcl::Result<std::uint32_t> DriveOnChip::get_wave_demo_waveform() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_wave_demo_waveform(driver_, &value), value);
}

hcl::Status DriveOnChip::set_wave_demo_amplitude(std::int32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_wave_demo_amplitude(driver_, value));
}

hcl::Result<std::int32_t> DriveOnChip::get_wave_demo_amplitude() {
    std::int32_t value = 0;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_wave_demo_amplitude(driver_, &value), value);
}

hcl::Status DriveOnChip::set_wave_demo_period(std::uint32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_wave_demo_period(driver_, value));
}

hcl::Result<std::uint32_t> DriveOnChip::get_wave_demo_period() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_wave_demo_period(driver_, &value), value);
}

hcl::Status DriveOnChip::set_wave_demo_offset(std::int32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_wave_demo_offset(driver_, value));
}

hcl::Result<std::int32_t> DriveOnChip::get_wave_demo_offset() {
    std::int32_t value = 0;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_wave_demo_offset(driver_, &value), value);
}

hcl::Status DriveOnChip::wave_demo_reset() {
    return hcl::StatusHelper::to_status(hcl_driveonchip_wave_demo_reset(driver_));
}

hcl::Status DriveOnChip::wave_demo_update() {
    return hcl::StatusHelper::to_status(hcl_driveonchip_wave_demo_update(driver_));
}

hcl::Status DriveOnChip::set_dump_mode(std::uint32_t value) {
    return hcl::StatusHelper::to_status(hcl_driveonchip_set_dump_mode(driver_, value));
}

hcl::Result<std::uint32_t> DriveOnChip::get_dump_mode() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(hcl_driveonchip_get_dump_mode(driver_, &value), value);
}

}  // namespace hcl::drivers