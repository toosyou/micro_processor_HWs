#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int bool;
#define false 0
#define true 1

void display_41();

void SysTick_Handler(void) {
	display_41();
}

void GPIO_init(void){
    RCC->AHB2ENR |= 6;
    GPIOC->MODER &= 0b11111111111111110101010101010101;
    GPIOC->PUPDR = 0b11111111111111110101010101010101;
    GPIOC->OSPEEDR |= 0x5555;

    GPIOB->MODER &= 0b01010111111111111111111111111111;
    GPIOB->PUPDR = 0b01010111111111111111111111111111;
    GPIOB->OSPEEDR |= 0x54000000;

    return;
}


void WriteToLCD(int input, bool isCmd){

    if(isCmd == true){ // set rs, rw and enable to zero
        GPIOB->ODR &= 0b0001111111111111;
    }
    else{
        GPIOB->ODR =  0b0010000000000000;
    }
    GPIOC->ODR = input;

    //turn enable on
    GPIOB->ODR |= 0b1000000000000000;

    //do nothing
    for(int i=0;i<500;++i);

    //turn enable off
    GPIOB->ODR &= 0b0111111111111111;

    //do nothing
    for(int i=0;i<500;++i);

    return;
}

void init_LCD() {

	// LCD Register
	WriteToLCD(0x38, true); // Function Setting
	WriteToLCD(0x06, true); // Entering Mode
	WriteToLCD(0x0E, true); // Display on
	WriteToLCD(0x01, true); // Clear Screen
	WriteToLCD(0x80, true); // Move to top left

}

void display_41(){

    static int index_4 = 0;
    static int index_1 = 1;
    static int last_index_4 = 0;
    static int last_index_1 = 0;

    //clear last 4
    WriteToLCD(128 + last_index_4, true);
    WriteToLCD(0b11111110,false);

    //write 4
    WriteToLCD(128 + index_4, true);
    WriteToLCD(0b00110100,false);

    //write 1
    WriteToLCD(128 + index_1, true);
    WriteToLCD(0b00110001,false);


    last_index_4 = index_4;
    last_index_1 = index_1;
    index_1++;
    index_4++;

    if(index_1 == 16)
        index_1 += 48;
    else if(index_1 == 80)
        index_1 = 0;
    if(index_4 == 16)
        index_4 += 48;
    else if(index_4 == 80)
        index_4 = 0;

}

int main() {

    GPIO_init();
	init_LCD();

	SysTick_Config(1300000UL);
	while(1){

	}
	return 0;
}
