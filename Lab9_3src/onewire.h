#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include "stm32l476xx.h"

typedef struct {
	GPIO_TypeDef* GPIOx;           /*!< GPIOx port to be used for I/O functions */
	uint32_t GPIO_Pin;             /*!< GPIO Pin to be used for I/O functions */
} OneWire_t;

void delay_ms(int ms);
void set_input(OneWire_t *ows);
void set_output(OneWire_t *ows);
void set_low(OneWire_t *ows);
void set_high(OneWire_t *ows);
int read_wire(OneWire_t *ows);
void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin);
void OneWire_SkipROM(OneWire_t* OneWireStruct);
uint8_t OneWire_Reset(OneWire_t* OneWireStruct);
uint8_t OneWire_ReadByte(OneWire_t* OneWireStruct);
void OneWire_WriteByte(OneWire_t* OneWireStruct, uint8_t byte);
void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit);
uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct);


#endif /* ONEWIRE_H_ */
