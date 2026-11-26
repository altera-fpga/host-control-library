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

#include <stdbool.h>
#include <stdint.h>

#include "hcl/drivers/altera_vvp_switch_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_vvp_switch hcl_altera_vvp_switch;

typedef enum hcl_altera_vvp_switch_input_config {
    HCL_VVP_SWITCH_INPUT_DISABLED = 0,
    HCL_VVP_SWITCH_INPUT_ENABLED = 1,
    HCL_VVP_SWITCH_INPUT_CONSUMED = 3
} hcl_altera_vvp_switch_input_config;

typedef enum hcl_altera_vvp_switch_intf_type {
    HCL_VVP_SWITCH_INTF_FULL = 0,
    HCL_VVP_SWITCH_INTF_LITE = 1,
    HCL_VVP_SWITCH_INTF_FR = 2
} hcl_altera_vvp_switch_intf_type;

typedef struct hcl_altera_vvp_switch_output_config {
    bool enabled;
    uint8_t input;
} hcl_altera_vvp_switch_output_config;

hcl_altera_vvp_switch* hcl_altera_vvp_switch_create(hcl_register_interface* reg,
                                                    uint32_t base_address);

void hcl_altera_vvp_switch_destroy(hcl_altera_vvp_switch* driver);

hcl_status hcl_altera_vvp_switch_init(hcl_altera_vvp_switch* driver);

hcl_altera_vvp_switch_intf_type hcl_altera_vvp_switch_get_intf_type(const hcl_altera_vvp_switch* driver);
bool hcl_altera_vvp_switch_get_debug_enabled(const hcl_altera_vvp_switch* driver);
bool hcl_altera_vvp_switch_has_uninterrupted_inputs(const hcl_altera_vvp_switch* driver);
bool hcl_altera_vvp_switch_has_auto_consume(const hcl_altera_vvp_switch* driver);
uint8_t hcl_altera_vvp_switch_get_num_inputs(const hcl_altera_vvp_switch* driver);
uint8_t hcl_altera_vvp_switch_get_num_outputs(const hcl_altera_vvp_switch* driver);
bool hcl_altera_vvp_switch_is_crash_switch(const hcl_altera_vvp_switch* driver);
bool hcl_altera_vvp_switch_uses_tready(const hcl_altera_vvp_switch* driver);

hcl_status hcl_altera_vvp_switch_is_running(hcl_altera_vvp_switch* driver, bool* out_running);
hcl_status hcl_altera_vvp_switch_commit_is_pending(hcl_altera_vvp_switch* driver, bool* out_pending);
hcl_status hcl_altera_vvp_switch_get_status(hcl_altera_vvp_switch* driver, uint32_t* out_status);

hcl_status hcl_altera_vvp_switch_set_input_config(hcl_altera_vvp_switch* driver,
                                                  uint8_t input,
                                                  hcl_altera_vvp_switch_input_config input_config);
hcl_status hcl_altera_vvp_switch_get_input_config(hcl_altera_vvp_switch* driver,
                                                  uint8_t input,
                                                  hcl_altera_vvp_switch_input_config* out_input_config);

hcl_status hcl_altera_vvp_switch_set_output_config(hcl_altera_vvp_switch* driver,
                                                   uint8_t output,
                                                   bool enabled,
                                                   uint8_t input);
hcl_status hcl_altera_vvp_switch_get_output_config(hcl_altera_vvp_switch* driver,
                                                   uint8_t output,
                                                   hcl_altera_vvp_switch_output_config* out_output_config);

hcl_status hcl_altera_vvp_switch_commit(hcl_altera_vvp_switch* driver);

#ifdef __cplusplus
}
#endif
