
void USART_SetBaudRate(USART_TypeDef *usart, int cpuRate, int baudRate){
	cpuRate *= 1000000;
	usart->BRR = (cpuRate + baudRate / 2) / baudRate;
}

void USART_SetWordLength(USART_TypeDef *usart, int bits){

}

void USART_SetStopBit(USART_TypeDef *usart, int bits){

}

void USART_Enable(USART_TypeDef *usart, int rx, int tx){
	usart->CR1 &= ~(3 << 2);
	usart->CR1 |= rx << USART_CR1_RE_Pos;
	usart->CR1 |= tx << USART_CR1_TE_Pos;
	usart->CR1 |= USART_CR1_UE;
}

void USARTEnable(USART_TypeDef *usart, int rx, int tx){

}
