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

#include "hcl/drivers/abr_bridge.h"
#include "hcl/drivers/abr_bridge_regs.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AbrBridge {
public:
    using Configuration = hcl_abr_bridge_configuration;
    using Control       = hcl_abr_bridge_control;
    using ChannelConfig = hcl_abr_bridge_channel_config;
    using ChannelParams = hcl_abr_bridge_channel_params;

    AbrBridge(std::shared_ptr<hcl::RegisterInterface> reg, std::uint32_t base_address);
    ~AbrBridge();

    hcl::Result<Configuration>  read_configuration();
    hcl::Status                 write_control(const Control& ctrl);
    hcl::Result<Control>        read_control();
    hcl::Result<std::uint32_t>  read_status();
    hcl::Status                 set_channel_enable(std::uint32_t channel);

    hcl::Result<ChannelConfig>  read_channel_config(std::uint32_t channel);
    hcl::Status                 write_channel_config(std::uint32_t channel, const ChannelConfig& cfg);
    hcl::Status                 set_channel_num_lines(std::uint32_t channel, std::uint16_t num_lines);
    hcl::Result<std::uint16_t>  get_channel_num_lines(std::uint32_t channel);
    hcl::Status                 set_channel_num_buffers(std::uint32_t channel, std::uint8_t num_buffers);
    hcl::Result<std::uint8_t>   get_channel_num_buffers(std::uint32_t channel);
    hcl::Status                 set_channel_load_buffer(std::uint32_t channel, std::uint8_t load_buffer);
    hcl::Status                 set_channel_roce_qp(std::uint32_t channel, std::uint8_t qp);
    hcl::Status                 set_channel_roce_rkey(std::uint32_t channel, std::uint8_t buffer, std::uint32_t rkey);
    hcl::Status                 set_channel_offset(std::uint32_t channel, std::uint32_t offset);
    hcl::Status                 set_channel_remote_address(std::uint32_t channel, std::uint8_t buffer, std::uint64_t remote_address);
    hcl::Status                 set_channel_metadata_address(std::uint32_t channel, std::uint64_t metadata_address);
    hcl::Status                 set_channel_metadata_key(std::uint32_t channel, std::uint32_t metadata_key);

    hcl::Result<std::uint32_t>  read_measured_vid_clk_hz();
    hcl::Result<std::uint32_t>  read_measured_eth_clk_hz();

    hcl::Status enable_channel(std::uint32_t channel);
    hcl::Status configure_channel(std::uint32_t channel, const ChannelParams& params);

private:
    std::shared_ptr<hcl::RegisterInterface> reg_; /* keeps the C handle alive */
    hcl_abr_bridge* driver_;
};

}  // namespace hcl::drivers
