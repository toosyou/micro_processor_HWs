#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>

void SystemClock_Config(){
 //TODO: Setup system clock and SysTick timer interrupt

	SysTick->CTRL |= (uint32_t)0x00000001; //Set ENABLE to 1b - enable SYSTICK.

	SysTick->CTRL |= (uint32_t)0x00000004; //Set CLKSOURCE to 1b - processor clock (0b - processor clock / 16)
	SysTick->CTRL |= (uint32_t)0x00000002; //Set TICKINT to 1b - enable interrupt on wrap.
	SysTick->LOAD &= (uint32_t)0xFF000000;
	SysTick->LOAD |= (uint32_t)0x16E3600; //Set Reload Register to 48000d -> 1ms.
										  //so *500 => 24000000d-> 500ms=0.5sec


	SysTick->CTRL &= ~(uint32_t)0x00000001; //Clear ENABLE to 1b - enable SYSTICK.
}
void SysTick_Handler(void) {
 //TODO: Toggle the LED

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
	SystemClock_Config();
	GPIO_init();
	while(1){
		if(user_press_button())
			{

				//TODO: Enable or disable Systick timer
			}
	}
}

