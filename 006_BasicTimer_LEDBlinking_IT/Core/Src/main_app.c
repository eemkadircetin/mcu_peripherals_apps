#include "main_app.h"

GPIO_InitTypeDef hgpio = {0};
TIM_HandleTypeDef htim6 = {0};

// Flag to track the system state (1: Ready to start, 0: Running)
volatile uint8_t status = 1;

int main(void){

	// Initialize HAL and System Clock
	HAL_Init();
	SystemClockConfig();

	// Initialize peripherals
	Button_Init();
	LED_Init();
	TIM_Init();

	while(TRUE){
		// Main loop is empty.
		// Logic is handled by External Interrupts (Button) and Timer Interrupts.
	}

	return 0;
}

// Timer 6 Interrupt Handler
// This function executes every time the timer overflows (every 1 second)
void TIM6_DAC_IRQHandler(void){

	HAL_TIM_IRQHandler(&htim6);

	// Toggle LED state
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

// External Interrupt Callback (Button Press)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	// Check if the interrupt comes from the User Button
	if(GPIO_Pin == GPIO_PIN_13){

		// Check internal state flag
		if(1 == status){
			// Start the timer with interrupts
			HAL_TIM_Base_Start_IT(&htim6);
			status = 0; // Update flag to indicate running state
		}
		else{
			// Stop the timer
			HAL_TIM_Base_Stop_IT(&htim6);

			// Ensure LED is turned OFF when stopped
			if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_SET){
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			}
			status = 1; // Update flag to indicate stopped state
		}
	}
}

// Hardware Interrupt Handler for EXTI Line [15:10]
void EXTI15_10_IRQHandler(void){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

// Configure TIM6 as a timebase source
void TIM_Init(void){

	__HAL_RCC_TIM6_CLK_ENABLE();

	htim6.Instance = TIM6;

	// Timer Calculation for 1 Hz (1 Second period):
	// System Freq = 84 MHz
	// Prescaler = 8400 -> Timer Clock = 84 MHz / 8400 = 10 KHz (0.1 ms per tick)
	// Period = 10000 -> Duration = 10000 * 0.1 ms = 1000 ms = 1 Second
	htim6.Init.Period = 10000-1;
	htim6.Init.Prescaler = 8400-1;

	if(HAL_TIM_Base_Init(&htim6) != HAL_OK){
		Error_Handler();
	}

	// Set Timer interrupt priority lower than Button (Priority 1 vs 0)
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

// Configure Button (PC13) with Interrupts
void Button_Init(void){

	__HAL_RCC_GPIOC_CLK_ENABLE();

	hgpio.Mode = GPIO_MODE_IT_FALLING; // Trigger on button press
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_LOW;
	hgpio.Pin = GPIO_PIN_13;

	HAL_GPIO_Init(GPIOC, &hgpio);

	// Set Button priority to highest (0)
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// Configure LED (PA5) as Output
void LED_Init(void){

	__HAL_RCC_GPIOA_CLK_ENABLE();

	hgpio.Mode = GPIO_MODE_OUTPUT_PP;
	hgpio.Pin = GPIO_PIN_5;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(GPIOA, &hgpio);
}

// Configure System Clock to 84 MHz
void SystemClockConfig(void){

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	// Configure Oscillator (HSE -> PLL)
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
		Error_Handler();
	}

	// Configure Bus Clocks
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK){
		Error_Handler();
	}
}

void Error_Handler(void){
	while(TRUE);
}

void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
