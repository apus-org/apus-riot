/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_xpt2046
 * @{
 *
 * @file
 * @brief       Driver adaption to touch_dev generic interface
 *
 * @author      Jakob Jungreuthmayer
 * @}
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "periph/gpio.h"

#include "xpt2046.h"
#include "xpt2046_touch_dev.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static uint16_t _xpt2046_height(const touch_dev_t *touch_dev)
{
    const xpt2046_t *dev = (const xpt2046_t *)touch_dev;
    assert(dev);

    return dev->params.ymax;
}

static uint16_t _xpt2046_width(const touch_dev_t *touch_dev)
{
    const xpt2046_t *dev = (const xpt2046_t *)touch_dev;
    assert(dev);

    return dev->params.xmax;
}

static uint8_t _xpt2046_touches(const touch_dev_t *touch_dev, touch_t *touches, size_t len)
{
    const xpt2046_t *dev = (const xpt2046_t *)touch_dev;
    assert(dev);

    if (touches == NULL) {
        xpt2046_touch_state_t state;
        xpt2046_read_touch_state(dev, &state);
        return state == XPT2046_TOUCH_STATE_PRESSED;
    }

    if (len == 0) {
        return 0;
    }

    xpt2046_touch_position_t pos;
    if (xpt2046_read_touch_position(dev, &pos) != 0) {
        return 0;
    }

    touches[0].x = pos.x;
    touches[0].y = pos.y;

    DEBUG("X: %i, Y: %i\n", touches[0].x, touches[0].y);

    return 1;
}

static void _xpt2046_set_event_callback(const touch_dev_t *touch_dev, touch_event_cb_t cb,
                                        void *arg)
{
    xpt2046_t *dev = (xpt2046_t *)touch_dev;
    assert(dev);

    if (gpio_is_valid(dev->params.int_pin)) {
        dev->int_enabled = gpio_init_int(dev->params.int_pin, GPIO_IN_PU, GPIO_FALLING,
                                         cb, arg) == 0;
    }
}

const touch_dev_driver_t xpt2046_touch_dev_driver = {
    .height     = _xpt2046_height,
    .width      = _xpt2046_width,
    .touches    = _xpt2046_touches,
    .set_event_callback = _xpt2046_set_event_callback,
};
