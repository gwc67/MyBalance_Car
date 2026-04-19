#include "LED.h"

void LED1_ON(void)
{
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}

void LED1_OFF(void)
{
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
}

// void LED2_ON(void)
// {
//     HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
// }

// void LED2_OFF(void)
// {
//     HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
// }

// uint32_t LED1_Count, LED2_Count;
// uint16_t LED1_Mode, LED2_Mode;

// void LED1_SetMode(uint16_t mode)
// {
//     LED1_Mode = mode;
// }
// // void LED2_SetMode(uint16_t mode)
// // {
// //     LED2_Mode = mode;
// // }

// void LED_Tick(void)
// {

//     if (LED1_Mode == 0)
//     {
//         LED1_OFF();
//     }

//     else if (LED1_Mode == 1)
//     {
//         LED1_ON();
//     }
//     else if (LED1_Mode == 2)
//     {
//         LED1_Count++;
//         if (LED1_Count > 999)
//         {
//             LED1_Count = 0;
//         }

//         if (LED1_Count < 500)
//         {
//             LED1_ON();
//         }
//         else
//         {
//             LED1_OFF();
//         }
//     }
//     else if (LED1_Mode == 3)
//     {
//         LED1_Count++;
//         if (LED1_Count > 400)
//         {
//             LED1_Count = 0;
//         }
//         if (LED1_Count < 200)
//         {
//             LED1_ON();
//         }
//         else
//         {
//             LED1_OFF();
//         }
//     }

//     else if (LED1_Mode == 4)
//     {
//         LED1_Count++;
//         if (LED1_Count > 999)
//         {
//             LED1_Count = 0;
//         }

//         if (LED1_Count < 100)
//         {
//             LED1_ON();
//         }
//         else
//         {
//             LED1_OFF();
//         }
//     }


//      if (LED2_Mode == 0)
//     {
//         LED2_OFF();
//     }

//     else if (LED2_Mode == 1)
//     {
//         LED2_ON();
//     }
//     else if (LED2_Mode == 2)
//     {
//         LED2_Count++;
//         if (LED2_Count > 999)
//         {
//             LED2_Count = 0;
//         }

//         if (LED2_Count < 500)
//         {
//             LED2_ON();
//         }
//         else
//         {
//             LED2_OFF();
//         }
//     }
//     else if (LED2_Mode == 3)
//     {
//         LED2_Count++;
//         if (LED2_Count > 400)
//         {
//             LED2_Count = 0;
//         }
//         if (LED2_Count < 200)
//         {
//             LED2_ON();
//         }
//         else
//         {
//             LED2_OFF();
//         }
//     }

//     else if (LED2_Mode == 4)
//     {
//         LED2_Count++;
//         if (LED2_Count > 999)
//         {
//             LED2_Count = 0;
//         }

//         if (LED2_Count < 100)
//         {
//             LED2_ON();
//         }
//         else
//         {
//             LED2_OFF();
//         }
//     }
// }