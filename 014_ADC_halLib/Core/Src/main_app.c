#include "main_app.h"

// ADC Handle and variables
ADC_HandleTypeDef hadc1 = {0};
uint8_t adc_value = 0;
float Vadc = 0.0f;

int main(void){

	// Initialize HAL Library and System Clock
	HAL_Init();
	SystemClockConfig();

	// Configure ADC Peripheral
	ADC_Config();

	while(TRUE){

		// Perform a single ADC conversion
		Read_ADC();

		// Convert raw 8-bit value (0-255) to Voltage (0-3.3V)
		// Formula: Voltage = (RawValue / MaxValue) * Vref
		Vadc = (float)adc_value / 255.0f * 3.3f;

		// Sampling delay
		HAL_Delay(100);
	}

	return 0;
}

// Function to handle the ADC polling sequence
void Read_ADC(void){

	// Start the ADC peripheral
	if(HAL_ADC_Start(&hadc1) != HAL_OK){
		Error_Handler();
	}

	// Wait for the conversion to finish
	if(HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK){
		Error_Handler();
	}

	// Read the converted value
	adc_value = HAL_ADC_GetValue(&hadc1);

	// Stop the ADC to save power or prepare for next trigger
	HAL_ADC_Stop(&hadc1);
}

// Configure ADC1 settings and channel
void ADC_Config(void){

	ADC_ChannelConfTypeDef sConfig = {0};

	// Initialize Low Level Hardware (GPIO & Clocks)
	ADC_LowLevel_Config();

	// ADC Main Configuration
	hadc1.Instance = ADC1;
	hadc1.Init.Resolution = ADC_RESOLUTION_8B; // 8-bit resolution (0-255)

	// Clock Prescaler:
	// APB2 = 84MHz. DIV4 gives 21MHz (Safe for ADC).
	// DIV2 (42MHz) would exceed the 36MHz limit.
	hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;

	hadc1.Init.ScanConvMode = DISABLE; // Single channel mode
	hadc1.Init.ContinuousConvMode = DISABLE; // We trigger manually in loop
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

	if(HAL_ADC_Init(&hadc1) != HAL_OK){
		Error_Handler();
	}

	// Configure ADC Channel (Channel 0 on PA0)
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES; // Longer sampling for better stability

	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK){
		Error_Handler();
	}
}

// Configure GPIO and Clock Enable (MSP)
void ADC_LowLevel_Config(void){

	GPIO_InitTypeDef hgpio = {0};

	// Enable Clocks
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure PA0 as Analog Input
	hgpio.Pin = GPIO_PIN_0;
	hgpio.Mode = GPIO_MODE_ANALOG;
	hgpio.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &hgpio);
}

void SystemClockConfig(void){

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	// Configure Oscillator
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 84;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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
