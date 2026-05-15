#ifndef __TRACK_H__
#define __TRACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define D1    HAL_GPIO_ReadPin(D1_GPIO_Port, D1_Pin)
#define D2    HAL_GPIO_ReadPin(D2_GPIO_Port, D2_Pin)
#define D3    HAL_GPIO_ReadPin(D3_GPIO_Port, D3_Pin)
#define D4    HAL_GPIO_ReadPin(D4_GPIO_Port, D4_Pin)
#define D5    HAL_GPIO_ReadPin(D5_GPIO_Port, D5_Pin)
#define D6    HAL_GPIO_ReadPin(D6_GPIO_Port, D6_Pin)
#define D7    HAL_GPIO_ReadPin(D7_GPIO_Port, D7_Pin)

void Track_Init(void);
void Track_move(void);
uint8_t Track_AnyLine(void);

#ifdef __cplusplus
}
#endif
#endif /* __TRACK_H__ */
