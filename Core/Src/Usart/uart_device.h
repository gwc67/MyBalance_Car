#ifndef __UART_DEVICE_H
#define __UART_DEVICE_H

#include "main.h"
#include "Uart.h"

#define UART_DEV_NUM_MAX    3
#define UART_DEVICE_1               emUartDevNum0
#define UART_DEVICE_2               emUartDevNum1
#define UART_DEVICE_3               emUartDevNum2



typedef enum
{
    emUartErrNone,
    emUartErrSoftWare,
    emUartErrHardWare,

} emUartErrTdf;

typedef enum 
{
    emUartDevNum0   = 0,
    emUartDevNum1,
    emUartDevNum2,
} emUartDevNumTdf;

typedef enum
{
    emChecksumType_None = 0,
    emChecksumType_Sum,
}emChecksumTypeTdf;


typedef struct
{
  uint16_t usFrameHead;

  uint8_t ucSumCheck; // 0 无校验 1 和校验

} stUARTFrameTdf;
// 结构体的声明方法
typedef struct stUartOpsTdf stUartOpsTdf;

//device的本质是使用对应的发送与接受函数
typedef struct
{
    stUartOpsTdf *pstOps;

    void *pvPrivData;

} stUartDeviceTdf, *pstUartDeviceTdf;

extern  pstUartDeviceTdf gapstUartDevice[3];

typedef struct stUartOpsTdf
{
    emUartErrTdf (*pfInit)(stUartDeviceTdf *pstDev);
    emUartErrTdf (*pfSendData)(stUartDeviceTdf *pstDev, uint8_t *pucData, uint16_t usSize);   
    emUartErrTdf (*pfStartRecvData)(stUartDeviceTdf *pstDev);
    emUartErrTdf (*pfOnRxCallBackData)(stUartDeviceTdf *pstDev);
    emUartErrTdf (*pfProcessData)(stUartDeviceTdf *pstDev);

} stUartOpsTdf, *pstUartOpsTdf;



typedef struct 
{
    uint32_t ulBaterate;
    uint32_t ulWordLength;
    uint32_t ulStopBits; 
    uint32_t ulParity;
    uint32_t ulMode;
}stUartConfigTdf;


//   USART_InitStruct.BaudRate = 9600;
//   USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
//   USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
//   USART_InitStruct.Parity = LL_USART_PARITY_NONE;
//   USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
//   USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
//   USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

typedef struct 
{
    USART_TypeDef *pstHandle;
    DMA_TypeDef   *pstDmaHandle;
    uint32_t       ulDmaChannel;
    uint8_t        *pucRxRingBuf;
    uint32_t        ulRxRingBufSize;
    uint32_t        ulDmaRxBufSize;
    stUARTFrameTdf  stFrameCfg;  // 帧格式配置体
    stUartConfigTdf  stUartCfg;  // USART参数配置
}stUartParamInitTdf;



stUartDeviceTdf* pstUartDeviceCreate(stUartParamInitTdf *pstUartPriv);


static inline emUartErrTdf emUartInitInline(stUartDeviceTdf* pstDev)
{
    if (pstDev == NULL || pstDev->pstOps == NULL || pstDev->pstOps->pfInit == NULL)
    {
        return emUartErrSoftWare;
    }
    
    return pstDev->pstOps->pfInit(pstDev);
}
static inline emUartErrTdf emUartSendInline(stUartDeviceTdf* pstDev, uint8_t *pucData, uint16_t usSize)
{
    if (pstDev == NULL || pstDev->pstOps == NULL || pstDev->pstOps->pfSendData == NULL)
    {
        return emUartErrSoftWare;
    }
    
    return pstDev->pstOps->pfSendData(pstDev, pucData, usSize);
}
static inline emUartErrTdf emUartCallBackInline(stUartDeviceTdf* pstDev)
{
    if (pstDev == NULL || pstDev->pstOps == NULL || pstDev->pstOps->pfOnRxCallBackData == NULL)
    {
        return emUartErrSoftWare;
    }
    
    return pstDev->pstOps->pfOnRxCallBackData(pstDev);
}
static inline emUartErrTdf emUartProcessInline(stUartDeviceTdf* pstDev)
{
    if (pstDev == NULL || pstDev->pstOps == NULL || pstDev->pstOps->pfProcessData == NULL)
    {
        return emUartErrSoftWare;
    }
    
    return pstDev->pstOps->pfProcessData(pstDev);
}
static inline emUartErrTdf emUartStartRecvInline(stUartDeviceTdf* pstDev)
{
    if (pstDev == NULL || pstDev->pstOps == NULL || pstDev->pstOps->pfStartRecvData == NULL)
    {
        return emUartErrSoftWare;
    }
    
    return pstDev->pstOps->pfStartRecvData(pstDev);
}


#endif
