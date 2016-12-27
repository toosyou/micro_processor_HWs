#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int bool;
#define false 0
#define true 1

float resistor = 0.0f;

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


void set_resolution_adc(ADC_TypeDef *adc, int resolution){
	// 12 -> 00
	// 10 -> 01
	// 8 -> 10
	// 6 -> 11
	adc->CFGR &= ~ADC_CFGR_RES; //clear
	adc->CFGR |= ((12-resolution)/2) << ADC_CFGR_RES_Pos;
	return;
}

void enable_continuous_convetion_adc(ADC_TypeDef *adc, bool enable){
	if(enable == true){
		adc->CFGR |= ADC_CFGR_CONT;
	}else{
		adc->CFGR &= ~ADC_CFGR_CONT;
	}
	return ;
}

void set_data_align_adc(ADC_TypeDef *adc, bool left){
	// false -> right
	// true -> left
	if(left) // left
		adc->CFGR |= ADC_CFGR_ALIGN;
	else
		adc->CFGR &= ~ADC_CFGR_ALIGN;
	return;
}

void enable_regulator_adc(ADC_TypeDef *adc){
	adc->CR &= ~ADC_CR_DEEPPWD;
	adc->CR |= ADC_CR_ADVREGEN;
	for(int i=0;i<500;++i);//do nothing
	return;
}

void enable_adc(ADC_TypeDef *adc){
	adc->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY)); // wait for enable
	return;
}

int read_adc(ADC_TypeDef *adc){
	return adc->DR;
}

void convert_adc(ADC_TypeDef *adc){
	adc->CR |= ADC_CR_ADSTART;
	return;
}

void set_interrupt_adc(ADC_TypeDef *adc, unsigned int pos_mask, bool enable){
	if(enable)
		adc->IER |= pos_mask;
	else
		adc->IER &= ~pos_mask;
	return;
}

void set_convert_mode_adc(int mode){
	ADC123_COMMON->CCR &= ~ADC_CCR_DUAL;
	ADC123_COMMON->CCR |= mode << ADC_CCR_DUAL_Pos;
	return;
}

void set_clock_mode_adc(int mode){
	ADC123_COMMON->CCR &= ~ADC_CCR_CKMODE;
	ADC123_COMMON->CCR |= mode << ADC_CCR_CKMODE_Pos;
	return;
}

void set_prescaler_adc(int prescaler){
	ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;
	ADC123_COMMON->CCR |= prescaler << ADC_CCR_PRESC_Pos;
}

void set_dma_mode_adc(int mode){
	ADC123_COMMON->CCR &= ~ADC_CCR_MDMA;
	ADC123_COMMON->CCR |= mode << ADC_CCR_MDMA_Pos;
}

void set_delay_adc(int delay_clk){
	ADC123_COMMON->CCR &= ~ADC_CCR_DELAY;
	ADC123_COMMON->CCR |= delay_clk << ADC_CCR_DELAY_Pos;
}

void set_channel_adc(ADC_TypeDef *adc, int channel, int rank, int sampleTime){
	if(rank <= 4){
		adc->SQR1 &= ~(ADC_SQR1_SQ1 << (rank * 6));
		adc->SQR1 |= (channel << (rank * 6));
	}
	if(channel <= 9){
		adc->SMPR1 &= ~(ADC_SMPR1_SMP0 << (channel * 3));
		adc->SMPR1 |= (sampleTime << (channel * 3));
	}
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
	convert_adc(ADC1);
	return;
}

void ADC1_2_IRQHandler(void){
	while(!(ADC1->ISR&ADC_ISR_EOC)); // wait for complete
	float voltage = (float)read_adc(ADC1) / 4096.0f * 5.0f;
	resistor = ( 5.0f - voltage ) * 10000.0f / voltage;
	return;
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
	set_resolution_adc(ADC1, 12); // 12 bits
	enable_continuous_convetion_adc(ADC1, false); // enable continuous conversion
	set_data_align_adc(ADC1, false); // set right align
	set_convert_mode_adc(0); // independent mode
	set_clock_mode_adc(1); // hclk / 1
	set_prescaler_adc(0); //div 1
	set_dma_mode_adc(0); // disable dma
	set_delay_adc(0b0100); // 5 adc clk cycle
	set_channel_adc(ADC1, 1, 1, 2); // channel 1, rank 1, 12.5 adc clock cycle
	enable_regulator_adc(ADC1);
	set_interrupt_adc(ADC1, ADC_IER_EOCIE, 1);
	NVIC_EnableIRQ(ADC1_2_IRQn);
	enable_adc(ADC1);
}

int main(){
	SCB->CPACR |= (0xF << 20);
	GPIO_Init();
	SysTickConfig(40000);
	usart_init();
	ADC_init();
	while(1){
		if(bottom_clicked() == true){
			char message[50] = {0};
			sprintf(message, "%f Ω\t", resistor);
			UART_Transmit(USART1, message, strlen(message));
			sprintf(message, "%f V\t", (float)read_adc(ADC1) / 4096.0f * 5.0f);
			UART_Transmit(USART1, message, strlen(message));
		}
	}
	return 0;
}
