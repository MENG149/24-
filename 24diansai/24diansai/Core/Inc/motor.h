#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
extern int32_t encoder;    // 编码器1计数值
extern int32_t encoder1;   // 编码器2计数值
/* USER CODE END Private defines */


/* USER CODE BEGIN Prototypes */
void motor_speed2(int16_t speed);
void motor_speed1(int16_t speed);
void motor_start(int16_t z_lun, int16_t y_lun);
void motor_Init(void);
int16_t motor_get_encoder1(void);
int16_t motor_get_encoder2(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ MOTOR_H__ */

