#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>

extern void GPIO_init(void);

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
	SysTick->LOAD |= (uint32_t)5000000; //Set Reload Register to 48000d -> 1ms.
										  //so *500 => 24000000d-> 500ms=0.5sec

	//SysTick->CTRL &= ~(uint32_t)0x00000001; //Clear ENABLE to 1b - enable SYSTICK.
}
void SysTick_Handler(void) {
	static int on_or_not = 0;
 	//TODO: Toggle the LED
	if(on_or_not == 1)
		GPIOA->ODR |= 0b100000;
	else
		GPIOA->ODR &= ~0b100000;

	on_or_not = 1 - on_or_not;
	return;
}

int user_press_button(){
	static int debounce = 0;
	if( (GPIOC->IDR & 0b0010000000000000) == 0){ // pressed
		debounce = debounce >= 1 ? 1 : debounce+1;
		return 0;
	}
	else if( debounce >= 1 ){
		debounce = 0;
		return 1;
	}
	return 0;
}


int main(){
	clock_init();
	SystemClock_Config();
	GPIO_init();

	while(1){
		if(user_press_button()){
			//TODO: Enable or disable Systick timer

		}
	}
}
