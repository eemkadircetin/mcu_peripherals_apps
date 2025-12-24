/*
 * main_app.h
 *
 * Created on: Dec 18, 2024
 * Author: User
 */

#ifndef SRC_MAIN_APP_H_
#define SRC_MAIN_APP_H_

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

// Standard Boolean definitions
#define TRUE				1
#define FALSE				0
#define ENABLE				TRUE
#define DISABLE				FALSE

// Function Prototypes
void SystemClockConfig(void);
void Error_Handler(void);
void TIM2_Init(void);
void TIM2_LowLevel_Config(void); // Corrected typo: LowLewel -> LowLevel

#endif /* SRC_MAIN_APP_H_ */
