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

#include "hcl/drivers/altera_avalon_pio.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraAvalonPIO {
public:
    AlteraAvalonPIO(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address);
    ~AlteraAvalonPIO();

    /* Returns the underlying C driver handle; lifetime is tied to this object. */
    [[nodiscard]] hcl_altera_avalon_pio* c_driver() const { return driver_; }

    hcl::Result<std::uint32_t> read_data();
    hcl::Status write_data(std::uint32_t value);

    hcl::Result<std::uint32_t> read_direction();
    hcl::Status write_direction(std::uint32_t value);

    hcl::Result<std::uint32_t> read_irq_mask();
    hcl::Status write_irq_mask(std::uint32_t value);

    hcl::Result<std::uint32_t> read_edge_cap();
    hcl::Status clear_edge_cap(std::uint32_t value);

    hcl::Status set_bits(std::uint32_t value);
    hcl::Status clear_bits(std::uint32_t value);

private:
    std::shared_ptr<hcl::RegisterInterface> reg_; /* keeps the C handle alive */
    hcl_altera_avalon_pio* driver_;
};

}  // namespace hcl::drivers
