#ifndef __PID_H
#define __PID_H

#include "ALL.h"

// 如何使用： 

// 离散型PID 需要调控周期T 
// void TIM1_UP_IRQHandler(void)
// {
//   /* USER CODE BEGIN TIM1_UP_IRQn 0 */
//   static uint8_t count1, count2;
//   if (LL_TIM_IsActiveFlag_UPDATE(TIM1) == 1)
//   {
//     count1++;
//     LL_TIM_ClearFlag_UPDATE(TIM1);
//     Key_Tick();
//     if (count1 >= 40)
//     {
//       count1 = 0;
//       Speed = Encode_Get(); // speed / 1040 =    (speed的单位 边沿数/40ms) 转每40ms
//       Location += Speed;
//       Inner.Actual = Speed;
//       PID_Update(&Inner);
//       Servo_SetSpeed(Inner.Out);
//     }
//     count2++;
//     if (count2 >= 40)
//     {
//       count2 = 0;
//       Outer.Actual = Location; // speed / 1040 =    (speed的单位 边沿数/40ms) 转每40ms
//       Outer.Target = -(data / 4035.0 * 2000 - 1000);
//       PID_Update(&Outer); 
//       Inner.Target = Outer.Out;
//     }
//   }
//   /* USER CODE END TIM1_UP_IRQn 0 */
//   /* USER CODE BEGIN TIM1_UP_IRQn 1 */

//   /* USER CODE END TIM1_UP_IRQn 1 */
// }

typedef struct PID_t
{
    float Target;
    float Actual;
    float Out;

    float Kp;
    float Ki;
    float Kd;

    float Error0;
    float Error1;
    float ErrorInt;

    float OutMax;
    float OutMin;
    
}PID_t;

void PID_Update(PID_t* p);

#endif