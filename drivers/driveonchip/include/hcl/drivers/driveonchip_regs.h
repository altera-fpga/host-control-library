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
#define HCL_DRIVEONCHIP_REG_DRIVE_STATE        0x00u
#define HCL_DRIVEONCHIP_REG_RUNTIME            0x04u
#define HCL_DRIVEONCHIP_REG_DSP_MODE           0x08u
#define HCL_DRIVEONCHIP_REG_APP_STATE          0x0Cu
#define HCL_DRIVEONCHIP_REG_LATENCY1           0x10u
#define HCL_DRIVEONCHIP_REG_DUMP_MODE          0x14u
#define HCL_DRIVEONCHIP_REG_TRIG_SEL           0x18u
#define HCL_DRIVEONCHIP_REG_TRIG_EDGE          0x1Cu
#define HCL_DRIVEONCHIP_REG_TRIG_VALUE         0x20u
#define HCL_DRIVEONCHIP_REG_FFT_SELECT         0x24u
#define HCL_DRIVEONCHIP_REG_SPEED              0x28u
#define HCL_DRIVEONCHIP_REG_SPEED_FILT         0x2Cu
#define HCL_DRIVEONCHIP_REG_POSITION           0x30u
#define HCL_DRIVEONCHIP_REG_BUTTON_DSP_MODE    0x34u
#define HCL_DRIVEONCHIP_REG_BUTTON_DRIVE_RESET 0x38u
#define HCL_DRIVEONCHIP_REG_CONTROL_LOOP_MODE  0x3Cu
#define HCL_DRIVEONCHIP_REG_FFT0_PK_DET_LIM    0x40u
#define HCL_DRIVEONCHIP_REG_FFT1_PK_DET_LIM    0x44u
#define HCL_DRIVEONCHIP_REG_I_PI_KP            0x48u
#define HCL_DRIVEONCHIP_REG_I_PI_KI            0x4Cu
#define HCL_DRIVEONCHIP_REG_SPEED_PI_KP        0x50u
#define HCL_DRIVEONCHIP_REG_SPEED_PI_KI        0x54u
#define HCL_DRIVEONCHIP_REG_SPEED_SETP0        0x58u
#define HCL_DRIVEONCHIP_REG_AXIS_SELECT        0x5Cu
#define HCL_DRIVEONCHIP_REG_FFT_PING_STATUS    0x60u
#define HCL_DRIVEONCHIP_REG_POS_SETP0          0x64u
#define HCL_DRIVEONCHIP_REG_HYS_GAIN           0x68u
#define HCL_DRIVEONCHIP_REG_DAMP_COEFF         0x6Cu
#define HCL_DRIVEONCHIP_REG_FILT_DC_GAIN_FL    0x70u
#define HCL_DRIVEONCHIP_REG_WAVE_DEMO_MODE     0x74u
#define HCL_DRIVEONCHIP_REG_POS_SPEED_LIMIT    0x78u
#define HCL_DRIVEONCHIP_REG_POS_PI_KP          0x7Cu
#define HCL_DRIVEONCHIP_REG_NATURAL_FREQ       0x80u
#define HCL_DRIVEONCHIP_REG_WAVE_DEMO_PERIOD   0x84u
#define HCL_DRIVEONCHIP_REG_WAVE_DEMO_OFFSET   0x88u
#define HCL_DRIVEONCHIP_REG_WAVE_DEMO_WAVEFORM 0x8Cu
#define HCL_DRIVEONCHIP_REG_WAVE_DEMO_AMP_F    0x90u
#define HCL_DRIVEONCHIP_REG_CMD_WAVE_AMP_POS   0x94u
#define HCL_DRIVEONCHIP_REG_CMD_WAVE_AMP_SPD   0x98u
#define HCL_DRIVEONCHIP_REG_FILT_FN_HZ_FL      0x9Cu
#define HCL_DRIVEONCHIP_REG_FILT_FD_HZ_FL      0xA0u
#define HCL_DRIVEONCHIP_REG_FILT_ZN_FL         0xA4u
#define HCL_DRIVEONCHIP_REG_FILT_ZD_FL         0xA8u
#define HCL_DRIVEONCHIP_REG_FILT_EN            0xACu
#define HCL_DRIVEONCHIP_REG_DCDC_V_SETP0       0xB0u
#define HCL_DRIVEONCHIP_REG_DCDC_EN            0xB4u
#define HCL_DRIVEONCHIP_REG_DCDC_OL_EN         0xB8u
#define HCL_DRIVEONCHIP_REG_DCDC_OL_DUTY       0xBCu
#define HCL_DRIVEONCHIP_REG_DCDC_FREQ          0xC0u
#define HCL_DRIVEONCHIP_REG_DCDC_V_P           0xC4u
#define HCL_DRIVEONCHIP_REG_DCDC_V_I           0xC8u
#define HCL_DRIVEONCHIP_REG_DCDC_I_P           0xCCu
#define HCL_DRIVEONCHIP_REG_DCDC_I_I           0xD0u
#define HCL_DRIVEONCHIP_REG_DC_DC_V_IN         0xD4u
#define HCL_DRIVEONCHIP_REG_DC_DC_V_LINK       0xD8u
#define HCL_DRIVEONCHIP_REG_ADC_TYPE           0xDCu
#define HCL_DRIVEONCHIP_REG_LATENCY2           0xE0u
#define HCL_DRIVEONCHIP_REG_LATENCY3           0xE4u
#define HCL_DRIVEONCHIP_REG_TRACE_DEPTH        0xE8u
#define HCL_DRIVEONCHIP_REG_TRIGGER_POS        0xECu
#define HCL_DRIVEONCHIP_REG_TRACE_SAMPLES      0xF0u
#define HCL_DRIVEONCHIP_REG_WAVE_DEMO_UPDATE   0xF4u
#define HCL_DRIVEONCHIP_REG_HYS_THLD           0xF8u
#define HCL_DRIVEONCHIP_REG_TRACE_ENABLE       0xFCu
#define HCL_DRIVEONCHIP_REG_DC_DC_I_IN         0x100u
#define HCL_DRIVEONCHIP_REG_DC_DC_I_LINK       0x104u
#define HCL_DRIVEONCHIP_REG_MEM_LENGTH         0x108u