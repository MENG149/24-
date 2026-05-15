#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void LED_Init(void);
void LED_ON(void);
void LED_OFF(void);
void LED_Toggle(void);

#ifdef __cplusplus
}
#endif
#endif /* __LED_H__ */
