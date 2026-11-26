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

#include "hcl/drivers/altera_avalon_pio.hpp"

#include <utility>

#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

AlteraAvalonPIO::AlteraAvalonPIO(std::shared_ptr<hcl::RegisterInterface> reg,
                 std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_avalon_pio_create(reg_ ? reg_->c_handle() : nullptr,
                       base_address)) {}

AlteraAvalonPIO::~AlteraAvalonPIO() {
    hcl_altera_avalon_pio_destroy(driver_);
    driver_ = nullptr;
}

hcl::Result<std::uint32_t> AlteraAvalonPIO::read_data() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(
    hcl_altera_avalon_pio_read_data(driver_, &value), value);
}

hcl::Status AlteraAvalonPIO::write_data(std::uint32_t value) {
    return hcl::StatusHelper::to_status(
    hcl_altera_avalon_pio_write_data(driver_, value));
}

hcl::Result<std::uint32_t> AlteraAvalonPIO::read_direction() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(
    hcl_altera_avalon_pio_read_direction(driver_, &value), value);
}

hcl::Status AlteraAvalonPIO::write_direction(std::uint32_t value) {
    return hcl::StatusHelper::to_status(
    hcl_altera_avalon_pio_write_direction(driver_, value));
}

hcl::Result<std::uint32_t> AlteraAvalonPIO::read_irq_mask() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(
    hcl_altera_avalon_pio_read_irq_mask(driver_, &value), value);
}

hcl::Status AlteraAvalonPIO::write_irq_mask(std::uint32_t value) {
    return hcl::StatusHelper::to_status(
    hcl_altera_avalon_pio_write_irq_mask(driver_, value));
}

hcl::Result<std::uint32_t> AlteraAvalonPIO::read_edge_cap() {
    std::uint32_t value = 0u;
    return hcl::StatusHelper::to_result(
    hcl_altera_avalon_pio_read_edge_cap(driver_, &value), value);
}

hcl::Status AlteraAvalonPIO::clear_edge_cap(std::uint32_t value) {
    return hcl::StatusHelper::to_status(
    hcl_altera_avalon_pio_clear_edge_cap(driver_, value));
}

hcl::Status AlteraAvalonPIO::set_bits(std::uint32_t value) {
    return hcl::StatusHelper::to_status(
    hcl_altera_avalon_pio_set_bits(driver_, value));
}

hcl::Status AlteraAvalonPIO::clear_bits(std::uint32_t value) {
    return hcl::StatusHelper::to_status(
    hcl_altera_avalon_pio_clear_bits(driver_, value));
}

}  // namespace hcl::drivers
