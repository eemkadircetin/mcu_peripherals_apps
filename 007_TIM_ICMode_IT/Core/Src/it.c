
#include "main_app.h"

extern TIM_HandleTypeDef htim2;


void TIM2_IRQHandler(void){

	HAL_TIM_IRQHandler(&htim2);

}

void NMI_Handler(void){ while(TRUE); }

void HardFault_Handler(void){ while(TRUE); }

void MemManage_Handler(void){ while(TRUE); }

void BusFault_Handler(void){ while(TRUE); }

void UsageFault_Handler(void){ while(TRUE); }

void DebugMon_Handler(void){ while(TRUE); }

void SysTick_Handler(void){

	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

}
