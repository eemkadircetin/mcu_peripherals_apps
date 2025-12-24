/*
 * main_app.h
 *
 * Created on: Dec 18, 2024
 * Author: User
 */

#ifndef SRC_MAIN_APP_H_
#define SRC_MAIN_APP_H_

#include "stm32f4xx_hal.h"

// Standard boolean definitions
#define TRUE				1
#define FALSE				0
#define ENABLE				TRUE
#define DISABLE				FALSE

// Function Prototypes
void Read_ADC(void);           // Manages the ADC start, poll, and stop sequence
void ADC_Config(void);         // General ADC configuration (Resolution, Prescaler)
void ADC_LowLevel_Config(void);// Low-level hardware init (GPIO pins, Bus Clocks)
void SystemClockConfig(void);  // Configures the system clock settings
void Error_Handler(void);      // Trap for critical errors

#endif /* SRC_MAIN_APP_H_ */
