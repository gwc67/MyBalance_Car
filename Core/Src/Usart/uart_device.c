#include "uart_device.h"
#include <string.h>

pstUartDeviceTdf gapstUartDevice[3] = {0}; 

typedef struct
{
    stUartDeviceTdf *pstSelf;
    USART_TypeDef *pstHandle; // 指向父设备
    DMA_TypeDef *pstDmaHandle;
    uint32_t ulDmaChannel;
    pstRingBufTdf pstRxRingBuf; // 接受环形缓冲区，等效   vRingBufInit(&stRingBuf_t, 20, buf);  stRingBuf_t
    uint8_t *pucDmaRxBuf;
    uint32_t ulDmaRxBufSize; // DMA缓冲区大小；
    stUARTFrameTdf stFrameCfg;
    stUartConfigTdf stUartCfg;
} stUartPrivTdf;

static emUartErrTdf s_emUartInit(stUartDeviceTdf *pstDev)
{
    stUartPrivTdf *pstPriv = (stUartPrivTdf *)pstDev->pvPrivData;
    if (pstPriv == NULL || pstPriv->pstHandle == NULL)
    {
        return emUartErrSoftWare;
    }

    if (pstPriv->stUartCfg.ulBaterate != 0)
    {
        LL_USART_Disable(pstPriv->pstHandle);

        uint32_t periphclk = LL_RCC_PERIPH_FREQUENCY_NO;
        LL_RCC_ClocksTypeDef rcc_clocks;
        LL_RCC_GetSystemClocksFreq(&rcc_clocks);
        if (pstPriv->pstHandle == USART1)
        {
            periphclk = rcc_clocks.PCLK2_Frequency;
        }
        else if (pstPriv->pstHandle == USART2 || pstPriv->pstHandle == USART3)
        {
            periphclk = rcc_clocks.PCLK1_Frequency;
        }
        if (periphclk == LL_RCC_PERIPH_FREQUENCY_NO)
        {
            return emUartErrHardWare;
        }

        LL_USART_SetBaudRate(pstPriv->pstHandle, periphclk, pstPriv->stUartCfg.ulBaterate);
        LL_USART_SetDataWidth(pstPriv->pstHandle, pstPriv->stUartCfg.ulWordLength);
        LL_USART_SetStopBitsLength(pstPriv->pstHandle, pstPriv->stUartCfg.ulStopBits);
        LL_USART_SetParity(pstPriv->pstHandle, pstPriv->stUartCfg.ulParity);
        LL_USART_SetTransferDirection(pstPriv->pstHandle, pstPriv->stUartCfg.ulMode);
        LL_USART_Enable(pstPriv->pstHandle);
    }

    if (pstPriv->pstDmaHandle != NULL && pstPriv->pucDmaRxBuf != NULL)
    {
        LL_DMA_DisableChannel(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel);

        LL_DMA_ConfigAddresses(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel,
                               LL_USART_DMA_GetRegAddr(pstPriv->pstHandle),
                               (uint32_t)pstPriv->pucDmaRxBuf,
                               LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
        // 这个长度指的就是接受数组的长度
        LL_DMA_SetDataLength(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel, pstPriv->ulDmaRxBufSize);

        LL_DMA_SetMemoryIncMode(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphIncMode(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel, LL_DMA_PERIPH_NOINCREMENT);

        LL_DMA_SetMemorySize(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel, LL_DMA_MDATAALIGN_BYTE);
        LL_DMA_SetPeriphSize(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel, LL_DMA_PDATAALIGN_BYTE);

        LL_DMA_SetMode(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel, LL_DMA_MODE_CIRCULAR);
        LL_DMA_SetChannelPriorityLevel(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel, LL_DMA_PRIORITY_MEDIUM);

        LL_USART_EnableIT_IDLE(pstPriv->pstHandle);
        LL_USART_EnableDMAReq_RX(pstPriv->pstHandle);

        LL_DMA_EnableChannel(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel);
    }
    return emUartErrNone;

    // char msg[] = "Init!\r\n";
    // for (int i = 0; i < strlen(msg); i++)
    // {
    //     LL_USART_TransmitData8(USART2, msg[i]);
    //     while (LL_USART_IsActiveFlag_TXE(USART2) == RESET)
    //         ;
    // }
}

static emUartErrTdf s_emUartSend(stUartDeviceTdf *pstDev, uint8_t *pucData, uint16_t usSize)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    if (pstPriv == NULL || pstPriv->pstHandle == NULL)
    {
        return emUartErrSoftWare;
    }

    for (int i = 0; i < usSize; i++)
    {
        LL_USART_TransmitData8(pstPriv->pstHandle, pucData[i]);
        while (LL_USART_IsActiveFlag_TXE(pstPriv->pstHandle) == RESET)
            ;
    }
    return emUartErrNone;
}

// 这是一个启动接受的函数
static emUartErrTdf s_emUartRecv(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    if (pstPriv == NULL || pstPriv->pstHandle == NULL)
    {
        return emUartErrSoftWare;
    }
    LL_DMA_ConfigAddresses(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel,
                           LL_USART_DMA_GetRegAddr(pstPriv->pstHandle),
                           (uint32_t)pstPriv->pucDmaRxBuf,
                           LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    LL_USART_EnableIT_IDLE(pstPriv->pstHandle);
    LL_USART_EnableDMAReq_RX(pstPriv->pstHandle);
    // char msg[] = "Recv\r\n";
    // for (int i = 0; i < strlen(msg); i++)
    // {
    //     LL_USART_TransmitData8(USART2, msg[i]);
    //     while (LL_USART_IsActiveFlag_TXE(USART2) == RESET)
    //         ;
    // }
    return emUartErrNone;
}
static emUartErrTdf s_emUartCallBack(stUartDeviceTdf *pstDev)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;

    if (pstPriv == NULL || pstPriv->pstHandle == NULL)
    {
        return emUartErrSoftWare;
    }

    // 必须先停DMA再清IDLE，否则LL_USART_ClearFlag_IDLE内部读DR会与DMA竞争
    LL_DMA_DisableChannel(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel);
    LL_USART_ClearFlag_IDLE(pstPriv->pstHandle);
    uint32_t received = pstPriv->ulDmaRxBufSize - LL_DMA_GetDataLength(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel);
    LL_DMA_SetDataLength(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel, pstPriv->ulDmaRxBufSize);
    LL_DMA_EnableChannel(pstPriv->pstDmaHandle, pstPriv->ulDmaChannel);

    for (int i = 0; i < received; i++)
    {
        ucRingBufWrite(pstPriv->pstRxRingBuf, pstPriv->pucDmaRxBuf[i]);
                                                                            //对接受的数据进行回传
        // LL_USART_TransmitData8(pstPriv->pstHandle, pstPriv->pucDmaRxBuf[i]);
        // while (LL_USART_IsActiveFlag_TXE(pstPriv->pstHandle) == RESET)
        //     ;
    }
    return emUartErrNone;
}

union
{
    float fConvertFloat;
    uint8_t ucConvertData[4];
} unConvertUIN;

static PID_t *s_pstGetPidByName(const char *pucName)
{
    if (strcmp(pucName, "AnglePID") == 0) return &AnglePID;
    if (strcmp(pucName, "GyroPID") == 0)  return &GyroPID;
    if (strcmp(pucName, "SpeedPID") == 0) return &SpeedPID;
    if (strcmp(pucName, "TurnPID") == 0)  return &TurnPID;
    return NULL;
}

static float s_fAtof(const char *pucStr)
{
    float fResult = 0.0f;
    float fSign = 1.0f;
    if (*pucStr == '-') { fSign = -1.0f; pucStr++; }

    while (*pucStr >= '0' && *pucStr <= '9')
        fResult = fResult * 10.0f + (*pucStr++ - '0');

    if (*pucStr == '.')
    {
        pucStr++;
        float fFrac = 0.1f;
        while (*pucStr >= '0' && *pucStr <= '9')
        {
            fResult += (*pucStr++ - '0') * fFrac;
            fFrac *= 0.1f;
        }
    }
    return fSign * fResult;
}

// Parse text command: {Name kp ki kd} or {Report} from ring buffer
static emUartErrTdf s_emParseTextCmd(stUartDeviceTdf *pstDev, pstRingBufTdf pstRingBuf, uint8_t *pucOutData, uint16_t usLen)
{
    uint32_t ulLen = ucRingBufGetLength(pstRingBuf);
    if (ulLen < 5) return emUartErrNone;

    uint8_t ucByte;
    if (ucRingBufPeek(pstRingBuf, 0) != '{')
        return emUartErrNone;

    // Find '}' position (max 50 bytes)
    int32_t lEndPos = -1;
    for (uint32_t i = 1; i < ulLen && i < 50; i++)
    {
        if (ucRingBufPeek(pstRingBuf, i) == '}')
        {
            lEndPos = (int32_t)i;
            break;
        }
    }
    if (lEndPos < 0) return emUartErrNone; // Wait for more data

    // Extract command bytes
    char acCmdBuf[50] = {0};
    for (int32_t i = 0; i <= lEndPos; i++)
        acCmdBuf[i] = (char)ucRingBufPeek(pstRingBuf, (uint32_t)i);
    acCmdBuf[lEndPos] = '\0';

    // Consume from ring buffer
    for (int32_t i = 0; i <= lEndPos; i++)
        ucRingBufRead(pstRingBuf, &ucByte);

    // Parse: skip '{', extract name
    char *pucPtr = acCmdBuf + 1;
    while (*pucPtr == ' ') pucPtr++;

    char acName[16] = {0};
    uint32_t ulIdx = 0;
    while (*pucPtr != ' ' && *pucPtr != '\0' && ulIdx < 15)
        acName[ulIdx++] = *pucPtr++;
    acName[ulIdx] = '\0';

    // Handle {Report} command
    if (strcmp(acName, "Report") == 0)
    {
        char acBuf[128];
        int n = sprintf(acBuf,
            "R,%.2f,%.2f,%.2f,%.2f,%d,%.3f,%.3f,%.3f,%u\r\n",
            Angle, GyroY_Actual, AnglePID.Target,  AnglePID.Out,
            AvePwm, AnglePID.Kp, AnglePID.Ki, AnglePID.Kd, RunFlag);
        s_emUartSend(pstDev, (uint8_t *)acBuf, (uint16_t)n);
        if (usLen >= 2) { pucOutData[0] = 0x00; pucOutData[1] = 0x00; }
        return emUartErrNone;
    }

    PID_t *pstPid = s_pstGetPidByName(acName);
    if (pstPid == NULL) return emUartErrNone;

    // Parse 3 floats: kp, ki, kd
    float afVals[3];
    for (int i = 0; i < 3; i++)
    {
        while (*pucPtr == ' ') pucPtr++;
        if (*pucPtr == '\0') return emUartErrNone;
        afVals[i] = s_fAtof(pucPtr);
        while (*pucPtr != ' ' && *pucPtr != '\0') pucPtr++;
    }

    pstPid->Kp = afVals[0];
    pstPid->Ki = afVals[1];
    pstPid->Kd = afVals[2];

    Menu_SyncVarToFlash(&pstPid->Kp);
    Menu_SyncVarToFlash(&pstPid->Ki);
    Menu_SyncVarToFlash(&pstPid->Kd);

    Menu_FlashSave();
    // Signal success via ucOutData
    if (usLen >= 2)
    {
        pucOutData[0] = 0x00;
        pucOutData[1] = 0x00;
    }
    return emUartErrNone;
}

// usLen 想要获取的数据的长度
static emUartErrTdf s_emUartProcess(stUartDeviceTdf *pstDev, uint8_t *ucOutData, uint16_t usLen)
{

    stUartPrivTdf *pstPriv = pstDev->pvPrivData;
    if (pstPriv == NULL || pstPriv->pstHandle == NULL)
    {
        return emUartErrSoftWare;
    }

    // Try text command fi
    if (ucRingBufGetLength(pstPriv->pstRxRingBuf) > 0 && ucRingBufPeek(pstPriv->pstRxRingBuf, 0) == '{')
    {
        s_emParseTextCmd(pstDev, pstPriv->pstRxRingBuf, ucOutData, usLen);
    }

    if (ucRingBufGetLength(pstPriv->pstRxRingBuf) > 5)
    {
        uint8_t head1, head2, len, sum, temp;
        uint8_t calc_sum = 0;
        // uint8_t data_buf[30];
        uint16_t temp_head;

        head1 = ucRingBufPeek(pstPriv->pstRxRingBuf, 0);
        head2 = ucRingBufPeek(pstPriv->pstRxRingBuf, 1);

        temp_head = ((uint16_t)head1 << 8) | head2;

        if (temp_head != pstPriv->stFrameCfg.usFrameHead)
        {
            ucRingBufRead(pstPriv->pstRxRingBuf, &temp);
            return emUartErrSoftWare;
        }

        ucRingBufRead(pstPriv->pstRxRingBuf, &head1);
        ucRingBufRead(pstPriv->pstRxRingBuf, &head2);
        ucRingBufRead(pstPriv->pstRxRingBuf, &len);

        calc_sum = head1 + head2 + len;
        if (len > 14)
        {
            return emUartErrSoftWare;
            // continue理解为重新执行循环
        }
        for (int i = 0; i < len; i++)
        {
            uint8_t byte;
            ucRingBufRead(pstPriv->pstRxRingBuf, &byte);
            calc_sum += byte;
            if (i < usLen)
            {
                ucOutData[i] = byte;
            }
        }
        if (pstPriv->stFrameCfg.ucSumCheck == 1)
        {
            ucRingBufRead(pstPriv->pstRxRingBuf, &sum);
            if (calc_sum != sum)
            {
                return emUartErrSoftWare;
            }
        }
        // BlueSerial_SendArray(data_buf, len);
        // float fArray[3];
        // for (int i = 0; i < len; i += 4)
        // {
        //     unConvertUIN.ucConvertData[0] = data_buf[i];
        //     unConvertUIN.ucConvertData[1] = data_buf[i + 1];
        //     unConvertUIN.ucConvertData[2] = data_buf[i + 2];
        //     unConvertUIN.ucConvertData[3] = data_buf[i + 3];
        //     fArray[i / 4] = unConvertUIN.fConvertFloat;
        // }

        // SpeedPID.Kp = fArray[0];
        // SpeedPID.Ki = fArray[1];
        // SpeedPID.Kd = fArray[2];
        // Menu_SyncVarToFlash(&(SpeedPID.Kp));
        // Menu_SyncVarToFlash(&(SpeedPID.Ki));
        // Menu_SyncVarToFlash(&(SpeedPID.Kd));
        // Menu_FlashSave();
    }
    return emUartErrNone;
}

stUartOpsTdf stUartOps = {
    .pfInit = s_emUartInit,
    .pfSendData = s_emUartSend,
    .pfStartRecvData = s_emUartRecv,
    .pfOnRxCallBackData = s_emUartCallBack,
    .pfProcessData = s_emUartProcess,
};

// 对传入的pstUartPriv 对应的串口号进行函数绑定
// 执行结果： 创建的pstDev下的stUartOpsTdf *pstOps; 有了执行函数
//  void *pvPrivData;  有了对应的串口USART的序号

stUartDeviceTdf *pstUartDeviceCreate(stUartParamInitTdf *pstUartPriv)
{
    if (pstUartPriv == NULL)
    {
        return NULL;
    }

    // 内存分配的本质思路，malloc后获取对应的指针指向的位置
    stUartDeviceTdf *pstDev = (stUartDeviceTdf *)malloc(sizeof(stUartDeviceTdf)); // 定义一个这种类型指向多少字节大小的指针
    stUartPrivTdf *pstPriv = (stUartPrivTdf *)malloc(sizeof(stUartPrivTdf));
    pstRingBufTdf pstRingBuf = (pstRingBufTdf)malloc(sizeof(stRingBufTdf));
    uint8_t *pucDmaRxBuf = (uint8_t *)malloc(pstUartPriv->ulDmaRxBufSize);
    stUARTFrameTdf *pstUARTFrame = (stUARTFrameTdf *)malloc(sizeof(stUARTFrameTdf));
    stUartConfigTdf *pstUartConfig = (stUartConfigTdf *)malloc(sizeof(stUartConfigTdf));

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

    pstDev->pvPrivData = pstPriv; // 将pstDev下的pstPriv的参数配置好
    pstDev->pstOps = &stUartOps;  // 这是执行函数

    // 将pstPriv指针的指向对象进行初始化
    pstPriv->pstHandle = pstUartPriv->pstHandle; // main.c 中 会创建stUartPriv（实参）里面填了对应的pstHandle
    pstPriv->pstDmaHandle = pstUartPriv->pstDmaHandle;
    pstPriv->ulDmaChannel = pstUartPriv->ulDmaChannel;
    pstPriv->pstRxRingBuf = pstRingBuf;
    pstPriv->pstSelf = pstDev;                             // 将pstDev下的pstPriv的参数配置好
    pstPriv->pucDmaRxBuf = pucDmaRxBuf;                    // 将pstPriv 下的DMA缓冲数组指向该pucDmaRxBuf 知识点：指针始终都是8字节，因此pstPriv->pucDmaRxBuf本质还是指针
    pstPriv->ulDmaRxBufSize = pstUartPriv->ulDmaRxBufSize; // 将pstPriv 下的DMA缓冲数组的大小转到 pstUartPriv中定义的数组大小

    memcpy(&pstPriv->stFrameCfg, &pstUartPriv->stFrameCfg, sizeof(stUARTFrameTdf)); // 对于stFrameCfg实参，需要将对应实参的数据使用memcpy复制过来
    memcpy(&pstPriv->stUartCfg, &pstUartPriv->stUartCfg, sizeof(stUartConfigTdf));

    // 初始化环形缓冲区
    vRingBufInit(pstRingBuf, pstUartPriv->ulRxRingBufSize, pstUartPriv->pucRxRingBuf); // stUartPriv中会配置环形缓冲区的数组 数组长度   同时对应pstPriv->pstRxRingBuf = pstRingBuf; 这步中pstRingBuf的进一步配置

    return pstDev;
}