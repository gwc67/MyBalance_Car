#ifndef __BLUESERIAL_H
#define __BLUESERIAL_H
#include "ALL.h"
#if !receive_str
#define MAX_FLOAT_COUNT 10
extern float FloatArray[MAX_FLOAT_COUNT];
#else

#endif
extern uint8_t BlueSerial_RxFlag;
extern uint8_t BlueSerial_RxPacket[100];
void BlueSerial_Init(void);

void BlueSerial_SendByte_LL(uint8_t Byte);
 
void BlueSerial_SendArray(uint8_t Array[], uint16_t lenth);
 
void BlueSerial_SendString(char String[]);
 
uint32_t BlueSerial_Pow(uint32_t X, uint32_t Y);
 
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length);
 
void BlueSerial_Printf(char *format, ...);
// void BlueSerial_SendInt16(int16_t Data);
void BlueSerial_SendVaribleLength(uint8_t* Data,uint8_t length);

//使用方法 while循环下 int16_t gyro[3] = {raw.Accx,raw.Accy,raw.accz}
void BlueSerial_SendInt16Array(int16_t* Data,uint8_t length);
void BlueSerial_SendFloatArray(float *data, uint8_t count);

#endif

