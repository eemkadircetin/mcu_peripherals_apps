#include "main_app.h"

// Structure for GPIO initialization
GPIO_InitTypeDef hgpio = {0};

int main(void){

	// Initialize the HAL Library and System Clock
	HAL_Init();
	SystemClockConfig();

	// Initialize peripherals
	LED_Init();
	Button_Init();

	while(TRUE){
		// The main loop is empty because the system is event-driven.
		// All logic handles inside the Interrupt Callback function.
	}

	return 0;
}

// This callback is called automatically when an external interrupt occurs
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	// Check if the interrupt came from the User Button (Pin 13)
	if(GPIO_Pin == GPIO_PIN_13){
		// Toggle the LED state
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
}

// Hardware Interrupt Handler for EXTI Line [15:10]
void EXTI15_10_IRQHandler(void){
	// Call the HAL common interrupt handler
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

// Configure PA5 as Output for the LED
void LED_Init(void){

	__HAL_RCC_GPIOA_CLK_ENABLE();

	hgpio.Mode = GPIO_MODE_OUTPUT_PP; // Push-Pull Output
	hgpio.Speed = GPIO_SPEED_HIGH;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Pin = GPIO_PIN_5;

	HAL_GPIO_Init(GPIOA, &hgpio);
}

// Configure PC13 as Input with Interrupt capability
void Button_Init(void){

	__HAL_RCC_GPIOC_CLK_ENABLE();

	// Trigger interrupt on the Falling Edge (when button is pressed)
	hgpio.Mode = GPIO_MODE_IT_FALLING;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_HIGH;
	hgpio.Pin = GPIO_PIN_13;

	HAL_GPIO_Init(GPIOC, &hgpio);

	// Enable Interrupts in the NVIC (Nested Vector Interrupt Controller)
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
}

void SystemClockConfig(void){
	// TODO: Configure system clock speed and source
}

void Error_Handler(void){
	// Block execution if an error occurs
	while(TRUE);
}

void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
