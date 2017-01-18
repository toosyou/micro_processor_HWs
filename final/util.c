#include "util.h"

void gpio_set_output(GPIO_TypeDef *GPIO, int pin){
    // 01
    GPIO->MODER &= ~( 0b11 << (pin*2));
    GPIO->MODER |= ( 0b01 << (pin*2));
    return;
}

void gpio_set_input(GPIO_TypeDef *GPIO, int pin){
    // 00
    GPIO->MODER &= ~( 0b11 << (pin*2));
    GPIO->MODER |= ( 0b00 << (pin*2));
    return;
}

void gpio_set_alternate(GPIO_TypeDef *GPIO, int pin){
    // 10
    GPIO->MODER &= ~( 0b11 << (pin*2));
    GPIO->MODER |= ( 0b10 << (pin*2));
    return;
}

void gpio_set_high_speed(GPIO_TypeDef *GPIO, int pin){
    // 10
    GPIO->OSPEEDR &= ~( 0b11 << (pin*2));
    GPIO->OSPEEDR |= ( 0b10 << (pin*2));
    return;
}

void gpio_set_pull_up(GPIO_TypeDef *GPIO, int pin){
    // 01
    GPIO->PUPDR &= ~( 0b11 << (pin*2));
    GPIO->PUPDR |= ( 0b01 << (pin*2));
    return;
}

void gpio_set_pull_down(GPIO_TypeDef *GPIO, int pin){
    // 10
    GPIO->PUPDR &= ~( 0b11 << (pin*2));
    GPIO->PUPDR |= ( 0b10 << (pin*2));
    return;
}

void gpio_set_push_pull(GPIO_TypeDef *GPIO, int pin){
    GPIO->OTYPER &= ~(1 << pin);
    return;
}
