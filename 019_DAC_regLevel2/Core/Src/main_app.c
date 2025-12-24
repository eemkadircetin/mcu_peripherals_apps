#include "main_app.h"

DAC_HandleTypeDef hdac1;

int main(void){

	// Initialize HAL Library
	HAL_Init();

	// Configure System Clock (Target: 84 MHz)
	SystemClockConfig();

	// Configure DAC Peripheral
	DAC1_Config();

	// Start DAC on Channel 1
	if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}

	uint32_t value = 0;
	uint8_t direction = 1; // Flag: 1 for Ramp Up, 0 for Ramp Down

	while(TRUE){

		// Write digital value to DAC Data Register (12-bit Right Aligned)
		// Output Voltage = (Value / 4095) * Vref
		HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, value);

		// Triangle Wave Logic
		if(direction){
			// Ramp Up
			value += 50;
			if(value >= 4095){ // Check upper limit (12-bit max)
				value = 4095;
				direction = 0; // Change direction to down
			}
		}
		else{
			// Ramp Down
			if(value > 50){
				value -= 50;
			}
			else{
				value = 0;
				direction = 1; // Change direction to up
			}
		}

		// Control the frequency of the wave
		HAL_Delay(1);
	}

	return 0;
}

// Configure DAC settings
void DAC1_Config(void){

	// Initialize Low Level Hardware (GPIO & Clock)
	DAC_LowLevel_Config();

	hdac1.Instance = DAC1;

	if(HAL_DAC_Init(&hdac1) != HAL_OK){
		Error_Handler();
	}

	DAC_ChannelConfTypeDef sConfig = {0};

	// Configure Channel 1
	sConfig.DAC_Trigger = DAC_TRIGGER_NONE; // Software trigger only
	sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE; // Enable output buffer

	if(HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}
}

// Low-Level Init: GPIO and Clocks
void DAC_LowLevel_Config(void){

	GPIO_InitTypeDef hgpio1 = {0};

	// Enable Clocks
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_DAC_CLK_ENABLE();

	// Configure PA4 as Analog (DAC_OUT1)
	hgpio1.Mode = GPIO_MODE_ANALOG;
	hgpio1.Pin = GPIO_PIN_4;
	hgpio1.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOA, &hgpio1);
}

// System Clock Configuration (84 MHz)
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
	RCC_OscInitStruct.PLL.PLLM = 16;
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

// Fault Handlers
void NMI_Handler(void){ while(TRUE); }
void HardFault_Handler(void){ while(TRUE); }
void MemManage_Handler(void){ while(TRUE); }
void BusFault_Handler(void){ while(TRUE); }
void UsageFault_Handler(void){ while(TRUE); }
void DebugMon_Handler(void){ while(TRUE); }
