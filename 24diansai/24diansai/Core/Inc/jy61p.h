#ifndef __JY61P_H_
#define __JY61P_H_

#include "main.h"
#include "usart.h"

void JY61p_Init(void);
void JY61p_GetAngle(float *Roll, float *Pitch, float *Yaw);

#endif /* __JY61P_H_ */
