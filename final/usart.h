#ifndef __USART_H__
#define __USART_H__

#include <stdlib.h>
#include <stdio.h>
#include "stm32l476xx.h"

void set_baud_rate_uart(USART_TypeDef *usart, int fck, int baud);
void set_length_stop_uart(USART_TypeDef *usart, int length);
void enable_rt_usart(USART_TypeDef *usart, int rx, int tx);
void enable_usart(USART_TypeDef *usart);
void set_length_word_uart(USART_TypeDef *usart, int length);
int flag_status_usart(USART_TypeDef *usart, int flag);
int UART_Transmit(USART_TypeDef *usart, char *arr, uint32_t size) ;
void usart_init(void);

#endif
