#include "adc.h"


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
	set_channel_adc(ADC1, 1, 1, 7); // channel 1, rank 1, 12.5 adc clock cycle
	enable_regulator_adc(ADC1);
	set_interrupt_adc(ADC1, ADC_IER_EOCIE, 1);
	NVIC_EnableIRQ(ADC1_2_IRQn);
	enable_adc(ADC1);
}
