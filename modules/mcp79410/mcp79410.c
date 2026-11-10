/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_mcp79410
 * @{
 *
 * @file
 * @brief       Driver implementation for the MCP79410 I2C real-time clock
 *
 * @author      Jakob Jungreuthmayer
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "bcd.h"
#include "kernel_defines.h"
#include "rtc_utils.h"
#include "ztimer.h"

#include "mcp79410.h"
#include "mcp79410_internal.h"

#define ENABLE_DEBUG        0
#include "debug.h"

static int _read_regs(const mcp79410_t *dev, uint8_t reg, void *buf, size_t len)
{
    if (i2c_read_regs(dev->i2c, MCP79410_I2C_ADDR, reg, buf, len, 0) != 0) {
        return -EIO;
    }
    return 0;
}

static int _write_regs(const mcp79410_t *dev, uint8_t reg, const void *buf, size_t len)
{
    if (i2c_write_regs(dev->i2c, MCP79410_I2C_ADDR, reg, buf, len, 0) != 0) {
        return -EIO;
    }
    return 0;
}

static int _clrset(const mcp79410_t *dev, uint8_t reg, uint8_t clr, uint8_t set)
{
    uint8_t val;
    int res = _read_regs(dev, reg, &val, 1);

    if (res == 0) {
        uint8_t new = (val & ~clr) | set;
        if (new != val) {
            res = _write_regs(dev, reg, &new, 1);
        }
    }
    return res;
}

static void _sleep_released(const mcp79410_t *dev, uint32_t ms)
{
    i2c_release(dev->i2c);
    ztimer_sleep(ZTIMER_MSEC, ms);
    i2c_acquire(dev->i2c);
}

static int _hour_from_reg(uint8_t reg)
{
    if (!(reg & MCP79410_HOUR_12H)) {
        return bcd_to_byte(reg & MCP79410_HOUR_24H_MASK);
    }

    int hour = bcd_to_byte(reg & MCP79410_HOUR_12H_MASK) % 12;
    if (reg & MCP79410_HOUR_PM) {
        hour += 12;
    }
    return hour;
}

static void _time_from_regs(const uint8_t *raw, struct tm *time)
{
    memset(time, 0, sizeof(*time));

    time->tm_sec  = bcd_to_byte(raw[0] & MCP79410_SEC_MASK);
    time->tm_min  = bcd_to_byte(raw[1] & MCP79410_MIN_MASK);
    time->tm_hour = _hour_from_reg(raw[2]);
    time->tm_wday = (raw[3] & MCP79410_WKDAY_MASK) - MCP79410_WKDAY_OFFSET;
    time->tm_mday = bcd_to_byte(raw[4] & MCP79410_DATE_MASK);
    time->tm_mon  = bcd_to_byte(raw[5] & MCP79410_MTH_MASK) - MCP79410_MTH_OFFSET;
    time->tm_isdst = -1;
}

static int _wait_oscrun(const mcp79410_t *dev, bool running)
{
    for (unsigned i = 0; i < MCP79410_OSC_TIMEOUT_MS / MCP79410_OSC_POLL_MS; i++) {
        uint8_t wkday;
        int res = _read_regs(dev, MCP79410_REG_RTCWKDAY, &wkday, 1);
        if (res != 0) {
            return res;
        }
        if (((wkday & MCP79410_RTCWKDAY_OSCRUN) != 0) == running) {
            return 0;
        }
        _sleep_released(dev, MCP79410_OSC_POLL_MS);
    }
    return -ETIMEDOUT;
}

/* RTCWKDAY also holds the day of the week, so it must not be rewritten
 * across midnight */
static int _set_vbaten(const mcp79410_t *dev, bool enable)
{
    uint8_t raw[MCP79410_REG_RTCWKDAY + 1];

    for (unsigned i = 0; i < MCP79410_SEC59_TIMEOUT_MS / MCP79410_SEC59_POLL_MS; i++) {
        int res = _read_regs(dev, MCP79410_REG_RTCSEC, raw, sizeof(raw));
        if (res != 0) {
            return res;
        }
        if (!(raw[MCP79410_REG_RTCSEC] & MCP79410_RTCSEC_ST) ||
            (bcd_to_byte(raw[MCP79410_REG_RTCSEC] & MCP79410_SEC_MASK) != 59)) {
            uint8_t wkday = raw[MCP79410_REG_RTCWKDAY] &
                            ~(MCP79410_RTCWKDAY_VBATEN | MCP79410_RTCWKDAY_PWRFAIL);
            if (enable) {
                wkday |= MCP79410_RTCWKDAY_VBATEN;
            }
            return _write_regs(dev, MCP79410_REG_RTCWKDAY, &wkday, 1);
        }
        _sleep_released(dev, MCP79410_SEC59_POLL_MS);
    }
    return -ETIMEDOUT;
}

/* MCP7941X silicon errata DS80000604, issue 5: the date is written again
 * after month and year */
static int _write_time(const mcp79410_t *dev, const uint8_t *raw)
{
    uint8_t sec = raw[0] | MCP79410_RTCSEC_ST;

    int res = _clrset(dev, MCP79410_REG_RTCSEC, MCP79410_RTCSEC_ST, 0);
    if (res == 0) {
        res = _wait_oscrun(dev, false);
    }
    if (res == 0) {
        res = _write_regs(dev, MCP79410_REG_RTCSEC, raw, MCP79410_TIME_LEN);
    }
    if (res == 0) {
        res = _write_regs(dev, MCP79410_REG_RTCDATE, &raw[4], 1);
    }
    if (res == 0) {
        res = _write_regs(dev, MCP79410_REG_RTCSEC, &sec, 1);
    }
    if (res != 0) {
        _clrset(dev, MCP79410_REG_RTCSEC, 0, MCP79410_RTCSEC_ST);
    }
    return res;
}

/* MCP7941X silicon errata DS80000604, issue 6: the day of the week is checked
 * after the oscillator started and rewritten unless the time advanced */
static int _check_wkday(const mcp79410_t *dev, const uint8_t *raw)
{
    uint8_t now[MCP79410_REG_RTCWKDAY + 1];

    int res = _wait_oscrun(dev, true);
    if (res == 0) {
        res = _read_regs(dev, MCP79410_REG_RTCSEC, now, sizeof(now));
    }
    if ((res == 0) &&
        ((now[MCP79410_REG_RTCSEC] & MCP79410_SEC_MASK) == raw[0]) &&
        ((now[MCP79410_REG_RTCWKDAY] & MCP79410_WKDAY_MASK) !=
         (raw[3] & MCP79410_WKDAY_MASK))) {
        DEBUG("[mcp79410] set_time: rewriting the day of the week\n");
        res = _write_regs(dev, MCP79410_REG_RTCWKDAY, &raw[3], 1);
    }
    return res;
}

static int _validate_time(const struct tm *time)
{
    if (!rtc_tm_valid(time)) {
        return -ERANGE;
    }
    /* the device counts year 00 as a common year, like 2100 */
    if ((time->tm_year <= MCP79410_YEAR_OFFSET) ||
        (time->tm_year > MCP79410_YEAR_OFFSET + 99)) {
        return -ERANGE;
    }
    if ((time->tm_wday < 0) || (time->tm_wday > 6)) {
        return -ERANGE;
    }
    return 0;
}

static int _validate_alarm(const struct tm *time, mcp79410_alarm_match_t match)
{
    static const uint8_t mdays[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    bool all = (match == MCP79410_ALARM_MATCH_ALL);

    assert(((unsigned)match <= MCP79410_ALARM_MATCH_MDAY) || all);

    if ((all || (match == MCP79410_ALARM_MATCH_SEC)) &&
        ((time->tm_sec < 0) || (time->tm_sec > 59))) {
        return -ERANGE;
    }
    if ((all || (match == MCP79410_ALARM_MATCH_MIN)) &&
        ((time->tm_min < 0) || (time->tm_min > 59))) {
        return -ERANGE;
    }
    if ((all || (match == MCP79410_ALARM_MATCH_HOUR)) &&
        ((time->tm_hour < 0) || (time->tm_hour > 23))) {
        return -ERANGE;
    }
    if ((all || (match == MCP79410_ALARM_MATCH_WDAY)) &&
        ((time->tm_wday < 0) || (time->tm_wday > 6))) {
        return -ERANGE;
    }
    if ((match == MCP79410_ALARM_MATCH_MDAY) &&
        ((time->tm_mday < 1) || (time->tm_mday > 31))) {
        return -ERANGE;
    }
    if (all && ((time->tm_mon < 0) || (time->tm_mon > 11) || (time->tm_mday < 1) ||
                (time->tm_mday > mdays[time->tm_mon]))) {
        return -ERANGE;
    }
    return 0;
}

static int _nvram_read(nvram_t *nvram, uint8_t *dst, uint32_t src, size_t size)
{
    const mcp79410_t *dev = nvram->extra;

    if ((src > nvram->size) || (size > nvram->size - src)) {
        return -EOVERFLOW;
    }

    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MCP79410_REG_SRAM + src, dst, size);
    i2c_release(dev->i2c);

    return (res == 0) ? (int)size : res;
}

static int _nvram_write(nvram_t *nvram, const uint8_t *src, uint32_t dst, size_t size)
{
    const mcp79410_t *dev = nvram->extra;

    if ((dst > nvram->size) || (size > nvram->size - dst)) {
        return -EOVERFLOW;
    }

    i2c_acquire(dev->i2c);
    int res = _write_regs(dev, MCP79410_REG_SRAM + dst, src, size);
    i2c_release(dev->i2c);

    return (res == 0) ? (int)size : res;
}

int mcp79410_init(mcp79410_t *dev, const mcp79410_params_t *params)
{
    assert(dev && params);

    dev->i2c = params->i2c;
    dev->nvram.read = _nvram_read;
    dev->nvram.write = _nvram_write;
    dev->nvram.size = MCP79410_SRAM_SIZE;
    dev->nvram.extra = dev;

    uint8_t raw[MCP79410_REG_CONTROL + 1];

    i2c_acquire(dev->i2c);

    int res = _read_regs(dev, MCP79410_REG_RTCSEC, raw, sizeof(raw));
    if (res != 0) {
        DEBUG("[mcp79410] init: no device found\n");
        goto out;
    }

    /* the general purpose output drives MFP while no alarm is enabled */
    res = _clrset(dev, MCP79410_REG_CONTROL, 0, MCP79410_CONTROL_OUT);
    if (res != 0) {
        goto out;
    }

    if (((raw[MCP79410_REG_RTCWKDAY] & MCP79410_RTCWKDAY_VBATEN) != 0) != params->vbat_enable) {
        res = _set_vbaten(dev, params->vbat_enable);
        if (res != 0) {
            goto out;
        }
    }

    if (!(raw[MCP79410_REG_RTCSEC] & MCP79410_RTCSEC_ST)) {
        DEBUG("[mcp79410] init: oscillator was stopped\n");
        res = _clrset(dev, MCP79410_REG_RTCSEC, 0, MCP79410_RTCSEC_ST);
        i2c_release(dev->i2c);
        return (res == 0) ? -ENODATA : res;
    }

    i2c_release(dev->i2c);

    if (!(raw[MCP79410_REG_RTCWKDAY] & MCP79410_RTCWKDAY_OSCRUN)) {
        DEBUG("[mcp79410] init: oscillator is not running\n");
        return -ENODATA;
    }

    if (raw[MCP79410_REG_RTCHOUR] & MCP79410_HOUR_12H) {
        struct tm time;

        DEBUG("[mcp79410] init: converting to 24-hour format\n");
        res = mcp79410_get_time(dev, &time);
        if (res == 0) {
            res = mcp79410_set_time(dev, &time);
        }
    }
    return res;

out:
    i2c_release(dev->i2c);
    return res;
}

int mcp79410_get_time(const mcp79410_t *dev, struct tm *time)
{
    assert(dev && time);

    uint8_t raw[MCP79410_TIME_LEN];

    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MCP79410_REG_RTCSEC, raw, sizeof(raw));
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    _time_from_regs(raw, time);
    time->tm_year = bcd_to_byte(raw[6]) + MCP79410_YEAR_OFFSET;

    return (rtc_tm_valid(time) && (time->tm_wday >= 0)) ? 0 : -EIO;
}

int mcp79410_set_time(const mcp79410_t *dev, const struct tm *time)
{
    assert(dev && time);

    int res = _validate_time(time);
    if (res != 0) {
        return res;
    }

    uint8_t raw[MCP79410_TIME_LEN];

    raw[0] = bcd_from_byte(time->tm_sec);
    raw[1] = bcd_from_byte(time->tm_min);
    raw[2] = bcd_from_byte(time->tm_hour);
    raw[3] = time->tm_wday + MCP79410_WKDAY_OFFSET;
    raw[4] = bcd_from_byte(time->tm_mday);
    raw[5] = bcd_from_byte(time->tm_mon + MCP79410_MTH_OFFSET);
    raw[6] = bcd_from_byte(time->tm_year - MCP79410_YEAR_OFFSET);

    i2c_acquire(dev->i2c);

    /* MCP7941X silicon errata DS80000604, issue 6: the day of the week is
     * written while the oscillator runs */
    uint8_t wkday;
    res = _read_regs(dev, MCP79410_REG_RTCWKDAY, &wkday, 1);
    if ((res == 0) && !(wkday & MCP79410_RTCWKDAY_OSCRUN)) {
        res = _clrset(dev, MCP79410_REG_RTCSEC, 0, MCP79410_RTCSEC_ST);
        if (res == 0) {
            res = _wait_oscrun(dev, true);
        }
    }
    if (res == 0) {
        raw[3] |= wkday & MCP79410_RTCWKDAY_VBATEN;
        res = _write_regs(dev, MCP79410_REG_RTCWKDAY, &raw[3], 1);
    }
    if (res == 0) {
        res = _write_time(dev, raw);
    }
    if (res == 0) {
        res = _check_wkday(dev, raw);
    }

    i2c_release(dev->i2c);
    return res;
}

int mcp79410_poweron(const mcp79410_t *dev)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    int res = _clrset(dev, MCP79410_REG_RTCSEC, 0, MCP79410_RTCSEC_ST);
    i2c_release(dev->i2c);
    return res;
}

int mcp79410_poweroff(const mcp79410_t *dev)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    int res = _clrset(dev, MCP79410_REG_RTCSEC, MCP79410_RTCSEC_ST, 0);
    i2c_release(dev->i2c);
    return res;
}

int mcp79410_set_alarm(const mcp79410_t *dev, const struct tm *time,
                       mcp79410_alarm_match_t match)
{
    assert(dev && time);

    int res = _validate_alarm(time, match);
    if (res != 0) {
        return res;
    }

    struct tm alarm = { .tm_mday = 1 };

    switch (match) {
    case MCP79410_ALARM_MATCH_SEC:
        alarm.tm_sec = time->tm_sec;
        break;
    case MCP79410_ALARM_MATCH_MIN:
        alarm.tm_min = time->tm_min;
        break;
    case MCP79410_ALARM_MATCH_HOUR:
        alarm.tm_hour = time->tm_hour;
        break;
    case MCP79410_ALARM_MATCH_WDAY:
        alarm.tm_wday = time->tm_wday;
        break;
    case MCP79410_ALARM_MATCH_MDAY:
        alarm.tm_mday = time->tm_mday;
        break;
    case MCP79410_ALARM_MATCH_ALL:
        alarm = *time;
        break;
    }

    uint8_t raw[MCP79410_ALM_LEN];

    raw[0] = bcd_from_byte(alarm.tm_sec);
    raw[1] = bcd_from_byte(alarm.tm_min);
    raw[2] = bcd_from_byte(alarm.tm_hour);
    raw[3] = (alarm.tm_wday + MCP79410_WKDAY_OFFSET) |
             ((uint8_t)match << MCP79410_ALMWKDAY_MSK_POS);
    raw[4] = bcd_from_byte(alarm.tm_mday);
    raw[5] = bcd_from_byte(alarm.tm_mon + MCP79410_MTH_OFFSET);

    i2c_acquire(dev->i2c);
    res = _clrset(dev, MCP79410_REG_CONTROL, MCP79410_CONTROL_ALM0EN, 0);
    if (res == 0) {
        res = _write_regs(dev, MCP79410_REG_ALM0SEC, raw, sizeof(raw));
    }
    i2c_release(dev->i2c);

    return res;
}

int mcp79410_get_alarm(const mcp79410_t *dev, struct tm *time,
                       mcp79410_alarm_match_t *match)
{
    assert(dev && time && match);

    uint8_t raw[MCP79410_ALM_LEN];

    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MCP79410_REG_ALM0SEC, raw, sizeof(raw));
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    _time_from_regs(raw, time);
    *match = (raw[3] & MCP79410_ALMWKDAY_MSK_MASK) >> MCP79410_ALMWKDAY_MSK_POS;

    return 0;
}

int mcp79410_set_alarm_int(const mcp79410_t *dev, bool enable)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    int res;
    if (enable) {
        res = _clrset(dev, MCP79410_REG_CONTROL, 0, MCP79410_CONTROL_ALM0EN);
    }
    else {
        res = _clrset(dev, MCP79410_REG_CONTROL, MCP79410_CONTROL_ALM0EN, 0);
        if (res == 0) {
            res = _clrset(dev, MCP79410_REG_ALM0WKDAY, MCP79410_ALMWKDAY_IF, 0);
        }
    }
    i2c_release(dev->i2c);
    return res;
}

int mcp79410_get_alarm_flag(const mcp79410_t *dev, bool *flag)
{
    assert(dev && flag);

    uint8_t wkday;

    i2c_acquire(dev->i2c);
    int res = _read_regs(dev, MCP79410_REG_ALM0WKDAY, &wkday, 1);
    i2c_release(dev->i2c);
    if (res != 0) {
        return res;
    }

    *flag = (wkday & MCP79410_ALMWKDAY_IF) != 0;
    return 0;
}

int mcp79410_clear_alarm_flag(const mcp79410_t *dev)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    int res = _clrset(dev, MCP79410_REG_ALM0WKDAY, MCP79410_ALMWKDAY_IF, 0);
    i2c_release(dev->i2c);
    return res;
}

int mcp79410_set_sqw(const mcp79410_t *dev, mcp79410_sqw_freq_t freq)
{
    assert(dev);
    assert((unsigned)freq <= MCP79410_SQW_32768HZ);

    i2c_acquire(dev->i2c);
    int res = _clrset(dev, MCP79410_REG_CONTROL, MCP79410_CONTROL_SQWFS_MASK,
                      MCP79410_CONTROL_SQWEN | freq);
    i2c_release(dev->i2c);
    return res;
}

int mcp79410_disable_sqw(const mcp79410_t *dev)
{
    assert(dev);

    i2c_acquire(dev->i2c);
    int res = _clrset(dev, MCP79410_REG_CONTROL, MCP79410_CONTROL_SQWEN, 0);
    i2c_release(dev->i2c);
    return res;
}

#if IS_USED(MODULE_WALLTIME_IMPL_MCP79410)
#include "mcp79410_params.h"

static mcp79410_t _walltime_dev;
static bool _walltime_init_done;

void walltime_impl_init(void)
{
    int res = mcp79410_init(&_walltime_dev, &mcp79410_params[0]);
    _walltime_init_done = (res == 0) || (res == -ENODATA);
}

int walltime_impl_get(struct tm *time, uint16_t *ms)
{
    if (!_walltime_init_done) {
        return -ENODEV;
    }
    *ms = 0;
    return mcp79410_get_time(&_walltime_dev, time);
}

int walltime_impl_set(struct tm *time)
{
    if (!_walltime_init_done) {
        return -ENODEV;
    }
    return mcp79410_set_time(&_walltime_dev, time);
}
#endif
