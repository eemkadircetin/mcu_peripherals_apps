#include "main_app.h"

extern TIM_HandleTypeDef htim2;
extern GPIO_InitTypeDef hgpio;
extern UART_HandleTypeDef huart2;

// Configure Timer 2 for Input Capture Mode
void TIM2_Init(void){

	TIM2_LowLevel_Config();

	TIM_IC_InitTypeDef htim2ic = {0};

	htim2.Instance = TIM2;
	htim2.Init.ClockDivision = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xFFFFFFFF; // Max period for 32-bit timer (TIM2/TIM5)
	htim2.Init.Prescaler = 1;       // Run at full speed for max resolution

	if(HAL_TIM_IC_Init(&htim2) != HAL_OK){
		Error_Handler();
	}

	// Configure Channel 1 for Input Capture
	htim2ic.ICFilter = 0;
	htim2ic.ICPrescaler = TIM_ICPSC_DIV1; // Capture every edge
	htim2ic.ICPolarity = TIM_ICPOLARITY_RISING; // Trigger on Rising Edge
	htim2ic.ICSelection = TIM_ICSELECTION_DIRECTTI;

	if(HAL_TIM_IC_ConfigChannel(&htim2, &htim2ic, TIM_CHANNEL_1) != HAL_OK){
		Error_Handler();
	}
}

// Configure UART2 for Data Transmission (9600 baud)
void UART2_Init(void){

	UART2_LowLevel_Config();

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 9600;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX; // Only TX is needed for this app
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;

	if(HAL_UART_Init(&huart2) != HAL_OK){
		Error_Handler();
	}
}

// MSP (Low Level) Init for TIM2
void TIM2_LowLevel_Config(void){

	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure PA0 as Alternate Function for TIM2_CH1
	hgpio.Pin = GPIO_PIN_0;
	hgpio.Mode = GPIO_MODE_AF_PP;
	hgpio.Alternate = GPIO_AF1_TIM2;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(GPIOA, &hgpio);

	// Enable Timer Interrupts
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0); // Low priority
}

// MSP (Low Level) Init for UART2
void UART2_LowLevel_Config(void){

	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Configure PA2 as Alternate Function for USART2_TX
	hgpio.Mode = GPIO_MODE_AF_PP;
	hgpio.Alternate = GPIO_AF7_USART2;
	hgpio.Pin = GPIO_PIN_2;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(GPIOA, &hgpio);
}
