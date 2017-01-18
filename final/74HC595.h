#ifndef __74HC595_H__
#define __74HC595_H__
#include "stm32l476xx.h"
#include "util.h"


typedef unsigned int bool;
#define false 0
#define true 1

typedef struct{
    GPIO_TypeDef *GPIO;
    int clk_pin;
    int cs_pin;
    int data_pin;
    int reset_pin;
}HC595;

void HC595_clk_set(HC595 ic, int high_or_low);
void HC595_cs_set(HC595 ic, int high_or_low);
void HC595_data_set(HC595 ic, int high_or_low);
void HC595_reset_set(HC595 ic, int high_or_low);
void HC595Send(HC595 ic, unsigned int data, bool immediate_send);
void HC595Init(HC595 ic);
void HC595Load(HC595 ic);
void HC595Reset(HC595 ic);
void LedRowOut(HC595 ic, unsigned int Data);

extern void delay(int ms);
#endif
