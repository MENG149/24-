/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define pwm1_Pin GPIO_PIN_2
#define pwm1_GPIO_Port GPIOA
#define pwm2_Pin GPIO_PIN_3
#define pwm2_GPIO_Port GPIOA
#define mg310zuo_Pin GPIO_PIN_4
#define mg310zuo_GPIO_Port GPIOA
#define button1_Pin GPIO_PIN_12
#define button1_GPIO_Port GPIOB
#define button2_Pin GPIO_PIN_13
#define button2_GPIO_Port GPIOB
#define button3_Pin GPIO_PIN_14
#define button3_GPIO_Port GPIOB
#define button4_Pin GPIO_PIN_15
#define button4_GPIO_Port GPIOB
#define led_Pin GPIO_PIN_14
#define led_GPIO_Port GPIOD
#define buzzer_Pin GPIO_PIN_15
#define buzzer_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_3
#define D1_GPIO_Port GPIOB
#define D2_Pin GPIO_PIN_4
#define D2_GPIO_Port GPIOB
#define D3_Pin GPIO_PIN_5
#define D3_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_6
#define D4_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_7
#define D5_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_8
#define D6_GPIO_Port GPIOB
#define D7_Pin GPIO_PIN_9
#define D7_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
