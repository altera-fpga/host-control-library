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

#include <stddef.h>
#include <stdint.h>

#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"
#include "hcl/drivers/altera_roce_regs.h"
#include "hcl/drivers/altera_roce_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_roce hcl_altera_roce;

hcl_altera_roce* hcl_altera_roce_create(hcl_register_interface* reg,
                                        uint32_t base_address);

void hcl_altera_roce_destroy(hcl_altera_roce* driver);

hcl_status hcl_altera_roce_init_global(hcl_altera_roce* driver);

hcl_status hcl_altera_roce_configure_qp_uc(hcl_altera_roce* driver,
                                           const hcl_roce_uc_qp_params* params);

hcl_status hcl_altera_roce_configure_src_hdr(hcl_altera_roce* driver,
                                             uint32_t entry,
                                             const hcl_roce_src_hdr* hdr);

hcl_status hcl_altera_roce_configure_dst_hdr(hcl_altera_roce* driver,
                                             uint32_t entry,
                                             const hcl_roce_dst_hdr* hdr);

hcl_status hcl_altera_roce_init_qp_uc(hcl_altera_roce* driver,
                                      const hcl_roce_uc_qp_params* params);

#ifdef __cplusplus
}
#endif
