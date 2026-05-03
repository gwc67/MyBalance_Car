#include "uart_device.h"

static emUartErrTdf s_emUartInit(stUartDeviceTdf *pstDev)
{
    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "Init!\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART1, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART1) == RESET)
            ;
    }
}

static emUartErrTdf s_emUartSend(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "Send\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART1, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART1) == RESET)
            ;
    }
}
static emUartErrTdf s_emUartRecv(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "Recv\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART1, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART1) == RESET)
            ;
    }
}
static emUartErrTdf s_emUartCallBack(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "CallBack\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART1, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART1) == RESET)
            ;
    }
}
static emUartErrTdf s_emUartProcess(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "Process\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART1, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART1) == RESET)
            ;
    }
}


stUartOpsTdf stUartOps = {
    .pfInit = s_emUartInit,
    .pfSendData = s_emUartSend,
    .pfStartRecvData = s_emUartRecv,
    .pfOnRxCallBackData = s_emUartCallBack,
    .pfProcessData = s_emUartProcess,
};


//对传入的pstUartPriv 对应的串口号进行函数绑定
//执行结果： 创建的pstDev下的stUartOpsTdf *pstOps; 有了执行函数
// void *pvPrivData;  有了对应的串口USART的序号

stUartDeviceTdf* pstUartDeviceCreate(stUartPrivTdf *pstUartPriv)
{
    if(pstUartPriv == NULL)
    {
        return NULL;
    }

    stUartDeviceTdf* pstDev = (stUartDeviceTdf*)malloc(sizeof(stUartDeviceTdf));
    stUartPrivTdf*   pstPriv = (stUartPrivTdf*)malloc(sizeof(stUartPrivTdf));

    pstDev->pvPrivData = pstPriv;
    pstDev->pstOps = &stUartOps;
    pstPriv->pstHandle = pstUartPriv->pstHandle;

    return pstDev;
}