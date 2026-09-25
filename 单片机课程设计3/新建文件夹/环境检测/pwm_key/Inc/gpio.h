/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#define DHT11PORT GPIOC//将port引脚定义成端口C
#define DHT11PIN  GPIO_PIN_4//第4个引脚
	 
#define DAT_H()     HAL_GPIO_WritePin(DHT11PORT, DHT11PIN,GPIO_PIN_SET)//将GPIOC的第4个引脚置成高电平
#define DAT_L()     HAL_GPIO_WritePin(DHT11PORT, DHT11PIN,GPIO_PIN_RESET)//将GPIOC的第4个引脚置成低电平
#define DAT_Read()  (HAL_GPIO_ReadPin(DHT11PORT, DHT11PIN))//读电平高低状态
	 
#define KEY0 HAL_GPIO_ReadPin(KEY0_GPIO_Port,KEY0_Pin)
#define KEY1 HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)
#define KEY2 HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)
#define KEY3 HAL_GPIO_ReadPin(KEY3_GPIO_Port,KEY3_Pin)
#define KEY4 HAL_GPIO_ReadPin(KEY4_GPIO_Port,KEY4_Pin)
	 
extern float temperature;
extern float humidity;
	 
	 
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
	 
uint8_t DHT11_Init(void);
uint8_t DHT11_Read_Data(float *temp,float *humi);
void DHT11_Show_Data(void);
void LCD1602_WaitReady(void);
void LCD1602_WriteCmd(uint8_t cmd);
void LCD1602_WriteDat(uint8_t dat);
void LCD1602_SetCursor(uint8_t x,uint8_t y);
void LCD1602_ShowStr(uint8_t x,uint8_t y,uint8_t *str,uint8_t len);
void LCD1602_ShowChar(uint8_t x,uint8_t y,uint8_t str);
void LCD1602_Init(void);
void LCD1602_Clear(void);


void MX_GPIO_Init(void);
uint8_t keyscan(uint8_t kk);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
