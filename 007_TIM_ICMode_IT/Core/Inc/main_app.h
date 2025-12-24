/*
 * main_app.h
 *
 * Created on: Dec 18, 2024
 * Author: User
 */

#ifndef SRC_MAIN_APP_H_
#define SRC_MAIN_APP_H_

#include "stm32f4xx_hal.h"
#include "it.h"
#include "periph_conf.h"
#include <stdio.h>
#include <string.h>

// Standard Boolean Logic
#define TRUE				1
#define FALSE				0
#define ENABLE				TRUE
#define DISABLE				FALSE

// Function Prototypes
void LSE_Config(void);
void SystemClockConfig(void);
void Error_Handler(void);

#endif /* SRC_MAIN_APP_H_ */
