#include "PID.h"

// 该函数的本质：Update此时的PID输出Out，得到Out；
void PID_Update(PID_t *p)
{
    p->Error1 = p->Error0;
    p->Error0 = p->Target - p->Actual;

    if (p->Ki != 0)
    {
        p->ErrorInt += p->Error0;
    }
    else
    {
        p->ErrorInt = 0;
    }
    if (p->ErrorInt > 500)
    {
        p->ErrorInt = 500;
    }
    else if (p->ErrorInt < -500)
    {
        p->ErrorInt = -500;
    }
    p->Out = p->Kp * p->Error0 + p->Ki * p->ErrorInt + p->Kd * (p->Error0 - p->Error1);

    if (p->Out > p->OutMax)
    {
        p->Out = p->OutMax;
    }
    else if (p->Out < p->OutMin)
    {
        p->Out = p->OutMin;
    }
}