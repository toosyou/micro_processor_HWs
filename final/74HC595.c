#include "74HC595.h"


void HC595_clk_set(HC595 ic, int high_or_low){
    if(high_or_low == 1)
        ic.GPIO->BSRR = 1 << ic.clk_pin;
    else
        ic.GPIO->BRR = 1 << ic.clk_pin;
    return;
}

void HC595_cs_set(HC595 ic, int high_or_low){
    if(high_or_low == 1)
        ic.GPIO->BSRR = 1 << ic.cs_pin;
    else
        ic.GPIO->BRR = 1 << ic.cs_pin;
    return;
}

void HC595_data_set(HC595 ic, int high_or_low){
    if(high_or_low == 1)
        ic.GPIO->BSRR = 1 << ic.data_pin;
    else
        ic.GPIO->BRR = 1 << ic.data_pin;
    return;
}

void HC595_reset_set(HC595 ic, int high_or_low){
    if(high_or_low == 1)
        ic.GPIO->BSRR = 1 << ic.reset_pin;
    else
        ic.GPIO->BRR = 1 << ic.reset_pin;
    return;
}

void HC595Init(HC595 ic){

    RCC->APB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN;

    // clk
    gpio_set_output(ic.GPIO, ic.clk_pin);
    gpio_set_high_speed(ic.GPIO, ic.clk_pin);
    gpio_set_pull_up(ic.GPIO, ic.clk_pin);
    gpio_set_push_pull(ic.GPIO, ic.clk_pin);

    // data
    gpio_set_output(ic.GPIO, ic.data_pin);
    gpio_set_high_speed(ic.GPIO, ic.data_pin);
    gpio_set_pull_up(ic.GPIO, ic.data_pin);
    gpio_set_push_pull(ic.GPIO, ic.data_pin);

    // cs
    gpio_set_output(ic.GPIO, ic.cs_pin);
    gpio_set_high_speed(ic.GPIO, ic.cs_pin);
    gpio_set_pull_up(ic.GPIO, ic.cs_pin);
    gpio_set_push_pull(ic.GPIO, ic.cs_pin);

    //reset
    gpio_set_output(ic.GPIO, ic.reset_pin);
    gpio_set_high_speed(ic.GPIO, ic.reset_pin);
    gpio_set_pull_up(ic.GPIO, ic.reset_pin);
    gpio_set_push_pull(ic.GPIO, ic.reset_pin);

    HC595Reset(ic);

    HC595_clk_set(ic, 1);
    HC595_data_set(ic, 1);
    HC595_cs_set(ic, 1);

}

void HC595Send(HC595 ic, unsigned int data, bool immediate_send){
    data &= 0xFF;
    for (int i=0; i < 8; i++){
        if( (data & 0b10000000) != 0)
            HC595_data_set(ic, 1);
        else
            HC595_data_set(ic, 0);
        HC595_clk_set(ic, 0);
        delay(5);
        data <<= 1;
        HC595_clk_set(ic, 1);
        delay(5);
    }

    if(immediate_send == true)
        HC595Load(ic);
}

void HC595Load(HC595 ic){
    HC595_cs_set(ic, 0);
    HC595_cs_set(ic, 1);
}

void HC595Reset(HC595 ic){
    HC595_reset_set(ic, 0);
    HC595_reset_set(ic, 1);
    return;
}

//end of file
