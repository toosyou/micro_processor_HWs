#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int bool;
#define false 0
#define true 1

extern void GPIO_init();


int WriteToLCD(int input, int isCmd) {

	//TODO: Write command to LCD or let LCD display character fetched from memory.
}
void WriteToLCD(int input, int isCmd)
{
	//TODO: Write command to LCD or let LCD display character fetched from memory.
	//...
	TM_GPIO_SetPinLow(GPIOB, LCD_RSPin);
	//...
	TM_GPIO_SetPinHigh(GPIOB, LCD_RSPin);
	TM_GPIO_SetPinLow(GPIOB, LCD_RWPin);

	for (int index = 0 ; index < 8 ; index++)
	{
		//...
		TM_GPIO_SetPinHigh(GPIOA, LCD_dataPin[index]);
		//...
		TM_GPIO_SetPinLow(GPIOA, LCD_dataPin[index]);
		//...
	}
	TM_GPIO_SetPinHigh(GPIOB, LCD_ENPin);
	delay_ms(10);
	TM_GPIO_SetPinLow(GPIOB, LCD_ENPin);
	delay_ms(10);

}
void init_LCD() {

	// LCD Register
	WriteToLCD(0x38, 1); // Function Setting
	WriteToLCD(0x06, 1); // Entering Mode
	WriteToLCD(0x0C, 1); // Display on
	WriteToLCD(0x01, 1); // Clear Screen
	WriteToLCD(0x80, 1); // Move to top left

}

void SysTick_Handler() {

}

void init() {
	for (int i = 0; i < 8; i++) {
		TM_GPIO_Init(GPIOA, LCD_dataPin[i], TM_GPIO_Mode_OUT, TM_GPIO_OType_PP,
		TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	}
	TM_GPIO_Init(GPIOB, LCD_VSPin, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP,TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(GPIOB, LCD_RSPin, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP,TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(GPIOB, LCD_RWPin, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP,TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	TM_GPIO_Init(GPIOB, LCD_ENPin, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP,TM_GPIO_PuPd_UP, TM_GPIO_Speed_Medium);
	init_LCD();
}

int main() {
	init();
	SysTick_Config(1300000UL);
	while(1){

	}
	return 0;
}
