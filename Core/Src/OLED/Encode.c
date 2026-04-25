#include "Encode.h"

#define Encode_left TIM3
#define Encode_right TIM4

void Encode_Init(void)
{
    LL_TIM_EnableCounter(Encode_left);
    LL_TIM_CC_EnableChannel(Encode_left, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(Encode_left, LL_TIM_CHANNEL_CH2);

    LL_TIM_EnableCounter(Encode_right);
    LL_TIM_CC_EnableChannel(Encode_right, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(Encode_right, LL_TIM_CHANNEL_CH2);
}

int16_t Encode_Get_left(void)
{
    int16_t temp;
    temp = LL_TIM_GetCounter(Encode_left);
    LL_TIM_SetCounter(Encode_left, 0);
    return temp;
}
int16_t Encode_Get_right(void)
{
    int16_t temp;
    temp = LL_TIM_GetCounter(Encode_right);
    LL_TIM_SetCounter(Encode_right, 0);
    return temp;
}