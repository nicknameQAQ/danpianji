#ifndef _BSP_LCD1602_H
#define _BSP_LCD1602_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "bsp_SysTick.h"

#define LCD1602_CLK  RCC_APB2Periph_GPIOB  

#define LCD1602_GPIO_PORT   GPIOB

#define LCD1602_E    GPIO_Pin_10              //定义使能引脚
#define LCD1602_RW   GPIO_Pin_11             //定义读写引脚
#define LCD1602_RS   GPIO_Pin_12             //定义数据、命名引脚



#define EO(X)         X? (GPIO_SetBits(LCD1602_GPIO_PORT,LCD1602_E)):(GPIO_ResetBits(LCD1602_GPIO_PORT,LCD1602_E))
#define RWO(X)        X? (GPIO_SetBits(LCD1602_GPIO_PORT,LCD1602_RW)):(GPIO_ResetBits(LCD1602_GPIO_PORT,LCD1602_RW))
#define RSO(X)        X? (GPIO_SetBits(LCD1602_GPIO_PORT,LCD1602_RS)):(GPIO_ResetBits(LCD1602_GPIO_PORT,LCD1602_RS))

//只能是某个GPIO口的低八位
#define DB0					GPIO_Pin_0
#define DB1					GPIO_Pin_1
#define DB2					GPIO_Pin_2
#define DB3					GPIO_Pin_3
#define DB4					GPIO_Pin_4
#define DB5					GPIO_Pin_5
#define DB6					GPIO_Pin_6
#define DB7					GPIO_Pin_7

void LCD1602_Init(void);  //初始化LCD602；
void LCD1602_ShowStr(uint8_t x, uint8_t y, uint8_t *str,uint8_t len);
void LCD_ShowNum(uint8_t x, uint8_t y,uint8_t num);
void LCD_ShowChar(uint8_t x, uint8_t y,uint8_t dat);







#endif //_BSP_LCD1602_H
