#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int bool;
#define false 0
#define true 1

void SysTickConfig(int tick){
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
	SysTick->LOAD = tick & SysTick_LOAD_RELOAD_Msk;
	SysTick->VAL = 0;
	SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk);
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(){

}

void GPIO_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	// UART //PA9,PA10
	GPIOA->MODER &= ~(0b11 << (9*2));
	GPIOA->MODER &= ~(0b11 << (10*2));
	GPIOA->MODER |= (0b10 <<(9*2));
	GPIOA->MODER |= (0b10 <<(10*2));
	GPIOA->OTYPER &= ~(1 << 9);
	GPIOA->OTYPER &= ~(1 << 10);
	GPIOA->PUPDR &= ~(0b11 << (9 * 2));	//no pull
	GPIOA->PUPDR &= ~(0b11 << (10 * 2));
	GPIOA->AFR[9 >> 3] &= ~(0b1111 << ((9 & 7) << 2));
	GPIOA->AFR[9 >> 3] |= (7 << ((9 & 7) << 2));
	GPIOA->AFR[10 >> 3] &= ~(0b1111 << ((10 & 7) << 2));
	GPIOA->AFR[10 >> 3] |= (7 << ((10 & 7) << 2));
	GPIOA->OSPEEDR &= ~(0b11 << (9*2));
	GPIOA->OSPEEDR &= ~(0b11 << (10*2));

	// BUTTON	//PC13
	GPIOC->MODER &= ~(0b11 << (13*2));
	GPIOC->MODER |= (0b00 <<(13*2));
	GPIOC->OSPEEDR &= ~(0b11 << (13*2));
	GPIOC->OSPEEDR |= ~(0b01 << (13*2));
	GPIOC->OTYPER &= ~(1 << 13);
	GPIOC->PUPDR &= ~(0b11 << (13 * 2));
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

void usart_init(void) {

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

	set_baud_rate_uart( USART1, 4000000, 9600);
	set_length_word_uart( USART1, 8);
	set_length_stop_uart( USART1, 0); // 1 bit
	enable_rt_usart(USART1, 1, 1);
	enable_usart(USART1);
}

void ADC_init(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER |= 0b11;
	GPIOC->ASCR |= 1;
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
	ADC_Resolution(ADC1, 0); // 12 bits
	ADC_ContinuousConversion(ADC1, 0); // enable continuous conversion
	ADC_DataAlign(ADC1, 0); // set right align
	ADC_CommonDualMode(0); // independent mode
	ADC_CommonClockMode(1); // hclk / 1
	ADC_CommonPrescaler(0); //div 1
	ADC_CommonDMAMode(0); // disable dma
	ADC_CommonDelayTwoSampling(0b0100); // 5 adc clk cycle
	ADC_Channel(ADC1, 1, 1, 2); // channel 1, rank 1, 12.5 adc clock cycle
	ADC_Wakeup(ADC1);
	ADC_Interrupt(ADC1, ADC_IER_EOCIE, 1);
	NVIC_EnableIRQ(ADC1_2_IRQn);
	ADC_Enable(ADC1);
}

void TWO(void){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;	//PC enable
	GPIOC->MODER &= ~(0b11 << (13 * 2));	//PC13
	int last = 0;
	while(1){
		if(( bottom_clicked() ) && !last){
			char s[20];
			sprintf(s, "%f\n", resistor);
			USARTPrintString(USART1, s);
		}
		last = x;
	}
}

int main(){
	SCB->CPACR |= (0xF << 20);
	GPIO_Init();
	SysTickConfig(40000);
	usart_init();
	ADC_init();
	TWO();
	/*
	while(1){
		if(bottom_clicked() == true){
			char message[50] = "HeI10, W0r1d!";
			UART_Transmit(USART1, message, strlen(message));
		}
	}
	*/
	return 0;
}
