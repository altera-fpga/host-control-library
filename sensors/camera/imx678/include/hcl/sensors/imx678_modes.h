/*
 * Copyright (c) 2026 Altera Corporation
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "hcl/sensors/imx678_regs.h"

typedef struct hcl_imx678_mode {
    const hcl_imx678_reg_value_t* registers;
    size_t register_count;
} hcl_imx678_mode;

/* 1280x720 @ 30 fps, 10-bit */
static hcl_imx678_reg_value_t imx678_1280x720_30fps_10bit[] = {
    {HCL_IMX678_WINMODE,      0x04},
    {HCL_IMX678_MDBIT,        0x00},
    {HCL_IMX678_ADBIT,        0x00},
    {HCL_IMX678_PIX_HST_LOW,  0x00},
    {HCL_IMX678_PIX_HST_HIGH, 0x05},
    {HCL_IMX678_PIX_VST_LOW,  0xD0},
    {HCL_IMX678_PIX_VST_HIGH, 0x02},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x00},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x05},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0xD0},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x02},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x4C},
    {HCL_IMX678_HMAX_HIGH,    0x04},
};

/* 1280x720 @ 30 fps, 12-bit */
static hcl_imx678_reg_value_t imx678_1280x720_30fps_12bit[] = {
    {HCL_IMX678_WINMODE,      0x04},
    {HCL_IMX678_MDBIT,        0x01},
    {HCL_IMX678_ADBIT,        0x01},
    {HCL_IMX678_PIX_HST_LOW,  0x00},
    {HCL_IMX678_PIX_HST_HIGH, 0x05},
    {HCL_IMX678_PIX_VST_LOW,  0xD0},
    {HCL_IMX678_PIX_VST_HIGH, 0x02},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x00},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x05},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0xD0},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x02},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x4C},
    {HCL_IMX678_HMAX_HIGH,    0x04},
};

/* 1280x720 @ 60 fps, 10-bit */
static hcl_imx678_reg_value_t imx678_1280x720_60fps_10bit[] = {
    {HCL_IMX678_WINMODE,      0x04},
    {HCL_IMX678_MDBIT,        0x00},
    {HCL_IMX678_ADBIT,        0x00},
    {HCL_IMX678_PIX_HST_LOW,  0x00},
    {HCL_IMX678_PIX_HST_HIGH, 0x05},
    {HCL_IMX678_PIX_VST_LOW,  0xD0},
    {HCL_IMX678_PIX_VST_HIGH, 0x02},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x00},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x05},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0xD0},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x02},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x26},
    {HCL_IMX678_HMAX_HIGH,    0x02},
};

/* 1280x720 @ 60 fps, 12-bit */
static hcl_imx678_reg_value_t imx678_1280x720_60fps_12bit[] = {
    {HCL_IMX678_WINMODE,      0x04},
    {HCL_IMX678_MDBIT,        0x01},
    {HCL_IMX678_ADBIT,        0x01},
    {HCL_IMX678_PIX_HST_LOW,  0x00},
    {HCL_IMX678_PIX_HST_HIGH, 0x05},
    {HCL_IMX678_PIX_VST_LOW,  0xD0},
    {HCL_IMX678_PIX_VST_HIGH, 0x02},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x00},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x05},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0xD0},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x02},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x26},
    {HCL_IMX678_HMAX_HIGH,    0x02},
};

/* 1920x1080 @ 30 fps, 10-bit */
static hcl_imx678_reg_value_t imx678_1920x1080_30fps_10bit[] = {
    {HCL_IMX678_WINMODE,      0x04},
    {HCL_IMX678_MDBIT,        0x00},
    {HCL_IMX678_ADBIT,        0x00},
    {HCL_IMX678_PIX_HST_LOW,  0xC0},
    {HCL_IMX678_PIX_HST_HIGH, 0x03},
    {HCL_IMX678_PIX_VST_LOW,  0x1C},
    {HCL_IMX678_PIX_VST_HIGH, 0x02},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x80},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x07},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0x38},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x04},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x4C},
    {HCL_IMX678_HMAX_HIGH,    0x04},
};

/* 1920x1080 @ 30 fps, 12-bit */
static hcl_imx678_reg_value_t imx678_1920x1080_30fps_12bit[] = {
    {HCL_IMX678_WINMODE,      0x04},
    {HCL_IMX678_MDBIT,        0x01},
    {HCL_IMX678_ADBIT,        0x01},
    {HCL_IMX678_PIX_HST_LOW,  0xC0},
    {HCL_IMX678_PIX_HST_HIGH, 0x03},
    {HCL_IMX678_PIX_VST_LOW,  0x1C},
    {HCL_IMX678_PIX_VST_HIGH, 0x02},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x80},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x07},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0x38},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x04},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x4C},
    {HCL_IMX678_HMAX_HIGH,    0x04},
};

/* 1920x1080 @ 60 fps, 10-bit */
static hcl_imx678_reg_value_t imx678_1920x1080_60fps_10bit[] = {
    {HCL_IMX678_WINMODE,      0x04},
    {HCL_IMX678_MDBIT,        0x00},
    {HCL_IMX678_ADBIT,        0x00},
    {HCL_IMX678_PIX_HST_LOW,  0xC0},
    {HCL_IMX678_PIX_HST_HIGH, 0x03},
    {HCL_IMX678_PIX_VST_LOW,  0x1C},
    {HCL_IMX678_PIX_VST_HIGH, 0x02},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x80},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x07},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0x38},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x04},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x26},
    {HCL_IMX678_HMAX_HIGH,    0x02},
};

/* 1920x1080 @ 60 fps, 12-bit */
static hcl_imx678_reg_value_t imx678_1920x1080_60fps_12bit[] = {
    {HCL_IMX678_WINMODE,      0x04},
    {HCL_IMX678_MDBIT,        0x01},
    {HCL_IMX678_ADBIT,        0x01},
    {HCL_IMX678_PIX_HST_LOW,  0xC0},
    {HCL_IMX678_PIX_HST_HIGH, 0x03},
    {HCL_IMX678_PIX_VST_LOW,  0x1C},
    {HCL_IMX678_PIX_VST_HIGH, 0x02},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x80},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x07},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0x38},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x04},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x26},
    {HCL_IMX678_HMAX_HIGH,    0x02},
};

/* 3840x2160 @ 30 fps, 10-bit */
static hcl_imx678_reg_value_t imx678_3840x2160_30fps_10bit[] = {
    {HCL_IMX678_WINMODE,      0x00},
    {HCL_IMX678_MDBIT,        0x00},
    {HCL_IMX678_ADBIT,        0x00},
    {HCL_IMX678_PIX_HST_LOW,  0x00},
    {HCL_IMX678_PIX_HST_HIGH, 0x00},
    {HCL_IMX678_PIX_VST_LOW,  0x00},
    {HCL_IMX678_PIX_VST_HIGH, 0x00},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x00},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x0F},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0x70},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x08},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x4C},
    {HCL_IMX678_HMAX_HIGH,    0x04},
};

/* 3840x2160 @ 30 fps, 12-bit */
static hcl_imx678_reg_value_t imx678_3840x2160_30fps_12bit[] = {
    {HCL_IMX678_WINMODE,      0x00},
    {HCL_IMX678_MDBIT,        0x01},
    {HCL_IMX678_ADBIT,        0x01},
    {HCL_IMX678_PIX_HST_LOW,  0x00},
    {HCL_IMX678_PIX_HST_HIGH, 0x00},
    {HCL_IMX678_PIX_VST_LOW,  0x00},
    {HCL_IMX678_PIX_VST_HIGH, 0x00},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x00},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x0F},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0x70},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x08},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x4C},
    {HCL_IMX678_HMAX_HIGH,    0x04},
};

/* 3840x2160 @ 60 fps, 10-bit */
static hcl_imx678_reg_value_t imx678_3840x2160_60fps_10bit[] = {
    {HCL_IMX678_WINMODE,      0x00},
    {HCL_IMX678_MDBIT,        0x00},
    {HCL_IMX678_ADBIT,        0x00},
    {HCL_IMX678_PIX_HST_LOW,  0x00},
    {HCL_IMX678_PIX_HST_HIGH, 0x00},
    {HCL_IMX678_PIX_VST_LOW,  0x00},
    {HCL_IMX678_PIX_VST_HIGH, 0x00},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x00},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x0F},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0x70},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x08},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x26},
    {HCL_IMX678_HMAX_HIGH,    0x02},
};

/* 3840x2160 @ 60 fps, 12-bit */
static hcl_imx678_reg_value_t imx678_3840x2160_60fps_12bit[] = {
    {HCL_IMX678_WINMODE,      0x00},
    {HCL_IMX678_MDBIT,        0x01},
    {HCL_IMX678_ADBIT,        0x01},
    {HCL_IMX678_PIX_HST_LOW,  0x00},
    {HCL_IMX678_PIX_HST_HIGH, 0x00},
    {HCL_IMX678_PIX_VST_LOW,  0x00},
    {HCL_IMX678_PIX_VST_HIGH, 0x00},
    {HCL_IMX678_PIX_HWIDTH_LOW,  0x00},
    {HCL_IMX678_PIX_HWIDTH_HIGH, 0x0F},
    {HCL_IMX678_PIX_VWIDTH_LOW,  0x70},
    {HCL_IMX678_PIX_VWIDTH_HIGH, 0x08},
    {HCL_IMX678_VMAX_LOW,     0xCA},
    {HCL_IMX678_VMAX_MID,     0x08},
    {HCL_IMX678_VMAX_HIGH,    0x00},
    {HCL_IMX678_HMAX_LOW,     0x26},
    {HCL_IMX678_HMAX_HIGH,    0x02},
};

static const hcl_imx678_mode IMX678_MODE_1280X720_30FPS_10BIT = {
    imx678_1280x720_30fps_10bit,
    sizeof(imx678_1280x720_30fps_10bit) / sizeof(imx678_1280x720_30fps_10bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_1280X720_30FPS_12BIT = {
    imx678_1280x720_30fps_12bit,
    sizeof(imx678_1280x720_30fps_12bit) / sizeof(imx678_1280x720_30fps_12bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_1280X720_60FPS_10BIT = {
    imx678_1280x720_60fps_10bit,
    sizeof(imx678_1280x720_60fps_10bit) / sizeof(imx678_1280x720_60fps_10bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_1280X720_60FPS_12BIT = {
    imx678_1280x720_60fps_12bit,
    sizeof(imx678_1280x720_60fps_12bit) / sizeof(imx678_1280x720_60fps_12bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_1920X1080_30FPS_10BIT = {
    imx678_1920x1080_30fps_10bit,
    sizeof(imx678_1920x1080_30fps_10bit) / sizeof(imx678_1920x1080_30fps_10bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_1920X1080_30FPS_12BIT = {
    imx678_1920x1080_30fps_12bit,
    sizeof(imx678_1920x1080_30fps_12bit) / sizeof(imx678_1920x1080_30fps_12bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_1920X1080_60FPS_10BIT = {
    imx678_1920x1080_60fps_10bit,
    sizeof(imx678_1920x1080_60fps_10bit) / sizeof(imx678_1920x1080_60fps_10bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_1920X1080_60FPS_12BIT = {
    imx678_1920x1080_60fps_12bit,
    sizeof(imx678_1920x1080_60fps_12bit) / sizeof(imx678_1920x1080_60fps_12bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_3840X2160_30FPS_10BIT = {
    imx678_3840x2160_30fps_10bit,
    sizeof(imx678_3840x2160_30fps_10bit) / sizeof(imx678_3840x2160_30fps_10bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_3840X2160_30FPS_12BIT = {
    imx678_3840x2160_30fps_12bit,
    sizeof(imx678_3840x2160_30fps_12bit) / sizeof(imx678_3840x2160_30fps_12bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_3840X2160_60FPS_10BIT = {
    imx678_3840x2160_60fps_10bit,
    sizeof(imx678_3840x2160_60fps_10bit) / sizeof(imx678_3840x2160_60fps_10bit[0]),
};

static const hcl_imx678_mode IMX678_MODE_3840X2160_60FPS_12BIT = {
    imx678_3840x2160_60fps_12bit,
    sizeof(imx678_3840x2160_60fps_12bit) / sizeof(imx678_3840x2160_60fps_12bit[0]),
};
