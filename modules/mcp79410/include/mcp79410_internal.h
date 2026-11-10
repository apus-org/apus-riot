/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_mcp79410
 * @{
 *
 * @file
 * @brief       Register definitions for the MCP79410
 *
 * @author      Jakob Jungreuthmayer
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Register addresses
 * @{
 */
#define MCP79410_REG_RTCSEC         (0x00)
#define MCP79410_REG_RTCMIN         (0x01)
#define MCP79410_REG_RTCHOUR        (0x02)
#define MCP79410_REG_RTCWKDAY       (0x03)
#define MCP79410_REG_RTCDATE        (0x04)
#define MCP79410_REG_RTCMTH         (0x05)
#define MCP79410_REG_RTCYEAR        (0x06)
#define MCP79410_REG_CONTROL        (0x07)
#define MCP79410_REG_ALM0SEC        (0x0a)
#define MCP79410_REG_ALM0WKDAY      (0x0d)
#define MCP79410_REG_SRAM           (0x20)
/** @} */

/**
 * @name    Register lengths
 * @{
 */
#define MCP79410_TIME_LEN           (7U)
#define MCP79410_ALM_LEN            (6U)
/** @} */

/**
 * @name    Time register bits
 * @{
 */
#define MCP79410_RTCSEC_ST          (0x80)
#define MCP79410_SEC_MASK           (0x7f)
#define MCP79410_MIN_MASK           (0x7f)
#define MCP79410_HOUR_12H           (0x40)
#define MCP79410_HOUR_PM            (0x20)
#define MCP79410_HOUR_12H_MASK      (0x1f)
#define MCP79410_HOUR_24H_MASK      (0x3f)
#define MCP79410_RTCWKDAY_OSCRUN    (0x20)
#define MCP79410_RTCWKDAY_PWRFAIL   (0x10)
#define MCP79410_RTCWKDAY_VBATEN    (0x08)
#define MCP79410_WKDAY_MASK         (0x07)
#define MCP79410_DATE_MASK          (0x3f)
#define MCP79410_MTH_MASK           (0x1f)
/** @} */

/**
 * @name    CONTROL register bits
 * @{
 */
#define MCP79410_CONTROL_OUT        (0x80)
#define MCP79410_CONTROL_SQWEN      (0x40)
#define MCP79410_CONTROL_ALM1EN     (0x20)
#define MCP79410_CONTROL_ALM0EN     (0x10)
#define MCP79410_CONTROL_SQWFS_MASK (0x03)
/** @} */

/**
 * @name    ALM0WKDAY register bits
 * @{
 */
#define MCP79410_ALMWKDAY_MSK_POS   (4U)
#define MCP79410_ALMWKDAY_MSK_MASK  (0x70)
#define MCP79410_ALMWKDAY_IF        (0x08)
/** @} */

/**
 * @name    Offsets between struct tm and the registers
 * @{
 */
#define MCP79410_WKDAY_OFFSET       (1)
#define MCP79410_MTH_OFFSET         (1)
#define MCP79410_YEAR_OFFSET        (100)
/** @} */

/**
 * @name    Timing
 * @{
 */
#define MCP79410_OSC_TIMEOUT_MS     (3000U)
#define MCP79410_OSC_POLL_MS        (1U)
#define MCP79410_SEC59_TIMEOUT_MS   (1200U)
#define MCP79410_SEC59_POLL_MS      (10U)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
