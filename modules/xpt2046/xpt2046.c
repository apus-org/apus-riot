/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_xpt2046
 * @{
 *
 * @file
 * @brief       Device driver implementation for the XPT2046 touchscreen controller
 *
 * @author      Jakob Jungreuthmayer
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/spi.h"
#include "ztimer.h"

#include "xpt2046.h"
#include "xpt2046_constants.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define BUS                 (dev->params.spi)
#define CS                  (dev->params.cs)
#define CLK                 (dev->params.clk)
#define INT_PIN             (dev->params.int_pin)

static_assert(CONFIG_XPT2046_SAMPLES >= 1 && CONFIG_XPT2046_SAMPLES <= 255,
              "CONFIG_XPT2046_SAMPLES must be within 1 and 255");
static_assert(CONFIG_XPT2046_Z_THRESHOLD >= 1 &&
              CONFIG_XPT2046_Z_THRESHOLD <= XPT2046_Z_MAX,
              "CONFIG_XPT2046_Z_THRESHOLD must be within 1 and 8190");

/* conversions pull the pen interrupt low, mask it while they run */
static inline void _acquire(const xpt2046_t *dev)
{
    spi_acquire(BUS, CS, SPI_MODE_0, CLK);
    if (dev->int_enabled) {
        gpio_irq_disable(INT_PIN);
    }
}

static inline void _release(const xpt2046_t *dev)
{
    if (dev->int_enabled) {
        gpio_irq_enable(INT_PIN);
    }
    spi_release(BUS);
}

static uint16_t _conv(const xpt2046_t *dev, uint8_t cmd, bool cont)
{
    uint8_t buf[3] = { cmd, 0, 0 };

    spi_transfer_bytes(BUS, CS, cont, buf, buf, sizeof(buf));
    return ((((uint16_t)buf[1] << 8) | buf[2]) >> 3) & XPT2046_RAW_MAX;
}

static void _power_down(const xpt2046_t *dev)
{
    const uint8_t buf[3] = { XPT2046_CMD_POWER_DOWN, 0, 0 };

    spi_transfer_bytes(BUS, CS, false, buf, NULL, sizeof(buf));
}

static uint16_t _average(const xpt2046_t *dev, uint8_t cmd)
{
    uint32_t sum = 0;

    /* the first conversion after switching the channel is not settled */
    _conv(dev, cmd, true);
    for (unsigned i = 0; i < CONFIG_XPT2046_SAMPLES; i++) {
        sum += _conv(dev, cmd, true);
    }
    return sum / CONFIG_XPT2046_SAMPLES;
}

static uint16_t _pressure(const xpt2046_t *dev)
{
    uint16_t z1 = _average(dev, XPT2046_CMD_Z1);
    uint16_t z2 = _average(dev, XPT2046_CMD_Z2);

    return z1 + XPT2046_RAW_MAX - z2;
}

static uint16_t _scale(uint16_t raw, uint16_t min, uint16_t max, uint16_t size)
{
    if (raw <= min) {
        return 0;
    }
    if (raw >= max) {
        return size - 1;
    }
    return ((uint32_t)(raw - min) * size) / ((uint32_t)max - min + 1);
}

static int _probe(const xpt2046_t *dev)
{
    _acquire(dev);
    _conv(dev, XPT2046_CMD_TEMP0, false);
    _release(dev);

    ztimer_sleep(ZTIMER_MSEC, XPT2046_VREF_SETTLE_MS);

    _acquire(dev);
    uint16_t temp = _conv(dev, XPT2046_CMD_TEMP0, true);
    _power_down(dev);
    _release(dev);

    DEBUG("[xpt2046] probe: TEMP0 %u\n", temp);

    if ((temp == 0) || (temp == XPT2046_RAW_MAX)) {
        return -ENODEV;
    }
    return 0;
}

int xpt2046_init(xpt2046_t *dev, const xpt2046_params_t *params,
                 xpt2046_event_cb_t cb, void *arg)
{
    assert(dev && params);
    assert(params->xmax && params->ymax);
    assert(params->raw_xmin < params->raw_xmax);
    assert(params->raw_ymin < params->raw_ymax);
    assert(params->raw_xmax <= XPT2046_RAW_MAX);
    assert(params->raw_ymax <= XPT2046_RAW_MAX);

    dev->params = *params;
    dev->int_enabled = false;

    if (spi_init_cs(BUS, CS) != SPI_OK) {
        DEBUG("[xpt2046] error: unable to configure the chip select pin\n");
        return -EIO;
    }

    if (_probe(dev) != 0) {
        DEBUG("[xpt2046] error: no device found\n");
        return -ENODEV;
    }

    if (gpio_is_valid(INT_PIN)) {
        int ret;

        if (cb) {
            ret = gpio_init_int(INT_PIN, GPIO_IN_PU, GPIO_FALLING, cb, arg);
        }
        else {
            ret = gpio_init(INT_PIN, GPIO_IN_PU);
        }
        if (ret != 0) {
            DEBUG("[xpt2046] error: unable to configure the interrupt pin\n");
            return -EIO;
        }
        dev->int_enabled = (cb != NULL);
    }

    DEBUG("[xpt2046] initialization successful\n");

    return 0;
}

int xpt2046_read_touch_state(const xpt2046_t *dev, xpt2046_touch_state_t *state)
{
    assert(dev && state);

    _acquire(dev);
    uint16_t z = _pressure(dev);
    _power_down(dev);
    _release(dev);

    /* a touch that began during the conversions raised no event */
    bool pen_down = gpio_is_valid(INT_PIN) && !gpio_read(INT_PIN);

    *state = ((z >= CONFIG_XPT2046_Z_THRESHOLD) || pen_down) ? XPT2046_TOUCH_STATE_PRESSED
                                                             : XPT2046_TOUCH_STATE_RELEASED;
    return 0;
}

int xpt2046_read_touch_position(const xpt2046_t *dev, xpt2046_touch_position_t *position)
{
    assert(dev && position);

    _acquire(dev);
    uint16_t z_start = _pressure(dev);
    uint16_t raw_x = _average(dev, XPT2046_CMD_X);
    uint16_t raw_y = _average(dev, XPT2046_CMD_Y);
    uint16_t z_end = _pressure(dev);
    _power_down(dev);
    _release(dev);

    DEBUG("[xpt2046] position: raw X %u, Y %u, Z %u..%u\n", raw_x, raw_y, z_start, z_end);

    if ((z_start < CONFIG_XPT2046_Z_THRESHOLD) || (z_end < CONFIG_XPT2046_Z_THRESHOLD)) {
        return -ENODATA;
    }

    bool swap = dev->params.xyconv & XPT2046_SWAP_XY;
    uint16_t x = _scale(raw_x, dev->params.raw_xmin, dev->params.raw_xmax,
                        swap ? dev->params.ymax : dev->params.xmax);
    uint16_t y = _scale(raw_y, dev->params.raw_ymin, dev->params.raw_ymax,
                        swap ? dev->params.xmax : dev->params.ymax);

    position->x = swap ? y : x;
    position->y = swap ? x : y;

    if (dev->params.xyconv & XPT2046_MIRROR_X) {
        position->x = dev->params.xmax - 1 - position->x;
    }
    if (dev->params.xyconv & XPT2046_MIRROR_Y) {
        position->y = dev->params.ymax - 1 - position->y;
    }

    return 0;
}

int xpt2046_read_raw(const xpt2046_t *dev, xpt2046_raw_t *raw)
{
    assert(dev && raw);

    _acquire(dev);
    raw->z = _pressure(dev);
    raw->x = _average(dev, XPT2046_CMD_X);
    raw->y = _average(dev, XPT2046_CMD_Y);
    _power_down(dev);
    _release(dev);

    return 0;
}
