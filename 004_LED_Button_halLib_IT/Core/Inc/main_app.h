/*
 * main_app.h
 *
 * Created on: Dec 18, 2024
 * Author: User
 */

#ifndef SRC_MAIN_APP_H_
#define SRC_MAIN_APP_H_

#include "stm32f4xx_hal.h"

// Boolean definitions for code clarity
#define TRUE				1
#define FALSE				0
#define ENABLE				TRUE
#define DISABLE				FALSE

// Button state macros
#define BUTTON_PRESSED		ENABLE
#define BUTTON_NOT_PRESSED	DISABLE

// Function Prototypes
void SystemClockConfig(void);
void Error_Handler(void);
void LED_Init(void);
void Button_Init(void);

#endif /* SRC_MAIN_APP_H_ */
