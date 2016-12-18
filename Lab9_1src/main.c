#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int bool;
#define false 0
#define true 1

extern void GPIO_init();


void WriteToLCD(int input, int isCmd){

    


    return;
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

int main() {
    GPIO_init();
	init_LCD();
	SysTick_Config(1300000UL);
	while(1){

	}
	return 0;
}
