#include "buzzer.h"

void Buzzer_Init(void)
{
    /* 已由 MX_GPIO_Init() 初始化，默认低电平（关） */
}

void Buzzer_ON(void)
{
    HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_SET);
}

void Buzzer_OFF(void)
{
    HAL_GPIO_WritePin(buzzer_GPIO_Port, buzzer_Pin, GPIO_PIN_RESET);
}

void Buzzer_Beep(uint32_t ms)
{
    Buzzer_ON();
    HAL_Delay(ms);
    Buzzer_OFF();
    HAL_Delay(ms);
}
