#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>


int UART_Transmit(uint8_t *arr, uint32_t size) {
	//TODO: Send str to UART and return how many bytes are successfully transmitted.

}

void init_UART() {
	// Initialize UART registers
}

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
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
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
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

	// BUTTON	//PC13
	GPIOC->MODER &= ~(0b11 << (13*2));
	GPIOC->MODER |= (0b01 <<(13*2));
	GPIOC->OSPEEDR &= ~(0b11 << (13*2));
	GPIOC->OSPEEDR |= ~(0b01 << (13*2));
	GPIOC->OTYPER &= ~(1 << 13);
	GPIOC->PUPDR &= ~(0b11 << (13 * 2));
}
int button_debounce(){
	int cnt = 0;
	for(int i=500;i--; )
		cnt += (GPIOC->IDR >> 13) & 1;
	return cnt < 100;
}

typedef struct{

};

void USART_Init(void) {

	/* Enable clock for USART??? */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
	USART_SetBaudRate(USART1,int cpuRate 4, int baudRate);
	USARTSetWordLength(USART1,8);
	USARTSetStopBit(USART1,0);


	// CR1
	MODIFY_REG(USART1->CR1, USART_CR1_M);
	// CR2
	MODIFY_REG(USART???->CR2, USART_CR2_STOP, 0x0); // 1-bit stop
	// CR3
	MODIFY_REG(USART???->CR3, (USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT), 0x0); // none hwflowctl
	MODIFY_REG(USART???->BRR, 0xFF, 4000000L/???? L);
	/* In asynchronous mode, the following bits must be kept cleared:
	- LINEN and CLKEN bits in the USART_CR2 register,
	- SCEN, HDSEL and IREN bits in the USART_CR3 register.*/
	USART???->CR2 &= ~(USART_CR2_LINEN | USART_CR2_CLKEN);
	USART???->CR3 &= ~(USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN);
	// Enable UART
	USART???->CR1 |= (USART_CR1_UE);
}

void ONE(){
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
	GPIOC->MODER &= ~(0b11 << (13 * 2));
	int last = 0;	//?
	while(1){
		int x = botton_debounce();
		if(x == 1 && last == 0){
			USARTPrintString(USART1, "Hello World!");
		}
		last = x;
	}
}

int main(){
	//SCB->CPACR |= (0xF << 20);

	__DSB();
	__ISB();
	GPIO_init();
	USART_init();
	//ADCInit();
	SysTickConfig(40000);
	ONE();
	while(1);
	return 0;
}
