#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int bool;
#define false 0
#define true 1

#define SIZE_CHANGE_ME 10

int index_change_me = 0;
int index_4 = 0;
int index_1 = 1;
int last_index_4 = 0;
int last_index_1 = 0;
int mode = 0;

struct OneWire_t{

};

void ONEWIRE_INPUT(OneWire_t OneWireStruct){

}

void ONEWIRE_LOW(OneWire_t OneWireStruct){

}

void ONEWIRE_OUTPUT(OneWire_t OneWireStruct){

}

void ONEWIRE_DELAY(int delay){
	 SysTick->LOAD=delay*60;          //time load
	 SysTick->CTRL|=0x01;             //countdown
	 while(!(SysTick->CTRL&(1<<16))); //wait for time arrive
	 SysTick->CTRL=0X00000000;        //shut down timer
	 SysTick->VAL=0X00000000;         //clear timer

}

void Write_Scratchpad(){

}

uint8_t OneWire_Reset(OneWire_t* OneWireStruct) {
	//...

	/* Line low, and wait 480us */
	ONEWIRE_INPUT(OneWireStruct);
	ONEWIRE_DELAY(3);//
	ONEWIRE_LOW(OneWireStruct);
	ONEWIRE_OUTPUT(OneWireStruct);
	ONEWIRE_DELAY(480);

	/* Release line and wait for 70us */
	ONEWIRE_INPUT(OneWireStruct);
	ONEWIRE_DELAY(70);

	/* Check bit value */
	//...
	if (Read_Scratchpad()==0)
		ONEWIRE_DELAY(410);
	ONEWIRE_INPUT(OneWireStruct);
	/* Delay for 410 us */
	//ONEWIRE_DELAY(410);

	//...
}

void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit) {
	ONEWIRE_INPUT(OneWireStruct);
	if (bit) {
		/* Set line low */
		ONEWIRE_LOW(OneWireStruct);
		ONEWIRE_OUTPUT(OneWireStruct);
		//...
		/* Bit high */
		ONEWIRE_INPUT(OneWireStruct);
		//...
	}
	else {
		/* Set line low */
		ONEWIRE_LOW(OneWireStruct);
		ONEWIRE_OUTPUT(OneWireStruct);
		//...
	}
	ONEWIRE_INPUT(OneWireStruct);
}

void SysTick_Handler(void) {
    if(mode == 0)
	   display_41();
    else
        display_change_me();
}

void GPIO_init(void){
    RCC->AHB2ENR |= 6;
    GPIOC->MODER &= 0b11110011111111110101010101010101;
    GPIOC->PUPDR =  0b11111111111111110101010101010101;
    GPIOC->OSPEEDR |= 0x4005555;

    GPIOB->MODER &= 0b01010111111111111111111111111111;
    GPIOB->PUPDR = 0b01010111111111111111111111111111;
    GPIOB->OSPEEDR |= 0x54000000;

    return;
}
/*
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
}*/

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

/*
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

}*/
/*
void display_change_me(){
    if(index_change_me == 0)
        WriteToLCD(1, true); // clean screen
    WriteToLCD(128 + index_change_me, true);
    WriteToLCD(change_me[index_change_me], false);
    index_change_me = (index_change_me + 1) % SIZE_CHANGE_ME;
}*/

int main() {

    GPIO_init();
	init_LCD();
	OneWire_Reset();

    //set cg ram
    for(int i=0;i<2;++i){
        for(int j=0;j<8;++j){
            WriteToLCD(64 + (i<<3) + j, true);
            WriteToLCD(cgram[i][j], false);
        }
    }


	SysTick_Config(1300000UL);
	while(1){
        if(bottom_clicked() == true){
            index_change_me = 0;
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
