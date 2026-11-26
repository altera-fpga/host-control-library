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

#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"
#include "hcl/drivers/driveonchip_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_driveonchip hcl_driveonchip;

hcl_driveonchip* hcl_driveonchip_create(hcl_register_interface* reg, double scale_factor);

void hcl_driveonchip_destroy(hcl_driveonchip* driver);

hcl_status hcl_driveonchip_init(const hcl_driveonchip* driver);

hcl_status hcl_driveonchip_set_speed(const hcl_driveonchip* driver, int32_t value);
hcl_status hcl_driveonchip_set_and_get_speed(const hcl_driveonchip* driver,
                                             int32_t value,
                                             int32_t* out_value);
hcl_status hcl_driveonchip_get_speed(const hcl_driveonchip* driver, int32_t* out_value);
hcl_status hcl_driveonchip_get_speed_setpoint(const hcl_driveonchip* driver, int32_t* out_value);

hcl_status hcl_driveonchip_set_position(const hcl_driveonchip* driver, int32_t value);
hcl_status hcl_driveonchip_set_and_get_position(const hcl_driveonchip* driver,
                                                int32_t value,
                                                double* out_value);
hcl_status hcl_driveonchip_get_position(const hcl_driveonchip* driver, double* out_value);
hcl_status hcl_driveonchip_get_position_setpoint(const hcl_driveonchip* driver, int32_t* out_value);

hcl_status hcl_driveonchip_set_position_speed_limit(const hcl_driveonchip* driver, int32_t value);
hcl_status hcl_driveonchip_get_position_speed_limit(const hcl_driveonchip* driver, uint32_t* out_value);

hcl_status hcl_driveonchip_set_wave_demo_mode(const hcl_driveonchip* driver, uint32_t value);
hcl_status hcl_driveonchip_get_wave_demo_mode(const hcl_driveonchip* driver, uint32_t* out_value);

hcl_status hcl_driveonchip_set_wave_demo_waveform(const hcl_driveonchip* driver, uint32_t value);
hcl_status hcl_driveonchip_get_wave_demo_waveform(const hcl_driveonchip* driver, uint32_t* out_value);

hcl_status hcl_driveonchip_set_wave_demo_amplitude(const hcl_driveonchip* driver, int32_t value);
hcl_status hcl_driveonchip_get_wave_demo_amplitude(const hcl_driveonchip* driver, int32_t* out_value);

hcl_status hcl_driveonchip_set_wave_demo_period(const hcl_driveonchip* driver, uint32_t value);
hcl_status hcl_driveonchip_get_wave_demo_period(const hcl_driveonchip* driver, uint32_t* out_value);

hcl_status hcl_driveonchip_set_wave_demo_offset(const hcl_driveonchip* driver, int32_t value);
hcl_status hcl_driveonchip_get_wave_demo_offset(const hcl_driveonchip* driver, int32_t* out_value);

hcl_status hcl_driveonchip_wave_demo_reset(const hcl_driveonchip* driver);
hcl_status hcl_driveonchip_wave_demo_update(const hcl_driveonchip* driver);

hcl_status hcl_driveonchip_set_dump_mode(const hcl_driveonchip* driver, uint32_t value);
hcl_status hcl_driveonchip_get_dump_mode(const hcl_driveonchip* driver, uint32_t* out_value);

#ifdef __cplusplus
}
#endif