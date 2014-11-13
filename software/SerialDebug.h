
#ifdef _SERIAL_DEBUG
#include <LUFA/Drivers/Peripheral/Serial.h>
#include <stdio.h>

extern FILE debugSerial;
void SerialDebug_init();
#define SerialDebug_printf(...)	fprintf(debugSerial, __VA_ARGS__)
void SerialDebug_printByteArray(uint8_t* arrayData, uint16_t arraySize);

#else

#define SerialDebug_init()
#define SerialDebug_printf(...)
#define SerialDebug_printByteArray(arrayData, arraySize)

#endif
