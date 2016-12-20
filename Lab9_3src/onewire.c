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

void set_low(OneWire_t *ows){   //set to 0
    // set dq low
    ows->GPIOx->ODR &= ~(1 << ows->GPIO_Pin);
    return ;
}

void set_high(OneWire_t *ows){  //set to 1
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

 #define SkipROM 0xCC
 #define WriteScratchpad 0x4E
 #define AlarmTL 0x8A
 #define AlarmTH 0x64
 #define Precision 0x7F
 #define CopyScratchpad 0x48

void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin) {
	// TODO
    /*DisableINT();
    OneWire_Reset(&OneWireStruct);
    OneWire_WriteByte(SkipROM);
    OneWire_WriteByte(WriteScratchpad);
    OneWire_WriteByte(AlarmTL);
    OneWire_WriteByte(AlarmTH);
    OneWire_WriteByte(Precision);

    ResetDS18B20();
    OneWire_WriteByte(SkipROM);
    OneWire_WriteByte(CopyScratchpad);
    EnableINT();

    while(!GetDQ());  */


    OneWireStruct->GPIOx = GPIOx;
    OneWireStruct->GPIO_Pin = GPIO_Pin;
    set_input(OneWireStruct);
    OneWireStruct->GPIOx->OSPEEDR |= (1 << OneWireStruct->GPIO_Pin*2);
    OneWireStruct->GPIOx->PUPDR= (GPIOB->PUPDR & (~3<<OneWireStruct->GPIO_Pin*2) ) | (1<<OneWireStruct->GPIO_Pin*2);
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
    int rtn;
    set_input(OneWireStruct);
    delay_ms(50);   //
    //set_low(OneWireStruct);
    set_output(OneWireStruct);
    OneWireStruct->GPIOx->BRR=(1<<3);
    delay_ms(480);
    set_input(OneWireStruct);

    delay_ms(70);

    //while( read(OneWireStruct) != 0);
    if(read(OneWireStruct))
        rtn=1;
    else
        rtn=0;
    delay_ms(410);
    set_input(OneWireStruct);
    //set_output(OneWireStruct);

    // set dq high
    //set_high(OneWireStruct);

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
    delay_ms(5);
    if(bit == 1){

        set_low(OneWireStruct);
        set_output(OneWireStruct);
        delay_ms(10);//

        set_input(OneWireStruct);
        delay_ms(55);//

    }
    else{
    	set_low(OneWireStruct);
        set_output(OneWireStruct);
        delay_ms(65);//
        set_input(OneWireStruct);//
        delay_ms(5);
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
    delay_ms(5);
    set_output(OneWireStruct);
    set_high(OneWireStruct);
    delay_ms(2);
    set_input(OneWireStruct);
    delay_ms(5);
    if (read(OneWireStruct))
        rtn=1;
    else
        rtn=0;
    delay_ms(60);
    set_input(OneWireStruct);
    delay_ms(5);
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
    //delay_ms(2);
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
        if (read(OneWireStruct))
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
