#include "track.h"
#include "motor.h"
#include "led.h"
#include "buzzer.h"

void Track_Init(void)
{
    motor_Init();
    LED_Init();
    Buzzer_Init();
}

/* 任一传感器检测到线返回 1 */
uint8_t Track_AnyLine(void)
{
    return (D1 || D2 || D3 || D4 || D5 || D6 || D7) ? 1 : 0;
}

/* 弧线循迹：仅在弧段调用，不做模式切换
 * 关键约束：
 *   1. 中断内调用，严禁任何阻塞（无 while/delay）
 *   2. 本帧设差速 → 下一帧再判断
 *   3. 只前进，不反转（比赛规则）
 *   4. 单分支 if/else if，按"外→内"优先取最偏的传感器做误差
 */
void Track_move(void)
{
    uint8_t count = (uint8_t)(D1 + D2 + D3 + D4 + D5 + D6 + D7);

    /* 丢线：沿用惯性轻微前进，由状态机的 no_line_cnt 负责退弧 */
    if (count == 0) {
        motor_start(13, 13);
        return;
    }

    /* 过多传感器同时压线：弧-直衔接 / 交叉帧，直行穿过，避免被瞬态误差带歪 */
    if (count >= 4) {
        motor_start(15, 15);
        return;
    }

    int16_t l = 15, r = 15;

    /* 优先级：最外侧 > 中间 > 居中 */
    if      (D1) { l = 20; r = 4;  }   /* 最左压线 -> 大幅右转（前进差速） */
    else if (D7) { l = 4;  r = 20; }   /* 最右压线 -> 大幅左转 */
    else if (D2) { l = 18; r = 6;  }   /* 左      -> 中幅右转 */
    else if (D6) { l = 6;  r = 18; }   /* 右      -> 中幅左转 */
    else if (D3) { l = 16; r = 10; }   /* 左微    -> 小幅右转 */
    else if (D5) { l = 10; r = 16; }   /* 右微    -> 小幅左转 */
    else if (D4) { l = 15; r = 15; }   /* 居中    -> 直行 */

    motor_start(l, r);
}
