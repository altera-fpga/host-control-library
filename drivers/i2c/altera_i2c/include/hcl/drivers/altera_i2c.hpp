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

#include <chrono>
#include <cstdint>
#include <memory>
#include <span>

#include "hcl/drivers/altera_i2c.h"
#include "hcl/interfaces/i2c.hpp"
#include "hcl/drivers/altera_i2c_regs.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraI2C : public hcl::interfaces::I2C {
public:
    AlteraI2C(std::shared_ptr<hcl::RegisterInterface> reg,
              std::uint32_t base_address,
              std::uint32_t sys_clk,
              std::chrono::milliseconds transaction_timeout = std::chrono::milliseconds{1000});
    ~AlteraI2C() override;

    /* Returns the generic C driver handle; lifetime is tied to this object. */
    [[nodiscard]] hcl_i2c* c_driver() const override { return driver_; }

    hcl::Status init() override;
    hcl::Status enable();
    hcl::Status disable();

    hcl::Status write(std::uint8_t address, std::uint8_t reg, std::uint8_t data) override;
    hcl::Result<std::uint8_t> read(std::uint8_t address, std::uint8_t reg) override;
    hcl::Status write_extended(std::uint8_t address, std::uint16_t reg, std::uint8_t data) override;
    hcl::Result<std::uint8_t> read_extended(std::uint8_t address, std::uint16_t reg) override;

    hcl::Status wait_busy();

private:
    std::shared_ptr<hcl::RegisterInterface> reg_; /* keeps the C handle alive */
    hcl_i2c* driver_;
};

}  // namespace hcl::drivers
