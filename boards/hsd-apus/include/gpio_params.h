/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_hsd-apus
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Jakob Jungreuthmayer
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED3",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED4",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED5",
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "BUTTON0",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE
    },
    {
        .name = "BUTTON1",
        .pin  = BTN1_PIN,
        .mode = BTN1_MODE
    },
    {
        .name = "WAKE_UP",
        .pin  = BTN2_PIN,
        .mode = BTN2_MODE
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
