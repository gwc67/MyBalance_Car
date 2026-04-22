#ifndef  __MPU6050_H
#define  __MPU6050_H

#include "ALL.h"
#include "math.h"
typedef struct MPU6050_raw
{
    int16_t AccX;
    int16_t AccY;
    int16_t AccZ;
    int16_t GyroX;
    int16_t GyroY;
    int16_t GyroZ;
    uint16_t Temp;

      float yaw;       // 偏航角
    float roll;      // 翻滚角
    float pitch;     // 俯仰角
    float temp;      //实际温度值
    //四元素描述法
    float q0;
    float q1;
    float q2;
    float q3;
}MPU6050_raw;

 
extern MPU6050_raw raw;


void MPU6050_Init(GPIO_TypeDef *SCL_Port, uint16_t SCL_Pin, GPIO_TypeDef *SDA_Port, uint16_t SDA_Pin);
void MPU6050_Get_Raw(MPU6050_raw* this);
uint8_t MPU6050_ID(void);
float MPU6050_GetTemp();
void MPU6050_Get_Angle(MPU6050_raw *this);
void MPU6050_Get_Angle_Plus(MPU6050_raw *this);

#endif 