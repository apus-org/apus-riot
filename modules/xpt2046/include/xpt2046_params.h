/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_xpt2046
 *
 * @{
 * @file
 * @brief       Default configuration for XPT2046
 *
 * @author      Jakob Jungreuthmayer
 */

#include "board.h"
#include "xpt2046.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the XPT2046
 * @{
 */
#ifndef XPT2046_PARAM_SPI_DEV
#define XPT2046_PARAM_SPI_DEV           SPI_DEV(0)
#endif
#ifndef XPT2046_PARAM_CLK
#define XPT2046_PARAM_CLK               SPI_CLK_1MHZ
#endif
#ifndef XPT2046_PARAM_CS
#define XPT2046_PARAM_CS                GPIO_PIN(0, 0)
#endif
#ifndef XPT2046_PARAM_INT_PIN
#define XPT2046_PARAM_INT_PIN           GPIO_UNDEF
#endif
#ifndef XPT2046_PARAM_XMAX
#define XPT2046_PARAM_XMAX              (240U)
#endif
#ifndef XPT2046_PARAM_YMAX
#define XPT2046_PARAM_YMAX              (320U)
#endif
#ifndef XPT2046_PARAM_RAW_XMIN
#define XPT2046_PARAM_RAW_XMIN          (0U)
#endif
#ifndef XPT2046_PARAM_RAW_XMAX
#define XPT2046_PARAM_RAW_XMAX          (4095U)
#endif
#ifndef XPT2046_PARAM_RAW_YMIN
#define XPT2046_PARAM_RAW_YMIN          (0U)
#endif
#ifndef XPT2046_PARAM_RAW_YMAX
#define XPT2046_PARAM_RAW_YMAX          (4095U)
#endif
#ifndef XPT2046_PARAM_XYCONV
#define XPT2046_PARAM_XYCONV            (XPT2046_NO_CONV)
#endif

#ifndef XPT2046_PARAMS
#define XPT2046_PARAMS                  { .spi = XPT2046_PARAM_SPI_DEV,         \
                                          .clk = XPT2046_PARAM_CLK,             \
                                          .cs = XPT2046_PARAM_CS,               \
                                          .int_pin = XPT2046_PARAM_INT_PIN,     \
                                          .xmax = XPT2046_PARAM_XMAX,           \
                                          .ymax = XPT2046_PARAM_YMAX,           \
                                          .raw_xmin = XPT2046_PARAM_RAW_XMIN,   \
                                          .raw_xmax = XPT2046_PARAM_RAW_XMAX,   \
                                          .raw_ymin = XPT2046_PARAM_RAW_YMIN,   \
                                          .raw_ymax = XPT2046_PARAM_RAW_YMAX,   \
                                          .xyconv = XPT2046_PARAM_XYCONV,       \
                                        }
#endif
/**@}*/

/**
 * @brief   Configure XPT2046
 */
static const xpt2046_params_t xpt2046_params[] =
{
    XPT2046_PARAMS
};

/**
 * @brief   Default screen identifiers
 */
#ifndef XPT2046_PARAM_SCREEN_IDS
#define XPT2046_PARAM_SCREEN_IDS        0
#endif

/**
 * @brief   Configure screen identifiers
 */
static const uint8_t xpt2046_screen_ids[] =
{
    XPT2046_PARAM_SCREEN_IDS,
};

#ifdef __cplusplus
}
#endif

/** @} */
