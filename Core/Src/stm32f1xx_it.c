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
int16_t SpeedL, SpeedR;
extern uint8_t BlueSerial_RxPacket[100];
extern uint8_t BlueSerial_RxFlag;
float AngleAcc;
float AngleGyro;
float Angle;
float GyroY_Actual;
float AveSpeed,DifSpeed;//平均、差分速度
PID_t AnglePID = {
    .Kd = 0,  //12
    .Ki = 0,
    .Kp = 0,    //7.2
    // .Kd = 12,  //12                          // 4.19
    // .Ki = 0.1,                      
    // .Kp = 7.2,    //7.2                      // 9.0     
    .OutMax = 50,
    .OutMin = -50,
    .ErrorIntMax = 600,
    .ErrorIntMin = -600,
    .OutOffset = 2,
    
  
};

PID_t GyroPID = {
  .Kp = 0,
  .Kd = 0,
  .Ki = 0,
  // .Kp = 0.39,
  // .Kd = 6,
  // .Ki = 1,
  .OutMax = 5,
  .OutMin = -5
  
};

PID_t SpeedPID = 
{
    .Kp = 0,   // 4.0
    .Ki = 0,    
    .Kd = 0,    //0.8
    .OutMax = 13,  // 这里的单位是角度
    .OutMin = -13,
    .ErrorIntMax = 150,
    .ErrorIntMin = -150,
};

PID_t TurnPID =
{
  .Kp = 0,
  .Ki = 0,
  .Kd = 0,
  .ErrorIntMax = 150,
  .ErrorIntMin = -150,
};
#define Max 100
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

  // int16_t AX,AY,AZ,GX,GY,GZ;

void TIM1_UP_IRQHandler ()
{
	static uint16_t Count0,Count1;
	if (LL_TIM_IsActiveFlag_UPDATE(TIM1) == SET)
	{
		LL_TIM_ClearFlag_UPDATE(TIM1);
		
		Key_Tick();//定时器扫描按键
		
		Count0++;
		if (Count0>=10)//10ms定时
		{
			Count0=0;
			
			/*读MPU6050传感器值*/
			// MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
			MPU6050_Get_Raw(&raw);
			//GY-=10;//初步校准
			
			/*Y轴角度：加速度计*/
			AngleAcc=-atan2(raw.AccX,raw.AccZ)/3.14158*180;
			
			/*Y轴角度：角速度计，积分*/
			AngleGyro=Angle+raw.GyroY/32768.0*2000*0.01;
			
			/*Y轴角速度：角速度计*/
			GyroY_Actual=raw.GyroY/32768.0*2000;
			
			/*角度：互补滤波算法*/
			//滤波系数（以陀螺仪计为主，越增大滤波系数，稳态误差越小，加速度计的缺点越明显）
			float Alpha=0.01;
			Angle=Alpha*AngleAcc+(1-Alpha)*AngleGyro;
			
			if (Angle>70 || Angle<-70)//角度超出可控范围
			{
				RunFlag=0;
			}
    
			if (RunFlag)
			{
				/*角度环调控*/
				AnglePID.Actual=Angle;
				PID_Update(&AnglePID);
				
			 /*角速度环调控*/
				GyroPID.Target=AnglePID.Out;
				GyroPID.Actual=GyroY_Actual;
				PID_Update(&GyroPID);
				
				AvePwm=-(AnglePID.Out+GyroPID.Out);
				
				SpeedL=AvePwm+DifPwm/2;
				SpeedR=AvePwm-DifPwm/2;
				
				if (SpeedL>100)SpeedL=100;else if (SpeedL<-100)SpeedL=-100;
				if (SpeedR>100)SpeedR=100;else if (SpeedR<-100)SpeedR=-100;
	
				Servo_SetSpeed_left(SpeedL);Servo_SetSpeed_right( SpeedR);
			}
			else
			{
				Servo_SetSpeed_left(0);Servo_SetSpeed_right( 0);//电机停转，防止失控
			}
			
		}
		Count1++;
		if (Count1>=50)//50ms定时
		{
			Count1=0;
			//编码电机速度：编码器测速值/极数/时间/减速比
			SpeedL= Encode_Get_left() /44.0/0.05/20;
			SpeedR=Encode_Get_right()/44.0/0.05/20;
			
			AveSpeed=(SpeedL+SpeedR)/2.0;
			DifSpeed=SpeedL-SpeedR;
			
			if (RunFlag)
			{
				/*速度环调控*/
				SpeedPID.Actual=AveSpeed;
				PID_Update(&SpeedPID);
				AnglePID.Target=SpeedPID.Out;
				
				/*转向环调控*/
			TurnPID.Actual=DifSpeed;
			PID_Update(&TurnPID);
			DifPwm=TurnPID.Out;
		}
	}
		 
	}
}

  /* USER CODE END TIM1_UP_IRQn 0 */
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */

  /* USER CODE END TIM1_UP_IRQn 1 */
 
 