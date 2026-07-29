/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ALL.h"
#include "MPU6050.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
MPU6050_raw raw;
// extern void Delay_us(uint32_t xus);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern uint16_t time1;
extern uint8_t TimeErrorFlag;
extern float SpeedL_f, SpeedR_f;
extern float AngleAcc;
extern float AngleGyro;
// extern   int16_t AX,AY,AZ,GX,GY,GZ;

uint8_t flag;
uint8_t RunFlag;
float LeftPwm, RightPwm;
float AvePwm, DifPwm;

stRingBufTdf stRingBuf_t;

uint8_t rece_it_data;

uint8_t buf[30];

stUARTFrameTdf stUARTFrame = {
    .usFrameHead = 0x6AA6,
    .ucSumCheck = 1,

};

// extern uint8_t dma_buf[30];
void USART1_IRQHandler(void)
{
  // 检查是否接收到数据
  if (LL_USART_IsActiveFlag_IDLE(USART1) && LL_USART_IsEnabledIT_IDLE(USART1))
  {

    // LL_USART_ClearFlag_IDLE(USART1);

    // // 停止DMA接受 ，以便读取当前接受了多少数据
    // LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);

    // uint32_t remaining = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_5);

    // LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, 10);

    // LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);

    // uint32_t received = 10 - remaining;
    // for (int i = 0; i < received; i++)
    // {
    //   ucRingBufWrite(&stRingBuf_t, dma_buf[i]);
    // }
    // emUartCallBackInline( gapstUartDevice[UART_DEVICE_1]);
  }
}

uint8_t temp_RxRingBuf[200];
stUartParamInitTdf stUartParamInit_1 = {
    .pstHandle = USART2,
    .pucRxRingBuf = temp_RxRingBuf,
    .ulRxRingBufSize = 200,
    .ulDmaRxBufSize = 64,
    .stFrameCfg = {0x6AA6, emChecksumType_Sum},
    .stUartCfg = {
        .ulBaterate = 9600,
        .ulMode = LL_USART_DIRECTION_TX_RX,
        .ulParity = LL_USART_PARITY_NONE,
        .ulStopBits = LL_USART_STOPBITS_1,
        .ulWordLength = LL_USART_DATAWIDTH_8B,
    },
    .pstDmaHandle = DMA1,
    .ulDmaChannel = LL_DMA_CHANNEL_6,
};
// 6A A6 0C 00 00 4C 40 00 00 4C 40 00 00 00 00 34
void USART2_IRQHandler(void)
{
  if (LL_USART_IsActiveFlag_IDLE(USART2) && LL_USART_IsEnabledIT_IDLE(USART2))
  {
    emUartCallBackInline(gapstUartDevice[UART_DEVICE_1]);
  }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  MPU6050_Init(MPU6050_SCL_GPIO_Port, MPU6050_SCL_Pin, MPU6050_SDA_GPIO_Port, MPU6050_SDA_Pin);
  // BlueSerial_Init();
  MX_TIM1_Init();
  Servo_Init();
  // Serial_Init_LL();
  Key_Init();
  Encode_Init();
  Store_Init();
  OLED_Init();
  Menu_Init();

  // 初始化自己的串口的标准操作
  gapstUartDevice[UART_DEVICE_1] = pstUartDeviceCreate(&stUartParamInit_1);
  if (gapstUartDevice[UART_DEVICE_1] != NULL)
  {
    emUartInitInline(gapstUartDevice[UART_DEVICE_1]);
    emUartStartRecvInline(gapstUartDevice[UART_DEVICE_1]);
  }
  char temp[50];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (RunFlag)
    {
      LED1_OFF();
      sprintf(temp, "%.2f,%.2f,%.2f,%.2f,%.2f,%.1f,%.2f,%.2f\n", SpeedPID.Kp, SpeedPID.Ki, SpeedPID.Kd, Angle, SpeedPID.Out, AvePwm, SpeedPID.Actual, AnglePID.Target);
      emUartSendInline(gapstUartDevice[UART_DEVICE_1], (uint8_t *)temp, strlen(temp));
    }
    else
    {
      LED1_ON();
    }

    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
      RunFlag = !RunFlag;
    }
    uint8_t out_data[2] = {0x01, 0x01};
    emUartProcessInline(gapstUartDevice[UART_DEVICE_1], out_data, 2);
    if (out_data[0] == 0x00 || out_data[1] == 0x00)
    {
      BlueSerial_SendArray((uint8_t *)"OK!\r\n", 5);
    }
    OLED_Clear();
    Menu_Choose();
    OLED_Update();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
