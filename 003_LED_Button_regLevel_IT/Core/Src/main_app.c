#include "main_app.h"

int main(void){

	// Initialize HAL library and System Clock
	HAL_Init();
	SystemClockConfig();

	// Initialize peripherals using direct register access
	LED_Init();
	Button_Init();

	while(TRUE){

		// Main loop is empty.
		// Logic is handled via External Interrupt (EXTI) in the handler below.
	}

	return 0;
}

// Interrupt Service Routine for EXTI Lines 10 to 15
void EXTI15_10_IRQHandler(void){

	// Check if the interrupt comes from Line 13 (Button Pin)
	if((EXTI->PR & 1<<13) == (1<<13)){

		// Clear the pending bit immediately to prevent re-triggering
		EXTI->PR |= (1<<13);

		// Logic to toggle LED state
		// Note: 'status' should be static to retain value between interrupts
		static uint8_t status = 0;

		if(status == 0){
			// Set PA5 High (Turn LED ON) using BSRR register
			GPIOA->BSRR = (1<<5);
			status = 1;
		}
		else{
			// Reset PA5 Low (Turn LED OFF) using high bits of BSRR
			GPIOA->BSRR = (1<<21);
			status = 0;
		}
	}
}

// Configure PA5 as Output for LED
void LED_Init(void){

	// Enable GPIOA Clock (AHB1 Bus)
	RCC->AHB1ENR |= (1<<0);

	// Set PA5 to Output Mode (01)
	GPIOA->MODER |= (1<<10);
	GPIOA->MODER &= ~(1<<11);

	// No Pull-up / No Pull-down
	GPIOA->PUPDR &= ~(1<<10);
	GPIOA->PUPDR &= ~(1<<11);

	// Set Output Speed to High
	GPIOA->OSPEEDR |= (1<<11);
	GPIOA->OSPEEDR &= ~(1<<10);

	// Set Output Type to Push-Pull
	GPIOA->OTYPER &= ~(1<<5);
}

// Configure PC13 as Input for Button with Interrupts
void Button_Init(void){

	// Enable GPIOC (AHB1) and SYSCFG (APB2) Clocks
	RCC->AHB1ENR |= (1<<2);
	RCC->APB2ENR |= (1<<14);

	// Set PC13 to Input Mode
	GPIOC->MODER &= ~(1<<26);
	GPIOC->MODER &= ~(1<<27);

	// No Pull-up / No Pull-down (External pull-up usually exists on buttons)
	GPIOC->PUPDR &= ~(1<<26);
	GPIOC->PUPDR &= ~(1<<27);

	// Set Output Type (Not strictly necessary for Input mode, but good practice to clear)
	GPIOC->OTYPER &= ~(1<<13);

	// Map EXTI Line 13 to Port C
	// 0x20 selects Port C for EXTI13 in SYSCFG_EXTICR4
	SYSCFG->EXTICR[3] |= 0x20;

	// Configure Falling Edge Trigger for EXTI13
	EXTI->FTSR |= (1<<13);

	// Unmask (Enable) Interrupt for EXTI13
	EXTI->IMR |= (1<<13);

	// Enable Interrupt in NVIC and set priority
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	NVIC_SetPriority(EXTI15_10_IRQn, 0);
}

void SystemClockConfig(void){
	// TODO: Configure system clock parameters
}

void Error_Handler(void){
	// Block execution on error
	while(TRUE);
}

void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
