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



void MyI2C_Init(I2CBus_Struct *I2C_Bus, GPIO_TypeDef *SCL_Port, uint16_t SCL_Pin, GPIO_TypeDef *SDA_Port, uint16_t SDA_Pin, uint8_t Address, uint16_t Delay_Time);

// 0 : 8-bit 1:16-bit
void MyI2C_SetMode(I2CBus_Struct *I2C_Bus, uint8_t mode);

void MyI2C_WriteReg(I2CBus_Struct *I2C_Bus, uint8_t RegAddress, uint16_t Data);

uint16_t MyI2C_ReadReg(I2CBus_Struct *I2C_Bus, uint8_t RegAddress);

void MyI2C_WriteReg_Continue(I2CBus_Struct *I2CBus, uint8_t RegAddress, uint8_t *arrary, uint16_t arrary_size);

void MyI2C_ReadReg_Continue(I2CBus_Struct *I2CBus, uint8_t RegAddress, uint16_t readlen, uint8_t *data_buf);

uint8_t MyI2C_AddScan(I2CBus_Struct *I2CBus);

#endif