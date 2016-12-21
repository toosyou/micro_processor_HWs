#include "ds18b20.h"

/* Send ConvT through OneWire with resolution
 * param:
 *   OneWire: send through this
 *   resolution: temperature resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
int DS18B20_ConvT(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	// TODO

	// convert T
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0x44); // convert T

	DS18B20_SetResolution(OneWire, resolution);

	return 0;
}

/* Read temperature from OneWire
 * param:
 *   OneWire: send through this
 *   destination: output temperature
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
uint8_t DS18B20_Read(OneWire_t* OneWire, float *destination, DS18B20_Resolution_t resolution) {
	// TODO
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0xBE);

	unsigned int tmp = 0;
	tmp |= OneWire_ReadByte(OneWire);
	tmp |= OneWire_ReadByte(OneWire) << 8;
	//if( get[1] >= 0x80 )
	//	tmp |= 0xffff0000;
	*destination = (float)(tmp >> (12 - resolution) );
	for(int i=0;i<(resolution-8);++i)
		*destination *= 0.5f;

	return 0;
}

/* Set resolution of the DS18B20
 * param:
 *   OneWire: send through this
 *   resolution: set to this resolution
 * retval:
 *    0 -> OK
 *    1 -> Error
 */
uint8_t DS18B20_SetResolution(OneWire_t* OneWire, DS18B20_Resolution_t resolution) {
	// TODO
	// i want to write bytes
	OneWire_Reset(OneWire);
	OneWire_SkipROM(OneWire);
	OneWire_WriteByte(OneWire, 0x4E);

	OneWire_WriteByte(OneWire, 0 );
	OneWire_WriteByte(OneWire, 0 );
	OneWire_WriteByte(OneWire, (resolution-9)<<5 | (0b11111) );

	return 0;
}

/* Check if the temperature conversion is done or not
 * param:
 *   OneWire: send through this
 * retval:
 *    0 -> OK
 *    1 -> Not yet
 */
uint8_t DS18B20_Done(OneWire_t* OneWire) {
	// TODO
	set_output(OneWire);
	if(read_wire(OneWire) == 1)
		return 0; // done
	return 1;
}
