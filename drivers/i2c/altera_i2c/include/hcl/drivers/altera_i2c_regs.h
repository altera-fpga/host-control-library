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
#define HCL_I2C_REG_TFR_CMD           0x00u
#define HCL_I2C_REG_RX_DATA           0x04u
#define HCL_I2C_REG_CTRL              0x08u
#define HCL_I2C_REG_ISER              0x0Cu
#define HCL_I2C_REG_ISR               0x10u
#define HCL_I2C_REG_STATUS            0x14u
#define HCL_I2C_REG_TFR_CMD_FIFO_LVL  0x18u
#define HCL_I2C_REG_RX_DATA_FIFO_LVL  0x1Cu
#define HCL_I2C_REG_SCL_LOW           0x20u
#define HCL_I2C_REG_SCL_HIGH          0x24u
#define HCL_I2C_REG_SDA_HOLD          0x28u

/* Transfer command flags written into TFR_CMD. */
#define HCL_I2C_WRITE_FLAG 0x000u
#define HCL_I2C_READ_FLAG  0x001u
#define HCL_I2C_START_FLAG 0x200u
#define HCL_I2C_STOP_FLAG  0x100u

/* ISR / ISER masks. */
#define HCL_I2C_ISR_ABN_DET_MASK   0x0Cu
#define HCL_I2C_ISER_BURST_ENABLE  0x0Du
