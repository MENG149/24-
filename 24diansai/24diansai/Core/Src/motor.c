#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"
#include "gpio.h"
#include <stdint.h>
#include "motor.h"

// 编码器计数值变量（定义）
int32_t encoder=0;
int32_t encoder1=0;


void motor_Init(void)
{
    MX_GPIO_Init();
    MX_TIM2_Init();
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_2);
}
void motor_speed1(int16_t speed)
{
   

    if (speed >= 0)
    {
        HAL_GPIO_WritePin(mg310zuo_GPIO_Port, mg310zuo_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        PWM_SetCompare3(speed);
    }
    else
    {
        HAL_GPIO_WritePin(mg310zuo_GPIO_Port, mg310zuo_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        PWM_SetCompare3(-speed);
    }
   
}

void motor_speed2(int16_t speed)
{
  

    if (speed >= 0)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
        PWM_SetCompare4(speed);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
        PWM_SetCompare4(-speed);
    }
   
}
void motor_start(int16_t z_lun, int16_t y_lun)
{                   
    motor_speed1(z_lun);
    motor_speed2(y_lun);
}
int16_t motor_get_encoder1(void)
{
    encoder = __HAL_TIM_GET_COUNTER(&htim3);
    __HAL_TIM_SET_COUNTER(&htim3, 0);  // 正确的清零方法
    return encoder;
}

int16_t motor_get_encoder2(void)
{
    encoder1 = __HAL_TIM_GET_COUNTER(&htim4);
    __HAL_TIM_SET_COUNTER(&htim4, 0);  // 正确的清零方法
    return encoder1;
}