/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f4xx_hal.h"

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
#define KEY0_Pin GPIO_PIN_0
#define KEY0_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_1
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_3
#define KEY3_GPIO_Port GPIOA
#define KEY4_Pin GPIO_PIN_4
#define KEY4_GPIO_Port GPIOA

#define LCD1602_E_Pin GPIO_PIN_10
#define LCD1602_E_GPIO_Port GPIOD
#define LCD1602_RW_Pin GPIO_PIN_11
#define LCD1602_RW_GPIO_Port GPIOD
#define LCD1602_RS_Pin GPIO_PIN_12
#define LCD1602_RS_GPIO_Port GPIOD
#define DB0_Pin GPIO_PIN_0
#define DB0_GPIO_Port GPIOD
#define DB1_Pin GPIO_PIN_1
#define DB1_GPIO_Port GPIOD
#define DB2_Pin GPIO_PIN_2
#define DB2_GPIO_Port GPIOD
#define DB3_Pin GPIO_PIN_3
#define DB3_GPIO_Port GPIOD
#define DB4_Pin GPIO_PIN_4
#define DB4_GPIO_Port GPIOD
#define DB5_Pin GPIO_PIN_5
#define DB5_GPIO_Port GPIOD
#define DB6_Pin GPIO_PIN_6
#define DB6_GPIO_Port GPIOD
#define DB7_Pin GPIO_PIN_7
#define DB7_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */
#define LED4_Pin GPIO_PIN_7
#define LED4_GPIO_Port GPIOA
#define LCD1602_GPIO_PORT GPIOD//将port引脚定义成端口D
	 
#define EO(X)   X?  (HAL_GPIO_WritePin(LCD1602_GPIO_PORT,LCD1602_E_Pin,GPIO_PIN_SET)):(HAL_GPIO_WritePin(LCD1602_GPIO_PORT,LCD1602_E_Pin,GPIO_PIN_RESET))//将GPIOC的第4个引脚置成高电平
#define RWO(X)  X?  (HAL_GPIO_WritePin(LCD1602_GPIO_PORT,LCD1602_RW_Pin,GPIO_PIN_SET)):(HAL_GPIO_WritePin(LCD1602_GPIO_PORT,LCD1602_RW_Pin,GPIO_PIN_RESET))//
#define RSO(X)  X?  (HAL_GPIO_WritePin(LCD1602_GPIO_PORT,LCD1602_RS_Pin,GPIO_PIN_SET)):(HAL_GPIO_WritePin(LCD1602_GPIO_PORT,LCD1602_RS_Pin,GPIO_PIN_RESET))//

#define BUSY_Read()   (HAL_GPIO_ReadPin(DB7_GPIO_Port, DB7_Pin))
void LCD1602_Init(void);//初始化LCD1602
void LCD1602_ShowStr(uint8_t x,uint8_t y,uint8_t *str,uint8_t len);
void LCD1602_Clear(void);
void LCD1602_ShowChar(uint8_t x,uint8_t y,uint8_t str);
void key_with(void);
void write_guanbiao(uint8_t x,uint8_t y,uint8_t date);
void MX_GPIO_Init_FW(void);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
