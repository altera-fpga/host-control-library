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

#include "hcl/drivers/altera_vvp_core.h"
#include "hcl/register_interface_common.hpp"

namespace hcl::drivers {

class AlteraVvp {
public:
    using Id = hcl_altera_vvp_id;
    using Version = hcl_altera_vvp_version;

    AlteraVvp() = delete;

    static hcl::Status parse_id(std::uint32_t vid_pid, Id& out_id) {
        return hcl::StatusHelper::to_status(hcl_altera_vvp_core_parse_id(vid_pid, &out_id));
    }

    static hcl::Status parse_version(std::uint32_t version, Version& out_version) {
        return hcl::StatusHelper::to_status(hcl_altera_vvp_core_parse_version(version, &out_version));
    }
};

}  // namespace hcl::drivers
