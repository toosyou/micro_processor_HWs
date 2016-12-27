void USART_PrintString(USART_TypeDef *usart, const char *ptr){
	while(*ptr){
		USARTSendData(usart, *(ptr++));
	}
}

void USART_SendData(USART_TypeDef *usart, int data){
	while(!(usart->ISR & USART_ISR_TXE));
	usart->TDR = data & 0x1FF;
	while(!(usart->ISR & USART_ISR_TXE));
}

int USART_GetData(USART_TypeDef *usart){
	while(!(usart->ISR &USART_ISR_RXNE));
	return usart->RDR & 0x1FF;
}

void USARTEnableRXTX(USART_TypeDef *usart, int rx, int tx){
	usart->CR1 &= ~(3 << 2);
	usart->CR1 |= rx << USART_CR1_RE_Pos;
	usart->CR1 |= tx << USART_CR1_TE_Pos;
}

void USARTEnable(USART_TypeDef *usart){
	usart->CR1 |= USART_CR1_UE;
}


int USARTGetLine(USART_TypeDef *usart, char *ptr, int len){
	len--;
	char x;
	for(int i=0;i<len;i++){
		x = USART_GetData(usart);
		if(x == '\n' || x == '\r'){
			ptr[i] = 0;
			USART_PrintString(usart, "\r\n");
			return i;
		}else if(x == 127){
			if(i){
				i--;
				USART_SendData(usart, x);
			}
			i--;
		}else{
			USART_SendData(usart, x);
			ptr[i] = x;
		}
	}
	return len;
}

void set_baud_rate_uart(USART_TypeDef *usart, int fck, int baud){
	usart->BRR = (fck + baud / 2) / baud;
	return;
}

void set_length_stop_uart(USART_TypeDef *usart, int length){
	//0 -> 1
	//1 -> 0.5
	//2 -> 2
	//3 -> 1.5
	usart->CR2 &= ~USART_CR2_STOP_Msk;
	usart->CR2 |= length << USART_CR2_STOP_Pos;
	return;
}

void enable_rt_usart(USART_TypeDef *usart, int rx, int tx){
	usart->CR1 &= ~(3 << 2); // clear
	usart->CR1 |= rx << USART_CR1_RE_Pos;
	usart->CR1 |= tx << USART_CR1_TE_Pos;
	return;
}

void enable_usart(USART_TypeDef *usart){
	usart->CR1 |= USART_CR1_UE;
	return;
}


void set_length_word_uart(USART_TypeDef *usart, int length){
	if(length == 7){ // 10
		usart->CR1 |= USART_CR1_M1;
		usart->CR1 &= ~USART_CR1_M0;
	}
	else if(length == 8){ // 00
		usart->CR1 &= ~USART_CR1_M1;
		usart->CR1 &= ~USART_CR1_M0;
	}
	else if(length == 9){ // 01
		usart->CR1 &= ~USART_CR1_M1;
		usart->CR1 |= USART_CR1_M0;
	}
	return;
}

int UART_Transmit(USART_TypeDef *usart, char *arr, uint32_t size) {

	for(unsigned int i=0;i<size;++i){
		//Transmit data register empty
		while(!flag_status_usart(usart, USART_ISR_TXE));

		usart->TDR = arr[i];
		//Transmission complete
		while(!flag_status_usart(usart, USART_ISR_TC));
	}

	return 0;
}
