#ifndef __ADC_H__
#define __ADC_H__

#include "stdio.h"
#include "stdlib.h"
#include "stm32l476xx.h"

typedef unsigned int bool;
#define false 0
#define true 1

void set_resolution_adc(ADC_TypeDef *adc, int resolution);
void enable_continuous_convetion_adc(ADC_TypeDef *adc, bool enable);
void set_data_align_adc(ADC_TypeDef *adc, bool left);
void enable_regulator_adc(ADC_TypeDef *adc);
void enable_adc(ADC_TypeDef *adc);
int read_adc(ADC_TypeDef *adc);
void convert_adc(ADC_TypeDef *adc);
void set_interrupt_adc(ADC_TypeDef *adc, unsigned int pos_mask, bool enable);
void set_convert_mode_adc(int mode);
void set_clock_mode_adc(int mode);
void set_prescaler_adc(int prescaler);
void set_dma_mode_adc(int mode);
void set_delay_adc(int delay_clk);
void set_channel_adc(ADC_TypeDef *adc, int channel, int rank, int sampleTime);
void ADC_init(void);


#endif
