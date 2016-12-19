#include "onewire.h"

void delay_ms(int ms){
    SysTick->LOAD=ms*4;              //time load
    SysTick->CTRL|=0x01;             //countdown
    while(!(SysTick->CTRL&(1<<16))); //wait for time arrive
    SysTick->CTRL=0X00000000;        //shut down timer
    SysTick->VAL=0X00000000;         //clear timer
    return;
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

void set_low(OneWire_t *ows){
    // set dq low
    ows->GPIOx->ODR &= ~(1 << ows->GPIO_Pin);
    return ;
}

void set_high(OneWire_t *ows){
    ows->GPIOx->ODR |= 1 << ows->GPIO_Pin;
    return ;
}

int read(OneWire_t *ows){
    return (ows->GPIOx->IDR >> ows->GPIO_Pin) & 1;
}

/* Init OneWire Struct with GPIO information
 * param:
 *   OneWire: struct to be initialized
 *   GPIOx: Base of the GPIO DQ used, e.g. GPIOA
 *   GPIO_Pin: The pin GPIO DQ used, e.g. 5
 */
void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin) {
	// TODO
    OneWireStruct->GPIOx = GPIOx;
    OneWireStruct->GPIO_Pin = GPIO_Pin;
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
uint8_t OneWire_Reset(OneWire_t* OneWireStruct) {
	// TODO
    set_output(OneWireStruct);
    // set dq low
    set_low(OneWireStruct);
    //delay
    delay_ms(480);
    set_input(OneWireStruct);

    delay_ms(60);

    while( read(OneWireStruct) != 0);
    delay_ms(500);

    set_output(OneWireStruct);

    // set dq high
    set_high(OneWireStruct);

    return (uint8_t)0;
}

/* Write 1 bit through OneWireStruct
 * Please implement the send 1-bit protocol
 * param:
 *   OneWireStruct: wire to send
 *   bit: bit to send
 */
void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit) {
	// TODO
    if(bit == 1){
        set_output(OneWireStruct);
        set_low(OneWireStruct);
        delay_ms(2);
        set_input(OneWireStruct);
        delay_ms(15);
    }
    else{
        set_output(OneWireStruct);
        set_low(OneWireStruct);
        delay_ms(80);
        set_high(OneWireStruct);
    }
}

/* Read 1 bit through OneWireStruct
 * Please implement the read 1-bit protocol
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct) {
	// TODO
    set_high(OneWireStruct);
    delay_ms(2);
    set_low(OneWireStruct);
    delay_ms(2);
    set_high(OneWireStruct);
    set_input(OneWireStruct);
    delay_ms(4);
    if (){

    }
    delay_ms(62);
    return 0;
}

/* A convenient API to write 1 byte through OneWireStruct
 * Please use OneWire_WriteBit to implement
 * param:
 *   OneWireStruct: wire to send
 *   byte: byte to send
 */
void OneWire_WriteByte(OneWire_t* OneWireStruct, uint8_t byte) {
	// TODO
    set_output(OneWireStruct);
    set_low(OneWireStruct);
    delay_ms(2);
    for(int i=0;i<8;++i){
        OneWire_WriteBit(OneWireStruct, byte&1);
        byte >>= 1;
    }
    set_high(OneWireStruct);
    return;
}

/* A convenient API to read 1 byte through OneWireStruct
 * Please use OneWire_ReadBit to implement
 * param:
 *   OneWireStruct: wire to read from
 */
uint8_t OneWire_ReadByte(OneWire_t* OneWireStruct) {
	// TODO
    return 0;
}

/* Send ROM Command, Skip ROM, through OneWireStruct
 * You can use OneWire_WriteByte to implement
 */
void OneWire_SkipROM(OneWire_t* OneWireStruct) {
	// TODO
}
