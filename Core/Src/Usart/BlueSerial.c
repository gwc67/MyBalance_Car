#include "BlueSerial.h"
#define receive_str 0 // 接受字符 还是接受 float

uint8_t BlueSerial_RxFlag;

#define MyUSART USART2
void BlueSerial_Init(void)
{
    LL_USART_EnableIT_RXNE(MyUSART);
}

void BlueSerial_SendByte_LL(uint8_t Byte)
{
    LL_USART_TransmitData8(MyUSART, Byte);
    while (LL_USART_IsActiveFlag_TXE(MyUSART) == RESET)
        ;
}

#define BlueSerial_SendByte(Byte) BlueSerial_SendByte_LL(Byte)

void BlueSerial_SendArray(uint8_t Array[], uint16_t lenth)
{
    for (uint16_t i = 0; i < lenth; i++)
    {
        BlueSerial_SendByte(Array[i]);
    }
}

void BlueSerial_SendString(char String[])
{
    for (uint16_t i = 0; String[i] != '\0'; i++)
    {
        BlueSerial_SendByte(String[i]);
    }
}

uint32_t BlueSerial_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1; // 设置结果初值为1
    while (Y--)          // 执行Y次
    {
        Result *= X; // 将X累乘到结果
    }
    return Result;
}
// 还是文本模式
void BlueSerial_SendNumber(uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++) // 根据数字长度遍历数字的每一位
    {
        BlueSerial_SendByte(Number / BlueSerial_Pow(10, Length - i - 1) % 10 + '0'); // 依次调用BlueSerial_SendByte发送每位数字
    }
}
void BlueSerial_Printf(char *format, ...)
{
    char String[100];              // 定义字符数组
    va_list arg;                   // 定义可变参数列表数据类型的变量arg
    va_start(arg, format);         // 从format开始，接收参数列表到arg变量
    vsprintf(String, format, arg); // 使用vsprintf打印格式化字符串和参数列表到字符数组中
    va_end(arg);                   // 结束变量arg
    BlueSerial_SendString(String); // 串口发送字符数组（字符串）
}

#define Blue_Serial_Max_Packet_Size 255
// void BlueSerial_SendInt16(int16_t Data)
// {
//     uint8_t Packet[5];
//     Packet[0] = 0xA5;
//     Packet[1] = (uint8_t)(Data & 0xff);
//     Packet[2] = (uint8_t)(Data >> 8);
//     Packet[3] = (Packet[1] + Packet[2]) & 0xff; //校验和取低八位
//     Packet[4] = 0x5A;
//     BlueSerial_SendArray(Packet,5);
// }

// 不定长发送数组
void BlueSerial_SendVaribleLength(uint8_t *Data, uint8_t length)
{
    if (!Data || length == 0 || length > Blue_Serial_Max_Packet_Size)
    {
        return;
    }
    uint8_t Packet[3 + Blue_Serial_Max_Packet_Size];

    uint8_t checkSum = 0;
    for (uint8_t i = 0; i < length; i++)
    {
        checkSum += Data[i];
    }

    uint8_t index = 0;
    Packet[index++] = 0xA5;
    memcpy(&Packet[index], Data, length);
    index += length;
    Packet[index++] = checkSum;
    Packet[index] = 0x5A;

    BlueSerial_SendArray(Packet, length + 3);
}
void BlueSerial_SendInt16Array(int16_t *Data, uint8_t length)
{
    BlueSerial_SendVaribleLength((uint8_t *)Data, length * sizeof(int16_t));
}

// 记得把stm32_it.c里的那个去掉
void BlueSerial_SendFloatArray(float *data, uint8_t count)
{
    if (!data || count == 0)
    {
        return;
    }

    uint8_t total_bytes = 4 * count;

    uint8_t buffer[Blue_Serial_Max_Packet_Size];
    for (int i = 0; i < count; i++)
    {
        union
        {
            float f;
            uint8_t Data[4];
        } convert;
        convert.f = data[i];
        memcpy(&buffer[i * 4], convert.Data, 4);
    }
    BlueSerial_SendVaribleLength(buffer, total_bytes);
}

uint8_t BlueSerial_RxPacket[100];

#if !receive_str
float FloatArray[MAX_FLOAT_COUNT];
uint8_t float_count;
// static uint8_t Check = 0; // 校验和累加值
typedef union
{
    float f;
    uint8_t Data[4];
} FloatUnion;

#else
#endif

// void USART2_IRQHandler(void)
// {
//     /* USER CODE BEGIN USART1_IRQn 0 */
//     static uint8_t Rx_State;
//     static uint8_t P_RxPacket;

//     if (LL_USART_IsActiveFlag_RXNE(MyUSART))
//     {
//         LL_USART_ClearFlag_RXNE(MyUSART);
//         uint8_t RxData = LL_USART_ReceiveData8(MyUSART);
//         if (Rx_State == 0)
//         {
//             if (RxData == 0xA5)
//             {
//                 Rx_State = 1;
//                 P_RxPacket = 0;
// #if receive_str

// #else
//                 // Check = 0;
// #endif
//             }
//         }
//         else if (Rx_State == 1)
//         {
//             if (RxData == 0x5A)
//             {
//                 Rx_State = 0;
// #if receive_str
//                 BlueSerial_RxPacket[P_RxPacket] = '\0';
// #else
//                 // uint8_t receive_check = BlueSerial_RxPacket[P_RxPacket - 1];

//                 // if (receive_check == Check)
//                 // {
//                 uint8_t data_len = P_RxPacket;
//                 float_count = data_len / 4;
//                 for (int i = 0; i < float_count; i++)
//                 {
//                     FloatUnion convert;
//                     convert.Data[0] = BlueSerial_RxPacket[i * 4];
//                     convert.Data[1] = BlueSerial_RxPacket[i * 4 + 1];
//                     convert.Data[2] = BlueSerial_RxPacket[i * 4 + 2];
//                     convert.Data[3] = BlueSerial_RxPacket[i * 4 + 3];
//                     FloatArray[i] = convert.f;
//                     BlueSerial_SendFloatArray(&FloatArray[i], 1);
//                 }
//                 // }

// #endif
//                 BlueSerial_RxFlag = 1;
//             }
//             else
//             {

//                 BlueSerial_RxPacket[P_RxPacket++] = RxData;
// #if receive_str

// #else
//                 // Check += RxData;

// #endif
//             }

//             /* USER CODE END USART1_IRQn 0 */
//             /* USER CODE BEGIN USART1_IRQn 1 */

//             /* USER CODE END USART1_IRQn 1 */
//         }
//     }
// }