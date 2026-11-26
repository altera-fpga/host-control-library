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

#include "hcl/drivers/altera_i2c.hpp"

#include <utility>

#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

AlteraI2C::AlteraI2C(std::shared_ptr<hcl::RegisterInterface> reg,
                     std::uint32_t base_address,
                     std::uint32_t sys_clk,
                     std::chrono::milliseconds transaction_timeout)
    : reg_(std::move(reg)),
      driver_(hcl_altera_i2c_create(
          reg_ ? reg_->c_handle() : nullptr,
          base_address,
          sys_clk,
          static_cast<std::uint32_t>(transaction_timeout.count()))) {}

AlteraI2C::~AlteraI2C() {
    hcl_altera_i2c_destroy(driver_);
    driver_ = nullptr;
}

hcl::Status AlteraI2C::init() {
    return hcl::StatusHelper::to_status(hcl_i2c_init(driver_));
}

hcl::Status AlteraI2C::enable() {
    return hcl::StatusHelper::to_status(
        hcl_altera_i2c_enable(static_cast<hcl_altera_i2c*>(driver_->context)));
}

hcl::Status AlteraI2C::disable() {
    return hcl::StatusHelper::to_status(
        hcl_altera_i2c_disable(static_cast<hcl_altera_i2c*>(driver_->context)));
}

hcl::Status AlteraI2C::wait_busy() {
    return hcl::StatusHelper::to_status(
        hcl_altera_i2c_wait_busy(static_cast<hcl_altera_i2c*>(driver_->context)));
}

hcl::Status AlteraI2C::write(std::uint8_t address, std::uint8_t reg, std::uint8_t data) {
    return hcl::StatusHelper::to_status(
        hcl_i2c_write(driver_, address, reg, data));
}

hcl::Result<std::uint8_t> AlteraI2C::read(std::uint8_t address, std::uint8_t reg) {
    std::uint8_t value = 0u;
    return hcl::StatusHelper::to_result(
        hcl_i2c_read(driver_, address, reg, &value), value);
}

hcl::Status AlteraI2C::write_extended(std::uint8_t address,
                                      std::uint16_t reg,
                                      std::uint8_t data) {
    return hcl::StatusHelper::to_status(
        hcl_i2c_write_extended(driver_, address, reg, data));
}

hcl::Result<std::uint8_t> AlteraI2C::read_extended(std::uint8_t address, std::uint16_t reg) {
    std::uint8_t value = 0u;
    return hcl::StatusHelper::to_result(
        hcl_i2c_read_extended(driver_, address, reg, &value), value);
}

}  // namespace hcl::drivers
