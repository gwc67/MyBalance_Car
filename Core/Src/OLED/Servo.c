#include "Servo.h"

#define Servo_Time TIM2

void Servo_Init(void)
{
    TIM_Count_Enable(Servo_Time);
    TIM_PWM_Enable(Servo_Time, LL_TIM_CHANNEL_CH1);
    TIM_PWM_Enable(Servo_Time, LL_TIM_CHANNEL_CH2);
}

void Servo_SetSpeed_right(int32_t PWM)
{
    if (PWM >= 0)
    {
        AIN1(0);
        AIN2(1);
        TIM_SetCompare_PWMA(Servo_Time, PWM);
    }
    else
    {
        AIN1(1);
        AIN2(0);
        TIM_SetCompare_PWMA(Servo_Time, -PWM);
    }
}
void Servo_SetSpeed_left(int32_t PWM)
{
    if (PWM >= 0)
    {
        BIN1(0);
        BIN2(1);

        TIM_SetCompare_PWMB(Servo_Time, PWM);
    }
    else
    {
        BIN1(1);
        BIN2(0);
        TIM_SetCompare_PWMB(Servo_Time, -PWM);
    }
}
