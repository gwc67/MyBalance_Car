#ifndef __UART_H
#define __UART_H

#include "ALL.h"
#include "Uart.h"

//替换Serial_SendByte 后面的函数

void Serial_SendByte_LL(uint8_t Byte);

void Serial_SendArray(uint8_t Array[], uint16_t lenth);

void Serial_SendString(char String[]);

uint32_t Serial_Pow(uint32_t X, uint32_t Y);

void Serial_SendNumber(uint32_t Number, uint8_t Length);

void Serial_Printf(char *format, ...);

#endif