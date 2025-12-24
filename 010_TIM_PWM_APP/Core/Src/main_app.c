#include "main_app.h"

TIM_HandleTypeDef htim2 = {0};
GPIO_InitTypeDef hgpio = {0};

int main(void){

	uint16_t brightness = 0;

	// Initialize HAL and System Clock
	HAL_Init();
	SystemClockConfig();

	// Initialize Timer for PWM
	TIM2_Init();

	// Start PWM signal generation on Channel 1
	if(HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}

	while(TRUE){

		// Fade In: Gradually increase brightness
		while(brightness < htim2.Init.Period){
			brightness += 20;

			// Update the Duty Cycle (Pulse Width)
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, brightness);

			HAL_Delay(1); // Control the speed of fading
		}

		// Fade Out: Gradually decrease brightness
		while(brightness > 0){
			brightness -= 20;

			// Update the Duty Cycle
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, brightness);

			HAL_Delay(1);
		}
	}

	return 0;
}

// Helper function to initialize On-Board LED (PA5)
// Note: Not strictly used for PWM output on PA0, but good for debugging.
void GPIO_Init(void){

	hgpio.Mode = GPIO_MODE_OUTPUT_PP;
	hgpio.Pin = GPIO_PIN_5;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOA, &hgpio);
}

// Configure TIM2 for PWM Output
void TIM2_Init(void){

	TIM2_LowLevel_Config();

	TIM_OC_InitTypeDef tim2PWM_Config;

	htim2.Instance = TIM2;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;

	// PWM Frequency Calculation:
	// System Freq = 50 MHz (APB1 x 2)
	// Prescaler = 5 -> Timer Clock = 10 MHz
	// Period = 10000 -> PWM Freq = 10 MHz / 10000 = 1 kHz (1 ms period)
	htim2.Init.Period = 10000-1;
	htim2.Init.Prescaler = 5-1;

	if(HAL_TIM_PWM_Init(&htim2) != HAL_OK){
		Error_Handler();
	}

	// Reset config structure to avoid garbage values
	memset(&tim2PWM_Config, 0, sizeof(tim2PWM_Config));

	// Configure PWM Mode 1
	tim2PWM_Config.OCMode = TIM_OCMODE_PWM1;
	tim2PWM_Config.OCPolarity = TIM_OCPOLARITY_HIGH;
	tim2PWM_Config.Pulse = 0; // Initial Duty Cycle = 0%

	if(HAL_TIM_PWM_ConfigChannel(&htim2, &tim2PWM_Config, TIM_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}
}

// MSP (Low Level) Initialization for TIM2
void TIM2_LowLevel_Config(void){

	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure PA0 as Alternate Function (TIM2_CH1)
	hgpio.Mode = GPIO_MODE_AF_PP;
	hgpio.Alternate = GPIO_AF1_TIM2;
	hgpio.Pin = GPIO_PIN_0;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOA, &hgpio);
}

void SystemClockConfig(void){

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	// Enable Power Control Clock
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	// Configure Oscillator (HSI -> PLL)
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;

	// PLL Settings for 50 MHz Output
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 200;
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

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK){
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
