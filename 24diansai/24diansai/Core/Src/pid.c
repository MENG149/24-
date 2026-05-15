#include "stm32f1xx_hal.h"
#include "pid.h"
#include <math.h>

void PID_Update(PID_t *p)
{
    /* 误差计算 */
    p->Error1 = p->Error0;
    p->Error0 = p->Target - p->Actual;

    if (p->Ki != 0)
    {
        /* 变速积分：根据误差大小动态调整积分系数 */
        float ki_coeff;
        float abs_err = fabsf(p->Error0);

        if (abs_err >= p->ErrHighThresh)
            ki_coeff = 0.0f;                  // 误差过大，停止积分防止饱和
        else if (abs_err <= p->ErrLowThresh)
            ki_coeff = 1.0f;                  // 误差小，全速积分
        else
            ki_coeff = (p->ErrHighThresh - abs_err)
                     / (p->ErrHighThresh - p->ErrLowThresh); // 线性过渡

        p->ErrorInt += ki_coeff * p->Error0;

        /* 积分限幅 */
        if (p->ErrorInt > p->IntMax) p->ErrorInt = p->IntMax;
        if (p->ErrorInt < p->IntMin) p->ErrorInt = p->IntMin;
    }
    else
    {
        p->ErrorInt = 0;
    }

    /* PID 位置式计算 */
    p->Out = p->Kp * p->Error0
           + p->Ki * p->ErrorInt
           + p->Kd * (p->Error0 - p->Error1);

    /* 输出限幅 */
    if (p->Out > p->OutMax) p->Out = p->OutMax;
    if (p->Out < p->OutMin) p->Out = p->OutMin;
}
