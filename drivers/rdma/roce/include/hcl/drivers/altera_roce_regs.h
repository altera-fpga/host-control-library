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

/* Precomputed CSR addresses (space << 10 | offset). */
#define HCL_ROCE_STATUS_REG_ADDR           0x00Cu
#define HCL_ROCE_STATUS_INIT_DONE_BIT      (1u << 0)

#define HCL_ROCE_CONF0_REG_ADDR            0x400u
#define HCL_ROCE_TABLE_CTRL_REG_ADDR       0x470u
#define HCL_ROCE_TABLE_DATA_REG_BASE_ADDR  0x480u

#define HCL_ROCE_TABLE_CTRL_BUSY_BIT       (1u << 31)

#define HCL_ROCE_TABLE_QP_CONFIG  0u
#define HCL_ROCE_TABLE_SRC_HDR    1u
#define HCL_ROCE_TABLE_DST_HDR    2u

#define HCL_ROCE_QP_MODE_UC                4u
#define HCL_ROCE_DEFAULT_LOG2_PAYLOAD_MTU  10u

#define HCL_ROCE_QP_CONFIG_WORDS  6u
#define HCL_ROCE_SRC_HDR_WORDS    6u
#define HCL_ROCE_DST_HDR_WORDS    9u
