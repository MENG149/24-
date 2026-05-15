#ifndef __PID_H__
#define __PID_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
typedef struct {
	float Target;
	float Actual;
	float Out;

	float Kp;
	float Ki;
	float Kd;

	float Error0;
	float Error1;
	float ErrorInt;

	float OutMax;       // 输出上限
	float OutMin;       // 输出下限
	float IntMax;       // 积分限幅上限
	float IntMin;       // 积分限幅下限
	float ErrHighThresh;// 变速积分：误差高阈值（超过则积分系数=0）
	float ErrLowThresh; // 变速积分：误差低阈值（低于则积分系数=1）
} PID_t;

void PID_Update(PID_t *p);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ PID_H__ */

