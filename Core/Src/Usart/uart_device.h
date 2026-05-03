#ifndef __UART_DEVICE_H
#define __UART_DEVICE_H

#include "main.h"
#include "Uart.h"
typedef enum
{
    emUartErrNone,
    emUartErrSoftWare,
    emUartErrHardWare,

} emUartErrTdf;

// 结构体的声明方法
typedef struct stUartOpsTdf stUartOpsTdf;

typedef struct
{
    stUartOpsTdf *pstOps;

    void *pvPrivData;

} stUartDeviceTdf, *pstUartDeviceTdf;

typedef struct stUartOpsTdf
{
    emUartErrTdf (*pfInit)(stUartDeviceTdf *pstDev);
    emUartErrTdf (*pfSendData)(stUartDeviceTdf *pstDev);
    emUartErrTdf (*pfStartRecvData)(stUartDeviceTdf *pstDev);
    emUartErrTdf (*pfOnRxCallBackData)(stUartDeviceTdf *pstDev);
    emUartErrTdf (*pfProcessData)(stUartDeviceTdf *pstDev);

} stUartOpsTdf, *pstUartOpsTdf;

typedef struct
{
    USART_TypeDef *pstHandle;
} stUartPrivTdf;


stUartDeviceTdf* pstUartDeviceCreate(stUartPrivTdf *pstUartPriv);


static inline emUartErrTdf emUartInitInline(stUartDeviceTdf* pstDev)
{
    if (pstDev == NULL || pstDev->pstOps == NULL || pstDev->pstOps->pfInit == NULL)
    {
        return emUartErrSoftWare;
    }
    
    return pstDev->pstOps->pfInit(pstDev);
}
static inline emUartErrTdf emUartSendInline(stUartDeviceTdf* pstDev)
{
    if (pstDev == NULL || pstDev->pstOps == NULL || pstDev->pstOps->pfSendData == NULL)
    {
        return emUartErrSoftWare;
    }
    
    return pstDev->pstOps->pfSendData(pstDev);
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
