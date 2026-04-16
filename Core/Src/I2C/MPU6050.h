#ifndef  __MPU6050_H
#define  __MPU6050_H

#include "ALL.h"

typedef struct MPU6050_raw
{
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ;
    int16_t GyroX;
    int16_t GyroY;
    int16_t GyroZ;
    uint16_t Temp;
}MPU6050_raw;



void MPU6050_Init(GPIO_TypeDef* SCL_Port,uint16_t SCL_Pin,GPIO_TypeDef* SDA_Port,uint16_t SDA_Pin);
void MPU6050_Get_Raw(MPU6050_raw* this);
uint8_t MPU6050_ID(void);

#endif 