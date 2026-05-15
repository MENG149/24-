#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define BUTTON1   HAL_GPIO_ReadPin(button1_GPIO_Port, button1_Pin)
#define BUTTON2   HAL_GPIO_ReadPin(button2_GPIO_Port, button2_Pin)
#define BUTTON3   HAL_GPIO_ReadPin(button3_GPIO_Port, button3_Pin)
#define BUTTON4   HAL_GPIO_ReadPin(button4_GPIO_Port, button4_Pin)

#define BUTTON_DEBOUNCE_MS  20u

void Button_Init(void);
uint8_t Button1_Pressed(void);
uint8_t Button2_Pressed(void);
uint8_t Button3_Pressed(void);
uint8_t Button4_Pressed(void);

#ifdef __cplusplus
}
#endif
#endif /* __BUTTON_H__ */
