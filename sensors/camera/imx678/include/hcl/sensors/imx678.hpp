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
#include "hcl/sensors/imx678.h"
#include "hcl/sensors/imx678_modes.h"

namespace hcl::sensors {

class IMX678 {
public:
    IMX678(std::shared_ptr<hcl::interfaces::I2C> i2c, std::uint8_t address);
    ~IMX678();

    hcl::Status init(const hcl_imx678_mode& mode = IMX678_MODE_3840X2160_30FPS_12BIT);
    hcl::Status start();
    hcl::Status stop();
    hcl::Status set_gain(std::uint32_t gain);

private:
    std::shared_ptr<hcl::interfaces::I2C> i2c_;
    hcl_imx678* sensor_;
};

}  // namespace hcl::sensors
