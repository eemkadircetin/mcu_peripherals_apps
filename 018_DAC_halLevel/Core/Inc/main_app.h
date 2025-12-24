/*
 * main_app.h
 *
 * Created on: Dec 18, 2024
 * Author: User
 */

#ifndef SRC_MAIN_APP_H_
#define SRC_MAIN_APP_H_

#include "stm32f4xx_hal.h"

// Boolean definitions
#define TRUE				1
#define FALSE				0
#define ENABLE				TRUE
#define DISABLE				FALSE

// Function Prototypes
void DAC1_Config(void);
void DAC_LowLevel_Config(void);
void SystemClockConfig(void);
void Error_Handler(void);

#endif /* SRC_MAIN_APP_H_ */
