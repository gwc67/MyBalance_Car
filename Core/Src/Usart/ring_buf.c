#include "ring_buf.h"


void vRingBufInit(pstRingBufTdf pstRingBuf,uint32_t ulLen , uint8_t *pucBuf)
{
    pstRingBuf->ulReadIdx = 0;
    pstRingBuf->ulWriteIdx = 0;

    pstRingBuf->ulLength = ulLen;
    pstRingBuf->pucBuffer = pucBuf;
    
    for (int i = 0; i < ulLen; i++)
    {
        pucBuf[i] = 0;
    }
    
}

uint8_t ucRingBufWrite(pstRingBufTdf pstRingBuf,uint8_t ucValue)
{
    uint32_t ulNextWrite = pstRingBuf->ulWriteIdx + 1;
    if (ulNextWrite == pstRingBuf->ulLength)
    {
        ulNextWrite = 0;
    }

    if (ulNextWrite == pstRingBuf->ulReadIdx)
    {
        return 1;
    }
    else
    {
        pstRingBuf->pucBuffer[pstRingBuf->ulWriteIdx] = ucValue;
        pstRingBuf->ulWriteIdx = ulNextWrite;
    }
    return 0;
}

uint8_t ucRingBufRead(pstRingBufTdf pstRingBuf,uint8_t *pucData)
{
    if (pstRingBuf->ulReadIdx == pstRingBuf->ulWriteIdx)
    {
        return 1;
    }
    else
    {
        
        *pucData = pstRingBuf->pucBuffer[pstRingBuf->ulReadIdx];
        pstRingBuf->ulReadIdx++;
        if (pstRingBuf->ulReadIdx == pstRingBuf->ulLength)
        {
            pstRingBuf->ulReadIdx = 0;
        }
    }
    return 0;    
}

uint8_t ucRingBufGetLength(pstRingBufTdf pstRingBuf)
{
    return (pstRingBuf->ulWriteIdx - pstRingBuf->ulReadIdx + pstRingBuf->ulLength) % pstRingBuf->ulLength;
}

//ulPostion 0 1 2 3 4 5 6

//读取指定位置的函数

uint8_t ucRingBufPeek(pstRingBufTdf psRingBuf, uint32_t ulPostion)
{
    if (psRingBuf->ulWriteIdx == psRingBuf->ulReadIdx)
    {
        return 0xff;
    }

    uint32_t ulRealPos = (psRingBuf->ulReadIdx + ulPostion) % psRingBuf->ulLength;

    return psRingBuf->pucBuffer[ulRealPos];
    
}