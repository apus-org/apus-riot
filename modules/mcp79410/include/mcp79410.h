/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_mcp79410 MCP79410 I2C RTC driver
 * @ingroup     drivers_sensors
 * @brief       Driver for the Microchip MCP79410 real-time clock
 * @{
 *
 * The MCP79410 is a real-time clock with a 32.768 kHz crystal oscillator,
 * two alarms, a multi-function output pin (MFP), 64 bytes of battery-backed
 * SRAM and a 128 byte EEPROM. The EEPROM answers on its own I2C address and
 * is handled by the @ref drivers_at24cxxx driver. The SRAM is accessible
 * through the generic @ref nvram_t interface in @ref mcp79410_t::nvram.
 *
 * The driver can be used directly in an application or through the
 * @ref sys_walltime module by adding the following line to the
 * application's Makefile:
 * ```makefile
 * USEMODULE += walltime_impl_mcp79410
 * ```
 *
 * ## Implementation Status
 * Only alarm 0 is supported. With both alarms enabled, MFP would only be
 * asserted when both have matched.
 *
 * The driver does not handle interrupts from the real-time clock. The
 * application has to configure the pin connected to MFP with
 * @ref gpio_init_int and provide a callback. MFP is an open-drain output
 * that is driven low while the enabled alarm is pending.
 *
 * The device stores the year in two digits and counts year 00 as a common
 * year, so the supported range is 2001 to 2099. After 2099 the device reads
 * as 2000.
 *
 * The power-fail time stamps, the digital trimming, the external clock
 * input and the general purpose output of MFP are not supported.
 *
 * @file
 * @brief       Driver interface for the MCP79410 I2C real-time clock
 *
 * @author      Jakob Jungreuthmayer
 */

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "nvram.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   I2C address of the real-time clock
 */
#define MCP79410_I2C_ADDR       (0x6f)

/**
 * @brief   Size of the battery-backed SRAM in bytes
 */
#define MCP79410_SRAM_SIZE      (64U)

/**
 * @brief   Fields of the alarm time that are compared with the current time
 */
typedef enum {
    MCP79410_ALARM_MATCH_SEC  = 0,  /**< seconds */
    MCP79410_ALARM_MATCH_MIN  = 1,  /**< minutes */
    MCP79410_ALARM_MATCH_HOUR = 2,  /**< hours */
    MCP79410_ALARM_MATCH_WDAY = 3,  /**< day of the week */
    MCP79410_ALARM_MATCH_MDAY = 4,  /**< day of the month */
    MCP79410_ALARM_MATCH_ALL  = 7,  /**< seconds, minutes, hours, day of the
                                         week, day of the month and month */
} mcp79410_alarm_match_t;

/**
 * @brief   Square-wave output frequency
 */
typedef enum {
    MCP79410_SQW_1HZ     = 0,   /**< 1 Hz */
    MCP79410_SQW_4096HZ  = 1,   /**< 4.096 kHz */
    MCP79410_SQW_8192HZ  = 2,   /**< 8.192 kHz */
    MCP79410_SQW_32768HZ = 3,   /**< 32.768 kHz */
} mcp79410_sqw_freq_t;

/**
 * @brief   Configuration parameters for MCP79410 initialization
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    bool vbat_enable;           /**< switch over to the backup battery when
                                     the supply fails */
} mcp79410_params_t;

/**
 * @brief   Device descriptor for the MCP79410
 */
typedef struct {
    i2c_t i2c;                  /**< I2C bus the device is connected to */
    nvram_t nvram;              /**< battery-backed SRAM */
} mcp79410_t;

/**
 * @brief   Initialize the MCP79410 device
 *
 * Starts the oscillator, configures the battery switchover and converts a
 * time kept in 12-hour format to 24-hour format. If the oscillator was
 * stopped, e.g. after a power loss without backup battery, or does not run,
 * the time is invalid and has to be set with @ref mcp79410_set_time.
 *
 * Changing the battery switchover clears the power-fail flag and time stamps.
 *
 * @param[out] dev      device descriptor
 * @param[in]  params   device parameters
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 * @retval  -ETIMEDOUT  the oscillator did not start or stop for the conversion
 *                      to 24-hour format
 * @retval  -ENODATA    the oscillator was stopped or does not run, the time
 *                      is invalid
 */
int mcp79410_init(mcp79410_t *dev, const mcp79410_params_t *params);

/**
 * @brief   Read the current time
 *
 * @param[in]  dev      device descriptor
 * @param[out] time     current time
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error or invalid time read from the device
 */
int mcp79410_get_time(const mcp79410_t *dev, struct tm *time);

/**
 * @brief   Set the current time
 *
 * The oscillator is stopped while the time registers are written and
 * started again afterwards. A stopped oscillator is started and waited for
 * first, which takes around a second and times out after three seconds, so
 * that the written day of the week is kept (MCP7941X silicon errata
 * DS80000604, issue 6). The first second after
 * setting the time can be shorter, as the device keeps the fraction of the
 * current second. Writing the day of the week clears the power-fail flag and
 * time stamps.
 *
 * @param[in] dev       device descriptor
 * @param[in] time      time to set, the year must be within 2001 and 2099
 *
 * @retval  0           on success
 * @retval  -ERANGE     time out of range
 * @retval  -EIO        on I2C communication error
 * @retval  -ETIMEDOUT  the oscillator did not start or stop
 */
int mcp79410_set_time(const mcp79410_t *dev, const struct tm *time);

/**
 * @brief   Start the oscillator
 *
 * @param[in] dev       device descriptor
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_poweron(const mcp79410_t *dev);

/**
 * @brief   Stop the oscillator
 *
 * @param[in] dev       device descriptor
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_poweroff(const mcp79410_t *dev);

/**
 * @brief   Set the alarm time
 *
 * Disables the alarm, writes the alarm time and clears the alarm flag. The
 * alarm has to be enabled again with @ref mcp79410_set_alarm_int.
 *
 * @param[in] dev       device descriptor
 * @param[in] time      alarm time, only the fields compared by @p match are used
 * @param[in] match     fields compared with the current time
 *
 * @retval  0           on success
 * @retval  -ERANGE     a compared field is out of range
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_set_alarm(const mcp79410_t *dev, const struct tm *time,
                       mcp79410_alarm_match_t match);

/**
 * @brief   Get the alarm time
 *
 * @param[in]  dev      device descriptor
 * @param[out] time     alarm time, the year is not stored and set to 0
 * @param[out] match    fields compared with the current time
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_get_alarm(const mcp79410_t *dev, struct tm *time,
                       mcp79410_alarm_match_t *match);

/**
 * @brief   Enable or disable the alarm
 *
 * An enabled alarm sets its flag and drives MFP low on a match. Disabling
 * the alarm also clears its flag.
 *
 * @param[in] dev       device descriptor
 * @param[in] enable    true to enable the alarm, false to disable it
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_set_alarm_int(const mcp79410_t *dev, bool enable);

/**
 * @brief   Get the alarm flag
 *
 * @param[in]  dev      device descriptor
 * @param[out] flag     true if the alarm matched since its flag was cleared
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_get_alarm_flag(const mcp79410_t *dev, bool *flag);

/**
 * @brief   Clear the alarm flag
 *
 * Releases MFP, unless the alarm time still matches: then the device sets
 * the flag again immediately.
 *
 * @param[in] dev       device descriptor
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_clear_alarm_flag(const mcp79410_t *dev);

/**
 * @brief   Output a square wave on MFP
 *
 * The square wave replaces the alarm output on MFP. The alarm flag is still
 * set on a match and drives MFP once the square wave is disabled.
 *
 * @param[in] dev       device descriptor
 * @param[in] freq      square-wave frequency
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_set_sqw(const mcp79410_t *dev, mcp79410_sqw_freq_t freq);

/**
 * @brief   Stop the square-wave output on MFP
 *
 * @param[in] dev       device descriptor
 *
 * @retval  0           on success
 * @retval  -EIO        on I2C communication error
 */
int mcp79410_disable_sqw(const mcp79410_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
