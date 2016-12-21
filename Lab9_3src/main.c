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

int initial = 1;
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

void display_temp(float temp);

void display_41();

void SysTick_Handler(void) {
    if(mode == 0){
        display_41();
        SysTick->LOAD = 3000000;
        SysTick->VAL = 3000000;
    }
    else{
        float temp = read_temperature();
    	display_temp(temp);
        SysTick->LOAD = 10000000;
        SysTick->VAL = 10000000;
    }
}

void display_temp(float temp){
    WriteToLCD(128 + 0x40, true);
    temp /= 100.0f;
    for(int i=0;i<3;++i){
        WriteToLCD( 0x30 + (int)temp%10, false );
        temp *= 10.0f;
    }
    WriteToLCD( 0x2E, false);
    for(int i=0;i<7;++i){
        WriteToLCD( 0x30 + (int)temp%10, false );
        temp *= 10.0f;
    }
    return ;
}

void SystemClock_Config(){
	//TODO: Setup system clock and SysTick timer interrupt
	/*set system clock 10mHz*/
	//turn on MSI at first time
	if(initial == 1){
		initial = 0;
		RCC->CR |= RCC_CR_MSION;
		while((RCC->CR & RCC_CR_MSIRDY) == 0);
	}
	//switch output to MSI
	RCC->CFGR &= (0 << RCC_CFGR_SW_Pos);
	//turn off PLL
	RCC->CR &= 0xFEFFFFFF;
	while((RCC->PLLCFGR & 0xFFFFFFF) != 0){
		RCC->PLLCFGR &= 0;
	}
	//set PLL source
	RCC->PLLCFGR |= (1 << RCC_PLLCFGR_PLLSRC_Pos);
	//set PLL frequency
	RCC->PLLCFGR |= (10 << RCC_PLLCFGR_PLLN_Pos);
	RCC->PLLCFGR |= (0 << RCC_PLLCFGR_PLLM_Pos);
	RCC->PLLCFGR |= (1 << RCC_PLLCFGR_PLLR_Pos);
	//set PLL/R as output
	RCC->PLLCFGR |= (1 << RCC_PLLCFGR_PLLREN_Pos);
	//turn on PLL
	RCC->CR |= RCC_CR_PLLON;
	while((RCC->CR & RCC_CR_PLLRDY) == 0);
	//set system clock output
	RCC->CFGR |= (3 << RCC_CFGR_SW_Pos);

	//clear current value
	SysTick->LOAD = 3000000;
	SysTick->VAL = 3000000;
	SCB->ICSR |= SCB_ICSR_PENDSTSET_Pos;
	SysTick->CTRL = 7;
}

void GPIO_init(void){
    RCC->AHB2ENR |= 6;
    GPIOC->MODER &= 0b11110011111111110101010101010101;
    GPIOC->PUPDR =  0b11111111111111110101010101010101;
    GPIOC->OSPEEDR |= 0x4005555;

    GPIOB->MODER &= 0b01010111111111111111111111111111;
    GPIOB->PUPDR =  0b01010111111111111111111111111111;
    GPIOB->OSPEEDR |= 0x54000000;
	GPIOB->OTYPER &= (1 << 10);
	//pb3 01
	//GPIOB->OSPEEDR |= (1<<(10*2));
	//GPIOB->OSPEEDR &= ~(1<<(10*2+1));

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
	DS18B20_ConvT(&one_wire_structure, TM_DS18B20_Resolution_11bits);
	while(DS18B20_Done(&one_wire_structure));
	DS18B20_Read(&one_wire_structure, &rtn, TM_DS18B20_Resolution_11bits);

	return rtn;
}

int main() {

	//copied from the Net.
	SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
    GPIO_init();
	init_LCD();
	SystemClock_Config();

	//set cg ram
    for(int i=0;i<2;++i){
        for(int j=0;j<8;++j){
            WriteToLCD(64 + (i<<3) + j, true);
            WriteToLCD(cgram[i][j], false);
        }
    }


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
