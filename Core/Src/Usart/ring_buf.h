#ifndef __RING_BUF_H
#define __RING_BUF_H

#include "main.h"
typedef struct 
{
    volatile uint32_t ulReadIdx;
    volatile uint32_t ulWriteIdx;
    volatile uint32_t ulLength;
    volatile uint8_t  *pucBuffer;


}stRingBufTdf,*pstRingBufTdf;

#include "ring_buf.h"


void vRingBufInit(pstRingBufTdf pstRingBuf,uint32_t ulLen , uint8_t *pucBuf);

uint8_t ucRingBufWrite(pstRingBufTdf pstRingBuf,uint8_t ucValue);

uint8_t ucRingBufRead(pstRingBufTdf pstRingBuf,uint8_t *pucData);

uint8_t ucRingBufGetLength(pstRingBufTdf pstRingBuf);

uint8_t ucRingBufPeek(pstRingBufTdf psRingBuf, uint32_t ulPostion);

#endif  