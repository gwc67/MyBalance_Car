#include "MyI2C.h"

static I2CBus_Struct *P_ThisBus;

#define I2C_SCL_Write(x) HAL_GPIO_WritePin(P_ThisBus->SCL_GPIO, P_ThisBus->SCL_Pin, (GPIO_PinState)x)
#define I2C_SDA_Write(x) HAL_GPIO_WritePin(P_ThisBus->SDA_GPIO, P_ThisBus->SDA_Pin, (GPIO_PinState)x)
#define I2C_SDA_Read() HAL_GPIO_ReadPin(P_ThisBus->SDA_GPIO, P_ThisBus->SDA_Pin)
#define I2C_Delay(Time) Delay_us(Time)

void SCL(uint8_t x)
{
    // I2C_SCL_Write(x);
    // I2C_Delay(P_ThisBus->Delay_Time);
    HAL_GPIO_WritePin(MPU6050_SCL_GPIO_Port,MPU6050_SCL_Pin,(GPIO_PinState)x);
    Delay_us(20);
}
void SDA(uint8_t x)
{
    // I2C_SDA_Write(x);
    // I2C_Delay(P_ThisBus->Delay_Time);
    HAL_GPIO_WritePin(MPU6050_SDA_GPIO_Port,MPU6050_SDA_Pin,(GPIO_PinState)x);
    Delay_us(20);
}
uint8_t SDA_Read()
{
    // uint8_t temp = I2C_SDA_Read();
    // I2C_Delay(P_ThisBus->Delay_Time);
    // return temp;
    uint8_t Value;
	Value = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_11);
	Delay_us(20);
	return Value;
}

 void MyI2C_Start()
{
    SDA(1);
    SCL(1);
    SDA(0);
    SCL(0);
}

 void MyI2C_Stop()
{
    SDA(0);
    SCL(1);
    SDA(1);
}

 void MyI2C_SendByte(uint8_t Byte)
{
    for (int i = 0; i < 8; i++)
    {
        SDA(Byte & (0x80 >> i));
        SCL(1);
        SCL(0);
    }
}

uint8_t MyI2C_ReceiveByte(void)
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

 void MyI2C_SendACK(uint8_t Ack)
{
    SDA(Ack);
    SCL(1);
    SCL(0);
}

// 0 应答 1 不应答
 uint8_t MyI2C_ReceiveACK()
{
    SDA(1);
    SCL(1);
    uint8_t ACK = SDA_Read();
    SCL(0);
    return ACK;
}

void MyI2C_Init(I2CBus_Struct *I2C_Bus, GPIO_TypeDef *SCL_Port, uint16_t SCL_Pin, GPIO_TypeDef* SDA_Port, uint16_t SDA_Pin, uint8_t Address, uint16_t Delay_Time)
{
    P_ThisBus = I2C_Bus;
    I2C_Bus->Address = Address << 1;
    I2C_Bus->Delay_Time = Delay_Time;
    I2C_Bus->SCL_GPIO = SCL_Port;
    I2C_Bus->SCL_Pin = SCL_Pin;
    I2C_Bus->SDA_GPIO = SDA_Port;
    I2C_Bus->SDA_Pin = SDA_Pin;
    I2C_Bus->Mode_16bit = 0; // 默认使用8bit模式

    if (SCL_Port == GPIOA || SDA_Port == GPIOA)
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }
    else if (SCL_Port == GPIOB || SDA_Port == GPIOB)
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }
    else if (SCL_Port == GPIOB || SDA_Port == GPIOB)
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }
    else if (SCL_Port == GPIOC || SDA_Port == GPIOC)
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    else if (SCL_Port == GPIOD || SDA_Port == GPIOD)
    {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    }

    LL_GPIO_InitTypeDef gpio_structure;
    gpio_structure.Mode = GPIO_MODE_OUTPUT_OD;
    gpio_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_structure.Pin = SDA_Pin;
    // gpio_structure.Pull = GPIO_PULLUP;
    LL_GPIO_Init(SDA_Port, &gpio_structure);
    gpio_structure.Pin = SCL_Pin;
    LL_GPIO_Init(SCL_Port, &gpio_structure);    

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
    MyI2C_Start();
    MyI2C_SendByte(I2C_Bus->Address);
    MyI2C_ReceiveACK();
    MyI2C_SendByte(RegAddress);
    MyI2C_ReceiveACK();
    if (I2C_Bus->Mode_16bit)
    {
        MyI2C_SendByte((uint8_t)(Data >> 8));
        MyI2C_ReceiveACK();
        MyI2C_SendByte((uint8_t)(Data));
    }
    else
    {
        MyI2C_SendByte((uint8_t)Data);
    }
    I2C_ReceiveAck();
    I2C_Stop();
}

uint16_t MyI2C_ReadReg(I2CBus_Struct *I2C_Bus, uint8_t RegAddress)
{
    // P_ThisBus = I2C_Bus;
    // MyI2C_Start();
    // MyI2C_SendByte(0xD0);
    // if (MyI2C_ReceiveACK() == 1)
    // {
    //     return 0xffff;
    // }
    // MyI2C_SendByte(RegAddress);
    // if (MyI2C_ReceiveACK() == 1)
    // {
    //     return 0xffff;
    // }

    I2C_Start();
    I2C_WriteByte(I2C_Bus->Address | 0x01); // 读模式
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
    MyI2C_Start();
    MyI2C_SendByte(I2CBus->Address);
    if (MyI2C_ReceiveACK() != 0)
    {
        MyI2C_Stop();
        return;
    }
    MyI2C_SendByte(RegAddress);
    if (MyI2C_ReceiveACK() != 0)
    {
        MyI2C_Stop();
        return;
    }
    for (uint16_t i = 0; i < arrary_size;)
    {
        if (I2CBus->Mode_16bit == 1 && i + 1 < arrary_size)
        {
            MyI2C_SendByte(arrary[i]);
            if (MyI2C_ReceiveACK() != 0)
            {
                MyI2C_Stop();
                return;
            }
            MyI2C_SendByte(arrary[i + 1]);
            if (MyI2C_ReceiveACK() != 0)
            {
                MyI2C_Stop();
                return;
            }
            i += 2;
        }
        else
        {
            MyI2C_SendByte(arrary[i]);
            if (MyI2C_ReceiveACK() != 0)
            {
                MyI2C_Stop();
                return;
            }
            i++;
        }
    }
    MyI2C_Stop();
}
void MyI2C_ReadReg_Continue(I2CBus_Struct *I2CBus, uint8_t RegAddress, uint16_t readlen, uint8_t *data_buf)
{
    P_ThisBus = I2CBus;
    MyI2C_Start();
    MyI2C_SendByte(I2CBus->Address);
    if (MyI2C_ReceiveACK() != 0)
    {
        MyI2C_Stop();
        return;
    }
    MyI2C_SendByte(RegAddress);
    if (MyI2C_ReceiveACK() != 0)
    {
        MyI2C_Stop();
        return;
    }
    // 重新开启时序才能够设置读写位；
    MyI2C_Start();
    MyI2C_SendByte(I2CBus->Address | 0x01);
    if (MyI2C_ReceiveACK() != 0)
    {
        MyI2C_Stop();
        return;
    }
    for (uint16_t i = 0; i < readlen; i++)
    {
        data_buf[i] = MyI2C_ReceiveByte();

        if (i < readlen - 1)
        {
            MyI2C_SendACK(0);
        }
        else
        {
            MyI2C_SendACK(1);
        }
    }
    MyI2C_Stop();
}

uint8_t MyI2C_AddScan(I2CBus_Struct *I2CBus)
{
    P_ThisBus = I2CBus;
    for (uint8_t address = 0x08; address < 0x7f; address++)
    {
        MyI2C_Start();
        MyI2C_SendByte(address << 1);
        if (MyI2C_ReceiveACK() == 0)
        {
            return address;
        }
        MyI2C_Stop();
    }
    return 0xff; // 没有设备；
}