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
 * @brief       CAN controller (bxCAN) parameters for the APUS board
 *
 * @author      Jakob Jungreuthmayer
 */

#include "can/device.h"
#include "periph/can.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CAN device configuration
 */
static const can_conf_t candev_conf[] = {
    {
        .can = CAN,
        .rcc_mask = RCC_APB1ENR_CANEN,
        .rx_pin = GPIO_PIN(PORT_B, 8),
        .tx_pin = GPIO_PIN(PORT_B, 9),
        .af = GPIO_AF4,
        .irqn = CEC_CAN_IRQn,
        .en_deep_sleep_wake_up = true,
        .ttcm = 0,
        .abom = 1,
        .awum = 1,
        .nart = 0,
        .rflm = 0,
        .txfp = 0,
    },
};

/**
 * @brief   CAN device parameters
 */
static const candev_params_t candev_params[] = {
    {
        .name = "can_stm32_0",
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
