/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_hsd-apus
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped PWM channels
 *
 * @author      Jakob Jungreuthmayer
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    No individual LED that needs dimming
 */
#define SAUL_PWM_NO_DIMMER

/**
 * @brief    The on-board RGB LED (common anode)
 */
static const saul_pwm_rgb_params_t saul_pwm_rgb_params[] =
{
    {
        .name  = "RGB",
        .channels = {
            { PWM_DEV(0), 0, SAUL_PWM_INVERTED },
            { PWM_DEV(0), 1, SAUL_PWM_INVERTED },
            { PWM_DEV(0), 2, SAUL_PWM_INVERTED }
        }
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
