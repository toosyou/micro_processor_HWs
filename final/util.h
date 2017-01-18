#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdlib.h>
#include <stdio.h>
#include "stm32l476xx.h"

void gpio_set_output(GPIO_TypeDef *GPIO, int pin);
void gpio_set_input(GPIO_TypeDef *GPIO, int pin);
void gpio_set_alternate(GPIO_TypeDef *GPIO, int pin);
void gpio_set_high_speed(GPIO_TypeDef *GPIO, int pin);
void gpio_set_pull_up(GPIO_TypeDef *GPIO, int pin);
void gpio_set_pull_down(GPIO_TypeDef *GPIO, int pin);
void gpio_set_push_pull(GPIO_TypeDef *GPIO, int pin);

#endif
