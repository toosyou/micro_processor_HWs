#include "onewire.h"

void delay_us(unsigned int us){
    /*SysTick->LOAD=us*4;              //time load
    SysTick->CTRL|=0x01;             //countdown
    while(!(SysTick->CTRL&(1<<16))); //wait for time arrive
    SysTick->CTRL=0X00000000;        //shut down timer
    SysTick->VAL=0X00000000;         //clear timer
    return;*/
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->PSC = 9;//
	TIM2->EGR = TIM_EGR_UG;//Reinitialize the counter

	TIM2->ARR=us;
	TIM2->CR1 |= TIM_CR1_CEN;
	while(TIM2->CNT< us-1);
	TIM2->CR1 &= ~TIM_CR1_CEN;
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
    ows->GPIOx->BRR |= (1 << ows->GPIO_Pin);
    return ;
}

void set_high(OneWire_t *ows){  //set to 1
    //ows->GPIOx->ODR |= 1 << ows->GPIO_Pin;
    ows->GPIOx->BSRR |= (1 << ows->GPIO_Pin);
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
    set_low(OneWireStruct);
    delay_us(600);
    set_input(OneWireStruct);
    delay_us(80);

    int rtn = read_wire(OneWireStruct);
    delay_us(600);
    return rtn;
}

/* Write 1 bit through OneWireStruct
 * Please implement the send 1-bit protocol
 * param:
 *   OneWireStruct: wire to send
 *   bit: bit to send
 */
void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit) {
	// TODO
    set_input(OneWireStruct);
    delay_us(5);
    if(bit == 1){

        set_low(OneWireStruct);
        set_output(OneWireStruct);
        delay_us(10);//

        set_input(OneWireStruct);
        delay_us(55);//

    }
    else{
    	set_low(OneWireStruct);
        set_output(OneWireStruct);
        delay_us(65);//
        set_input(OneWireStruct);//
        delay_us(5);
    }
}

/* Read 1 bit through OneWireStruct
 * Please implement the read 1-bit protocol
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct) {
	// TODO

    int rtn = 1;
    set_input(OneWireStruct);
    delay_us(5);
    set_output(OneWireStruct);
    set_high(OneWireStruct);
    delay_us(2);
    set_input(OneWireStruct);
    delay_us(5);
    if (read_wire(OneWireStruct))
        rtn=1;
    else
        rtn=0;
    delay_us(60);
    set_input(OneWireStruct);
    delay_us(5);
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
    int rtn=0;
    for (int i=0;i<8;i++)
    {
        rtn >>= 1;
        if (read_wire(OneWireStruct))
            rtn |= 0x80;
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
