#include "stm32l476xx.h"
#include "core_cm4.h"
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int bool;

#define false 0
#define true 1

extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

bool pressed[4][4];

bool button_pushed = false;
bool init_button_pressed_fuck_off = false;
int last_value = 0;
bool need_to_be_shut_up = false;

int activated_row = 0;
const int keys[4][4]={
    {15,7,4,1},
    {0,8,5,2},
    {14,9,6,3},
    {13,12,11,10}
};

void GPIO_init(){	//PB3,PB4,PB5	//7-segment
    RCC->AHB2ENR |= 0x7;
    GPIOB->MODER = 0b11111111111111111111010101111111;
    GPIOB->OSPEEDR = 0b101010000000;
    GPIOB->PUPDR = 0b11111111111111111111000000111111;

    GPIOC->MODER &= 0b11110011111111111111111111111111;//PA13
    GPIOC->OSPEEDR |= 0b1100000000000000000000000000;
    GPIOC->PUPDR |= 0b1000000000000000000000000000;

    GPIOA->MODER &= 0xFFFFFFFE;
    GPIOA->OSPEEDR |= 0b10;
    GPIOA->AFR[0] |= 0x00000001;
    GPIOA->AFR[0] &= 0xFFFFFFF1;
    GPIOA->AFR[1] |= 0x00000001;
    GPIOA->AFR[1] &= 0xFFFFFFF1;

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
    //Set PC6,7,8,9 as INPUT mode
    GPIOC->MODER=GPIOC->MODER&0xFFF00FFF;
    //set PC6,7,8,9 is Pull-down input
    GPIOC->PUPDR=GPIOC->PUPDR|0xAA000;
    //Set PC6,7,8,9 as medium speed mode
    GPIOC->OSPEEDR=GPIOC->OSPEEDR|0xAA000;
    return;
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
        for(int i=0;i<8;++i)
            max7219_send(i+1, 0xF);
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

void scan(int column){
    int position_r=column+6;
    int flag_keypad_r=GPIOC->IDR&1<<position_r;
    if(flag_keypad_r!=0){
        display(keys[(activated_row+3)%4][column]);
    }
    return;
}

void exti_config(void){	//input: PA6,7,8,9
	RCC->APB2ENR = 1;
    SYSCFG->EXTICR[1] = 0x2200;
    SYSCFG->EXTICR[2] = 0x0022;
    SYSCFG->EXTICR[3] = 0x0020;

    EXTI->IMR1 |= (0b10001111 << 6);
    EXTI->RTSR1 = 0;
    EXTI->FTSR1 |= (0b10001111 << 6);	//falling
    NVIC->ISER[0] |= (1<<EXTI9_5_IRQn);
    NVIC->ISPR[0] |= (1<<EXTI9_5_IRQn);
    NVIC->ISER[1] |= (1<<(EXTI15_10_IRQn-32));
    NVIC->ISPR[1] |= (1<<(EXTI15_10_IRQn-32));
    NVIC->IP[3] |= 0xFF00;

    return;
}

void EXTI9_5(void){
    EXTI->PR1 = 0xF << 6;

    return;
}

void EXTI15_10(void){
    EXTI->PR1 = 1 << 13;
    if(init_button_pressed_fuck_off == true && last_value != 0)
        button_pushed = true;
    if(need_to_be_shut_up == true){
        button_pushed = false;
        need_to_be_shut_up = false;
        TIM2->CR1 &= ~TIM_CR1_CEN;
    }
    return;
}

void clock_init(){ // 10Mhz
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


void InitializeTimer(uint32_t presc){

	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->CR1 &= 0xFFFFFF8F; // count up & Edge-aligned
    TIM2->CR1 |= 0x80;

	TIM2->ARR = (uint32_t)100;//Reload value
	TIM2->PSC = (uint32_t)presc;//Prescaler
	TIM2->EGR = TIM_EGR_UG;//Reinitialize the counter

    TIM2->CCMR1 |= 6 << 4;
    TIM2->CCMR1 |= 1 << 3;

    TIM2->CCR1 = 50;

    TIM2->CCER |= 1;
    TIM2->BDTR |= 1<<15;
    //TIM2->CR1 |= TIM_CR1_CEN;
}


void SysTick_Handler(){

    if(button_pushed == true){
        last_value = last_value - 1 >= 0 ? last_value-1: 0;
        display(last_value);
        if(last_value == 0){
            TIM2->CR1 |= TIM_CR1_CEN;
            need_to_be_shut_up = true;
        }
    }


    return;
}

int keypad_scan(){
    bool yes_return = false;
    int flag_keypad=GPIOC->IDR&0xF<<6;
    if(flag_keypad!=0){
        int flag_debounce = 1;
        if(flag_debounce!=0){
            for(int i=0;i<4;i++){ //scan keypad from first column
                int position_c=i+8;
                if(i==3)position_c++;
                //set PA8,9,10,12(column) low and set pin high from PA8
                GPIOA->ODR=(GPIOA->ODR&0xFFFFE8FF)|1<<position_c;
                for(int j=0;j<4;j++){ //read input from first row
                    int position_r=j+6;
                    int flag_keypad_r=GPIOC->IDR&1<<position_r;
                    if(flag_keypad_r!=0){
                        yes_return = true;
                        pressed[i][j] = true;
                    }
                    else
                        pressed[i][j] = false;
                }
            }
        }
        GPIOA->ODR=GPIOA->ODR|10111<<8; //set PA8,9,10,12(column) high
        return 1;
    }
    return -1;
}

void SystemClock_Config(){
 //TODO: Setup system clock and SysTick timer interrupt

	SysTick->CTRL |= (uint32_t)0x00000001; //Set ENABLE to 1b - enable SYSTICK.

	SysTick->CTRL |= (uint32_t)0x00000004; //Set CLKSOURCE to 1b - processor clock (0b - processor clock / 16)
	SysTick->CTRL |= (uint32_t)0x00000002; //Set TICKINT to 1b - enable interrupt on wrap.
	SysTick->LOAD &= (uint32_t)0xFF000000;
	SysTick->LOAD |= (uint32_t)10000000; //Set Reload Register to 48000d -> 1ms.
										  //so *500 => 24000000d-> 500ms=0.5sec

	//SysTick->CTRL &= ~(uint32_t)0x00000001; //Clear ENABLE to 1b - enable SYSTICK.
}

int main(){
	InitializeTimer(153);
    GPIO_init();
    keypad_init();
    max7219_init();
    exti_config();

	clock_init();
	SystemClock_Config();
    display(999);

    while(1){
        init_button_pressed_fuck_off = true;
        if(button_pushed == false){
            if(keypad_scan() == 1){
                for(int i=0;i<4;++i){
                    for(int j=0;j<4;++j){
                        if(pressed[i][j] == true){
                            last_value = keys[i][j];
                            display(keys[i][j]);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
