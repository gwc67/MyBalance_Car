#ifndef __ALL_H
#define __ALL_H

// extern float AngleAcc;
// extern float AngleGyro;
// extern float Angle;
#include "stm32f103xb.h"
#include "Encode.h"
#include "BlueSerial.h"
#include "usart.h"
#include "stm32f1xx_ll_tim.h"
#include "MyI2C.h"
#include "Key.h"
#include "LED.h"
#include "main.h"
#include "MENU_Show.h"
#include "MENU.h"
#include "OLED.h"
#include "OLED_Data.h"
#include "Servo.h"
#include "Store.h"
#include "stdbool.h"
#include "stdio.h"
#include "MPU6050.h"
#include "Uart.h"
#include "stm32f1xx_ll_gpio.h"
#include "stdarg.h"
#include "PID.h"
#include "ring_buf.h"
extern uint8_t RunFlag;
extern int16_t LeftPwm,RightPwm;
extern int16_t AvePwm,DifPwm; 
extern float Angle;
#endif