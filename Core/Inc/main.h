/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
#define USE_FULL_LL_DRIVER 1
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC
#define PWMB_Pin GPIO_PIN_0
#define PWMB_GPIO_Port GPIOA
#define PWMA_Pin GPIO_PIN_1
#define PWMA_GPIO_Port GPIOA
#define Key4_Pin GPIO_PIN_4
#define Key4_GPIO_Port GPIOA
#define Key3_Pin GPIO_PIN_5
#define Key3_GPIO_Port GPIOA
#define Key2_Pin GPIO_PIN_0
#define Key2_GPIO_Port GPIOB
#define Key1_Pin GPIO_PIN_1
#define Key1_GPIO_Port GPIOB
#define MPU6050_SCL_Pin GPIO_PIN_10
#define MPU6050_SCL_GPIO_Port GPIOB
#define MPU6050_SDA_Pin GPIO_PIN_11
#define MPU6050_SDA_GPIO_Port GPIOB
#define AIN1_Pin GPIO_PIN_12
#define AIN1_GPIO_Port GPIOB
#define AIN2_Pin GPIO_PIN_13
#define AIN2_GPIO_Port GPIOB
#define BIN1_Pin GPIO_PIN_14
#define BIN1_GPIO_Port GPIOB
#define BIN2_Pin GPIO_PIN_15
#define BIN2_GPIO_Port GPIOB
#define OLED_SCK_Pin GPIO_PIN_8
#define OLED_SCK_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
