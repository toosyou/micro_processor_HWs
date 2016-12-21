#include "onewire.h"

void delay_us(unsigned int us){
    /*SysTick->LOAD=us*4;              //time load
    SysTick->CTRL|=0x01;             //countdown
    while(!(SysTick->CTRL&(1<<16))); //wait for time arrive
    SysTick->CTRL=0X00000000;        //shut down timer
    SysTick->VAL=0X00000000;         //clear timer
    return;*/
	RCC->APB1ENR1 |= 0b1;
	if ((uint32_t) (us * 0.1) == 0)
		TIM2->ARR = 1;
	else
		TIM2->ARR = (uint32_t) (us * 0.1);
	TIM2->PSC = (uint32_t) 39999;
	TIM2->EGR = TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	int pre_val = 0;
	while (1) {
		int now_val = TIM2->CNT;
		if (pre_val > now_val) {
			TIM2->CR1 &= ~TIM_CR1_CEN;
			return;
		}
		pre_val = now_val;
	}
}

void set_input(OneWire_t *ows){
    // set dq to 00(input)
    ows->GPIOx->MODER &= ~(1 << (ows->GPIO_Pin*2+1));
    ows->GPIOx->MODER &= ~(1 << (ows->GPIO_Pin*2) );
    return ;
}

void set_output(OneWire_t *ows){
    // set dq to 01(output)
    ows->GPIOx->MODER &= ~(1 << (ows->GPIO_Pin*2+1));
    ows->GPIOx->MODER |= 1 << (ows->GPIO_Pin*2);

    return ;
}

void set_low(OneWire_t *ows){   //set to 0
    // set dq low
    //ows->GPIOx->ODR &= ~(1 << ows->GPIO_Pin);
    ows->GPIOx->BRR = (1 << ows->GPIO_Pin);
    return ;
}

void set_high(OneWire_t *ows){  //set to 1
    //ows->GPIOx->ODR |= 1 << ows->GPIO_Pin;
    ows->GPIOx->BSRR = (1 << ows->GPIO_Pin);
    return ;
}

int read_wire(OneWire_t *ows){
    return (ows->GPIOx->IDR >> ows->GPIO_Pin) & 1;
}

/* Init OneWire Struct with GPIO information
 * param:
 *   OneWire: struct to be initialized
 *   GPIOx: Base of the GPIO DQ used, e.g. GPIOA
 *   GPIO_Pin: The pin GPIO DQ used, e.g. 5
 */

 #define SkipROM 0xCC
 #define WriteScratchpad 0x4E
 #define AlarmTL 0x8A
 #define AlarmTH 0x64
 #define Precision 0x7F
 #define CopyScratchpad 0x48

void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin) {

	OneWireStruct->GPIOx=GPIOx;
	OneWireStruct->GPIO_Pin=GPIO_Pin;
	OneWireStruct->GPIOx->MODER&=~(3<<OneWireStruct->GPIO_Pin*2); //input
	OneWireStruct->GPIOx->OSPEEDR |= (2 << OneWireStruct->GPIO_Pin*2);
	OneWireStruct->GPIOx->PUPDR=(GPIOB->PUPDR&(~3<<OneWireStruct->GPIO_Pin*2))|(1<<OneWireStruct->GPIO_Pin*2);
    return;
}

/* Send reset through OneWireStruct
 * Please implement the reset protocol
 * param:
 *   OneWireStruct: wire to send
 * retval:
 *    0 -> Reset OK
 *    1 -> Reset Failed
 */
uint8_t OneWire_Reset(OneWire_t* OneWireStruct){
	// TODO  input low output
    set_output(OneWireStruct);
    set_high(OneWireStruct);
    delay(80);
    set_low(OneWireStruct);
    delay(500);
    set_high(OneWireStruct);
    delay(80);
    while(read_wire(OneWireStruct));
    delay(500);
    set_high(OneWireStruct);
    return 0;
}

/* Write 1 bit through OneWireStruct
 * Please implement the send 1-bit protocol
 * param:
 *   OneWireStruct: wire to send
 *   bit: bit to send
 */
void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit) {
	// TODO
    set_high(OneWireStruct);
    delay(80);
    set_low(OneWireStruct);
    delay(5);
    if(bit == 1)
        set_high(OneWireStruct);
    delay(75);
    set_high(OneWireStruct);
    delay(2);
    return;
}

/* Read 1 bit through OneWireStruct
 * Please implement the read 1-bit protocol
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct) {
	// TODO

    uint8_t rtn = 0;
    set_low(OneWireStruct);
    delay(4);
    set_high(OneWireStruct);
    delay(4);
    if (read_wire(OneWireStruct))
        rtn=1;
    delay(75);
    set_high(OneWireStruct);
    return rtn;
}

/* A convenient API to write 1 byte through OneWireStruct
 * Please use OneWire_WriteBit to implement
 * param:
 *   OneWireStruct: wire to send
 *   byte: byte to send
 */
void OneWire_WriteByte(OneWire_t* OneWireStruct, uint8_t byte) {
	// TODO
    //set_output(OneWireStruct);
    //set_low(OneWireStruct);
    //delay_us(2);
    for(int i=0;i<8;++i){
        OneWire_WriteBit(OneWireStruct, byte&1);
        byte >>= 1;
    }
    //set_high(OneWireStruct);
    return;
}

/* A convenient API to read 1 byte through OneWireStruct
 * Please use OneWire_ReadBit to implement
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadByte(OneWire_t* OneWireStruct) {
	// TODO
    uint8_t rtn=0;
    for (int i=0;i<8;i++){
        rtn |= OneWire_ReadBit(OneWireStruct) << i;
    }
    return rtn;
}

/* Send ROM Command, Skip ROM, through OneWireStruct
 * You can use OneWire_WriteByte to implement
 */
void OneWire_SkipROM(OneWire_t* OneWireStruct) {
	// TODO
    OneWire_WriteByte(OneWireStruct, 0xCC);
}
