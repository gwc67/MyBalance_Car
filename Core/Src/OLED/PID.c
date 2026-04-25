#include "PID.h"

// 该函数的本质：Update此时的PID输出Out，得到Out；
void PID_Update(PID_t *p)
{
    p->Error1 = p->Error0;
    p->Error0 = p->Target - p->Actual;

    if (p->Ki != 0)
    {
        p->ErrorInt += p->Error0; // 误差积分

        /*PID优化：积分限幅*/
        if (p->ErrorInt > p->ErrorIntMax)
        {
            p->ErrorInt = p->ErrorIntMax;
        }
        if (p->ErrorInt < p->ErrorIntMin)
        {
            p->ErrorInt = p->ErrorIntMin;
        }
    }
    else
    {
        p->ErrorInt = 0;
    }

    p->Out = p->Kp * p->Error0 + p->Ki * p->ErrorInt + p->Kd * (p->Error0 - p->Error1);
    /*PID优化：输出偏移*/
    if (p->Out > 0 || p->Out < 6)
    {
        p->Out += p->OutOffset;
    }
    if (p->Out < 0 || p->Out > -6)
    {
        p->Out -= p->OutOffset;
    }

    if (p->Out > p->OutMax)
    {
        p->Out = p->OutMax;
    }
    else if (p->Out < p->OutMin)
    {
        p->Out = p->OutMin;
    }
}