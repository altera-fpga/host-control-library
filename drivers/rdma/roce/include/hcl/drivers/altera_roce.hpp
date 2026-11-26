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

#include "hcl/drivers/altera_roce.h"
#include "hcl/register_interface.hpp"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraRoCE {
public:
    using SrcHdr     = hcl_roce_src_hdr;
    using DstHdr     = hcl_roce_dst_hdr;
    using UcQpParams = hcl_roce_uc_qp_params;

    AlteraRoCE(std::shared_ptr<hcl::RegisterInterface> reg,
               std::uint32_t base_address);
    ~AlteraRoCE();

    hcl::Status init_global();
    hcl::Status configure_qp_uc(const UcQpParams& params);
    hcl::Status configure_src_hdr(std::uint32_t entry, const SrcHdr& hdr);
    hcl::Status configure_dst_hdr(std::uint32_t entry, const DstHdr& hdr);
    hcl::Status init_qp_uc(const UcQpParams& params);

private:
    std::shared_ptr<hcl::RegisterInterface> reg_; /* keeps the C handle alive */
    hcl_altera_roce* driver_;
};

}  // namespace hcl::drivers
