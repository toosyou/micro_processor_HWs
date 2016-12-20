#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>
#include "onewire.h"
#include "ds18b20.h"

typedef unsigned int bool;
#define false 0
#define true 1

#define SIZE_CHANGE_ME 10

int index_4 = 0;
int index_1 = 1;
int last_index_4 = 0;
int last_index_1 = 0;
int mode = 0;

int cgram[2][8]={
    {
        0b11111,
        0b10001,
        0b10001,
        0b00111,
        0b00100,
        0b00000,
        0b00100,
        0b00000
    },
    {
        0b01110,
        0b11111,
        0b11111,
        0b11111,
        0b01110,
        0b00000,
        0b01110,
        0b00000
    }
};

float read_temperature();

void WriteToLCD(int input, bool isCmd);

void SysTick_Handler(void) {
	int temp = (int)read_temperature();
	WriteToLCD('a', false);
	WriteToLCD(temp%10+1, false);
}

void GPIO_init(void){
    RCC->AHB2ENR |= 6;
    GPIOC->MODER &= 0b11110011111111110101010101010101;
    GPIOC->PUPDR =  0b11111111111111110101010101010101;
    GPIOC->OSPEEDR |= 0x4005555;

    GPIOB->MODER &= 0b01010111110111111111111111111111;
    GPIOB->PUPDR =  0b01010111110111111111111111111111;
    GPIOB->OSPEEDR |= 0x54000000;
	//pb3 01
	GPIOB->OSPEEDR |= (1<<(10*2));
	GPIOB->OSPEEDR &= ~(1<<(10*2+1));

    return;
}

bool bottom_clicked(void){
    static int debounce = 0;
    if( (GPIOC->IDR & 0b10000000000000) == 0 ){ // pressing
        debounce = debounce+1 > 500 ? 500 : debounce+1;
    }
    else{
        if(debounce >= 500){
            debounce = 0;
            return true;
        }
        else
            debounce = 0;
    }
    return false;
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

    //clear last 4
    WriteToLCD(128 + last_index_4, true);
    WriteToLCD(0b11111110,false);

    //write ?
    WriteToLCD(128 + index_4, true);
    WriteToLCD(0,false);

    //write !
    WriteToLCD(128 + index_1, true);
    WriteToLCD(1,false);


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

float read_temperature(void){

	float rtn = 0.0f;
	OneWire_t one_wire_structure;
	OneWire_Init(&one_wire_structure, GPIOB, 10);
	while(OneWire_Reset(&one_wire_structure));
	DS18B20_ConvT(&one_wire_structure, TM_DS18B20_Resolution_9bits);
	while(DS18B20_Done(&one_wire_structure));
	DS18B20_Read(&one_wire_structure, &rtn);

	return rtn;
}

int main() {

    GPIO_init();
	init_LCD();

	//set cg ram
    for(int i=0;i<2;++i){
        for(int j=0;j<8;++j){
            WriteToLCD(64 + (i<<3) + j, true);
            WriteToLCD(cgram[i][j], false);
        }
    }


	//SysTick_Config(1300000UL);
	float temp = read_temperature();
	while(1){
        if(bottom_clicked() == true){
            index_4 = 0;
            index_1 = 1;
            last_index_4 = 0;
            last_index_1 = 0;
            WriteToLCD(1, true);
            mode = 1 - mode;
        }
	}
	return 0;
}
