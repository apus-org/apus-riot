/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_xpt2046 XPT2046 Touchscreen Controller
 * @ingroup     drivers_sensors
 * @brief       Device driver interface for the XPT2046 resistive touchscreen controller
 *
 * The XPT2046 (and the compatible ADS7846) measures the position of a
 * 4-wire resistive touch panel over SPI. It has no registers: every
 * conversion is started by a control byte and its 12-bit result is clocked
 * out in the following 16 clock cycles.
 *
 * Positions are averaged over @ref CONFIG_XPT2046_SAMPLES conversions and
 * mapped from the calibrated raw reading ranges
 * (@ref xpt2046_params_t::raw_xmin to @ref xpt2046_params_t::raw_xmax and
 * @ref xpt2046_params_t::raw_ymin to @ref xpt2046_params_t::raw_ymax)
 * to screen coordinates. A touch is detected by the pressure score, so the
 * pen interrupt pin is optional and only needed for the event callback.
 *
 * The event callback is called on pen down. Conversions pull the pen
 * interrupt low, so the driver masks it while they run: a touch that begins
 * during a conversion raises no event, but @ref xpt2046_read_touch_state
 * reports it if the panel is still pressed afterwards. The application is expected to poll the device after an event
 * until the panel is released.
 *
 * @{
 *
 * @file
 *
 * @author      Jakob Jungreuthmayer
 */

#include <stdbool.h>
#include <stdint.h>

#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef MODULE_TOUCH_DEV
#include "touch_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_xpt2046_config     XPT2046 driver compile configuration
 * @ingroup config_drivers_sensors
 * @{
 */
/**
 * @brief   Number of conversions averaged for one value, 1 to 255
 */
#ifndef CONFIG_XPT2046_SAMPLES
#define CONFIG_XPT2046_SAMPLES          (4U)
#endif

/**
 * @brief   Minimum pressure score that counts as a touch, 1 to 8190
 *
 * See @ref xpt2046_raw_t::z.
 */
#ifndef CONFIG_XPT2046_Z_THRESHOLD
#define CONFIG_XPT2046_Z_THRESHOLD      (100U)
#endif
/** @} */

/**
 * @brief   Touch state enum
 */
typedef enum {
    XPT2046_TOUCH_STATE_PRESSED,        /**< Touchscreen is pressed */
    XPT2046_TOUCH_STATE_RELEASED,       /**< Touchscreen is released */
} xpt2046_touch_state_t;

/**
 * @brief   Touch screen coordinate conversions
 *
 * The flags can be ORed for a combined conversion. Swapping is performed
 * before mirroring.
 *
 * @note    @ref xpt2046_params_t::xmax and @ref xpt2046_params_t::ymax define
 *          the dimension of the touch device in screen coordinates, i.e.
 *          after conversion.
 */
typedef enum {
    XPT2046_NO_CONV  = 0x00,    /**< No conversion */
    XPT2046_MIRROR_X = 0x01,    /**< Mirror X, applied after optional swapping */
    XPT2046_MIRROR_Y = 0x02,    /**< Mirror Y, applied after optional swapping */
    XPT2046_SWAP_XY  = 0x04,    /**< Swap XY, applied before optional mirroring */
} xpt2046_touch_conv_t;

/**
 * @brief   Touch position structure
 */
typedef struct {
    uint16_t x;                         /**< X position */
    uint16_t y;                         /**< Y position */
} xpt2046_touch_position_t;

/**
 * @brief   Raw conversion results
 */
typedef struct {
    uint16_t x;                         /**< X reading, 0 to 4095 */
    uint16_t y;                         /**< Y reading, 0 to 4095 */
    /**
     * @brief   Pressure score `Z1 + 4095 - Z2`, 0 to 8190
     *
     * The score rises with the force applied to the panel. It depends on
     * the panel and is no calibrated pressure. `x` and `y` are only valid
     * if the score is at least @ref CONFIG_XPT2046_Z_THRESHOLD.
     */
    uint16_t z;
} xpt2046_raw_t;

/**
 * @brief   Signature of touch event callback triggered from interrupt
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*xpt2046_event_cb_t)(void *arg);

/**
 * @brief   Device initialization parameters
 *
 * The raw bounds are the lowest and highest raw readings of each axis in
 * device coordinates, i.e. before conversion, with `raw_xmin < raw_xmax`
 * and `raw_ymin < raw_ymax`, at most 4095. A panel whose readings decrease towards the
 * screen origin is handled by mirroring.
 */
typedef struct {
    spi_t spi;                          /**< SPI bus */
    spi_clk_t clk;                      /**< SPI clock speed, at most 2 MHz */
    gpio_t cs;                          /**< Chip select pin */
    gpio_t int_pin;                     /**< Pen interrupt pin, or GPIO_UNDEF */
    uint16_t xmax;                      /**< Touch screen width in screen coordinates */
    uint16_t ymax;                      /**< Touch screen height in screen coordinates */
    uint16_t raw_xmin;                  /**< Lowest raw X reading */
    uint16_t raw_xmax;                  /**< Highest raw X reading */
    uint16_t raw_ymin;                  /**< Lowest raw Y reading */
    uint16_t raw_ymax;                  /**< Highest raw Y reading */
    xpt2046_touch_conv_t xyconv;        /**< Touch screen coordinates conversion,
                                             swapping is applied before mirroring */
} xpt2046_params_t;

/**
 * @brief   Device descriptor for the XPT2046
 */
typedef struct {
#ifdef MODULE_TOUCH_DEV
    touch_dev_t *dev;                   /**< Pointer to the generic touch device */
#endif
    xpt2046_params_t params;            /**< Device parameters */
    bool int_enabled;                   /**< Pen interrupt configured with a callback */
} xpt2046_t;

/**
 * @brief   Initialize the given XPT2046 device
 *
 * The device is detected by a conversion of the internal temperature
 * sensor. This is a best-effort check: SPI has no acknowledge, so only a
 * stuck or unconnected data line is detected.
 *
 * @param[out] dev          Device descriptor of the XPT2046
 * @param[in]  params       Initialization parameters of the XPT2046 device
 * @param[in]  cb           Callback function called on pen down, or NULL
 * @param[in]  arg          Context argument used in callback function
 *
 * @retval  0               on success
 * @retval  -EIO            when the chip select or interrupt pin cannot be configured
 * @retval  -ENODEV         when no device answers
 */
int xpt2046_init(xpt2046_t *dev, const xpt2046_params_t *params,
                 xpt2046_event_cb_t cb, void *arg);

/**
 * @brief   Read the touch state (pressed or released)
 *
 * The panel counts as pressed if the pressure score reaches
 * @ref CONFIG_XPT2046_Z_THRESHOLD, or if the pen interrupt pin signals a
 * touch after the conversions.
 *
 * @param[in]  dev          Device descriptor of the XPT2046
 * @param[out] state        Touch state
 *
 * @retval  0               on success
 */
int xpt2046_read_touch_state(const xpt2046_t *dev, xpt2046_touch_state_t *state);

/**
 * @brief   Read the touch position in screen coordinates
 *
 * @param[in]  dev          Device descriptor of the XPT2046
 * @param[out] position     Touch position, `x < xmax` and `y < ymax`
 *
 * @retval  0               on success
 * @retval  -ENODATA        when the screen is not pressed during the whole measurement
 */
int xpt2046_read_touch_position(const xpt2046_t *dev, xpt2046_touch_position_t *position);

/**
 * @brief   Read the averaged raw conversion results
 *
 * Intended for calibration: the lowest and highest readings over the panel
 * give the @ref xpt2046_params_t::raw_xmin, @ref xpt2046_params_t::raw_xmax,
 * @ref xpt2046_params_t::raw_ymin and @ref xpt2046_params_t::raw_ymax values.
 *
 * @param[in]  dev          Device descriptor of the XPT2046
 * @param[out] raw          Raw results
 *
 * @retval  0               on success
 */
int xpt2046_read_raw(const xpt2046_t *dev, xpt2046_raw_t *raw);

#ifdef __cplusplus
}
#endif

/** @} */
