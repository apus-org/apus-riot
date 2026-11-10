/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the XPT2046 touchscreen controller
 *
 * @author      Jakob Jungreuthmayer
 *
 * @}
 */

#include <stdio.h>

#include "mutex.h"
#include "ztimer.h"

#include "xpt2046.h"
#include "xpt2046_params.h"

#ifndef XPT2046_POLLING_PERIOD
#define XPT2046_POLLING_PERIOD   50
#endif

#if !IS_ACTIVE(XPT2046_POLLING_MODE)
static void _touch_event_cb(void *arg)
{
    mutex_unlock(arg);
}
#endif

int main(void)
{
#if !IS_ACTIVE(XPT2046_POLLING_MODE)
    mutex_t lock = MUTEX_INIT_LOCKED;
#endif
    xpt2046_t dev;

    puts("XPT2046 test application\n");

    printf("+------------Initializing------------+\n");
#if IS_ACTIVE(XPT2046_POLLING_MODE)
    int ret = xpt2046_init(&dev, &xpt2046_params[0], NULL, NULL);
#else
    int ret = xpt2046_init(&dev, &xpt2046_params[0], _touch_event_cb, &lock);
#endif
    if (ret != 0) {
        printf("[Error] Initialization failed: %d\n", ret);
        return 1;
    }

    puts("Initialization successful");

    while (1) {
#if !IS_ACTIVE(XPT2046_POLLING_MODE)
        /* wait for touch event */
        mutex_lock(&lock);
#endif

        xpt2046_touch_state_t state;
        xpt2046_read_touch_state(&dev, &state);

        if (state == XPT2046_TOUCH_STATE_PRESSED) {
            puts("Pressed!");

            while (state == XPT2046_TOUCH_STATE_PRESSED) {
                xpt2046_touch_position_t position;
                xpt2046_raw_t raw;

                if (xpt2046_read_touch_position(&dev, &position) == 0) {
                    xpt2046_read_raw(&dev, &raw);
                    printf("X: %u, Y: %u (raw X: %u, Y: %u, Z: %u)\n",
                           position.x, position.y, raw.x, raw.y, raw.z);
                }
                ztimer_sleep(ZTIMER_MSEC, XPT2046_POLLING_PERIOD);
                xpt2046_read_touch_state(&dev, &state);
            }

            puts("Released!");
        }

#if IS_ACTIVE(XPT2046_POLLING_MODE)
        ztimer_sleep(ZTIMER_MSEC, XPT2046_POLLING_PERIOD);
#endif
    }

    return 0;
}
