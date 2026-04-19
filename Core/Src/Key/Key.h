#ifndef __KEY_H
#define __KEY_H
#include "main.h"
 
/* 配置四个引脚名字 Key1, Key2, Key3, Key4  初始化时记得 HAL_TIM_Base_Start_It(&htim2) */

//使用LL库：
// D:\Downloads\stm32project\Key_ProMax\Core\Src\stm32f1xx_it.c  该文件下 找到 TIM2_IRQHandler

//   if (LL_TIM_IsActiveFlag_UPDATE(TIM2) == SET)   
//   {
//     LL_TIM_ClearFlag_UPDATE(TIM2);
//     Key_Tick();
//   }

// key_Init 改变TIM2 

#define HAL 0

#if HAL
	#include "stm32f1xx_hal_tim.h"
#else
	#include "stm32f1xx_ll_tim.h"
	#define GPIO_Read_Bit(GPIO,Pin)   HAL_GPIO_ReadPin(GPIO,Pin)
#endif
	
#define KEY_PRESSED         1
#define KEY_UNPRESSED       0


#define KEY_HOLD            (0x01)
#define KEY_DOWN            (0x01 << 1)
#define KEY_UP              (0x01 << 2)
#define KEY_SINGLE          (0x01 << 3)
#define KEY_DOUBLE          (0x01 << 4)
#define KEY_LONG            (0x01 << 5)
#define KEY_REPEAT          (0x01 << 6)


#define KEY_TIME_DOUBLE     200
#define KEY_TIME_LONG       2000
#define KEY_TIME_REPEAT     100

#define KEY_COUNT           4   // 按键数目
#define KEY_1               0   //对应按键
#define KEY_2               1
#define KEY_3               2
#define KEY_4               3
void Key_Tick(void);
uint8_t Key_Check(uint8_t n, uint8_t Flag);
void Key_Init(void);
 
#endif
