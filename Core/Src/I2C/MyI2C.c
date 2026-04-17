#include "MyI2C.h"

static I2CBus_Struct *P_ThisBus;
void Delay_us(uint32_t xus)
{
    SysTick->LOAD = 72 * xus;   // 设置定时器重装值
    SysTick->VAL = 0x00;        // 清空当前计数值
    SysTick->CTRL = 0x00000005; // 设置时钟源为HCLK，启动定时器
    while (!(SysTick->CTRL & 0x00010000))
        ;                       // 等待计数到0
    SysTick->CTRL = 0x00000004; // 关闭定时器
}
#define I2C_SCL_Write(x) HAL_GPIO_WritePin(P_ThisBus->SCL_GPIO, P_ThisBus->SCL_Pin, (GPIO_PinState)x)
#define I2C_SDA_Write(x) HAL_GPIO_WritePin(P_ThisBus->SDA_GPIO, P_ThisBus->SDA_Pin, (GPIO_PinState)x)
#define I2C_SDA_Read() HAL_GPIO_ReadPin(P_ThisBus->SDA_GPIO, P_ThisBus->SDA_Pin)
#define I2C_Delay(Time) Delay_us(Time)

inline static void SCL(uint8_t x)
{
    I2C_SCL_Write(x);
    I2C_Delay(P_ThisBus->Delay_Time);
}
inline static void SDA(uint8_t x)
{
    I2C_SDA_Write(x);
    I2C_Delay(P_ThisBus->Delay_Time);
}

inline static uint8_t SDA_Read()
{
    uint8_t temp = I2C_SDA_Read();
    I2C_Delay(P_ThisBus->Delay_Time);
    return temp;
}

static void I2C_Start()
{
    SCL(1);
    SDA(1);
    SDA(0);
    SCL(0);
}

static void I2C_Stop()
{
    SDA(0);
    SCL(1);
    SDA(1);
}

static void I2C_WriteByte(uint8_t Byte)
{
    for (int i = 0; i < 8; i++)
    {
        SDA(Byte & (0x80 >> i));
        SCL(1);
        SCL(0);
    }
}

static uint8_t I2C_ReceiveByte(void)
{
    uint8_t i, Byte = 0;
    SDA(1);
    for (i = 0; i < 8; i++)
    {
        SCL(1);
        if (SDA_Read() == 1)
        {
            Byte |= (0x80 >> i);
        }
        SCL(0);
    }
    return Byte;
}

static void I2C_WriteAck(uint8_t Ack)
{
    SDA(Ack);
    SCL(1);
    SCL(0);
}

// 0 应答 1 不应答
static uint8_t I2C_ReceiveAck()
{
    SDA(1);
    SCL(1);
    uint8_t ACK = SDA_Read();
    SCL(0);
    return ACK;
}

void MyI2C_Init(I2CBus_Struct *I2C_Bus, GPIO_TypeDef *SCL_Port, uint16_t SCL_Pin, GPIO_TypeDef *SDA_Port, uint16_t SDA_Pin, uint8_t Address, uint16_t Delay_Time)
{
    P_ThisBus = I2C_Bus;
    I2C_Bus->Address = Address << 1;
    I2C_Bus->Delay_Time = Delay_Time;
    I2C_Bus->SCL_GPIO = SCL_Port;
    I2C_Bus->SCL_Pin = SCL_Pin;
    I2C_Bus->SDA_GPIO = SDA_Port;
    I2C_Bus->SDA_Pin = SDA_Pin;
    I2C_Bus->Mode_16bit = 0; // 默认使用8bit模式

    I2C_SCL_Write(1);
    I2C_SDA_Write(1);
}

// 0 : 8-bit 1:16-bit
void MyI2C_SetMode(I2CBus_Struct *I2C_Bus, uint8_t mode)
{
    P_ThisBus = I2C_Bus;
    I2C_Bus->Mode_16bit = mode;
}
void MyI2C_WriteReg(I2CBus_Struct *I2C_Bus, uint8_t RegAddress, uint16_t Data)
{
    P_ThisBus = I2C_Bus;
    I2C_Start();
    I2C_WriteByte(I2C_Bus->Address);
    I2C_ReceiveAck();
    I2C_WriteByte(RegAddress);
    I2C_ReceiveAck();
    if (I2C_Bus->Mode_16bit)
    {
        I2C_WriteByte((uint8_t)(Data >> 8));
        I2C_ReceiveAck();
        I2C_WriteByte((uint8_t)(Data));
    }
    else
    {
        I2C_WriteByte((uint8_t)Data);
    }
    I2C_ReceiveAck();
    I2C_Stop();
//     I2C_Start();
// 	I2C_WriteByte(I2C_Bus->Address);
// //	Note  ： 可以if 条件展开
// 	I2C_ReceiveAck();
// 	I2C_WriteByte(RegAddress);
// 	I2C_ReceiveAck();
// 	I2C_WriteByte(Data);
// 	I2C_ReceiveAck();
// 	I2C_Stop();
}

uint16_t MyI2C_ReadReg(I2CBus_Struct *I2C_Bus, uint8_t RegAddress)
{
    P_ThisBus = I2C_Bus;
    I2C_Start();
    I2C_WriteByte(I2C_Bus->Address);
    if (I2C_ReceiveAck() == 1)
    {
        return 0xffff;
    }
    I2C_WriteByte(RegAddress);
    if (I2C_ReceiveAck() == 1)
    {
        return 0xffff;
    }

    I2C_Start();
    I2C_WriteByte(I2C_Bus->Address | 0x01); // 读模式
    if (I2C_ReceiveAck() == 1)
    {
        return 0xffff;
    }
    uint16_t Data;
    if (I2C_Bus->Mode_16bit == 1)
    {
        Data = (uint16_t)I2C_ReceiveByte() << 8;
        I2C_WriteAck(0);
        Data |= (uint16_t)I2C_ReceiveByte();
    }
    else
    {
        Data = (uint16_t)I2C_ReceiveByte();
    }
    I2C_WriteAck(1);
    I2C_Stop();
    return Data;
}

void MyI2C_WriteReg_Continue(I2CBus_Struct *I2CBus, uint8_t RegAddress, uint8_t *arrary, uint16_t arrary_size)
{
    P_ThisBus = I2CBus;
    I2C_Start();
    I2C_WriteByte(I2CBus->Address);
    if (I2C_ReceiveAck() != 0)
    {
        I2C_Stop();
        return;
    }
    I2C_WriteByte(RegAddress);
    if (I2C_ReceiveAck() != 0)
    {
        I2C_Stop();
        return;
    }
    for (uint16_t i = 0; i < arrary_size;)
    {
        if (I2CBus->Mode_16bit == 1 && i + 1 < arrary_size)
        {
            I2C_WriteByte(arrary[i]);
            if (I2C_ReceiveAck() != 0)
            {
                I2C_Stop();
                return;
            }
            I2C_WriteByte(arrary[i + 1]);
            if (I2C_ReceiveAck() != 0)
            {
                I2C_Stop();
                return;
            }
            i += 2;
        }
        else
        {
            I2C_WriteByte(arrary[i]);
            if (I2C_ReceiveAck() != 0)
            {
                I2C_Stop();
                return;
            }
            i++;
        }
    }
    I2C_Stop();
}
void MyI2C_ReadReg_Continue(I2CBus_Struct *I2CBus, uint8_t RegAddress, uint16_t readlen, uint8_t *data_buf)
{
    P_ThisBus = I2CBus;
    I2C_Start();
    I2C_WriteByte(I2CBus->Address);
    if (I2C_ReceiveAck() != 0)
    {
        I2C_Stop();
        return;
    }
    I2C_WriteByte(RegAddress);
    if (I2C_ReceiveAck() != 0)
    {
        I2C_Stop();
        return;
    }
    // 重新开启时序才能够设置读写位；
    I2C_Start();
    I2C_WriteByte(I2CBus->Address | 0x01);
    if (I2C_ReceiveAck() != 0)
    {
        I2C_Stop();
        return;
    }
    for (uint16_t i = 0; i < readlen; i++)
    {
        data_buf[i] = I2C_ReceiveByte();

        if (i < readlen - 1)
        {
            I2C_WriteAck(0);
        }
        else
        {
            I2C_WriteAck(1);
        }
    }
    I2C_Stop();
}

uint8_t MyI2C_AddScan(I2CBus_Struct *I2CBus)
{
    P_ThisBus = I2CBus;
    for (uint8_t address = 0x08; address < 0x7f; address++)
    {
        I2C_Start();
        I2C_WriteByte(address << 1);
        if (I2C_ReceiveAck() == 0)
        {
            return address;
        }
        I2C_Stop();
    }
    return 0xff; // 没有设备；
}