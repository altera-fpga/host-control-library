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

#include "hcl/drivers/altera_vvp_wbc_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_vvp_wbc hcl_altera_vvp_wbc;

typedef struct hcl_altera_vvp_wbc_cfa_settings {
    uint32_t cfa_00_color_scaler;
    uint32_t cfa_01_color_scaler;
    uint32_t cfa_10_color_scaler;
    uint32_t cfa_11_color_scaler;
} hcl_altera_vvp_wbc_cfa_settings;

hcl_altera_vvp_wbc* hcl_altera_vvp_wbc_create(hcl_register_interface* reg,
                                              uint32_t base_address);

void hcl_altera_vvp_wbc_destroy(hcl_altera_vvp_wbc* driver);

hcl_status hcl_altera_vvp_wbc_init(hcl_altera_vvp_wbc* driver);

bool hcl_altera_vvp_wbc_get_lite_mode(const hcl_altera_vvp_wbc* driver);
bool hcl_altera_vvp_wbc_get_debug_enabled(const hcl_altera_vvp_wbc* driver);
uint8_t hcl_altera_vvp_wbc_get_bits_per_sample_in(const hcl_altera_vvp_wbc* driver);
uint8_t hcl_altera_vvp_wbc_get_bits_per_sample_out(const hcl_altera_vvp_wbc* driver);
uint8_t hcl_altera_vvp_wbc_get_num_color_planes_in(const hcl_altera_vvp_wbc* driver);
uint8_t hcl_altera_vvp_wbc_get_num_color_planes_out(const hcl_altera_vvp_wbc* driver);
uint8_t hcl_altera_vvp_wbc_get_pixels_in_parallel(const hcl_altera_vvp_wbc* driver);
uint32_t hcl_altera_vvp_wbc_get_max_width(const hcl_altera_vvp_wbc* driver);
uint32_t hcl_altera_vvp_wbc_get_max_height(const hcl_altera_vvp_wbc* driver);
uint32_t hcl_altera_vvp_wbc_get_img_info_width(const hcl_altera_vvp_wbc* driver);
hcl_status hcl_altera_vvp_wbc_set_img_info_width(hcl_altera_vvp_wbc* driver, uint32_t width);
uint32_t hcl_altera_vvp_wbc_get_img_info_height(const hcl_altera_vvp_wbc* driver);
hcl_status hcl_altera_vvp_wbc_set_img_info_height(hcl_altera_vvp_wbc* driver, uint32_t height);

hcl_status hcl_altera_vvp_wbc_is_running(hcl_altera_vvp_wbc* driver, bool* out_running);
hcl_status hcl_altera_vvp_wbc_commit_is_pending(hcl_altera_vvp_wbc* driver, bool* out_pending);
hcl_status hcl_altera_vvp_wbc_get_status(hcl_altera_vvp_wbc* driver, uint32_t* out_status);
hcl_status hcl_altera_vvp_wbc_get_frame_stats(hcl_altera_vvp_wbc* driver, uint32_t* out_stats);
hcl_status hcl_altera_vvp_wbc_commit(hcl_altera_vvp_wbc* driver);

bool hcl_altera_vvp_wbc_get_bypass(const hcl_altera_vvp_wbc* driver);
hcl_status hcl_altera_vvp_wbc_set_bypass(hcl_altera_vvp_wbc* driver, bool bypass);

uint8_t hcl_altera_vvp_wbc_get_cfa_phase(const hcl_altera_vvp_wbc* driver);
hcl_status hcl_altera_vvp_wbc_set_cfa_phase(hcl_altera_vvp_wbc* driver, uint8_t cfa_phase);

hcl_status hcl_altera_vvp_wbc_get_cfa_00_color_scaler(hcl_altera_vvp_wbc* driver, uint32_t* out_scaler);
hcl_status hcl_altera_vvp_wbc_set_cfa_00_color_scaler(hcl_altera_vvp_wbc* driver, uint32_t scaler);

hcl_status hcl_altera_vvp_wbc_get_cfa_01_color_scaler(hcl_altera_vvp_wbc* driver, uint32_t* out_scaler);
hcl_status hcl_altera_vvp_wbc_set_cfa_01_color_scaler(hcl_altera_vvp_wbc* driver, uint32_t scaler);

hcl_status hcl_altera_vvp_wbc_get_cfa_10_color_scaler(hcl_altera_vvp_wbc* driver, uint32_t* out_scaler);
hcl_status hcl_altera_vvp_wbc_set_cfa_10_color_scaler(hcl_altera_vvp_wbc* driver, uint32_t scaler);

hcl_status hcl_altera_vvp_wbc_get_cfa_11_color_scaler(hcl_altera_vvp_wbc* driver, uint32_t* out_scaler);
hcl_status hcl_altera_vvp_wbc_set_cfa_11_color_scaler(hcl_altera_vvp_wbc* driver, uint32_t scaler);

hcl_status hcl_altera_vvp_wbc_set_cfa_settings(hcl_altera_vvp_wbc* driver,
                                               const hcl_altera_vvp_wbc_cfa_settings* settings);

#ifdef __cplusplus
}
#endif
