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

#include "hcl/sensors/framos_gmsl.hpp"

#include <utility>

#include "hcl/register_interface_common.hpp"

namespace hcl::sensors {

FramosGmsl::FramosGmsl(std::shared_ptr<hcl::interfaces::I2C> i2c,
                       std::uint8_t serializer_address,
                       std::uint8_t deserializer_address,
                       std::uint8_t gpio_address,
                       std::uint8_t sensor_address)
    : i2c_(std::move(i2c)),
      sensor_(hcl_framos_gmsl_create(i2c_ ? i2c_->c_driver() : nullptr,
                                     serializer_address,
                                     deserializer_address,
                                     gpio_address,
                                     sensor_address)) {}

FramosGmsl::~FramosGmsl() {
    hcl_framos_gmsl_destroy(sensor_);
    sensor_ = nullptr;
}

hcl::Status FramosGmsl::init() {
    return hcl::StatusHelper::to_status(hcl_framos_gmsl_init(sensor_));
}

hcl::Result<std::uint8_t> FramosGmsl::read_register(std::uint8_t device_address, std::uint16_t reg) {
    std::uint8_t value = 0u;
    return hcl::StatusHelper::to_result(hcl_framos_gmsl_read_register(sensor_, device_address, reg, &value),
                                        value);
}

hcl::Status FramosGmsl::write_register(std::uint8_t device_address,
                                       std::uint16_t reg,
                                       std::uint8_t data) {
    return hcl::StatusHelper::to_status(
        hcl_framos_gmsl_write_register(sensor_, device_address, reg, data));
}

hcl::Status FramosGmsl::write_gpio_register(std::uint8_t reg, std::uint8_t data) {
    return hcl::StatusHelper::to_status(
        hcl_framos_gmsl_write_gpio_register(sensor_, reg, data));
}

}  // namespace hcl::sensors
