/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_xpt2046
 * @{
 *
 * @file
 * @brief       Constants for XPT2046
 *
 * @author      Jakob Jungreuthmayer
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Control byte fields
 * @{
 */
#define XPT2046_CTRL_START          (0x80)
#define XPT2046_CTRL_A_TEMP0        (0x00)
#define XPT2046_CTRL_A_Y            (0x10)
#define XPT2046_CTRL_A_Z1           (0x30)
#define XPT2046_CTRL_A_Z2           (0x40)
#define XPT2046_CTRL_A_X            (0x50)
#define XPT2046_CTRL_MODE_8BIT      (0x08)
#define XPT2046_CTRL_SER            (0x04)
#define XPT2046_CTRL_PD_ADC_ON      (0x01)
#define XPT2046_CTRL_PD_VREF_ON     (0x02)
/** @} */

/**
 * @name    Control bytes
 * @{
 */
#define XPT2046_CMD_X               (XPT2046_CTRL_START | XPT2046_CTRL_A_X | \
                                     XPT2046_CTRL_PD_ADC_ON)
#define XPT2046_CMD_Y               (XPT2046_CTRL_START | XPT2046_CTRL_A_Y | \
                                     XPT2046_CTRL_PD_ADC_ON)
#define XPT2046_CMD_Z1              (XPT2046_CTRL_START | XPT2046_CTRL_A_Z1 | \
                                     XPT2046_CTRL_PD_ADC_ON)
#define XPT2046_CMD_Z2              (XPT2046_CTRL_START | XPT2046_CTRL_A_Z2 | \
                                     XPT2046_CTRL_PD_ADC_ON)
#define XPT2046_CMD_TEMP0           (XPT2046_CTRL_START | XPT2046_CTRL_A_TEMP0 | \
                                     XPT2046_CTRL_SER | XPT2046_CTRL_PD_VREF_ON | \
                                     XPT2046_CTRL_PD_ADC_ON)
#define XPT2046_CMD_POWER_DOWN      (XPT2046_CTRL_START | XPT2046_CTRL_A_Y)
/** @} */

/**
 * @name    Conversion limits and timing
 * @{
 */
#define XPT2046_RAW_MAX             (4095U)
#define XPT2046_Z_MAX               (2 * XPT2046_RAW_MAX)
#define XPT2046_VREF_SETTLE_MS      (1U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
