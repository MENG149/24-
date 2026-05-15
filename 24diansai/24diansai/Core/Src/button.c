#include "button.h"

void Button_Init(void)
{
    /* 已由 MX_GPIO_Init() 配置为输入上拉，无需额外初始化 */
}

uint8_t Button1_Pressed(void)
{
    if (BUTTON1 == GPIO_PIN_RESET)
    {
        HAL_Delay(BUTTON_DEBOUNCE_MS);
        if (BUTTON1 == GPIO_PIN_RESET)
        {
            while (BUTTON1 == GPIO_PIN_RESET);
            return 1;
        }
    }
    return 0;
}

uint8_t Button2_Pressed(void)
{
    if (BUTTON2 == GPIO_PIN_RESET)
    {
        HAL_Delay(BUTTON_DEBOUNCE_MS);
        if (BUTTON2 == GPIO_PIN_RESET)
        {
            while (BUTTON2 == GPIO_PIN_RESET);
            return 1;
        }
    }
    return 0;
}

uint8_t Button3_Pressed(void)
{
    if (BUTTON3 == GPIO_PIN_RESET)
    {
        HAL_Delay(BUTTON_DEBOUNCE_MS);
        if (BUTTON3 == GPIO_PIN_RESET)
        {
            while (BUTTON3 == GPIO_PIN_RESET);
            return 1;
        }
    }
    return 0;
}

uint8_t Button4_Pressed(void)
{
    if (BUTTON4 == GPIO_PIN_RESET)
    {
        HAL_Delay(BUTTON_DEBOUNCE_MS);
        if (BUTTON4 == GPIO_PIN_RESET)
        {
            while (BUTTON4 == GPIO_PIN_RESET);
            return 1;
        }
    }
    return 0;
}
