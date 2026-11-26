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

#include "hcl/drivers/altera_vvp_switch.hpp"

namespace hcl::drivers {

AlteraVvpSwitch::AlteraVvpSwitch(std::shared_ptr<hcl::RegisterInterface> reg,
                                 std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_switch_create(reg_ ? reg_->c_handle() : nullptr,
                                           base_address)) {}

AlteraVvpSwitch::~AlteraVvpSwitch() {
    hcl_altera_vvp_switch_destroy(driver_);
}

hcl::Status AlteraVvpSwitch::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_switch_init(driver_));
}

AlteraVvpSwitch::IntfType AlteraVvpSwitch::intf_type() const {
    return hcl_altera_vvp_switch_get_intf_type(driver_);
}

bool AlteraVvpSwitch::debug_enabled() const {
    return hcl_altera_vvp_switch_get_debug_enabled(driver_);
}

bool AlteraVvpSwitch::has_uninterrupted_inputs() const {
    return hcl_altera_vvp_switch_has_uninterrupted_inputs(driver_);
}

bool AlteraVvpSwitch::has_auto_consume() const {
    return hcl_altera_vvp_switch_has_auto_consume(driver_);
}

std::uint8_t AlteraVvpSwitch::num_inputs() const {
    return hcl_altera_vvp_switch_get_num_inputs(driver_);
}

std::uint8_t AlteraVvpSwitch::num_outputs() const {
    return hcl_altera_vvp_switch_get_num_outputs(driver_);
}

bool AlteraVvpSwitch::is_crash_switch() const {
    return hcl_altera_vvp_switch_is_crash_switch(driver_);
}

bool AlteraVvpSwitch::uses_tready() const {
    return hcl_altera_vvp_switch_uses_tready(driver_);
}

hcl::Result<bool> AlteraVvpSwitch::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_switch_is_running(driver_, &out), out);
}

hcl::Result<bool> AlteraVvpSwitch::commit_is_pending() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_switch_commit_is_pending(driver_, &out), out);
}

hcl::Result<std::uint32_t> AlteraVvpSwitch::get_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_switch_get_status(driver_, &out), out);
}

hcl::Status AlteraVvpSwitch::set_input_config(std::uint8_t input, AlteraVvpSwitch::InputConfig input_config) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_switch_set_input_config(driver_, input, input_config));
}

hcl::Result<AlteraVvpSwitch::InputConfig> AlteraVvpSwitch::get_input_config(std::uint8_t input) {
    AlteraVvpSwitch::InputConfig out = HCL_VVP_SWITCH_INPUT_DISABLED;
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_switch_get_input_config(driver_, input, &out), out);
}

hcl::Status AlteraVvpSwitch::set_output_config(std::uint8_t output, bool enabled, std::uint8_t input) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_switch_set_output_config(driver_, output, enabled, input));
}

hcl::Result<AlteraVvpSwitch::OutputConfig> AlteraVvpSwitch::get_output_config(std::uint8_t output) {
    AlteraVvpSwitch::OutputConfig out{false, 0u};
    return hcl::StatusHelper::to_result(
        hcl_altera_vvp_switch_get_output_config(driver_, output, &out), out);
}

hcl::Status AlteraVvpSwitch::commit() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_switch_commit(driver_));
}

}  // namespace hcl::drivers
