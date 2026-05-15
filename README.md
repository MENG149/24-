# 2024 电赛改装 — STM32F1 自主循迹小车

基于 STM32F103ZE 的差速轮式自主循迹小车，参加 **2024 年全国大学生电子设计竞赛**。

## 硬件平台

| 模块 | 型号/说明 |
|------|-----------|
| 主控 | STM32F103ZE |
| IMU | JY61P（UART1，角度包 0x55 0x53） |
| 电机驱动 | MG310 直流减速电机 ×2，PWM + 方向控制 |
| 编码器 | 双路 AB 相编码器（TIM3/TIM4 编码器模式） |
| 循迹 | 7 路红外对管 (D1–D7)，一字排列 |
| 调试 | USART2 printf 输出 |
| IDE | Keil MDK-ARM |

## 任务模式

4 个按键对应 4 种任务：

| 按键 | 任务 | 描述 |
|------|------|------|
| Button1 | **Q1** | 直行（yaw=0°），遇线停车 |
| Button2 | **Q2** | 直行→循迹出弧→180°直行→循迹→停车 |
| Button3 | **Q3** | 转向 33°→直行→循迹→147°直行→循迹（**1 圈**） |
| Button4 | **Q4** | 同 Q3，跑 **4 圈** |

## 软件架构

```
main.c          # 主程序：任务状态机、路径规划、控制调度
pid.c/h         # 位置式 PID + 变速积分
motor.c/h       # 双电机 PWM 驱动、编码器读取
track.c/h       # 7 路红外循迹、差速转向
jy61p.c/h       # JY61P 角度解析（状态机解包）
button.c/h      # 按键消抖检测
led.c/h         # LED 指示
buzzer.c/h      # 蜂鸣器提示
```

### 核心控制

- **PID 变速积分**：误差大时关闭积分防止饱和，误差小时全速积分快速收敛
- **Yaw 角校正**：直行时根据 IMU 偏航角动态修正左右轮差速
- **状态机导航**：每个任务由多段状态（转向→直行→循迹）串联，自动切换
- **中断驱动**：TIM5 定时中断作为主控周期，读取编码器+IMU+执行控制

## 引脚定义

```
PWM1:  PA2    (TIM2_CH3)   左轮 PWM
PWM2:  PA3    (TIM2_CH4)   右轮 PWM
DIR1:  PA4/PA5              左轮方向
DIR2:  PA6/PA7              右轮方向
ENC1:  PA0/PA1 (TIM3)       左轮编码器
ENC2:  PA8/PA9 (TIM4)       右轮编码器
D1-D7: PB3-PB9               红外循迹
BTN1-4: PB12-PB15            任务按键
LED:   PD14
BUZZ:  PD15
UART1: PA9/PA10              JY61P
UART2: PA2/PA3               printf 调试
```

## 编译与烧录

1. 使用 Keil MDK-ARM 打开 `MDK-ARM/24diansai.uvprojx`
2. 编译（ARM Compiler 5/6）
3. 通过 J-Link / ST-Link 烧录到 STM32F103ZE
