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

#include "hcl/interfaces/i2c.h"
#include "hcl/register_interface_common.h"
#include "hcl/sensors/imx678_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_imx678 hcl_imx678;

typedef struct hcl_imx678_mode hcl_imx678_mode;

hcl_imx678* hcl_imx678_create(hcl_i2c* i2c, uint8_t address);

void hcl_imx678_destroy(hcl_imx678* sensor);

hcl_status hcl_imx678_init(hcl_imx678* sensor,
                           const hcl_imx678_mode* mode);

hcl_status hcl_imx678_start(hcl_imx678* sensor);

hcl_status hcl_imx678_stop(hcl_imx678* sensor);

hcl_status hcl_imx678_set_gain(hcl_imx678* sensor,
                               uint32_t gain);

#ifdef __cplusplus
}
#endif
