/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_xpt2046
 * @{
 *
 * @file
 * @brief       Definition of the driver for the touch_dev generic interface
 *
 * @author      Jakob Jungreuthmayer
 */

#include "touch_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the touch device driver struct
 */
extern const touch_dev_driver_t xpt2046_touch_dev_driver;

#ifdef __cplusplus
}
#endif

/** @} */
