@defgroup   boards_hsd-apus HSD APUS
@ingroup    boards
@brief      Support for the HSD APUS

## Overview

APUS is a development board built around the STM32F072VBT6, with three LEDs,
an RGB LED, two user buttons, a buzzer, a potentiometer, a temperature sensor,
an RTC with EEPROM, a 240x320 display with resistive touch, CAN, USB and an
Arduino Uno header.

### MCU

| MCU                | STM32F072VBT6                                                                     |
| :----------------- | :-------------------------------------------------------------------------------- |
| Family             | ARM Cortex-M0                                                                     |
| Vendor             | ST Microelectronics                                                               |
| RAM                | 16KiB                                                                             |
| Flash              | 128KiB                                                                            |
| Frequency          | 48MHz (8MHz HSE crystal)                                                          |
| Datasheet          | [Datasheet](https://www.st.com/resource/en/datasheet/stm32f072vb.pdf)             |
| Reference Manual   | [Reference Manual](https://www.st.com/resource/en/reference_manual/dm00031936.pdf) |

## Implementation Status

| Device           | ID          | Supported   | Comments                                        |
| :--------------- | :---------- | :---------- | :---------------------------------------------- |
| MCU              | STM32F072VB | partly      | Energy saving modes not fully utilized          |
| Low-level driver | GPIO        | yes         | LED3–LED5 as LED0–LED2, buttons as BTN0–BTN2    |
|                  | ADC         | 7 lines     | potentiometer, A0–A5                            |
|                  | CAN         | yes         | SN65HVD230, standby on `CAN_STBY_PIN`           |
|                  | DAC         | 2 lines     | A2, D7                                          |
|                  | I2C         | 1 I2C       | MCP9803 (`lm75a`), MCP79410 EEPROM (`at24cxxx`) |
|                  | PWM         | 3 channels  | RGB LED on TIM3                                 |
|                  | SPI         | 2 SPIs      | SPI1 touch controller, SPI2 on D10–D13          |
|                  | Timer       | 1 timer     | TIM2, 32 bit                                    |
|                  | UART        | 2 UARTs     | USART3 on the VCOM, USART1 on D0/D1             |
|                  | USB         | yes         | USB FS device, clocked by the HSI48             |
| Display          | ST7789      | yes         | 240x320, MCU 8080 8-bit parallel (`st7789`)     |
| Buzzer           | GPIO        | yes         | active buzzer on `BUZZER_PIN`, active low       |

USART4 on the UART connector shares its interrupt with USART3, which RIOT's
UART driver does not support, so it is not configured. RIOT has no drivers for
the MCP79410 RTC and the XPT2046 touch controller.

USB needs support for the STM32F0 in RIOT's `usbdev_fs` driver, which is not
upstream yet.

The Arduino header provides `arduino_pins`, `arduino_analog`, `arduino_spi`
and `arduino_uart`. Its SDA/SCL pins share their nets with A4/A5, so no I2C bus
is mapped.

## Flashing the Board

The board is flashed with OpenOCD through the on-board ST-Link:

```
make BOARD=hsd-apus EXTERNAL_BOARD_DIRS=<apus-riot>/boards flash term
```

The console is USART3 at 115200 baud on the on-board FTDI USB to serial
converter.
