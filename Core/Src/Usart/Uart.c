#include "Uart.h"

#define MyUSART USART2

void Serial_SendByte_LL(uint8_t Byte)
{
    LL_USART_TransmitData8(MyUSART, Byte);
    while (LL_USART_IsActiveFlag_TXE(MyUSART) == RESET)
        ;
}

#define Serial_SendByte(Byte)  Serial_SendByte_LL(Byte)


void Serial_SendArray(uint8_t Array[], uint16_t lenth)
{
    for (uint16_t i = 0; i < lenth; i++)
    {
        Serial_SendByte(Array[i]);
    }
}

void Serial_SendString(char String[])
{
    for (uint16_t i = 0; String[i] != '\0'; i++)
    {
        Serial_SendByte(String[i]);
    }
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1; // 设置结果初值为1
    while (Y--)          // 执行Y次
    {
        Result *= X; // 将X累乘到结果
    }
    return Result;
}
//还是文本模式
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++) // 根据数字长度遍历数字的每一位
    {
        Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0'); // 依次调用Serial_SendByte发送每位数字
    }
}
void Serial_Printf(char *format, ...)
{
    char String[100];              // 定义字符数组
    va_list arg;                   // 定义可变参数列表数据类型的变量arg
    va_start(arg, format);         // 从format开始，接收参数列表到arg变量
    vsprintf(String, format, arg); // 使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);                   // 结束变量arg
    Serial_SendString(String);     // 串口发送字符数组（字符串）
}