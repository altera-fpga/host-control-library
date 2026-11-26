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

#include "hcl/drivers/abr_bridge.hpp"

#include <utility>

#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

AbrBridge::AbrBridge(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_abr_bridge_create(reg_ ? reg_->c_handle() : nullptr, base_address)) {}

AbrBridge::~AbrBridge() {
    hcl_abr_bridge_destroy(driver_);
    driver_ = nullptr;
}

hcl::Result<AbrBridge::Configuration> AbrBridge::read_configuration() {
    AbrBridge::Configuration out{};
    return hcl::StatusHelper::to_result(hcl_abr_bridge_read_configuration(driver_, &out), out);
}

hcl::Status AbrBridge::write_control(const AbrBridge::Control& ctrl) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_write_control(driver_, &ctrl));
}

hcl::Result<AbrBridge::Control> AbrBridge::read_control() {
    AbrBridge::Control out{};
    return hcl::StatusHelper::to_result(hcl_abr_bridge_read_control(driver_, &out), out);
}

hcl::Result<std::uint32_t> AbrBridge::read_status() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_abr_bridge_read_status(driver_, &out), out);
}

hcl::Status AbrBridge::set_channel_enable(std::uint32_t channel) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_set_channel_enable(driver_, channel));
}

hcl::Result<AbrBridge::ChannelConfig> AbrBridge::read_channel_config(std::uint32_t channel) {
    AbrBridge::ChannelConfig out{};
    return hcl::StatusHelper::to_result(hcl_abr_bridge_read_channel_config(driver_, channel, &out), out);
}

hcl::Status AbrBridge::write_channel_config(std::uint32_t channel, const AbrBridge::ChannelConfig& cfg) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_write_channel_config(driver_, channel, &cfg));
}

hcl::Status AbrBridge::set_channel_num_lines(std::uint32_t channel, std::uint16_t num_lines) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_set_channel_num_lines(driver_, channel, num_lines));
}

hcl::Result<std::uint16_t> AbrBridge::get_channel_num_lines(std::uint32_t channel) {
    std::uint16_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_abr_bridge_get_channel_num_lines(driver_, channel, &out), out);
}

hcl::Status AbrBridge::set_channel_num_buffers(std::uint32_t channel, std::uint8_t num_buffers) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_set_channel_num_buffers(driver_, channel, num_buffers));
}

hcl::Result<std::uint8_t> AbrBridge::get_channel_num_buffers(std::uint32_t channel) {
    std::uint8_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_abr_bridge_get_channel_num_buffers(driver_, channel, &out), out);
}

hcl::Status AbrBridge::set_channel_load_buffer(std::uint32_t channel, std::uint8_t load_buffer) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_set_channel_load_buffer(driver_, channel, load_buffer));
}

hcl::Status AbrBridge::set_channel_roce_qp(std::uint32_t channel, std::uint8_t qp) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_set_channel_roce_qp(driver_, channel, qp));
}

hcl::Status AbrBridge::set_channel_roce_rkey(std::uint32_t channel, std::uint8_t buffer, std::uint32_t rkey) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_set_channel_roce_rkey(driver_, channel, buffer, rkey));
}

hcl::Status AbrBridge::set_channel_offset(std::uint32_t channel, std::uint32_t offset) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_set_channel_offset(driver_, channel, offset));
}

hcl::Status AbrBridge::set_channel_remote_address(std::uint32_t channel,
                                                  std::uint8_t buffer,
                                                  std::uint64_t remote_address) {
    return hcl::StatusHelper::to_status(
        hcl_abr_bridge_set_channel_remote_address(driver_, channel, buffer, remote_address));
}

hcl::Status AbrBridge::set_channel_metadata_address(std::uint32_t channel, std::uint64_t metadata_address) {
    return hcl::StatusHelper::to_status(
        hcl_abr_bridge_set_channel_metadata_address(driver_, channel, metadata_address));
}

hcl::Status AbrBridge::set_channel_metadata_key(std::uint32_t channel, std::uint32_t metadata_key) {
    return hcl::StatusHelper::to_status(
        hcl_abr_bridge_set_channel_metadata_key(driver_, channel, metadata_key));
}

hcl::Result<std::uint32_t> AbrBridge::read_measured_vid_clk_hz() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_abr_bridge_read_measured_vid_clk_hz(driver_, &out), out);
}

hcl::Result<std::uint32_t> AbrBridge::read_measured_eth_clk_hz() {
    std::uint32_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_abr_bridge_read_measured_eth_clk_hz(driver_, &out), out);
}

hcl::Status AbrBridge::enable_channel(std::uint32_t channel) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_enable_channel(driver_, channel));
}

hcl::Status AbrBridge::configure_channel(std::uint32_t channel, const AbrBridge::ChannelParams& params) {
    return hcl::StatusHelper::to_status(hcl_abr_bridge_configure_channel(driver_, channel, &params));
}

}  // namespace hcl::drivers
