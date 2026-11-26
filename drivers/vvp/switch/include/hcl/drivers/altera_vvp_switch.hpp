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

#include "hcl/drivers/altera_vvp_core.hpp"
#include "hcl/drivers/altera_vvp_switch.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraVvpSwitch {
public:
    using InputConfig = hcl_altera_vvp_switch_input_config;
    using IntfType = hcl_altera_vvp_switch_intf_type;
    using OutputConfig = hcl_altera_vvp_switch_output_config;

    AlteraVvpSwitch(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address);
    ~AlteraVvpSwitch();

    hcl::Status init();

    IntfType intf_type() const;
    bool debug_enabled() const;
    bool has_uninterrupted_inputs() const;
    bool has_auto_consume() const;
    std::uint8_t num_inputs() const;
    std::uint8_t num_outputs() const;
    bool is_crash_switch() const;
    bool uses_tready() const;

    hcl::Result<bool> is_running();
    hcl::Result<bool> commit_is_pending();
    hcl::Result<std::uint32_t> get_status();

    hcl::Status set_input_config(std::uint8_t input, InputConfig input_config);
    hcl::Result<InputConfig> get_input_config(std::uint8_t input);

    hcl::Status set_output_config(std::uint8_t output, bool enabled, std::uint8_t input);
    hcl::Result<OutputConfig> get_output_config(std::uint8_t output);

    hcl::Status commit();

private:
    std::shared_ptr<hcl::RegisterInterface> reg_;
    hcl_altera_vvp_switch* driver_;
};

}  // namespace hcl::drivers
