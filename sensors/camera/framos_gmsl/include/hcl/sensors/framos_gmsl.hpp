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

#include "hcl/interfaces/i2c.hpp"
#include "hcl/register_interface_common.hpp"
#include "hcl/sensors/framos_gmsl.h"

namespace hcl::sensors {

class FramosGmsl {
public:
    FramosGmsl(std::shared_ptr<hcl::interfaces::I2C> i2c,
               std::uint8_t serializer_address,
               std::uint8_t deserializer_address,
               std::uint8_t gpio_address,
               std::uint8_t sensor_address);
    ~FramosGmsl();

    hcl::Status init();
    hcl::Result<std::uint8_t> read_register(std::uint8_t device_address, std::uint16_t reg);
    hcl::Status write_register(std::uint8_t device_address, std::uint16_t reg, std::uint8_t data);
    hcl::Status write_gpio_register(std::uint8_t reg, std::uint8_t data);

private:
    std::shared_ptr<hcl::interfaces::I2C> i2c_;
    hcl_framos_gmsl* sensor_;
};

}  // namespace hcl::sensors
