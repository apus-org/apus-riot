/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_hsd-apus
 * @{
 *
 * @file
 * @brief       Board initialization for the APUS board
 *
 * @author      Jakob Jungreuthmayer
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "kernel_defines.h"

void board_init(void)
{
    /* the USB peripheral and the CRS that trims it run on the HSI48 */
    if (IS_USED(MODULE_PERIPH_USBDEV)) {
        RCC->CR2 |= RCC_CR2_HSI48ON;
        while (!(RCC->CR2 & RCC_CR2_HSI48RDY)) {}
    }
}
