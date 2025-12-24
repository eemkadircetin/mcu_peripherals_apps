#include "main_app.h"

GPIO_InitTypeDef hgpio = {0};
TIM_HandleTypeDef htim6 = {0};

int main(void){

	// Initialize HAL and System Clock (Target: 84 MHz)
	HAL_Init();
	SystemClockConfig();

	// Initialize peripherals
	LED_Init();
	TIM_Init();

	// Start the basic timer to be used for delays
	HAL_TIM_Base_Start(&htim6);

	while(TRUE){

		// Toggle LED roughly every 10 microseconds
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		delay_us(10);
	}

	return 0;
}

// Custom blocking delay function in microseconds
void delay_us(uint16_t us){

	// Reset the timer counter to start from zero
	__HAL_TIM_SET_COUNTER(&htim6, 0);

	// Wait until the counter reaches the desired value
	// Since timer frequency is 1 MHz, each tick is 1 us.
	while(__HAL_TIM_GET_COUNTER(&htim6) < us - 1);
}

// Configure TIM6 as a timebase source
void TIM_Init(void){

	// Enable Timer 6 Clock
	__HAL_RCC_TIM6_CLK_ENABLE();

	htim6.Instance = TIM6;
	htim6.Init.Period = 0xFFFF; // Max period to avoid early overflow

	// Prescaler calculation:
	// System Clock is 84 MHz. To get 1 MHz (1 us per tick):
	// Prescaler = (TimerClock / TargetFreq) - 1 => (84MHz / 1MHz) - 1 = 83
	htim6.Init.Prescaler = 84-1;

	if(HAL_TIM_Base_Init(&htim6) != HAL_OK){
		Error_Handler();
	}
}

// Configure GPIO for LED
void LED_Init(void){

	__HAL_RCC_GPIOA_CLK_ENABLE();

	hgpio.Mode = GPIO_MODE_OUTPUT_PP;
	hgpio.Pin = GPIO_PIN_5;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_LOW; // Low speed is sufficient for toggling LED

	HAL_GPIO_Init(GPIOA, &hgpio);
}

// Configure System Clock to 84 MHz using HSE
void SystemClockConfig(void){

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	// Enable Power Control Clock
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	// Configure Oscillator (HSE -> PLL)
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

	// PLL Parameters for 84 MHz Output
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 2; // USB Clock (not used here but configured)
	RCC_OscInitStruct.PLL.PLLR = 2;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
		Error_Handler();
	}

	// Configure Bus Clocks (AHB, APB1, APB2)
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // APB1 = 42 MHz
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; // APB2 = 84 MHz

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK){
		Error_Handler();
	}
}

void Error_Handler(void){
	// Infinite loop to indicate failure
	while(TRUE);
}

void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
