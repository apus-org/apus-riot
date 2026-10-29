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
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * The SDA/SCL pins of the header share their nets with A4/A5, which the solder
 * jumpers SJ1/SJ2 connect to PB0/PB1 by default, so no I2C bus is mapped.
 *
 * @author      Jakob Jungreuthmayer
 */

#include "periph/gpio.h"
#include "periph/adc.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Arduino's UART devices
 * @{
 */
#define ARDUINO_UART_D0D1       UART_DEV(1)
/** @} */

/**
 * @name    Arduino's SPI buses
 * @{
 */
#define ARDUINO_SPI_D11D12D13   SPI_DEV(1)
/** @} */

/**
 * @brief   Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_A, 10)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_A, 9)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_B, 5)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_B, 2)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_A, 8)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_A, 7)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_A, 3)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_A, 5)
#define ARDUINO_PIN_8           GPIO_PIN(PORT_A, 15)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_B, 3)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_B, 12)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_B, 15)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_B, 14)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_B, 13)
#define ARDUINO_PIN_14          GPIO_PIN(PORT_B, 0)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_B, 1)
/* analog pins as digital pins: */
#define ARDUINO_PIN_16          GPIO_PIN(PORT_A, 1)
#define ARDUINO_PIN_17          GPIO_PIN(PORT_A, 2)
#define ARDUINO_PIN_18          GPIO_PIN(PORT_A, 4)
#define ARDUINO_PIN_19          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_20          GPIO_PIN(PORT_B, 0)
#define ARDUINO_PIN_21          GPIO_PIN(PORT_B, 1)

#define ARDUINO_PIN_LAST        21
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_16
#define ARDUINO_PIN_A1          ARDUINO_PIN_17
#define ARDUINO_PIN_A2          ARDUINO_PIN_18
#define ARDUINO_PIN_A3          ARDUINO_PIN_19
#define ARDUINO_PIN_A4          ARDUINO_PIN_20
#define ARDUINO_PIN_A5          ARDUINO_PIN_21
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(1)
#define ARDUINO_A1              ADC_LINE(2)
#define ARDUINO_A2              ADC_LINE(3)
#define ARDUINO_A3              ADC_LINE(4)
#define ARDUINO_A4              ADC_LINE(5)
#define ARDUINO_A5              ADC_LINE(6)

#define ARDUINO_ANALOG_PIN_LAST 5
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
