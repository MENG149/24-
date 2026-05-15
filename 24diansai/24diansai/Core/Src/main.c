#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "pid.h"
#include <stdio.h>
#include "motor.h"
#include "jy61p.h"
#include "track.h"
#include "button.h"

int32_t current_encoder1 = 0;
int32_t current_encoder2 = 0;
int32_t location = 0;

PID_t Inner = {
    .Target = 15, .Kp = 2.0f, .Ki = 0.05f, .Kd = 0,
    .OutMax = 15, .OutMin = -15, .IntMax = 80, .IntMin = -80,
    .ErrHighThresh = 30, .ErrLowThresh = 5,
};
PID_t Inner1 = {
    .Target = 15, .Kp = 2.0f, .Ki = 0.05f, .Kd = 0,
    .OutMax = 15, .OutMin = -15, .IntMax = 80, .IntMin = -80,
    .ErrHighThresh = 30, .ErrLowThresh = 5,
};

typedef enum {
    TASK_NONE = 0,
    TASK_Q1,
    TASK_Q2,
    TASK_Q3,
    TASK_Q4,
} TaskMode_t;

typedef enum {
    ST_TURN_TO_33 = 0,
    ST_LINE_33,
    ST_TRACK,
    ST_LINE_147,
    ST_TRACK2,
    ST_DONE,
} State_t;

typedef enum {
    Q2_LINE_0 = 0,
    Q2_TRACK_1,
    Q2_LINE_180,
    Q2_TRACK_2,
} Q2State_t;

static volatile TaskMode_t s_task = TASK_NONE;
static State_t s_state = ST_DONE;
static Q2State_t s_q2_state = Q2_LINE_0;

static uint8_t s_no_line_cnt = 0;
static uint8_t s_ignore_line_cnt = 0;
static uint8_t s_line_cnt = 0;
static float s_yaw_corr = 0.0f;
static uint8_t s_lap = 0;
static uint8_t s_target_laps = 4;

static int32_t s_segment_ticks = 0;

#define NO_LINE_EXIT_CNT             4
#define LINE_IGNORE_CNT              6
#define LINE_CONFIRM_CNT             2
#define Q2_LINE180_MIN_TICKS         180
#define Q2_FINISH_NO_LINE_CNT        10

#define YAW33_MIN                    20.0f
#define YAW33_MAX                    60.0f
#define YAW_DEADBAND                 2.0f
#define YAW_CORR_KP                  0.35f
#define YAW_CORR_STEP                1.5f

void SystemClock_Config(void);

static int32_t abs_i32(int32_t x)
{
    return (x < 0) ? -x : x;
}

static float limit_float(float x, float min, float max)
{
    if (x > max) return max;
    if (x < min) return min;
    return x;
}

static float approach_float(float now, float target, float step)
{
    if (now + step < target) return now + step;
    if (now - step > target) return now - step;
    return target;
}

static void motor_stop(void)
{
    motor_start(0, 0);
}

static void reset_drive_pid(void)
{
    Inner.Error0 = 0.0f;
    Inner.Error1 = 0.0f;
    Inner.ErrorInt = 0.0f;
    Inner.Out = 0.0f;

    Inner1.Error0 = 0.0f;
    Inner1.Error1 = 0.0f;
    Inner1.ErrorInt = 0.0f;
    Inner1.Out = 0.0f;
}

static void reset_line_state(uint8_t ignore_line)
{
    s_no_line_cnt = 0;
    s_line_cnt = 0;
    s_ignore_line_cnt = ignore_line;
}

static uint8_t line_confirmed(void)
{
    if (Track_AnyLine()) {
        if (s_line_cnt < LINE_CONFIRM_CNT)
            s_line_cnt++;
    } else {
        s_line_cnt = 0;
    }

    return (s_line_cnt >= LINE_CONFIRM_CNT) ? 1 : 0;
}

static void update_no_line_counter(void)
{
    if (Track_AnyLine())
        s_no_line_cnt = 0;
    else if (s_no_line_cnt < 200)
        s_no_line_cnt++;
}

static void act_turn_open_loop(void)
{
    motor_start(-10, 10);
}

static void act_drive_yaw(float yaw, float target, float speed, float corr_max)
{
    float yaw_err = yaw - target;
    if (yaw_err >  180.0f) yaw_err -= 360.0f;
    if (yaw_err < -180.0f) yaw_err += 360.0f;

    float yaw_corr_target = 0;
    if (yaw_err > YAW_DEADBAND || yaw_err < -YAW_DEADBAND)
        yaw_corr_target = limit_float(yaw_err * YAW_CORR_KP, -corr_max, corr_max);

    s_yaw_corr = approach_float(s_yaw_corr, yaw_corr_target, YAW_CORR_STEP);

    Inner.Target  = speed;
    Inner1.Target = speed;
    Inner.Actual  = current_encoder1;
    Inner1.Actual = current_encoder2;
    PID_Update(&Inner);
    PID_Update(&Inner1);
    motor_start((int16_t)(Inner.Out + s_yaw_corr), (int16_t)(Inner1.Out - s_yaw_corr));
}

static uint8_t yaw_in_33(float yaw)
{
    return (yaw > YAW33_MIN && yaw < YAW33_MAX) ? 1 : 0;
}

static void finish_task(void)
{
    s_task = TASK_NONE;
    s_state = ST_DONE;
    motor_stop();
    reset_drive_pid();
    s_yaw_corr = 0.0f;
}

static void enter_line_33(void)
{
    s_state = ST_LINE_33;
    reset_line_state(LINE_IGNORE_CNT);
    s_yaw_corr = 0.0f;
    reset_drive_pid();
}

static void enter_line_147(void)
{
    s_state = ST_LINE_147;
    reset_line_state(LINE_IGNORE_CNT);
    s_yaw_corr = 0.0f;
    reset_drive_pid();
}

static void path34_step(float yaw)
{
    switch (s_state)
    {
    case ST_TURN_TO_33:
        if (yaw_in_33(yaw)) enter_line_33();
        break;

    case ST_LINE_33:
        if (s_ignore_line_cnt > 0) {
            s_ignore_line_cnt--;
            break;
        }
        if (line_confirmed()) {
            s_state = ST_TRACK;
            reset_line_state(0);
            break;
        }
        if (!yaw_in_33(yaw)) {
            s_state = ST_TURN_TO_33;
            s_line_cnt = 0;
        }
        break;

    case ST_TRACK:
        update_no_line_counter();
        if (s_no_line_cnt >= NO_LINE_EXIT_CNT &&
            (yaw > 130.0f || yaw < -150.0f))
        {
            enter_line_147();
        }
        break;

    case ST_LINE_147:
        if (s_ignore_line_cnt > 0) {
            s_ignore_line_cnt--;
            break;
        }
        if (line_confirmed()) {
            s_state = ST_TRACK2;
            reset_line_state(0);
        }
        break;

    case ST_TRACK2:
        update_no_line_counter();
        if (s_no_line_cnt >= NO_LINE_EXIT_CNT)
        {
            s_lap++;
            reset_line_state(0);
            if (s_lap >= s_target_laps)
                finish_task();
            else
                enter_line_33();
        }
        break;

    case ST_DONE:
        finish_task();
        break;
    }
}

static void path34_run(float yaw)
{
    switch (s_state)
    {
    case ST_TURN_TO_33:
        act_turn_open_loop();
        printf("Q%d TURN1 Yaw:%.2f\r\n", (s_task == TASK_Q4) ? 4 : 3, (double)yaw);
        break;
    case ST_LINE_33:
        act_drive_yaw(yaw, 32.0f, 15.0f, 8.0f);
        printf("Q%d STR1 Yaw:%.2f\r\n", (s_task == TASK_Q4) ? 4 : 3, (double)yaw);
        break;
    case ST_TRACK:
        Track_move();
        printf("Q%d TRACK Yaw:%.2f\r\n", (s_task == TASK_Q4) ? 4 : 3, (double)yaw);
        break;
    case ST_LINE_147:
        act_drive_yaw(yaw, 140.0f, 12.0f, 5.0f);
        printf("Q%d STR2 Yaw:%.2f\r\n", (s_task == TASK_Q4) ? 4 : 3, (double)yaw);
        break;
    case ST_TRACK2:
        Track_move();
        printf("Q%d TRACK2 Lap:%d Yaw:%.2f\r\n", (s_task == TASK_Q4) ? 4 : 3, (int)s_lap, (double)yaw);
        break;
    case ST_DONE:
        motor_stop();
        break;
    }
}

static void task1_step(float yaw)
{
    act_drive_yaw(yaw, 0.0f, 15.0f, 8.0f);
    printf("Q1 STR0 Yaw:%.2f\r\n", (double)yaw);

    if (s_ignore_line_cnt > 0) {
        s_ignore_line_cnt--;
        s_line_cnt = 0;
        return;
    }

    if (line_confirmed())
        finish_task();
}

static void enter_task2_state(Q2State_t next_state, uint8_t ignore_line)
{
    s_q2_state = next_state;
    reset_line_state(ignore_line);
    s_segment_ticks = 0;
    s_yaw_corr = 0.0f;
    reset_drive_pid();
}

static void task2_step(float yaw)
{
    switch (s_q2_state)
    {
    case Q2_LINE_0:
        act_drive_yaw(yaw, 0.0f, 15.0f, 8.0f);
        printf("Q2 LINE0 Yaw:%.2f\r\n", (double)yaw);

        if (s_ignore_line_cnt > 0) {
            s_ignore_line_cnt--;
            s_line_cnt = 0;
            break;
        }
        if (line_confirmed())
            enter_task2_state(Q2_TRACK_1, 0);
        break;

    case Q2_TRACK_1:
        Track_move();
        update_no_line_counter();
        printf("Q2 TRACK1 Yaw:%.2f\r\n", (double)yaw);
        if (s_no_line_cnt >= NO_LINE_EXIT_CNT &&
            (yaw > 130.0f || yaw < -150.0f))
        {
            enter_task2_state(Q2_LINE_180, LINE_IGNORE_CNT);
        }
        break;

    case Q2_LINE_180:
    {
        float target = (yaw >= 0.0f) ? 180.0f : -180.0f;
        act_drive_yaw(yaw, target, 15.0f, 8.0f);
        printf("Q2 LINE180 Yaw:%.2f\r\n", (double)yaw);

        if (s_ignore_line_cnt > 0) {
            s_ignore_line_cnt--;
            s_line_cnt = 0;
            break;
        }
        if (s_segment_ticks >= Q2_LINE180_MIN_TICKS && line_confirmed())
            enter_task2_state(Q2_TRACK_2, 0);
        break;
    }

    case Q2_TRACK_2:
        Track_move();
        update_no_line_counter();
        printf("Q2 TRACK2 Yaw:%.2f\r\n", (double)yaw);
        if (s_no_line_cnt >= Q2_FINISH_NO_LINE_CNT)
            finish_task();
        break;
    }
}

static void reset_task_common(void)
{
    s_state = ST_DONE;
    s_q2_state = Q2_LINE_0;
    reset_line_state(0);
    s_yaw_corr = 0.0f;
    s_lap = 0;
    s_target_laps = 4;
    s_segment_ticks = 0;
    location = 0;
    current_encoder1 = 0;
    current_encoder2 = 0;
    reset_drive_pid();
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    motor_stop();
}

static void start_task_unlocked(TaskMode_t task)
{
    reset_task_common();

    switch (task)
    {
    case TASK_Q1:
        reset_line_state(LINE_IGNORE_CNT);
        s_task = TASK_Q1;
        break;

    case TASK_Q2:
        s_q2_state = Q2_LINE_0;
        reset_line_state(LINE_IGNORE_CNT);
        s_task = TASK_Q2;
        break;

    case TASK_Q3:
        s_state = ST_TURN_TO_33;
        s_target_laps = 1;
        s_task = TASK_Q3;
        break;

    case TASK_Q4:
        s_state = ST_TURN_TO_33;
        s_target_laps = 4;
        s_task = TASK_Q4;
        break;

    default:
        s_task = TASK_NONE;
        break;
    }
}

static void start_task(TaskMode_t task)
{
    __disable_irq();
    start_task_unlocked(task);
    __enable_irq();
}

static void poll_start_buttons(void)
{
    if (s_task != TASK_NONE)
        return;

    if (Button1_Pressed()) {
        start_task(TASK_Q1);
    } else if (Button2_Pressed()) {
        start_task(TASK_Q2);
    } else if (Button3_Pressed()) {
        start_task(TASK_Q3);
    } else if (Button4_Pressed()) {
        start_task(TASK_Q4);
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM4_Init();
    MX_TIM5_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    HAL_TIM_Base_Start_IT(&htim5);
    motor_Init();
    JY61p_Init();
    Track_Init();
    Button_Init();
    motor_stop();

    while (1) {
        poll_start_buttons();
    }
}

int fputc(int ch, FILE *f)
{
    while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) == RESET);
    huart2.Instance->DR = (uint8_t)ch;
    return ch;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    static uint16_t counter = 0;
    if (htim->Instance != TIM5) return;

    if (++counter < 4) {
        __HAL_TIM_CLEAR_IT(&htim5, TIM_IT_UPDATE);
        return;
    }
    counter = 0;

    current_encoder1 =  (int16_t)(__HAL_TIM_GET_COUNTER(&htim3));
    current_encoder2 = -(int16_t)(__HAL_TIM_GET_COUNTER(&htim4));
    location += current_encoder1;
    s_segment_ticks += (abs_i32(current_encoder1) + abs_i32(current_encoder2)) / 2;

    float roll, pitch, yaw;
    JY61p_GetAngle(&roll, &pitch, &yaw);

    switch (s_task)
    {
    case TASK_Q1:
        task1_step(yaw);
        break;

    case TASK_Q2:
        task2_step(yaw);
        break;

    case TASK_Q3:
    case TASK_Q4:
        path34_step(yaw);
        if (s_task != TASK_NONE)
            path34_run(yaw);
        break;

    case TASK_NONE:
    default:
        motor_stop();
        break;
    }

    __HAL_TIM_SET_COUNTER(&htim3, 0);
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    __HAL_TIM_CLEAR_IT(&htim5, TIM_IT_UPDATE);
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

void Error_Handler(void)
{
    __disable_irq();
    while (1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { }
#endif
