#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int bool;

#define false 0
#define true 1
#define DEBOUNCE_NUMBER 500

bool pressed[4][4];

extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

void GPIO_init(){
    RCC->AHB2ENR |= 0x2;
    GPIOB->MODER = 0b11111111111111111111010101111111;
    GPIOB->OSPEEDR = 0b101010000000;
    GPIOB->PUPDR = 0b11111111111111111110000001111111;
}

void keypad_init(){
    // SET keypad gpio OUTPUT //
    RCC->AHB2ENR = RCC->AHB2ENR|0x2;
    //Set PA8,9,10,12 as output mode
    GPIOA->MODER= GPIOA->MODER&0xFDD5FFFF;
    //set PA8,9,10,12 is Pull-up output
    GPIOA->PUPDR=GPIOA->PUPDR|0x1150000;
    //Set PA8,9,10,12 as medium speed mode
    GPIOA->OSPEEDR=GPIOA->OSPEEDR|0x33F0000;
    //Set PA8,9,10,12 as high
    GPIOA->ODR=GPIOA->ODR|10111<<8;
    // SET keypad gpio INPUT //
    //Set PB6,7,8,9 as INPUT mode
    GPIOB->MODER=GPIOB->MODER&0xFFF00FFF;
    //set PB6,7,8,9 is Pull-down input
    GPIOB->PUPDR=GPIOB->PUPDR|0x55000;
    //Set PB6,7,8,9 as medium speed mode
    GPIOB->OSPEEDR=GPIOB->OSPEEDR|0x55000;

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

void SysTick_Handler(void) {

	return;
}

int main(){
    GPIO_init();
    keypad_init();
    max7219_init();

	clock_init();
	SystemClock_Config();
    
    return 0;
}
