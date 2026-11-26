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

#include "hcl/sensors/imx678.hpp"

#include <utility>

#include "hcl/register_interface_common.hpp"

namespace hcl::sensors {

IMX678::IMX678(std::shared_ptr<hcl::interfaces::I2C> i2c, std::uint8_t address)
    : i2c_(std::move(i2c)),
      sensor_(hcl_imx678_create(i2c_ ? i2c_->c_driver() : nullptr, address)) {}

IMX678::~IMX678() {
    hcl_imx678_destroy(sensor_);
    sensor_ = nullptr;
}

hcl::Status IMX678::init(const hcl_imx678_mode& mode) {
    return hcl::StatusHelper::to_status(hcl_imx678_init(sensor_, &mode));
}

hcl::Status IMX678::start() {
    return hcl::StatusHelper::to_status(hcl_imx678_start(sensor_));
}

hcl::Status IMX678::stop() {
    return hcl::StatusHelper::to_status(hcl_imx678_stop(sensor_));
}

hcl::Status IMX678::set_gain(std::uint32_t gain) {
    return hcl::StatusHelper::to_status(hcl_imx678_set_gain(sensor_, gain));
}

}  // namespace hcl::sensors
