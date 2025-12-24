/*
 * it.h
 *
 * Created on: Dec 21, 2024
 * Author: User
 */

#ifndef INC_IT_H_
#define INC_IT_H_

#include "main_app.h"

// Timer 2 Interrupt Handler
void TIM2_IRQHandler(void);

// System Exception Handlers
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);
void SysTick_Handler(void);

#endif /* INC_IT_H_ */
