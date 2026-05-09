#include "uart_device.h"

typedef struct
{
    stUartDeviceTdf *pstSelf;
    USART_TypeDef *pstHandle;  // 指向父设备
    pstRingBufTdf pstRxRingBuf;  //接受环形缓冲区，等效   vRingBufInit(&stRingBuf_t, 20, buf);  stRingBuf_t
    uint8_t      *pucDmaRxBuf;
    uint32_t        ulDmaRxBufSize; // DMA缓冲区大小；
    stUARTFrameTdf  stFrameCfg;
    stUartConfigTdf stUartCfg;
} stUartPrivTdf;


static emUartErrTdf s_emUartInit(stUartDeviceTdf *pstDev)
{
    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "Init!\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART2, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART2) == RESET)
            ;
    }
}

static emUartErrTdf s_emUartSend(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "Send\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART2, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART2) == RESET)
            ;
    }
}
static emUartErrTdf s_emUartRecv(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "Recv\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART2, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART2) == RESET)
            ;
    }
}
static emUartErrTdf s_emUartCallBack(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "CallBack\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART2, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART2) == RESET)
            ;
    }
}
static emUartErrTdf s_emUartProcess(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    char msg[] = "Process\r\n";
    for (int i = 0; i < strlen(msg); i++)
    {
        LL_USART_TransmitData8(USART2, msg[i]);
        while (LL_USART_IsActiveFlag_TXE(USART2) == RESET)
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

stUartDeviceTdf* pstUartDeviceCreate(stUartParamInitTdf *pstUartPriv)
{
    if(pstUartPriv == NULL)
    {
        return NULL;
    }

    //内存分配的本质思路，malloc后获取对应的指针指向的位置
    stUartDeviceTdf* pstDev = (stUartDeviceTdf*)malloc(sizeof(stUartDeviceTdf));                //定义一个这种类型指向多少字节大小的指针
    stUartPrivTdf*   pstPriv = (stUartPrivTdf*)malloc(sizeof(stUartPrivTdf));
    pstRingBufTdf    pstRingBuf = (pstRingBufTdf)malloc(sizeof(stRingBufTdf));
    uint8_t         *pucDmaRxBuf = (uint8_t*)malloc(pstUartPriv->ulDmaRxBufSize);
    stUARTFrameTdf    *pstUARTFrame = (stUARTFrameTdf*)malloc(sizeof(stUARTFrameTdf));
    stUartConfigTdf   *pstUartConfig = (stUartConfigTdf*)malloc(sizeof(stUartConfigTdf));

    if (pstDev == NULL || pstPriv == NULL || pstRingBuf == NULL || pucDmaRxBuf == NULL || pstUARTFrame == NULL || pstUartConfig == NULL)
    {
        free(pstDev);
        free(pstPriv);
        free(pstRingBuf);
        free(pucDmaRxBuf);
        free(pstUARTFrame);
        free(pstUartConfig);
        return NULL;
    }
    
    pstDev->pvPrivData = pstPriv;                                   //将pstDev下的pstPriv的参数配置好
    pstDev->pstOps = &stUartOps;                                    //这是执行函数

    //将pstPriv指针的指向对象进行初始化
    pstPriv->pstHandle = pstUartPriv->pstHandle;                    // main.c 中 会创建stUartPriv（实参）里面填了对应的pstHandle
    pstPriv->pstRxRingBuf = pstRingBuf;
    pstPriv->pstSelf = pstDev;                                      //将pstDev下的pstPriv的参数配置好
    pstPriv->pucDmaRxBuf = pucDmaRxBuf;                             //将pstPriv 下的DMA缓冲数组指向该pucDmaRxBuf 知识点：指针始终都是8字节，因此pstPriv->pucDmaRxBuf本质还是指针
    pstPriv->ulDmaRxBufSize = pstUartPriv->ulDmaRxBufSize;          //将pstPriv 下的DMA缓冲数组的大小转到 pstUartPriv中定义的数组大小

    memcpy(&pstPriv->stFrameCfg,&pstUartPriv->stFrameCfg,sizeof(stUARTFrameTdf));           //对于stFrameCfg实参，需要将对应实参的数据使用memcpy复制过来
    memcpy(&pstPriv->stUartCfg,&pstUartPriv->stUartCfg,sizeof(stUartConfigTdf));

    //初始化环形缓冲区
    vRingBufInit(pstRingBuf,pstUartPriv->ulRxRingBufSize,pstUartPriv->pucRxRingBuf);        //stUartPriv中会配置环形缓冲区的数组 数组长度   同时对应pstPriv->pstRxRingBuf = pstRingBuf; 这步中pstRingBuf的进一步配置
                                                                                            


    return pstDev;
}