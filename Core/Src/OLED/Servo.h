#ifndef __SERVO_H
#define __SERVO_H

//note : 引脚命名为 AIN1 ， AIN2
// 修改Servo_Time

#include "ALL.h"

#define TIM_SetCompare_PWMB(TIM,Speed)  LL_TIM_OC_SetCompareCH1(TIM,Speed)
#define TIM_SetCompare_PWMA(TIM,Speed)  LL_TIM_OC_SetCompareCH2(TIM,Speed)
#define TIM_Count_Enable(TIM)               LL_TIM_EnableCounter(TIM)
#define TIM_PWM_Enable(TIM,Channel)            LL_TIM_CC_EnableChannel(TIM,Channel)

#define AIN1(x)  HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,(GPIO_PinState)x)
#define AIN2(x)  HAL_GPIO_WritePin(AIN2_GPIO_Port,AIN2_Pin,(GPIO_PinState)x)
// #define AIN1(x)  ((x)?  LL_GPIO_SetOutputPin(AIN1_GPIO_Port, AIN1_Pin) : LL_GPIO_ResetOutputPin(AIN1_GPIO_Port,AIN1_Pin))
// #define AIN2(x)  ((x)?   LL_GPIO_SetOutputPin(AIN2_GPIO_Port, AIN2_Pin) : LL_GPIO_ResetOutputPin(AIN2_GPIO_Port, AIN2_Pin))

// #define BIN1(x)  ((x)?  LL_GPIO_SetOutputPin(BIN1_GPIO_Port, BIN1_Pin) : LL_GPIO_ResetOutputPin(BIN1_GPIO_Port,BIN1_Pin))
// #define BIN2(x)  ((x)?   LL_GPIO_SetOutputPin(BIN2_GPIO_Port, BIN2_Pin) : LL_GPIO_ResetOutputPin(BIN2_GPIO_Port, BIN2_Pin))

#define BIN1(x)  HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,(GPIO_PinState)x)
#define BIN2(x)  HAL_GPIO_WritePin(BIN2_GPIO_Port,BIN2_Pin,(GPIO_PinState)x)

void Servo_Init(void);
void Servo_SetSpeed_left(int32_t PWM);
void Servo_SetSpeed_right(int32_t PWM); 

































#endif