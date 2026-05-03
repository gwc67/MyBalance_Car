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
extern float SpeedL, SpeedR;
extern void Serial_Init_LL(void);
extern void Serial_ProcessCommand_LL(void);
extern void Serial_ReportData_LL(void);

// extern   int16_t AX,AY,AZ,GX,GY,GZ;

uint8_t flag;
uint8_t RunFlag;
int16_t LeftPwm, RightPwm;
int16_t AvePwm, DifPwm;

stRingBufTdf stRingBuf_t;

uint8_t rece_it_data;

uint8_t buf[20];

typedef struct{
  uint16_t usFrameHead;
  
  uint8_t ucSumCheck ; // 0 无校验 1 和校验

}stUARTFrameTdf;

stUARTFrameTdf stUARTFrame = {
  .usFrameHead = 0x55AA,
  .ucSumCheck = 1,
  
};

void USART1_IRQHandler(void)
{
  // 检查是否接收到数据
  if (LL_USART_IsActiveFlag_RXNE(USART1))
  {
    // 读取接收到的数据
    LL_USART_ClearFlag_RXNE(USART1);
    rece_it_data = LL_USART_ReceiveData8(USART1);
    ucRingBufWrite(&stRingBuf_t, rece_it_data);
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  MPU6050_Init(MPU6050_SCL_GPIO_Port, MPU6050_SCL_Pin, MPU6050_SDA_GPIO_Port, MPU6050_SDA_Pin);
  BlueSerial_Init();
  MX_TIM1_Init();
  Servo_Init();
  Serial_Init_LL();
  Key_Init();
  Encode_Init();
  Store_Init();
  OLED_Init();
  Menu_Init();

  vRingBufInit(&stRingBuf_t, 20, buf);
  uint8_t temp_get_data;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (RunFlag)
    {
      LED1_OFF();
    }
    else
    {
      LED1_ON();
    }

    if (Key_Check(KEY_1, KEY_DOUBLE))
    {
      RunFlag = !RunFlag;
    }

    if (ucRingBufGetLength(&stRingBuf_t) > 6)
    {
      uint8_t head1,head2,len,sum,temp; 
      uint8_t calc_sum = 0;
      uint8_t data_buf[10];
      uint16_t temp_head;

      head1 = ucRingBufPeek(&stRingBuf_t,0);
      head2 = ucRingBufPeek(&stRingBuf_t,1);

      temp_head = ((uint16_t)head1 << 8) | head2;


      if (temp_head != stUARTFrame.usFrameHead)
      {
        ucRingBufRead(&stRingBuf_t,&temp);
        continue;
      }
      
      ucRingBufRead(&stRingBuf_t,&head1);
      ucRingBufRead(&stRingBuf_t,&head2);
      ucRingBufRead(&stRingBuf_t,&len);
      
      calc_sum = head1 + head2 +len;
      if (len > 10)
      {
        continue;
      }
      
      for (int i = 0; i < len; i++)
      {
        ucRingBufRead(&stRingBuf_t,&data_buf[i]);
        calc_sum += data_buf[i];
      }
      ucRingBufRead(&stRingBuf_t,&sum);

      if (calc_sum != sum)
      {
        continue;
      }
      
      char msg[10];
      char msg1[10];
      sprintf(msg,"length:%d\r\n",len);
      Serial_SendArray((uint8_t*)msg,strlen(msg));
      sprintf(msg1,"calc_sum:%d\r\n",calc_sum);
      Serial_SendArray((uint8_t*)msg1,strlen(msg1));
      
      if (data_buf[0] == 0x01 && data_buf[1] == 0x02)
      {
        Serial_SendArray("OK!\r\n",5);
      }
      
      // ucRingBufRead(&stRingBuf_t,&temp_get_data);
      
      // Serial_SendByte_LL(temp_get_data);
    }
    else{
      continue;
    }
    
    // if (BlueSerial_RxFlag)
    // {

    //   BlueSerial_RxFlag = 0;
    //   SpeedPID.Target = FloatArray[0] / 40;
    //   DifPwm = FloatArray[1] / 2;
    //   OLED_Printf(0,0,48,"%.2f",SpeedPID.Target);
    // }

    OLED_Clear();
    Menu_Choose();
    OLED_Update();

    // Serial_ProcessCommand_LL();  // 处理上位机指令
    // Serial_ReportData_LL(); // 上
    // Serial_Printf("%d",1);
    // Serial_SendString_LL("Hello from STM32 Balance Car!\r\n");
    // BlueSerial_Printf("%d,%d,%d\n",raw.AccX,raw.AccY,raw.AccZ);

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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
