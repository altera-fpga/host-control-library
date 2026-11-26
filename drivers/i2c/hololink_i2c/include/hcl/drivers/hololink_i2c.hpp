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

#if __has_include(<hololink/core/hololink.hpp>)
#include <hololink/core/hololink.hpp>
#endif

#include "hcl/interfaces/i2c.hpp"

#if __has_include(<hololink/core/hololink.hpp>)
namespace hcl::drivers {

class HololinkI2C final : public hcl::interfaces::I2C {
public:
    explicit HololinkI2C(std::shared_ptr<hololink::Hololink::I2c> i2c);
    ~HololinkI2C() override;

    [[nodiscard]] hcl_i2c* c_driver() const override { return driver_; }

    hcl::Status init() override;
    hcl::Status write(std::uint8_t address, std::uint8_t reg, std::uint8_t data) override;
    hcl::Result<std::uint8_t> read(std::uint8_t address, std::uint8_t reg) override;
    hcl::Status write_extended(std::uint8_t address, std::uint16_t reg, std::uint8_t data) override;
    hcl::Result<std::uint8_t> read_extended(std::uint8_t address, std::uint16_t reg) override;

private:
    std::shared_ptr<hololink::Hololink::I2c> i2c_;
    hcl_i2c* driver_{nullptr};
    bool started_{false};
};

}  // namespace hcl::drivers
#endif
