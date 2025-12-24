#include "main_app.h"

// Peripheral Handles
TIM_HandleTypeDef htim2 = {0};
UART_HandleTypeDef huart2 = {0};
GPIO_InitTypeDef hgpio = {0};

// Variables for Input Capture
uint8_t count = 0;
uint32_t input_captures[2] = {0};
volatile uint8_t is_capture_done = FALSE;

int main(void){

	uint32_t capture_difference = 0;
	double user_signal_time_period = 0;
	double user_signal_time_freq = 0;
	char usr_msg[100];

	// Initialize System
	HAL_Init();
	SystemClockConfig();

	// Initialize Peripherals
	TIM2_Init();
	UART2_Init();

	// Start Input Capture on TIM2 Channel 1 with Interrupts
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

	while(TRUE){

		// Process data only when capture is complete
		if(is_capture_done){

			// Calculate the difference between two consecutive captures
			if(input_captures[1] > input_captures[0]){
				capture_difference = input_captures[1] - input_captures[0];
			}
			else{
				// Handle Timer Overflow (Rollover) case
				// TIM2 is 32-bit, so period is 0xFFFFFFFF
				capture_difference = (0xFFFFFFFF - input_captures[0]) + input_captures[1];
			}

			// Calculate Frequency:
			// PCLK1 * 2 is used because APB1 prescaler is 4, causing TIM clock to double.
			// Formula: Freq = Timer_Clock / Capture_Count
			user_signal_time_freq = (1.0 / capture_difference ) * ((HAL_RCC_GetPCLK1Freq() * 2.0)
					/ (htim2.Init.Prescaler + 1.0));

			// Format and transmit the result via UART
			sprintf(usr_msg, "Frequency of Signal : %.3f Hz\r\n", user_signal_time_freq);
			HAL_UART_Transmit(&huart2, (uint8_t*)usr_msg, strlen(usr_msg), HAL_MAX_DELAY);

			// Reset flag to wait for the next capture
			is_capture_done = FALSE;
		}
	}

	return 0;
}

// Timer Input Capture Callback
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance == TIM2){

		if(!is_capture_done){
			if(count == 0){
				// Store the first rising edge timestamp
				input_captures[0] = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_1);
				count++;
			}
			else if(count == 1){
				// Store the second rising edge timestamp
				input_captures[1] = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_1);
				count = 0;
				is_capture_done = TRUE; // Signal main loop to process data
			}
			else{
				Error_Handler();
			}
		}
	}
}

void SystemClockConfig(void){

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	// Enable Power Control Clock
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	// Enable HSI and LSE (Low Speed External)
	// LSE is likely used here as a reference source to output on MCO pin
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

	// PLL Configuration
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 200;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4; // SYSCLK = 50 MHz
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
		Error_Handler();
	}

	// Bus Clock Configuration
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK){
		Error_Handler();
	}

	// Output LSE Clock on MCO1 (PA8) for testing purposes
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);
}

#if 0
void LSE_Config(void){
	// Optional specific LSE configuration if needed separately
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
		Error_Handler();
	}
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);
}
#endif

void Error_Handler(void){
	while(TRUE);
}
