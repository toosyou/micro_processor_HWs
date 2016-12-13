#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>
//#include "stm32l4xx_ll_exti.h"

typedef unsigned int bool;

#define false 0
#define true 1
#define DEBOUNCE_NUMBER 500

extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

bool pressed[4][4];

int activated_row = 0;
const int keys[4][4]={
    {1,2,3,0},
    {4,5,6,0},
    {7,8,9,0},
    {0,0,0,0}
};


void GPIO_init(){
    RCC->AHB2ENR |= 0x2;
    GPIOB->MODER = 0b11111111111111111111010101111111;
    GPIOB->OSPEEDR = 0b101010000000;
    GPIOB->PUPDR = 0b11111111111111111111000000111111;
}

void keypad_init(){
    // SET keypad gpio OUTPUT //
    RCC->AHB2ENR = RCC->AHB2ENR|0x5;
    //Set PA8,9,10,12 as output mode
    GPIOA->MODER= GPIOA->MODER&0xFDD5FFFF;
    //set PA8,9,10,12 is Pull-up output
    GPIOA->PUPDR=GPIOA->PUPDR|0x1150000;
    //Set PA8,9,10,12 as medium speed mode
    GPIOA->OSPEEDR=GPIOA->OSPEEDR|0x33F0000;
    //Set PA8,9,10,12 as high
    GPIOA->ODR=GPIOA->ODR|10111<<8;
    // SET keypad gpio INPUT //
    //Set PC5,6,7,8 as INPUT mode
    GPIOC->MODER=GPIOC->MODER&0xFFFC03FF;
    //set PC5,6,7,8 is Pull-down input
    GPIOC->PUPDR=GPIOC->PUPDR|0x2A800;
    //Set PC5,6,7,8 as medium speed mode
    GPIOC->OSPEEDR=GPIOC->OSPEEDR|0x3FC00;

    return;
}

void clock_init(){ // 1Mhz

  // hsi on and switch to hsi
  RCC->CR |= RCC_CR_HSION;
  while((RCC->CR & RCC_CR_HSIRDY) == 0); // check hsi ready
  RCC->CFGR |= 0b01;
  RCC->CFGR &= 0xFFFFFFF1;
  while ((RCC->CFGR & 0b11) != 0b01);

  // Turn PLL off
    RCC->CR &= ~RCC_CR_PLLON;
    while(RCC->CR & RCC_CR_PLLRDY);

  RCC->CFGR |= 0b1010<<4;
  RCC->CFGR &= 0xFFFFFFAF;

  RCC->PLLCFGR |= 0b001000000000001010000010010;
  RCC->PLLCFGR &= 0b11111001111111111001010010011110;

  RCC->CR |= RCC_CR_PLLON;
  while((RCC->CR & RCC_CR_PLLRDY) == 0);

  RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL);

  return;
}


void SystemClock_Config(){
 //TODO: Setup system clock and SysTick timer interrupt

	SysTick->CTRL |= (uint32_t)0x00000001; //Set ENABLE to 1b - enable SYSTICK.

	SysTick->CTRL |= (uint32_t)0x00000004; //Set CLKSOURCE to 1b - processor clock (0b - processor clock / 16)
	SysTick->CTRL |= (uint32_t)0x00000002; //Set TICKINT to 1b - enable interrupt on wrap.
	SysTick->LOAD &= (uint32_t)0xFF000000;
	SysTick->LOAD |= (uint32_t)1000000; //Set Reload Register to 48000d -> 1ms.
										  //so *500 => 24000000d-> 500ms=0.5sec

	//SysTick->CTRL &= ~(uint32_t)0x00000001; //Clear ENABLE to 1b - enable SYSTICK.
}

void display(int data){

	if(data / 100000000 != 0)
		return ;

    if(data == -1){
        for(int i=0;i<8;++i)
            max7219_send(i+1, 0xF);
        return;
    }
    if(data == -2){
        for(int i=0;i<8;++i)
            max7219_send(i+1, 0xF);
        max7219_send(1, 1);
        max7219_send(2, 0b1010);
        return;
    }
    if(data == 0){
        max7219_send(1, 0);
        return;
    }
    int index = 0;
    while(data != 0){
        max7219_send(index+1, data%10);
        data /= 10;
        index++;
    }

    for(;index<8;++index)
        max7219_send(index+1, 0xF);

	return ;
}

void exti_config(void){
	RCC->APB2ENR = 1;
    SYSCFG->EXTICR[1] = 0x2220;
    SYSCFG->EXTICR[2] = 0x0002;

    EXTI->IMR1 |= (0xF << 5);
    EXTI->RTSR1 = 0;
    EXTI->FTSR1 |= (0xF << 5);	//falling
    NVIC->ISER[0] |= (1<<EXTI9_5_IRQn);
    NVIC->ISPR[0] = (1<<EXTI9_5_IRQn);

    return;
}

void scan(int column){
    int position_r=column+6;
    int flag_keypad_r=GPIOB->IDR&1<<position_r;
    if(flag_keypad_r!=0){
        display(keys[activated_row][column]);
    }
    return;
}

int keypad_scan(){
	for(int i=0;i<4;i++){ //scan keypad from first column
	                    int position_c=i+8;
	                    if(i==3)position_c++;
	                    //set PA8,9,10,12(column) low and set pin high from PA8
	                    	GPIOA->ODR=(GPIOA->ODR&0xFFFFE8FF)|1<<position_c;
	                    for(int j=0;j<4;j++){ //read input from first row
	                        int position_r=j+5;
	                        if(j==3) position_r++;
	                        int flag_keypad_r=GPIOB->IDR&1<<position_r;
	                        if(flag_keypad_r!=0)display(keys[j][i]);
	                    }

						}
}


void EXTI9_5(void)
{
	scan(0);
	scan(1);
	scan(2);
	scan(3);

    return;
}
/*
void exti0(void){
	//EXTI->PR1 = 1<<0;
    display(6);
    //scan(0);
    return;
}
void exti1(void){
	//EXTI->PR1 = 1<<1;
    display(7);
    //scan(1);
}
void exti2(void){
	//EXTI->PR1 = 1<<2;
    display(8);
    //scan(2);
}
void exti3(void){
	//EXTI->PR1 = 1<<3;
    display(9);
    //scan(3);
}

*/

void SysTick_Handler(void) {
    activated_row = 0;
    int position_c=activated_row+8;
    if(activated_row==3)position_c++;
    GPIOA->ODR |= 0b1011100000000;
    //GPIOA->ODR=(GPIOA->ODR&0xFFFFE8FF)|1<<position_c;
    //activated_row = (activated_row+1)%4;
	return;
}

int main(){
    GPIO_init();
    keypad_init();
    max7219_init();
    exti_config();

	clock_init();
	SystemClock_Config();
    display(5);
    //exti1();
    EXTI9_5();
    while(1){
    }


}
