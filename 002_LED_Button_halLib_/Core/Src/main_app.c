#include "main_app.h"

// Define LED pin (Assuming pin 1 here)
#define GPIO_PIN_LED  GPIO_PIN_1

GPIO_InitTypeDef hgpio = {0};

int main(void){

	// Reset of all peripherals, Initializes the Flash interface and the Systick.
	HAL_Init();

	// Configure the system clock
	SystemClockConfig();

	// Initialize GPIOs for LED and Button
	LED_Init();
	Button_Init();

	// Main loop
	while(TRUE){

		// Check if button is pressed
		if(BUTTON_PRESSED == Button_Read()){
			LED_Write(ENABLE); // Turn on LED
		}
		else{
			LED_Write(DISABLE); // Turn off LED
		}

		// Small delay to debounce and reduce CPU load
		HAL_Delay(20);
	}

	return 0;
}

// Control the LED state (ON/OFF) safely
void LED_Write(uint8_t ENORDI){

	if(ENORDI == ENABLE){
		// Check if it is currently OFF to avoid redundant write
		if(! (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_LED))){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_LED, GPIO_PIN_SET);
		}
	}
	else if(ENORDI == DISABLE){
		// Check if it is currently ON before turning off
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_LED)){
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_LED, GPIO_PIN_RESET);
		}
	}
	else{
		// Invalid parameter passed
		Error_Handler();
	}
}

// Read the current state of the button
uint8_t Button_Read(void){

	// Check Pin 13 (Active Low logic usually)
	if(! (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) )){
		return BUTTON_PRESSED;
	}
	else{
		return BUTTON_NOT_PRESSED;
	}
}

// Configure GPIO for LED Output
void LED_Init(void){

	hgpio.Mode = GPIO_MODE_OUTPUT_PP; // Push-Pull mode
	hgpio.Speed = GPIO_SPEED_HIGH;
	hgpio.Pull = GPIO_NOPULL;
	hgpio.Pin = GPIO_PIN_LED;

	// Enable GPIO Clock
	__HAL_RCC_GPIOA_CLK_ENABLE(); // Note: Double check if this should be GPIOC

	HAL_GPIO_Init(GPIOC, &hgpio);
}

// Configure GPIO for Button Input
void Button_Init(void){

	__HAL_RCC_GPIOC_CLK_ENABLE();

	hgpio.Mode = GPIO_MODE_INPUT;
	hgpio.Pull = GPIO_NOPULL;
	// hgpio.Speed = GPIO_SPEED_HIGH; // Not strictly needed for input
	hgpio.Pin = GPIO_PIN_LED; // Note: Ensure this pin matches your button hardware

	HAL_GPIO_Init(GPIOC, &hgpio);
}

void SystemClockConfig(void){
	// Clock configuration logic goes here
	// TODO: Implement clock setup
}

void Error_Handler(void){
	// Stay here if an error occurs
	while(TRUE);
}

void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}
