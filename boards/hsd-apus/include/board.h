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
 * @brief       Board specific definitions for the APUS board
 *
 * @author      Jakob Jungreuthmayer
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        0
#define LED0_PORT           GPIO_PORT_C /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_C
#define LED0_IS_INVERTED    1

#define LED1_PIN_NUM        1
#define LED1_PORT           GPIO_PORT_C /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_C
#define LED1_IS_INVERTED    1

#define LED2_PIN_NUM        2
#define LED2_PORT           GPIO_PORT_C /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_C
#define LED2_IS_INVERTED    1
/** @} */

/**
 * @name    User buttons
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_D, 0)
#define BTN0_MODE           GPIO_IN_PD

#define BTN1_PIN            GPIO_PIN(PORT_D, 1)
#define BTN1_MODE           GPIO_IN_PD

#define BTN2_PIN            GPIO_PIN(PORT_A, 0)
#define BTN2_MODE           GPIO_IN_PD
/** @} */

/**
 * @name    Buzzer (active, active low)
 * @{
 */
#define BUZZER_PIN          GPIO_PIN(PORT_C, 9)
/** @} */

/**
 * @name    SN65HVD230 CAN transceiver
 *
 * Rs high is standby, like the STB pin of the TJA1042, so the tja1042
 * driver can control it.
 * @{
 */
#define CAN_STBY_PIN        GPIO_PIN(PORT_C, 13)
/** @} */

/**
 * @name    MCP9803 temperature sensor (LM75 compatible)
 * @{
 */
#define LM75_PARAM_I2C          I2C_DEV(0)
#define CONFIG_LM75_I2C_ADDR    (0x48)
#define LM75_PARAM_INT          GPIO_PIN(PORT_D, 4)
/** @} */

/**
 * @name    MCP79410 EEPROM (AT24C01A compatible)
 * @{
 */
#define AT24CXXX_PARAM_I2C          I2C_DEV(0)
#define AT24CXXX_PARAM_ADDR         (0x57)
#define AT24CXXX_PARAM_EEPROM_SIZE  (AT24C01A_EEPROM_SIZE)
#define AT24CXXX_PARAM_PAGE_SIZE    (AT24C01A_PAGE_SIZE)
#define AT24CXXX_PARAM_MAX_POLLS    (AT24C01A_MAX_POLLS)
/** @} */

/**
 * @name    MCP79410 RTC, alarm and square-wave output (MFP) on PD3
 * @{
 */
#define MCP79410_PARAM_I2C          I2C_DEV(0)
#define MCP79410_PARAM_VBAT_ENABLE  true
/** @} */

/**
 * @name    LCD display (ST7789, MCU 8080 8-bit parallel interface)
 * @{
 */
#define LCD_SCREEN_WIDTH    240                     /**< LCD width */
#define LCD_SCREEN_HEIGHT   320                     /**< LCD height */

#define LCD_D0              GPIO_PIN(PORT_E, 0)     /**< LCD_D0 pin */
#define LCD_D1              GPIO_PIN(PORT_E, 1)     /**< LCD_D1 pin */
#define LCD_D2              GPIO_PIN(PORT_E, 2)     /**< LCD_D2 pin */
#define LCD_D3              GPIO_PIN(PORT_E, 3)     /**< LCD_D3 pin */
#define LCD_D4              GPIO_PIN(PORT_E, 4)     /**< LCD_D4 pin */
#define LCD_D5              GPIO_PIN(PORT_E, 5)     /**< LCD_D5 pin */
#define LCD_D6              GPIO_PIN(PORT_E, 6)     /**< LCD_D6 pin */
#define LCD_D7              GPIO_PIN(PORT_E, 7)     /**< LCD_D7 pin */
#define LCD_CS              GPIO_PIN(PORT_E, 8)     /**< LCD_CS pin */
#define LCD_DC              GPIO_PIN(PORT_E, 9)     /**< LCD_DC pin */
#define LCD_WR              GPIO_PIN(PORT_E, 10)    /**< LCD_WR pin */
#define LCD_RD              GPIO_PIN(PORT_E, 11)    /**< LCD_RD pin */
#define LCD_RST             GPIO_PIN(PORT_F, 10)    /**< LCD_RST pin */

#define ST77XX_PARAM_CNTRL         ST77XX_CNTRL_ST7789  /**< ST77xx controller variant */
#define ST77XX_PARAM_SPI           SPI_UNDEF        /**< parallel interface is used */
#define ST77XX_PARAM_D0            LCD_D0           /**< ST77xx D0 signal */
#define ST77XX_PARAM_D1            LCD_D1           /**< ST77xx D1 signal */
#define ST77XX_PARAM_D2            LCD_D2           /**< ST77xx D2 signal */
#define ST77XX_PARAM_D3            LCD_D3           /**< ST77xx D3 signal */
#define ST77XX_PARAM_D4            LCD_D4           /**< ST77xx D4 signal */
#define ST77XX_PARAM_D5            LCD_D5           /**< ST77xx D5 signal */
#define ST77XX_PARAM_D6            LCD_D6           /**< ST77xx D6 signal */
#define ST77XX_PARAM_D7            LCD_D7           /**< ST77xx D7 signal */
#define ST77XX_PARAM_WRX           LCD_WR           /**< ST77xx WR signal */
#define ST77XX_PARAM_RDX           LCD_RD           /**< ST77xx RD signal */
#define ST77XX_PARAM_DCX           LCD_DC           /**< ST77xx DC signal */
#define ST77XX_PARAM_RST           LCD_RST          /**< ST77xx RST signal */
#define ST77XX_PARAM_CS            LCD_CS           /**< ST77xx CS signal */
#define ST77XX_PARAM_RGB           1                /**< ST77xx RGB mode */
#define ST77XX_PARAM_INVERTED      0                /**< ST77xx inverted colors */
#define ST77XX_PARAM_NUM_LINES     LCD_SCREEN_HEIGHT    /**< ST77xx number of lines */
#define ST77XX_PARAM_RGB_CHANNELS  LCD_SCREEN_WIDTH     /**< ST77xx number of columns */
#define ST77XX_PARAM_ROTATION      ST77XX_ROTATION_0    /**< ST77xx rotation */
/** @} */

/**
 * @name    XPT2046 touch controller
 * @{
 */
#define XPT2046_PARAM_SPI_DEV       SPI_DEV(0)
#define XPT2046_PARAM_CLK           SPI_CLK_400KHZ
#define XPT2046_PARAM_CS            GPIO_PIN(PORT_E, 12)
#define XPT2046_PARAM_INT_PIN       GPIO_PIN(PORT_F, 2)
#define XPT2046_PARAM_XMAX          LCD_SCREEN_WIDTH
#define XPT2046_PARAM_YMAX          LCD_SCREEN_HEIGHT
#define XPT2046_PARAM_RAW_XMIN      (202U)
#define XPT2046_PARAM_RAW_XMAX      (3820U)
#define XPT2046_PARAM_RAW_YMIN      (264U)
#define XPT2046_PARAM_RAW_YMAX      (3837U)
#define XPT2046_PARAM_XYCONV        (XPT2046_MIRROR_Y)
/** @} */

/**
 * @name    ztimer configuration values
 * @{
 */
#define CONFIG_ZTIMER_USEC_ADJUST_SET     10
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   19
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
