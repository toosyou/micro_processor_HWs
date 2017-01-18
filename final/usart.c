#include "usart.h"

void set_baud_rate_uart(USART_TypeDef *usart, int fck, int baud){
	usart->BRR = (fck + baud / 2) / baud;
	return;
}

void set_length_stop_uart(USART_TypeDef *usart, int length){
	//0 -> 1
	//1 -> 0.5
	//2 -> 2
	//3 -> 1.5
	usart->CR2 &= ~USART_CR2_STOP_Msk;
	usart->CR2 |= length << USART_CR2_STOP_Pos;
	return;
}

void enable_rt_usart(USART_TypeDef *usart, int rx, int tx){
	usart->CR1 &= ~(3 << 2); // clear
	usart->CR1 |= rx << USART_CR1_RE_Pos;
	usart->CR1 |= tx << USART_CR1_TE_Pos;
	return;
}

void enable_usart(USART_TypeDef *usart){
	usart->CR1 |= USART_CR1_UE;
	return;
}


void set_length_word_uart(USART_TypeDef *usart, int length){
	if(length == 7){ // 10
		usart->CR1 |= USART_CR1_M1;
		usart->CR1 &= ~USART_CR1_M0;
	}
	else if(length == 8){ // 00
		usart->CR1 &= ~USART_CR1_M1;
		usart->CR1 &= ~USART_CR1_M0;
	}
	else if(length == 9){ // 01
		usart->CR1 &= ~USART_CR1_M1;
		usart->CR1 |= USART_CR1_M0;
	}
	return;
}


int flag_status_usart(USART_TypeDef *usart, int flag){
	return usart->ISR & flag;
}

int UART_Transmit(USART_TypeDef *usart, char *arr, uint32_t size) {

	for(unsigned int i=0;i<size;++i){
		//Transmit data register empty
		while(!flag_status_usart(usart, USART_ISR_TXE));

		usart->TDR = arr[i];
		//Transmission complete
		while(!flag_status_usart(usart, USART_ISR_TC));
	}

	return 0;
}


void usart_init(void) {


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

	set_baud_rate_uart( USART1, 4000000, 9600);
	set_length_word_uart( USART1, 8);
	set_length_stop_uart( USART1, 0); // 1 bit
	enable_rt_usart(USART1, 1, 1);
	enable_usart(USART1);
}
