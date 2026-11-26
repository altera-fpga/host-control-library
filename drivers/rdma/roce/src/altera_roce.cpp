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

#include "hcl/drivers/altera_roce.hpp"

#include <utility>

#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

AlteraRoCE::AlteraRoCE(std::shared_ptr<hcl::RegisterInterface> reg,
                       std::uint32_t base_address)
    : reg_(std::move(reg)),
      driver_(hcl_altera_roce_create(
          reg_ ? reg_->c_handle() : nullptr,
          base_address)) {}

AlteraRoCE::~AlteraRoCE() {
    hcl_altera_roce_destroy(driver_);
    driver_ = nullptr;
}

hcl::Status AlteraRoCE::init_global() {
    return hcl::StatusHelper::to_status(hcl_altera_roce_init_global(driver_));
}

hcl::Status AlteraRoCE::configure_qp_uc(const UcQpParams& params) {
    return hcl::StatusHelper::to_status(
        hcl_altera_roce_configure_qp_uc(driver_, &params));
}

hcl::Status AlteraRoCE::configure_src_hdr(std::uint32_t entry, const SrcHdr& hdr) {
    return hcl::StatusHelper::to_status(
        hcl_altera_roce_configure_src_hdr(driver_, entry, &hdr));
}

hcl::Status AlteraRoCE::configure_dst_hdr(std::uint32_t entry, const DstHdr& hdr) {
    return hcl::StatusHelper::to_status(
        hcl_altera_roce_configure_dst_hdr(driver_, entry, &hdr));
}

hcl::Status AlteraRoCE::init_qp_uc(const UcQpParams& params) {
    return hcl::StatusHelper::to_status(
    hcl_altera_roce_init_qp_uc(driver_, &params));
}

}  // namespace hcl::drivers
