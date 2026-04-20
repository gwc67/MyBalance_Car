#ifndef __ENCODE_H
#define __ENCODE_H

#include "ALL.h"

//Encode_TIME 修改其对应的TIM的值 即可

int16_t Encode_Get_A(void);  // 得到的单位 边沿数/40ms   ->  转/40ms= ~ / (转一圈轮子看看得到多少边沿数)
int16_t Encode_Get_B(void);  // 得到的单位 边沿数/40ms   ->  转/40ms= ~ / (转一圈轮子看看得到多少边沿数)
void Encode_Init(void);

#endif
