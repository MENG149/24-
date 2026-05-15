#ifndef __BUZZER_H__
#define __BUZZER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void Buzzer_Init(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_Beep(uint32_t ms);

#ifdef __cplusplus
}
#endif
#endif /* __BUZZER_H__ */
