#ifndef __BLUESERIAL_H
#define __BLUESERIAL_H
#include "ALL.h"

extern char BlueSerial_RxPacket[100];
extern uint8_t BlueSerial_RxFlag;

void BlueSerial_SendByte_LL(uint8_t Byte);
 
void BlueSerial_SendArray(uint8_t Array[], uint16_t lenth);
 
void BlueSerial_SendString(char String[]);
 
uint32_t BlueSerial_Pow(uint32_t X, uint32_t Y);
 
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length);
 
void BlueSerial_Printf(char *format, ...);
 
#endif

