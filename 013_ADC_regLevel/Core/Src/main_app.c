#include "main_app.h"

// Variable to store ADC result
uint8_t adc_value = 0;

int main(void){

	// Initialize HAL and System Clock (84 MHz)
	HAL_Init();
	SystemClockConfig();

	// Configure ADC Peripheral
	ADC_Config();

	while(TRUE){

		// Poll ADC for new data
		adc_value = Read_ADC();

		// Small delay for stability
		HAL_Delay(100);
	}

	return 0;
}

// Function to start conversion and return 8-bit result
uint8_t Read_ADC(void){

	uint8_t value = 0;

	// Start the conversion (SWSTART bit 30 in CR2)
	ADC1->CR2 |= (1<<30);

	// Wait until End of Conversion (EOC bit 1 in SR) flag is set
	// Note: (2<<0) is effectively (1<<1), checking bit 1.
	while(!(ADC1->SR & (1<<1)));

	// Read the data register
	value = (uint8_t)ADC1->DR;

	return value;
}

// Configure ADC1 settings (Resolution, Prescaler, etc.)
void ADC_Config(void){

	// Enable Clocks and GPIO first
	ADC_LowLevel_Config();

	// Set Resolution to 8-bit (Bits [25:24] = 10)
	ADC1->CR1 &= ~(1<<24); // Clear bit 24
	ADC1->CR1 |= (1<<25);  // Set bit 25

	// Enable ADC Peripheral (ADON bit 0 in CR2)
	ADC1->CR2 |= (1<<0);

	// Set Sampling Time for Channel 0 to 56 Cycles (Bits [2:0] = 011)
	ADC1->SMPR2 |= (3<<0);

	// ADC Common Control Register (CCR) Configuration
	// CRITICAL FIX: APB2 is 84MHz. Max ADC clock is usually 36MHz.
	// Previous: Div2 -> 42MHz (Too High/Unstable)
	// New: Div4 -> 84MHz / 4 = 21MHz (Safe)
	// ADCPRE bits [17:16]: 00=Div2, 01=Div4, 10=Div6, 11=Div8
	ADC->CCR |= (1<<16);  // Set bit 16
	ADC->CCR &= ~(1<<17); // Clear bit 17
}

// Configure GPIO and Clock Enable (MSP)
void ADC_LowLevel_Config(void){

	// Enable GPIOA Clock (AHB1)
	RCC->AHB1ENR |= (1<<0);

	// Enable ADC1 Clock (APB2)
	RCC->APB2ENR |= (1<<8);

	// Configure PA0 as Analog Mode (11)
	GPIOA->MODER |= (1<<0);
	GPIOA->MODER |= (1<<1);
}

void SystemClockConfig(void){

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	// Configure PLL: HSI(16MHz) -> PLL -> 84MHz Output
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;  // 16MHz / 8 = 2MHz
	RCC_OscInitStruct.PLL.PLLN = 84; // 2MHz * 84 = 168MHz
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // 168MHz / 2 = 84MHz
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){
		Error_Handler();
	}

	// Configure Bus Clocks
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;  // HCLK = 84MHz
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;   // PCLK1 = 42MHz
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;   // PCLK2 = 84MHz (Fast!)

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
