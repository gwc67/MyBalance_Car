#include "Encode.h"

#define Encode_TIME_B TIM3
#define Encode_TIME_A TIM4

void Encode_Init(void)
{
    LL_TIM_EnableCounter(Encode_TIME_B);
    LL_TIM_CC_EnableChannel(Encode_TIME_B, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(Encode_TIME_B, LL_TIM_CHANNEL_CH2);

    LL_TIM_EnableCounter(Encode_TIME_A);
    LL_TIM_CC_EnableChannel(Encode_TIME_A, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(Encode_TIME_A, LL_TIM_CHANNEL_CH2);
}

int16_t Encode_Get_B(void)
{
    int16_t temp;
    temp = LL_TIM_GetCounter(Encode_TIME_B);
    LL_TIM_SetCounter(Encode_TIME_B, 0);
    return temp;
}
int16_t Encode_Get_A(void)
{
    int16_t temp;
    temp = LL_TIM_GetCounter(Encode_TIME_A);
    LL_TIM_SetCounter(Encode_TIME_A, 0);
    return temp;
}