/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the MCP79410 RTC driver
 *
 * @author      Jakob Jungreuthmayer
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mcp79410.h"
#include "mcp79410_params.h"

#include "container.h"
#include "rtc_utils.h"
#include "shell.h"
#include "ztimer.h"

#define ISOSTR_LEN  (20U)

static mcp79410_t _dev;

static const struct tm _ref_times[] = {
    { .tm_sec = 58, .tm_min = 59, .tm_hour = 23, .tm_wday = 2,
      .tm_mday = 10, .tm_mon = 10, .tm_year = 126 },
    { .tm_sec = 58, .tm_min = 59, .tm_hour = 23, .tm_wday = 2,
      .tm_mday = 29, .tm_mon = 1, .tm_year = 128 },
};

static int _tm_from_str(const char *str, struct tm *t)
{
    memset(t, 0, sizeof(*t));
    if ((strlen(str) != ISOSTR_LEN - 1) ||
        (sscanf(str, "%4d-%2d-%2dT%2d:%2d:%2d", &t->tm_year, &t->tm_mon, &t->tm_mday,
                &t->tm_hour, &t->tm_min, &t->tm_sec) != 6)) {
        return -1;
    }
    t->tm_year -= 1900;
    t->tm_mon -= 1;

    struct tm norm = *t;
    mktime(&norm);
    t->tm_wday = norm.tm_wday;
    return 0;
}

static void _print_time(const char *label, const struct tm *t)
{
    printf("%s%04d-%02d-%02dT%02d:%02d:%02d (wday %d)\n", label,
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
           t->tm_hour, t->tm_min, t->tm_sec, t->tm_wday);
}

static int _cmd_time_get(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    struct tm t;
    int res = mcp79410_get_time(&_dev, &t);
    if (res != 0) {
        printf("error: get_time failed (%d)\n", res);
        return 1;
    }
    _print_time("current time: ", &t);
    return 0;
}
SHELL_COMMAND(time_get, "read current time", _cmd_time_get);

static int _cmd_time_set(int argc, char **argv)
{
    struct tm t;

    if ((argc != 2) || (_tm_from_str(argv[1], &t) != 0)) {
        printf("usage: %s <YYYY-MM-DDTHH:mm:ss>\n", argv[0]);
        return 1;
    }

    int res = mcp79410_set_time(&_dev, &t);
    if (res != 0) {
        printf("error: set_time failed (%d)\n", res);
        return 1;
    }
    _print_time("time set to ", &t);
    return 0;
}
SHELL_COMMAND(time_set, "set time", _cmd_time_set);

static int _cmd_alarm_get(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    struct tm t;
    mcp79410_alarm_match_t match;
    bool flag;
    int res = mcp79410_get_alarm(&_dev, &t, &match);
    if (res == 0) {
        res = mcp79410_get_alarm_flag(&_dev, &flag);
    }
    if (res != 0) {
        printf("error: reading the alarm failed (%d)\n", res);
        return 1;
    }
    printf("alarm match %u, flag %u, ", match, flag);
    _print_time("time ", &t);
    return 0;
}
SHELL_COMMAND(alarm_get, "read alarm time, match and flag", _cmd_alarm_get);

static int _cmd_alarm_set(int argc, char **argv)
{
    struct tm t;

    if ((argc != 3) || (_tm_from_str(argv[2], &t) != 0)) {
        printf("usage: %s <match 0-4|7> <YYYY-MM-DDTHH:mm:ss>\n", argv[0]);
        puts("  match: 0=sec 1=min 2=hour 3=wday 4=mday 7=all");
        return 1;
    }

    int match = atoi(argv[1]);
    if ((match < 0) || ((match > 4) && (match != 7))) {
        puts("error: invalid match");
        return 1;
    }

    int res = mcp79410_set_alarm(&_dev, &t, (mcp79410_alarm_match_t)match);
    if (res != 0) {
        printf("error: set_alarm failed (%d)\n", res);
        return 1;
    }
    puts("alarm set, enable it with alarm_int 1");
    return 0;
}
SHELL_COMMAND(alarm_set, "set alarm time", _cmd_alarm_set);

static int _cmd_alarm_int(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <0|1>\n", argv[0]);
        return 1;
    }

    bool enable = (argv[1][0] == '1');
    int res = mcp79410_set_alarm_int(&_dev, enable);
    if (res != 0) {
        printf("error: set_alarm_int failed (%d)\n", res);
        return 1;
    }
    printf("alarm %s\n", enable ? "enabled" : "disabled");
    return 0;
}
SHELL_COMMAND(alarm_int, "enable/disable the alarm", _cmd_alarm_int);

static int _cmd_alarm_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    int res = mcp79410_clear_alarm_flag(&_dev);
    if (res != 0) {
        printf("error: clear_alarm_flag failed (%d)\n", res);
        return 1;
    }
    puts("alarm flag cleared");
    return 0;
}
SHELL_COMMAND(alarm_clear, "clear the alarm flag", _cmd_alarm_clear);

static int _cmd_sqw(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <0-3|off>  (0=1Hz 1=4.096kHz 2=8.192kHz 3=32.768kHz)\n",
               argv[0]);
        return 1;
    }

    int res;
    if (strcmp(argv[1], "off") == 0) {
        res = mcp79410_disable_sqw(&_dev);
    }
    else {
        int freq = atoi(argv[1]);
        if ((freq < 0) || (freq > 3)) {
            puts("error: frequency out of range (0-3)");
            return 1;
        }
        res = mcp79410_set_sqw(&_dev, (mcp79410_sqw_freq_t)freq);
    }
    if (res != 0) {
        printf("error: sqw command failed (%d)\n", res);
        return 1;
    }
    puts("ok");
    return 0;
}
SHELL_COMMAND(sqw, "configure the square-wave output", _cmd_sqw);

static int _cmd_power(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s <0|1>  (0=off, 1=on)\n", argv[0]);
        return 1;
    }

    int res = (argv[1][0] == '1') ? mcp79410_poweron(&_dev) : mcp79410_poweroff(&_dev);
    if (res != 0) {
        printf("error: power command failed (%d)\n", res);
        return 1;
    }
    puts("ok");
    return 0;
}
SHELL_COMMAND(power, "oscillator power on/off", _cmd_power);

static int _test_time(const struct tm *ref)
{
    struct tm t;

    _print_time("[test] setting ", ref);
    int res = mcp79410_set_time(&_dev, ref);
    if (res != 0) {
        printf("[test] FAIL: set_time returned %d\n", res);
        return 1;
    }

    ztimer_sleep(ZTIMER_MSEC, 2500);

    res = mcp79410_get_time(&_dev, &t);
    if (res != 0) {
        printf("[test] FAIL: get_time returned %d\n", res);
        return 1;
    }
    _print_time("[test] read back ", &t);

    struct tm earliest = *ref;
    earliest.tm_sec += 2;
    rtc_tm_normalize(&earliest);
    struct tm latest = *ref;
    latest.tm_sec += 3;
    rtc_tm_normalize(&latest);
    if ((rtc_tm_compare(&t, &earliest) < 0) || (rtc_tm_compare(&t, &latest) > 0) ||
        (t.tm_wday != ref->tm_wday + 1)) {
        puts("[test] FAIL: time did not advance into the next day by 2 to 3 s");
        return 1;
    }
    puts("[test] time and day rollover OK");
    return 0;
}

static int _test_alarm(void)
{
    struct tm t;

    int res = mcp79410_get_time(&_dev, &t);
    if (res != 0) {
        printf("[test] FAIL: get_time returned %d\n", res);
        return 1;
    }
    t.tm_sec = (t.tm_sec + 2) % 60;

    res = mcp79410_set_alarm(&_dev, &t, MCP79410_ALARM_MATCH_SEC);
    if (res == 0) {
        res = mcp79410_set_alarm_int(&_dev, true);
    }
    if (res != 0) {
        printf("[test] FAIL: setting the alarm returned %d\n", res);
        return 1;
    }

    bool flag = false;
    for (unsigned i = 0; (res == 0) && (i < 40) && !flag; i++) {
        ztimer_sleep(ZTIMER_MSEC, 100);
        res = mcp79410_get_alarm_flag(&_dev, &flag);
    }
    int res_disable = mcp79410_set_alarm_int(&_dev, false);
    if ((res != 0) || (res_disable != 0)) {
        printf("[test] FAIL: reading or disabling the alarm returned %d, %d\n",
               res, res_disable);
        return 1;
    }
    if (!flag) {
        puts("[test] FAIL: alarm flag not set within 4 s");
        return 1;
    }
    puts("[test] alarm OK");
    return 0;
}

static int _test_sram(void)
{
    uint8_t saved[MCP79410_SRAM_SIZE];
    uint8_t buf[MCP79410_SRAM_SIZE];
    nvram_t *nvram = &_dev.nvram;

    if (nvram->read(nvram, saved, 0, sizeof(saved)) != sizeof(saved)) {
        puts("[test] FAIL: SRAM read");
        return 1;
    }
    for (unsigned i = 0; i < sizeof(buf); i++) {
        buf[i] = i ^ 0x5a;
    }
    bool ok = (nvram->write(nvram, buf, 0, sizeof(buf)) == sizeof(buf));
    memset(buf, 0, sizeof(buf));
    ok = ok && (nvram->read(nvram, buf, 0, sizeof(buf)) == sizeof(buf));
    for (unsigned i = 0; ok && (i < sizeof(buf)); i++) {
        ok = (buf[i] == (uint8_t)(i ^ 0x5a));
    }

    if ((nvram->write(nvram, saved, 0, sizeof(saved)) != sizeof(saved)) ||
        (nvram->read(nvram, buf, 0, sizeof(buf)) != sizeof(buf)) ||
        (memcmp(buf, saved, sizeof(buf)) != 0)) {
        puts("[test] FAIL: SRAM restore");
        return 1;
    }
    if (!ok) {
        puts("[test] FAIL: SRAM write and read back");
        return 1;
    }
    if ((nvram->read(nvram, buf, 60, 8) != -EOVERFLOW) ||
        (nvram->read(nvram, buf, UINT32_MAX, 2) != -EOVERFLOW)) {
        puts("[test] FAIL: SRAM bounds");
        return 1;
    }
    puts("[test] SRAM OK");
    return 0;
}

static int _cmd_test(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (unsigned i = 0; i < ARRAY_SIZE(_ref_times); i++) {
        if (_test_time(&_ref_times[i])) {
            return 1;
        }
    }
    if (_test_alarm() || _test_sram()) {
        return 1;
    }
    puts("[test] SUCCESS");
    return 0;
}
SHELL_COMMAND(test, "run the self-test, sets the time", _cmd_test);

int main(void)
{
    puts("MCP79410 RTC test application\n");

    int res = mcp79410_init(&_dev, &mcp79410_params[0]);
    if (res == -ENODATA) {
        puts("Initialization successful, but the time is invalid");
    }
    else if (res != 0) {
        printf("[Error] Initialization failed: %d\n", res);
        return 1;
    }
    else {
        puts("Initialization successful");
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
