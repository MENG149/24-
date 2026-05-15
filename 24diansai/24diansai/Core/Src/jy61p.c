#include "jy61p.h"
#include <string.h>
#include <stdio.h>

/* 私有变量 */
static uint8_t s_angle_raw[8];  // [0..6]=数据, [7]=完成标志
static uint8_t s_rx_byte;

/* 解析后的角度值（供主循环读取） */
static volatile float s_roll  = 0.0f;
static volatile float s_pitch = 0.0f;
static volatile float s_yaw   = 0.0f;

/**
 * @brief  初始化JY61P，启动USART1接收中断
 */
void JY61p_Init(void)
{
    memset(s_angle_raw, 0, sizeof(s_angle_raw));
    HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1);
}

/**
 * @brief  JY61P单字节解析状态机（仅处理角度包 0x55 0x53）
 * @retval 1=一帧角度数据接收完成, 0=未完成
 */
static uint8_t JY61p_ParseByte(uint8_t byte)
{
    static uint8_t sta = 0;
    static uint8_t idx = 0;

    switch (sta)
    {
        case 0:
            if (byte == 0x55) sta = 1;
            break;

        case 1:
            if (byte == 0x53)
                { idx = 0; sta = 2; }
            else if (byte == 0x55)
                sta = 1;   // 连续帧头，保持等待
            else
                sta = 0;
            break;

        case 2:
            s_angle_raw[idx++] = byte;
            if (idx >= 7)
                { sta = 3; }
            break;

        case 3:  // 校验字节，跳过
            sta = 0;
            return 1;  // 一帧完成

        default:
            sta = 0;
            break;
    }
    return 0;
}

/**
 * @brief  UART接收完成回调 — USART1接收JY61P，解析完成后printf输出
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (JY61p_ParseByte(s_rx_byte))
        {
            s_roll  = (int16_t)((s_angle_raw[1] << 8) | s_angle_raw[0]) / 32768.0f * 180.0f;
            s_pitch = (int16_t)((s_angle_raw[3] << 8) | s_angle_raw[2]) / 32768.0f * 180.0f;
            s_yaw   = (int16_t)((s_angle_raw[5] << 8) | s_angle_raw[4]) / 32768.0f * 180.0f;
        }
        HAL_UART_Receive_IT(&huart1, &s_rx_byte, 1);
    }
}

/**
 * @brief  获取最新角度值（主循环轮询用）
 */
void JY61p_GetAngle(float *Roll, float *Pitch, float *Yaw)
{
    *Roll  = s_roll;
    *Pitch = s_pitch;
    *Yaw   = s_yaw;
}

