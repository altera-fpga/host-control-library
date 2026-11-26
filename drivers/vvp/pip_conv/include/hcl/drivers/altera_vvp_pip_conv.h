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

#include "hcl/drivers/altera_vvp_pip_conv_regs.h"
#include "hcl/register_interface.h"
#include "hcl/register_interface_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hcl_altera_vvp_pip_conv hcl_altera_vvp_pip_conv;

hcl_altera_vvp_pip_conv* hcl_altera_vvp_pip_conv_create(hcl_register_interface* reg,
                                                        uint32_t base_address);

void hcl_altera_vvp_pip_conv_destroy(hcl_altera_vvp_pip_conv* driver);

hcl_status hcl_altera_vvp_pip_conv_init(hcl_altera_vvp_pip_conv* driver);

bool hcl_altera_vvp_pip_conv_get_lite_mode(const hcl_altera_vvp_pip_conv* driver);
bool hcl_altera_vvp_pip_conv_get_debug_enabled(const hcl_altera_vvp_pip_conv* driver);
uint32_t hcl_altera_vvp_pip_conv_get_img_info_width(const hcl_altera_vvp_pip_conv* driver);
hcl_status hcl_altera_vvp_pip_conv_set_img_info_width(hcl_altera_vvp_pip_conv* driver, uint32_t width);
uint32_t hcl_altera_vvp_pip_conv_get_img_info_height(const hcl_altera_vvp_pip_conv* driver);
hcl_status hcl_altera_vvp_pip_conv_set_img_info_height(hcl_altera_vvp_pip_conv* driver, uint32_t height);
uint32_t hcl_altera_vvp_pip_conv_get_img_info_interlace(const hcl_altera_vvp_pip_conv* driver);
hcl_status hcl_altera_vvp_pip_conv_set_img_info_interlace(hcl_altera_vvp_pip_conv* driver, uint32_t interlace);
uint32_t hcl_altera_vvp_pip_conv_get_img_info_colorspace(const hcl_altera_vvp_pip_conv* driver);
hcl_status hcl_altera_vvp_pip_conv_set_img_info_colorspace(hcl_altera_vvp_pip_conv* driver, uint32_t colorspace);
uint32_t hcl_altera_vvp_pip_conv_get_img_info_subsampling(const hcl_altera_vvp_pip_conv* driver);
hcl_status hcl_altera_vvp_pip_conv_set_img_info_subsampling(hcl_altera_vvp_pip_conv* driver, uint32_t subsampling);
uint32_t hcl_altera_vvp_pip_conv_get_img_info_cositing(const hcl_altera_vvp_pip_conv* driver);
hcl_status hcl_altera_vvp_pip_conv_set_img_info_cositing(hcl_altera_vvp_pip_conv* driver, uint32_t cositing);
uint32_t hcl_altera_vvp_pip_conv_get_img_info_field_count(const hcl_altera_vvp_pip_conv* driver);

hcl_status hcl_altera_vvp_pip_conv_is_running(hcl_altera_vvp_pip_conv* driver,
                                              bool* out_running);
hcl_status hcl_altera_vvp_pip_conv_get_status(hcl_altera_vvp_pip_conv* driver,
                                              uint8_t* out_status);

#ifdef __cplusplus
}
#endif
