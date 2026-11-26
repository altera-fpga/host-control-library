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

/* Register byte offsets. */
#define HCL_PIO_REG_DATA        0x00u
#define HCL_PIO_REG_DIRECTION   0x04u
#define HCL_PIO_REG_IRQ_MASK    0x08u
#define HCL_PIO_REG_EDGE_CAP    0x0Cu
#define HCL_PIO_REG_SET_BITS    0x10u
#define HCL_PIO_REG_CLEAR_BITS  0x14u

/* Direction values for bi-directional PIOs. */
#define HCL_PIO_DIRECTION_INPUT   0u
#define HCL_PIO_DIRECTION_OUTPUT  1u
