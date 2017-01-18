#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "adc.h"
#include "util.h"
#include "74HC595.h"
#include "util.h"
#define MAX_SEND_COUNT 1000
#define MAX_MOVING_AVERAGE 5
#define TIM3_ARR 99999

typedef unsigned int bool;
#define false 0
#define true 1

extern void delay(int ms);

float moving_average[MAX_MOVING_AVERAGE];
int command_cnt[50];

float voltage = 0.0f;
int send_count = 0;
int led_mode = -1;
bool command_started = false;

float voltage_max = 2000.0f;
float voltage_min = 1990.0f;

int mode3_delay_time = 80000;

bool mode5_blinked = false;

const int led_sequence[16] = {0, 1, 2, 3, 4, 5, 6, 7, 15, 9, 14, 13, 12, 11, 10, 8};

void send_to_shoes(HC595 ic, unsigned int data);

float get_moving_average(int new_voltage){
	float average = 0.0f;
	for(int i=MAX_MOVING_AVERAGE-1;i>0;--i){
		moving_average[i] = moving_average[i-1];
		average += moving_average[i];
	}
	moving_average[0] = new_voltage;
	average += new_voltage;
	return average / MAX_MOVING_AVERAGE;
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

	float new_voltage = (float)read_adc(ADC1) - voltage_min;
	new_voltage /= voltage_max - voltage_min;
	new_voltage = new_voltage > 1.0f ? 1.0f : new_voltage;
	new_voltage = new_voltage < 0.0f ? 0.0f : new_voltage;
	voltage = 0.95f* get_moving_average(new_voltage) + new_voltage*0.05f;
	delay(100);
	return;
}

void GPIO_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

	//pa0
	gpio_set_alternate(GPIOA, 0);
	gpio_set_high_speed(GPIOA, 0);
	GPIOA->AFR[0] |= 0x00000001;
	GPIOA->AFR[0] &= 0xFFFFFFF1;

	// BUTTON	//PC13
	GPIOC->MODER &= ~(0b11 << (13*2));
	GPIOC->MODER |= (0b00 <<(13*2));
	GPIOC->OSPEEDR &= ~(0b11 << (13*2));
	GPIOC->OSPEEDR |= ~(0b01 << (13*2));
	GPIOC->OTYPER &= ~(1 << 13);
	GPIOC->PUPDR &= ~(0b11 << (13 * 2));

	//pc3 output
	gpio_set_input(GPIOC, 3);
	gpio_set_high_speed(GPIOC, 3);
	gpio_set_pull_up(GPIOC, 3);
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

void InitializeTimer(uint32_t presc){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->CR1 &= 0xFFFFFF8F; // count up & Edge-aligned
    TIM2->CR1 |= 0x80;

	TIM2->ARR = (uint32_t)256;//Reload value
	TIM2->PSC = (uint32_t)presc;//Prescaler
	TIM2->EGR = TIM_EGR_UG;//Reinitialize the counter

    TIM2->CCMR1 |= 6 << 4;
    TIM2->CCMR1 |= 1 << 3;

    TIM2->CCR1 = 500;

    TIM2->CCER |= 1;
    TIM2->BDTR |= 1<<15;
    TIM2->CR1 |= TIM_CR1_CEN;
}

void send_to_shoes(HC595 ic, unsigned int data){
	if(send_count > MAX_SEND_COUNT){
		send_count = 0;
		HC595Reset(ic);
	}

	int most_significant_8bits = 0;
	int least_significant_8bits = 0;
	int output_sequence[16];
	for(int i=0;i<16;++i){
		int index = led_sequence[i];
		if(i < 10)
			output_sequence[index] = ((data >> i) & 1) == 1 ? 0 : 1 ;
		else
			output_sequence[index] = (data >> i) & 1;
	}
	for(int i=0;i<8;++i){
		most_significant_8bits = (most_significant_8bits << 1) + output_sequence[15-i];
		least_significant_8bits = (least_significant_8bits << 1) + output_sequence[7-i];
	}

	HC595Send(ic, most_significant_8bits, false);
	HC595Send(ic, least_significant_8bits, true);
	send_count++;
	return;
}

void init_moving_average(void){
	for(int i=0;i<MAX_MOVING_AVERAGE;++i){
		moving_average[i] = 0.0f;
	}
	return;
}

void init_ir_timer(void){
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN;
	TIM3->CR1 &= ~TIM_CR1_DIR; // upcounting
	TIM3->PSC = 39; // 0.01 ms
	TIM3->ARR = TIM3_ARR; // 100 ms total
	TIM3->EGR |= TIM_EGR_UG;
	TIM3->CR1 |= TIM_CR1_CEN; // start
}

void timer_start(){
	TIM3->CR1 |= TIM_CR1_CEN;
}
void timer_stop(){
	TIM3->CR1 &= ~TIM_CR1_CEN;
}


void change_mode(int command){
	if(command == 2){ // OK
		if(led_mode == -1)
			led_mode = 0;
		else
			led_mode = -1;
	}
	else if(command == 104) // 1
		led_mode = 1;
	else if(command == 152) // 2
		led_mode = 2;
	else if(command == 176) // 3
		led_mode = 3;
	else if(command == 48) // 4
		led_mode = 4;
	else if(command == 24){ // 5
		led_mode = 5;
		mode5_blinked = false;
	}
	else if(command == 66) // *
		led_mode = 10;
	else if(command == 74){ // 0
		voltage_max = 2000.0f;
		voltage_min = 1990.0f;
		mode3_delay_time = 80000;
	}
	else if(command == 82) // #
		led_mode = 11;
	else if(command == 168){ // down
		voltage_max = voltage_max > 2039.0f ? voltage_max : voltage_max + 1;
		voltage_min = voltage_min > 2029.0f ? voltage_min : voltage_min + 1;
	}
	else if(command == 98){ // up
		voltage_max = voltage_max < 1970.0f ? voltage_max : voltage_max - 1;
		voltage_min = voltage_min < 1960.0f ? voltage_min : voltage_min - 1;
	}
	else if(command == 194){ // right
		if(led_mode == 3 || led_mode == 4)
			mode3_delay_time = mode3_delay_time <= 10000 ? mode3_delay_time : mode3_delay_time - 10000;
	}
	else if(command == 34){ // left
		if(led_mode == 3 || led_mode == 4)
			mode3_delay_time = mode3_delay_time >= 150000 ? mode3_delay_time : mode3_delay_time + 10000;
	}
	return;
}

int decode_command(){
	const int threshold_1 = 150;
	int addr = 0;
	int addr_cs = 0;
	int data = 0;
	int data_cs = 0;
	for(int i=0;i<8;++i){
		addr = (addr << 1) | ( command_cnt[i+2] > threshold_1 ? 1 : 0 );
		addr_cs = (addr_cs << 1) | ( command_cnt[i+10] > threshold_1 ? 1 : 0 );
	}
	for(int i=0;i<8;++i){
		data = (data << 1) | ( command_cnt[i+18] > threshold_1 ? 1 : 0 );
		data_cs = (data_cs << 1) | ( command_cnt[i+26] > threshold_1 ? 1 : 0 );
	}
	if(command_cnt[1] < 450 || command_cnt[1] > 550 || addr + addr_cs != 0xFF || data + data_cs != 0xFF)
		return -1;
	return (addr << 8) + data;
}

void EXTI3_IRQHandler(void){
	static unsigned int last_cnt = 0;
	static unsigned int rising_cnt = 0;

	unsigned int now_cnt = TIM3->CNT;
	int diff = now_cnt - last_cnt;
	if(diff < 60 && diff > 0){ // noise
		EXTI->PR1 = EXTI_PR1_PIF3_Msk;
		return;
	}
	command_cnt[rising_cnt++] = diff < 0 ? diff + TIM3_ARR : diff;
	last_cnt = now_cnt;

	if(rising_cnt == 2 && (command_cnt[1] < 450 || command_cnt[1] > 550) ){ // not start signal
		EXTI->PR1 = EXTI_PR1_PIF3_Msk;
		rising_cnt = 0;
		return;
	}

	if(rising_cnt == 34){ // command end
		rising_cnt = 0;
		int command = decode_command();
		change_mode(command);
	}

	EXTI->PR1 = EXTI_PR1_PIF3_Msk;
	return;
}

void EXTISetup(){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI3_PC;
	EXTI->IMR1 |= EXTI_IMR1_IM3;
	EXTI->RTSR1 |= EXTI_RTSR1_RT3;
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_SetPriority(EXTI3_IRQn, 0);
	NVIC_SetPriority(SysTick_IRQn, 1);
	NVIC_SetPriority(ADC1_2_IRQn, 2);
}


void init_ir(void){
	for(int i=0;i<50;++i){
		command_cnt[i] = 0;
	}
	init_ir_timer();
	EXTISetup();
}

int reverse15bit(int input){
	int rtn = 0;
	for(int i=0;i<15;++i){
		rtn = (rtn << 1) | (input & 1);
		input >>= 1;
	}
	return rtn;
}

int main(){
	SCB->CPACR |= (0xF << 20);
	init_moving_average();
	InitializeTimer(20);
	GPIO_Init();
	SysTickConfig(40000);
	usart_init();
	ADC_init();
	init_ir();
	HC595 ic;
	ic.GPIO = GPIOA;
	ic.clk_pin = 5;
	ic.cs_pin = 6;
	ic.data_pin = 7;
	ic.reset_pin = 8;
	HC595Init(ic);
	TIM2->CCR1 = 0;
	send_to_shoes(ic, 0xFFFF);
	while(1){
		if(led_mode == -1)
			TIM2->CCR1 = 256;
		else if(led_mode == 0){
			TIM2->CCR1 = 0; // full light
			send_to_shoes(ic, 0xFFFF);
			delay(500000);
			int now = 0;
			for(int i=0;i<15&&led_mode == 0;++i){
				for(int j=0;j<15-i&&led_mode == 0;++j){
					send_to_shoes(ic, now + (1<<j));
					delay(30000);
				}
				now += 1<<(14-i);
			}
			if(led_mode == 0)
				led_mode = 1;
		}
		else if(led_mode == 1){
			TIM2->CCR1 = (int)(256.0f * (1 - voltage) );
			send_to_shoes(ic, 0xFFFF);
			delay(10000); // 10 ms
		}
		else if(led_mode == 2){
			TIM2->CCR1 = 0;
			unsigned int level = (unsigned int)(60.0f * voltage);
			level = level > 15 ? 15 : level;
			send_to_shoes(ic, (1<<level) -1 );
			delay(10000); // 10 ms
		}
		else if(led_mode == 3){
			static int shift = 0;
			TIM2->CCR1 = 0;
			unsigned int pattern = 0b0011001100110011;
			send_to_shoes(ic, pattern << shift);
			delay(mode3_delay_time); // 80 ms default
			shift = (shift + 1) % 4;
		}
		else if(led_mode == 4){
			static int shift = 0;
			static int direction = 1;
			TIM2->CCR1 = 0;
			shift += direction;
			if(shift == 16){
				direction = -1;
				shift = 14;
			}
			if(shift == -1){
				direction = 1;
				shift = 0;
			}
			send_to_shoes(ic, 0b11 << shift);
			delay(mode3_delay_time);
		}
		else if(led_mode == 5){
			if(mode5_blinked == false){
				send_to_shoes(ic, 0xFFFF);
				TIM2->CCR1 = 0;
				delay(80000); // 80 ms
				mode5_blinked = true;
			}
			TIM2->CCR1 = 255;
			delay(50000); // 50 ms
		}
		else if(led_mode == 10){
			TIM2->CCR1 = 0;
			send_to_shoes(ic, 0xFFFF);
			delay(100000); // 100 ms
		}
		else if(led_mode == 11){
			TIM2->CCR1 = 256;
			send_to_shoes(ic, 0xFFFF);
			delay(100000); // 100 ms
		}
	}
	return 0;
}
