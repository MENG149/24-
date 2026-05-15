#include "led.h"

void LED_Init(void)
{
    /* 已由 MX_GPIO_Init() 初始化，默认低电平（灭） */
}

void LED_ON(void)
{
    HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_SET);
}

void LED_OFF(void)
{
    HAL_GPIO_WritePin(led_GPIO_Port, led_Pin, GPIO_PIN_RESET);
}

void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(led_GPIO_Port, led_Pin);
}
