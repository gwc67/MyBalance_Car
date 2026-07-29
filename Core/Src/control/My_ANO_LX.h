#ifndef __MY_ANO_LX_H
#define __MY_ANO_LX_H

#include "main.h"

// ANO-LX 协议模块 (上位机通信)

typedef struct {
    uint32_t dummy;
} AnoBase;

extern AnoBase *pstAnobase_Lx;

void ano_lx_set_rt_vel_z(int16_t vel_z);
void vano_WTS_set(AnoBase *base, uint8_t cmd, uint8_t val);

#endif
