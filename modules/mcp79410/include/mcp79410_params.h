/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_mcp79410_config  MCP79410 compile-time configuration
 * @ingroup     config_drivers_sensors
 * @{
 *
 * @file
 * @brief       Default configuration for the MCP79410
 *
 * @author      Jakob Jungreuthmayer
 */

#include "board.h"
#include "mcp79410.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration parameters for the MCP79410
 * @{
 */
#ifndef MCP79410_PARAM_I2C
#define MCP79410_PARAM_I2C          I2C_DEV(0)
#endif
#ifndef MCP79410_PARAM_VBAT_ENABLE
#define MCP79410_PARAM_VBAT_ENABLE  false
#endif

#ifndef MCP79410_PARAMS
#define MCP79410_PARAMS             { .i2c = MCP79410_PARAM_I2C,                \
                                      .vbat_enable = MCP79410_PARAM_VBAT_ENABLE }
#endif
/** @} */

/**
 * @brief   MCP79410 configuration
 */
static const mcp79410_params_t mcp79410_params[] =
{
    MCP79410_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
