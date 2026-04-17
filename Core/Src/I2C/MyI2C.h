#ifndef MYI2C_H
#define MYI2C_H

#include "ALL.h"
typedef struct I2CBus_Struct
{
    GPIO_TypeDef *SCL_GPIO;
    GPIO_TypeDef *SDA_GPIO;
    uint16_t SCL_Pin;
    uint16_t SDA_Pin;
    uint8_t Mode_16bit;
    uint8_t Address;     // I2C 从机地址 例如MPU6050的地址是0x68
    uint16_t Delay_Time; //
} I2CBus_Struct;

void MyI2C_Start();

void MyI2C_Stop();

void MyI2C_SendByte(uint8_t Byte);

uint8_t MyI2C_ReceiveByte(void);

void MyI2C_SendACK(uint8_t Ack);

// 0 应答 1 不应答
uint8_t MyI2C_ReceiveACK();

void MyI2C_Init();

// 0 : 8-bit 1:16-bit
void MyI2C_SetMode(I2CBus_Struct *I2C_Bus, uint8_t mode);

void MyI2C_WriteReg(I2CBus_Struct *I2C_Bus, uint8_t RegAddress, uint16_t Data);

uint16_t MyI2C_ReadReg(I2CBus_Struct *I2C_Bus, uint8_t RegAddress);

void MyI2C_WriteReg_Continue(I2CBus_Struct *I2CBus, uint8_t RegAddress, uint8_t *arrary, uint16_t arrary_size);

void MyI2C_ReadReg_Continue(I2CBus_Struct *I2CBus, uint8_t RegAddress, uint16_t readlen, uint8_t *data_buf);

uint8_t MyI2C_AddScan(I2CBus_Struct *I2CBus);

#endif