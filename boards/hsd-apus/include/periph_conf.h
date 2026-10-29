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
 * @brief       Peripheral MCU configuration for the APUS board
 *
 * @author      Jakob Jungreuthmayer
 */

/* HSE available on this board */
#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE        1
#endif

#include "periph_cpu.h"
#include "clk_conf.h"
#include "cfg_i2c1_pb6_pb7.h"
#include "cfg_timer_tim2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = USART3,
        .rcc_mask = RCC_APB1ENR_USART3EN,
        .rx_pin   = GPIO_PIN(PORT_D, 9),
        .tx_pin   = GPIO_PIN(PORT_D, 8),
        .rx_af    = GPIO_AF0,
        .tx_af    = GPIO_AF0,
        .bus      = APB1,
        .irqn     = USART3_4_IRQn
    },
    {
        .dev      = USART1,
        .rcc_mask = RCC_APB2ENR_USART1EN,
        .rx_pin   = GPIO_PIN(PORT_A, 10),
        .tx_pin   = GPIO_PIN(PORT_A, 9),
        .rx_af    = GPIO_AF1,
        .tx_af    = GPIO_AF1,
        .bus      = APB2,
        .irqn     = USART1_IRQn
    },
};

#define UART_0_ISR          (isr_usart3_4)
#define UART_1_ISR          (isr_usart1)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_C, 6) /* red */,   .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_C, 7) /* green */, .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_C, 8) /* blue */,  .cc_chan = 2 },
                      { .pin = GPIO_UNDEF,                      .cc_chan = 0 } },
        .af       = GPIO_AF0,
        .bus      = APB1
    },
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .mosi_pin = GPIO_PIN(PORT_E, 15),
        .miso_pin = GPIO_PIN(PORT_E, 14),
        .sclk_pin = GPIO_PIN(PORT_E, 13),
        .cs_pin   = GPIO_PIN(PORT_E, 12),   /* touch controller */
        .mosi_af  = GPIO_AF1,
        .miso_af  = GPIO_AF1,
        .sclk_af  = GPIO_AF1,
        .cs_af    = GPIO_AF1,
        .rccmask  = RCC_APB2ENR_SPI1EN,
        .apbbus   = APB2
    },
    {
        .dev      = SPI2,
        .mosi_pin = GPIO_PIN(PORT_B, 15),   /* D11 */
        .miso_pin = GPIO_PIN(PORT_B, 14),   /* D12 */
        .sclk_pin = GPIO_PIN(PORT_B, 13),   /* D13 */
        .cs_pin   = GPIO_PIN(PORT_B, 12),   /* D10 */
        .mosi_af  = GPIO_AF0,
        .miso_af  = GPIO_AF0,
        .sclk_af  = GPIO_AF0,
        .cs_af    = GPIO_AF0,
        .rccmask  = RCC_APB1ENR_SPI2EN,
        .apbbus   = APB1
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name   ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { GPIO_PIN(PORT_C, 4), 14 },    /* potentiometer */
    { GPIO_PIN(PORT_A, 1), 1 },     /* A0 */
    { GPIO_PIN(PORT_A, 2), 2 },     /* A1 */
    { GPIO_PIN(PORT_A, 4), 4 },     /* A2 */
    { GPIO_PIN(PORT_A, 6), 6 },     /* A3 */
    { GPIO_PIN(PORT_B, 0), 8 },     /* A4 */
    { GPIO_PIN(PORT_B, 1), 9 },     /* A5 */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name   DAC configuration
 * @{
 */
static const dac_conf_t dac_config[] = {
    { .pin = GPIO_PIN(PORT_A, 4), .chan = 0 },  /* A2 */
    { .pin = GPIO_PIN(PORT_A, 5), .chan = 1 },  /* D7 */
};

#define DAC_NUMOF           ARRAY_SIZE(dac_config)
/** @} */

/**
 * @name   USB device FS configuration
 * @{
 */
static const stm32_usbdev_fs_config_t stm32_usbdev_fs_config[] = {
    {
        .base_addr  = (uintptr_t)USB,
        .rcc_mask   = RCC_APB1ENR_USBEN | RCC_APB1ENR_CRSEN,
        .dm         = GPIO_PIN(PORT_A, 11),
        .dp         = GPIO_PIN(PORT_A, 12),
        .af         = GPIO_AF_UNDEF,
        .disconn    = GPIO_UNDEF,
        .irqn       = USB_IRQn,
        .apb        = APB1,
    },
};

#define USBDEV_ISR          isr_usb

#define USBDEV_NUMOF        ARRAY_SIZE(stm32_usbdev_fs_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
