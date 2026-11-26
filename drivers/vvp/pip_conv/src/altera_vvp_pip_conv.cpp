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

#include "hcl/drivers/altera_vvp_pip_conv.hpp"

namespace hcl::drivers {

AlteraVvpPipConv::AlteraVvpPipConv(std::shared_ptr<hcl::RegisterInterface> reg,
                                   std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_vvp_pip_conv_create(reg_ ? reg_->c_handle() : nullptr,
                                             base_address)) {}

AlteraVvpPipConv::~AlteraVvpPipConv() {
    hcl_altera_vvp_pip_conv_destroy(driver_);
}

hcl::Status AlteraVvpPipConv::init() {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_pip_conv_init(driver_));
}

bool AlteraVvpPipConv::lite_mode() const {
    return hcl_altera_vvp_pip_conv_get_lite_mode(driver_);
}

bool AlteraVvpPipConv::debug_enabled() const {
    return hcl_altera_vvp_pip_conv_get_debug_enabled(driver_);
}

std::uint32_t AlteraVvpPipConv::img_info_width() const {
    return hcl_altera_vvp_pip_conv_get_img_info_width(driver_);
}

hcl::Status AlteraVvpPipConv::set_img_info_width(std::uint32_t width) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_pip_conv_set_img_info_width(driver_, width));
}

std::uint32_t AlteraVvpPipConv::img_info_height() const {
    return hcl_altera_vvp_pip_conv_get_img_info_height(driver_);
}

hcl::Status AlteraVvpPipConv::set_img_info_height(std::uint32_t height) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_pip_conv_set_img_info_height(driver_, height));
}

std::uint32_t AlteraVvpPipConv::img_info_interlace() const {
    return hcl_altera_vvp_pip_conv_get_img_info_interlace(driver_);
}

hcl::Status AlteraVvpPipConv::set_img_info_interlace(std::uint32_t interlace) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_pip_conv_set_img_info_interlace(driver_, interlace));
}

std::uint32_t AlteraVvpPipConv::img_info_colorspace() const {
    return hcl_altera_vvp_pip_conv_get_img_info_colorspace(driver_);
}

hcl::Status AlteraVvpPipConv::set_img_info_colorspace(std::uint32_t colorspace) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_pip_conv_set_img_info_colorspace(driver_, colorspace));
}

std::uint32_t AlteraVvpPipConv::img_info_subsampling() const {
    return hcl_altera_vvp_pip_conv_get_img_info_subsampling(driver_);
}

hcl::Status AlteraVvpPipConv::set_img_info_subsampling(std::uint32_t subsampling) {
    return hcl::StatusHelper::to_status(
        hcl_altera_vvp_pip_conv_set_img_info_subsampling(driver_, subsampling));
}

std::uint32_t AlteraVvpPipConv::img_info_cositing() const {
    return hcl_altera_vvp_pip_conv_get_img_info_cositing(driver_);
}

hcl::Status AlteraVvpPipConv::set_img_info_cositing(std::uint32_t cositing) {
    return hcl::StatusHelper::to_status(hcl_altera_vvp_pip_conv_set_img_info_cositing(driver_, cositing));
}

std::uint32_t AlteraVvpPipConv::img_info_field_count() const {
    return hcl_altera_vvp_pip_conv_get_img_info_field_count(driver_);
}

hcl::Result<bool> AlteraVvpPipConv::is_running() {
    bool out = false;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_pip_conv_is_running(driver_, &out), out);
}

hcl::Result<std::uint8_t> AlteraVvpPipConv::get_status() {
    std::uint8_t out = 0u;
    return hcl::StatusHelper::to_result(hcl_altera_vvp_pip_conv_get_status(driver_, &out), out);
}

}  // namespace hcl::drivers
