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

#include "hcl/drivers/altera_vvp_core.h"
#include "hcl/drivers/altera_vvp_demosaic_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_vvp_demosaic hcl_altera_vvp_demosaic;

hcl_altera_vvp_demosaic* hcl_altera_vvp_demosaic_create(hcl_register_interface* reg,
                                                        uint32_t base_address);

void hcl_altera_vvp_demosaic_destroy(hcl_altera_vvp_demosaic* driver);

hcl_status hcl_altera_vvp_demosaic_init(hcl_altera_vvp_demosaic* driver);
hcl_status hcl_altera_vvp_demosaic_get_id(const hcl_altera_vvp_demosaic* driver,
                                          hcl_altera_vvp_id* out_id);
hcl_status hcl_altera_vvp_demosaic_get_version(const hcl_altera_vvp_demosaic* driver,
                                               hcl_altera_vvp_version* out_version);

bool hcl_altera_vvp_demosaic_get_lite_mode(const hcl_altera_vvp_demosaic* driver);
bool hcl_altera_vvp_demosaic_get_debug_enabled(const hcl_altera_vvp_demosaic* driver);
uint8_t hcl_altera_vvp_demosaic_get_bits_per_sample_in(const hcl_altera_vvp_demosaic* driver);
uint8_t hcl_altera_vvp_demosaic_get_bits_per_sample_out(const hcl_altera_vvp_demosaic* driver);
uint8_t hcl_altera_vvp_demosaic_get_num_color_planes_in(const hcl_altera_vvp_demosaic* driver);
uint8_t hcl_altera_vvp_demosaic_get_num_color_planes_out(const hcl_altera_vvp_demosaic* driver);
uint8_t hcl_altera_vvp_demosaic_get_pixels_in_parallel(const hcl_altera_vvp_demosaic* driver);
uint32_t hcl_altera_vvp_demosaic_get_max_width(const hcl_altera_vvp_demosaic* driver);
uint32_t hcl_altera_vvp_demosaic_get_max_height(const hcl_altera_vvp_demosaic* driver);
uint32_t hcl_altera_vvp_demosaic_get_img_info_width(const hcl_altera_vvp_demosaic* driver);
hcl_status hcl_altera_vvp_demosaic_set_img_info_width(hcl_altera_vvp_demosaic* driver, uint32_t width);
uint32_t hcl_altera_vvp_demosaic_get_img_info_height(const hcl_altera_vvp_demosaic* driver);
hcl_status hcl_altera_vvp_demosaic_set_img_info_height(hcl_altera_vvp_demosaic* driver, uint32_t height);

hcl_status hcl_altera_vvp_demosaic_is_running(hcl_altera_vvp_demosaic* driver, bool* out_running);
hcl_status hcl_altera_vvp_demosaic_get_status(hcl_altera_vvp_demosaic* driver, uint32_t* out_status);
hcl_status hcl_altera_vvp_demosaic_get_frame_stats(hcl_altera_vvp_demosaic* driver, uint32_t* out_stats);

bool hcl_altera_vvp_demosaic_get_bypass(const hcl_altera_vvp_demosaic* driver);
hcl_status hcl_altera_vvp_demosaic_set_bypass(hcl_altera_vvp_demosaic* driver, bool bypass);

uint8_t hcl_altera_vvp_demosaic_get_cfa_phase(const hcl_altera_vvp_demosaic* driver);
hcl_status hcl_altera_vvp_demosaic_set_cfa_phase(hcl_altera_vvp_demosaic* driver, uint8_t cfa_phase);

#ifdef __cplusplus
}
#endif
