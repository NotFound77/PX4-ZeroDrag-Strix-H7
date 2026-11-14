/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file board_config.h
 *
 * ZeroDrag Strix H7 R0.2 internal definitions
 */

#pragma once

/****************************************************************************************************
 * Included Files
 ****************************************************************************************************/

#include <px4_platform_common/px4_config.h>
#include <nuttx/compiler.h>
#include <stdint.h>

#include <stm32_gpio.h>

/****************************************************************************************************
 * Definitions
 ****************************************************************************************************/

/* LEDs:
 *   PE3 = Blue LED (active low)
 *   PE4 = Green LED (active low)
 */
#define GPIO_nLED_BLUE       /* PE3 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN3)
#define GPIO_nLED_GREEN      /* PE4 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN4)

#define BOARD_HAS_CONTROL_STATUS_LEDS   1
#define BOARD_OVERLOAD_LED              LED_BLUE
#define BOARD_ARMED_STATE_LED           LED_GREEN

/*
 * ADC channels (matching ZeroDrag Strix H7 hwdef.dat):
 *   PC0: BATT1 Voltage    -> ADC123_INP10 (channel 10)
 *   PC1: BATT1 Current    -> ADC123_INP11 (channel 11)
 *   PA4: BATT2 Voltage    -> ADC12_INP18  (channel 18)
 *   PA7: BATT2 Current    -> ADC12_INP7   (channel 7)
 *   PC4: Pressure/Airspeed-> ADC12_INP4   (channel 4)
 *   PC5: RSSI Analog      -> ADC12_INP8   (channel 8)
 */
#define ADC1_CH(n)                  (n)

/* Define GPIO pins used as ADC */
#define PX4_ADC_GPIO  \
	/* PC0 */  GPIO_ADC123_INP10, \
	/* PC1 */  GPIO_ADC123_INP11, \
	/* PA4 */  GPIO_ADC12_INP18,  \
	/* PA7 */  GPIO_ADC12_INP7,   \
	/* PC4 */  GPIO_ADC12_INP4,   \
	/* PC5 */  GPIO_ADC12_INP8

/* Channel numbers matching GPIO pins */
#define ADC_BATTERY_VOLTAGE_CHANNEL     /* PC0 */  ADC1_CH(10)
#define ADC_BATTERY_CURRENT_CHANNEL     /* PC1 */  ADC1_CH(11)
#define ADC_BATTERY1_VOLTAGE_CHANNEL    /* PC0 */  ADC1_CH(10)
#define ADC_BATTERY1_CURRENT_CHANNEL    /* PC1 */  ADC1_CH(11)
#define ADC_BATTERY2_VOLTAGE_CHANNEL    /* PA4 */  ADC1_CH(18)
#define ADC_BATTERY2_CURRENT_CHANNEL    /* PA7 */  ADC1_CH(7)
#define ADC_AIRSPEED_VOLTAGE_CHANNEL    /* PC4 */  ADC1_CH(4)
#define ADC_RC_RSSI_CHANNEL             /* PC5 */  ADC1_CH(8)

#define ADC_CHANNELS \
	((1 << ADC_BATTERY_VOLTAGE_CHANNEL)  | \
	 (1 << ADC_BATTERY_CURRENT_CHANNEL)  | \
	 (1 << ADC_BATTERY2_VOLTAGE_CHANNEL) | \
	 (1 << ADC_BATTERY2_CURRENT_CHANNEL) | \
	 (1 << ADC_AIRSPEED_VOLTAGE_CHANNEL) | \
	 (1 << ADC_RC_RSSI_CHANNEL))

/* Battery monitoring: 2 bricks supported */
#define BOARD_NUMBER_BRICKS                 2
#define BOARD_ADC_BRICK1_VALID              (true)
#define BOARD_ADC_BRICK2_VALID              (true)

/* Nominal battery voltage divider and A/V scaling (uncalibrated hardware defaults) */
#define BOARD_BATTERY1_V_DIV         (11.0f)
#define BOARD_BATTERY1_A_PER_V       (40.0f)

#define BOARD_BATTERY2_V_DIV         (11.0f)
/* Note: Battery 2 current scaling requires field calibration; uncalibrated in hwdef */

/* PWM / Timer configuration:
 * S1..S2:   TIM3 CH3/CH4 (PB0, PB1) - DShot / PWM
 * S3..S6:   TIM5 CH1..CH4 (PA0..PA3) - DShot / PWM
 * S7..S10:  TIM4 CH1..CH4 (PD12..PD15) - DShot / PWM
 * S11..S12: TIM15 CH1/CH2 (PE5, PE6) - PWM ONLY
 */
#define DIRECT_PWM_OUTPUT_CHANNELS   12
#define BOARD_NUM_IO_TIMERS          4

/* High-resolution timer (TIM8 CH1 on TIM8, reserved exclusively for HRT) */
#define HRT_TIMER                    8
#define HRT_TIMER_CHANNEL            1

/* USB connection status: board has no dedicated VBUS sense pin */
#define BOARD_ADC_USB_CONNECTED      (1)
#define BOARD_ADC_USB_VALID          BOARD_ADC_USB_CONNECTED

/* Serial Port Mapping:
 *   USART1  /dev/ttyS0  PA9/PA10    TEL2
 *   USART2  /dev/ttyS1  PD5/PD6     GPS1
 *   USART3  /dev/ttyS2  PD8/PD9     TEL3
 *   UART4   /dev/ttyS3  PB9/PB8     Aux UART
 *   USART6  /dev/ttyS4  PC6/PC7     RC (Serial RC)
 *   UART7   /dev/ttyS5  PE8/PE7     TEL1 (with RTS PE9, CTS PE10)
 *   UART8   /dev/ttyS6  PE1/PE0     TEL4
 */

/* RC Serial port - USART6 (PC6/PC7) */
#define RC_SERIAL_PORT               "/dev/ttyS4"
#define BOARD_SUPPORTS_RC_SERIAL_PORT_OUTPUT

/* SD Card configuration */
#define SDIO_SLOTNO                  0  /* Only one slot */
#define SDIO_MINOR                   0

/* DMA allocator pool size */
#define BOARD_DMA_ALLOC_POOL_SIZE    5120

/* Board provides board_on_reset */
#define BOARD_HAS_ON_RESET           1

#define BOARD_ENABLE_CONSOLE_BUFFER

#define PX4_GPIO_INIT_LIST { \
		PX4_ADC_GPIO, \
		GPIO_nLED_BLUE, \
		GPIO_nLED_GREEN, \
	}

__BEGIN_DECLS

/****************************************************************************************************
 * Public Functions
 ****************************************************************************************************/

extern void stm32_spiinitialize(void);
extern void stm32_usbinitialize(void);
extern int stm32_sdio_initialize(void);
extern void board_peripheral_reset(int ms);

/* Parameters stored in internal flash */
#define FLASH_BASED_PARAMS

#include <px4_platform_common/board_common.h>

__END_DECLS
