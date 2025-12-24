/*
 * main_app.h
 *
 * Created on: Dec 18, 2024
 * Author: User
 */

#ifndef SRC_MAIN_APP_H_
#define SRC_MAIN_APP_H_

#include "stm32f4xx_hal.h"

// Standard Boolean definitions
#define TRUE				1
#define FALSE				0
#define ENABLE				TRUE
#define DISABLE				FALSE

// Function Prototypes
void delay_us(uint16_t us);   // Custom microsecond delay
void TIM_Init(void);          // Timer configuration
void LED_Init(void);          // GPIO configuration
void SystemClockConfig(void); // System clock setup
void Error_Handler(void);

#endif /* SRC_MAIN_APP_H_ */
