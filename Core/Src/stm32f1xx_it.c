/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f1xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ALL.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint16_t time1;
uint8_t TimeErrorFlag;
float SpeedL, SpeedR;
extern uint8_t BlueSerial_RxPacket[100];
extern uint8_t BlueSerial_RxFlag;
// float AngleAcc;
// float AngleGyro;
// float Angle;
PID_t AnglePID = {
    .Kd = 0,
    .Ki = 0,
    .Kp = 0,
    .OutMax = 100,
    .OutMin = -100

};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  __asm("BKPT #0"); // 调试时停在这里
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles TIM1 update interrupt.
 */
int Vertical(float expectAngle,float actualAngle,float gyro_y)
{
	int temp;
	temp = AnglePID.Kp*(actualAngle-expectAngle)+AnglePID.Kd*gyro_y;
	return temp;
}
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */

  static uint16_t Count;
  if (LL_TIM_IsActiveFlag_UPDATE(TIM1) == SET)
  {
    Count++;
    if (Count > 5)
    {
      Count = 0;
      // MPU6050_Get_Raw(&raw);
      // // raw.GyroY += 35;
      // AngleAcc = atan2(raw.AccX, raw.AccZ) / 3.14159 * 180;
      // AngleGyro = Angle + raw.GyroY / 32768.0 * 2000 * 0.005;

      // float Alpha = 0.05;
      // Angle = Alpha * AngleAcc + (1 - Alpha) * AngleGyro;
      // MPU6050_Get_Angle(&raw);
      mpu_dmp_get_data(&raw.pitch,&raw.roll,&raw.yaw);

      if (RunFlag)
      {
        AnglePID.Actual = raw.pitch;
        PID_Update(&AnglePID);
        AvePwm = AnglePID.Out;
        // 当DifPwm大于0时 小车右转
        LeftPwm = AvePwm + DifPwm / 2;
        RightPwm = AvePwm - DifPwm / 2;
        // if ( LeftPwm > 0&& LeftPwm < 10)
        // {
        //   LeftPwm = 10;
        // }
        // else if ( LeftPwm < 0&& LeftPwm > -10)
        // {
        //   LeftPwm = -10;
        // }
        // if ( RightPwm > 0&& RightPwm < 10)
        // {
        //   RightPwm = 10;
        // }
        // else if ( RightPwm < 0&& RightPwm > -10)
        // {
        //   RightPwm = -10;
        // }
                
        if (LeftPwm > 100)
        {
          LeftPwm = 100;
        }
        else if (LeftPwm < -100)
        {
          LeftPwm = -100;
        }
        if (RightPwm > 100)
        {
          RightPwm = 100;
        }
        else if (RightPwm < -100)
        {
          RightPwm = -100;
        }

        Servo_SetSpeed_left(LeftPwm);
        Servo_SetSpeed_right(RightPwm);
      }
      else
      {
        Servo_SetSpeed_left(0);
        Servo_SetSpeed_right(0);
      }
    }

    Key_Tick();

    if ( raw.pitch > 70 ||   raw.pitch< -70)
    {
      RunFlag = 0;
    }

    LL_TIM_ClearFlag_UPDATE(TIM1);
    // if (LL_TIM_IsActiveFlag_UPDATE(TIM1) == SET)
    // {
    //   TimeErrorFlag = 1;
    //   LL_TIM_ClearFlag_UPDATE(TIM1);
    // }

    // time1 = LL_TIM_GetCounter(TIM1);
  }

  /* USER CODE END TIM1_UP_IRQn 0 */
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */

  /* USER CODE END TIM1_UP_IRQn 1 */
}

/**
 * @brief This function handles USART2 global interrupt.
 */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
