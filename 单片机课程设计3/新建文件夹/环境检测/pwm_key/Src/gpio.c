/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "stdio.h"
#include <stdint.h>
#include "main.h"

float temperature = 0;//温度
float humidity = 0;//湿度

#define Delay_T 100
void Delay_us(uint32_t t)//微秒
{
	uint8_t i=0;
	while(t--)//执行3次，即3微秒
	{
		i=3;
		while(i--);
	}
}

void DHT11_PIN_OUT(void)//端口配置为输出
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = DHT11PIN;//设置成输出口
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;//推挽输出
	GPIO_InitStruct.Pull = GPIO_PULLUP;//上拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DHT11PORT, &GPIO_InitStruct);
}

void DHT11_PIN_IN(void)//端口配置为输入
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = DHT11PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;//上拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DHT11PORT, &GPIO_InitStruct);
}
void DHT11_Rst(void)//DHT11起始信号
{
	DHT11_PIN_OUT();  //设置PC.4为通用输出模式
	DAT_L();          //拉低DAT总线
	HAL_Delay(20);    //拉低至少18ms
	DAT_H();          //拉高DAT总线    
	Delay_us(30);     //主机拉高20~40us
}

uint8_t DHT11_Check(void)//等待DHT11回应，返回1 未检测到DHT11
{
	uint8_t retry = 0;
	DHT11_PIN_IN();                         //设置PC.4为通用输入模式
	while(!DAT_Read() && (retry<Delay_T) ) //DHT11会拉低40~80us
	{
		retry++;
		Delay_us(1);
		
	}
	if( retry >= 100 )  return 1;           //返回1，表示DHT11没有正常响应，或响应失败
	else retry = 0;
	while(DAT_Read() && (retry<Delay_T))    //DHT11拉低后会再次拉高40~80us
	{
		retry++;
		Delay_us(1);
	}
	if( retry >= Delay_T ) return 1;        //返回1，表示DHT11没有正常响应，或响应失败
	return 0;                               //返回0，表示DHT11正常响应
}

uint8_t DHT11_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();           //时钟打开
	DHT11_PIN_OUT();                        //设置为输出
	DAT_H();
	DHT11_Rst();                            //起始信号的检测
	return DHT11_Check();                   //应答是否正常：检测DHT11是否有响应（0正常，1不正常）
}

uint8_t DHT11_Read_Bit(void)
{
	uint8_t retry = 0;
	while( DAT_Read() && (retry<Delay_T) )  //等待变为低电平
 	{
		retry++;
		Delay_us(1);
	}

	retry = 0;
	while( !DAT_Read() && (retry<Delay_T) ) //等待变为高电平，同时测量高电平时间
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);                           //等待40us
	if( DAT_Read() )   
		return 1;           //读取该位为1，则返回1
	else               
		return 0;            //否则返回0
}

uint8_t DHT11_Read_Byte(void)
{
	uint8_t i = 0,data = 0;
	for(i = 0; i < 8; i++)
	{
		data <<= 1;
		data |= DHT11_Read_Bit();             //一位一位读取（左移）
	}
	return data;                            //返回读取到数据
}
uint8_t DHT11_Read_Data(float *temp,float *humi)
{
	uint8_t buf[6] = {0,0,0,0,0,'\0'};      //用来存储8bit湿度整数
	uint8_t i = 0;
	DHT11_Rst();
	if(DHT11_Check() == 0)                  //正常响应后
	{
		for(i = 0; i < 5; i++)
		{
			buf[i] = DHT11_Read_Byte();         //读取5个字节数据存储到
		}
		if( (buf[0]+buf[1]+buf[2]+buf[3]) == buf[4] )
		{
			*humi=buf[0] + buf[1] / 256.0;      //存储湿度值，
			*temp=buf[2] + buf[3] / 256.0;      //存储湿度值，
		}
	}
	else return 1;
	return 0;
}


void DHT11_Show_Data(void)
{
	if(DHT11_Read_Data(&temperature,&humidity) == 0)
	{
		printf("\r\n温度: %d\r\n",(uint8_t)temperature);
		printf("湿度: %d\r\n",(uint8_t)humidity);

	}
}
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
void LCD1602_WaitReady(void)//检测忙状态
{
	uint8_t sta;
	GPIOD->ODR=0X00FF;
	RSO(0);
	RWO(1);
	EO(1);
	Delay_us(1);
	do{
		sta=BUSY_Read();
		EO(0);
	}while(sta);
}

void LCD1602_WriteCmd(uint8_t cmd)//写指令
{
	LCD1602_WaitReady();
	RSO(0);
	RWO(0);
	EO(0);
	Delay_us(1);
	LCD1602_GPIO_PORT->ODR &=(cmd|0xFF00);
	EO(1);
	Delay_us(1);
	EO(0);
	Delay_us(400);
}

void LCD1602_WriteDat(uint8_t dat)//写数据
{
	LCD1602_WaitReady();
	RSO(1);
	RWO(0);
	Delay_us(1);
	LCD1602_GPIO_PORT->ODR &=(dat|0xFF00);
	EO(1);
	Delay_us(1);
	EO(0);
	Delay_us(400);
}

void LCD1602_SetCursor(uint8_t x,uint8_t y)
{
	uint8_t addr;
	if(y==0)
		addr=0x00+x;
	else
		addr=0x40+x;
	LCD1602_WriteCmd(addr|0x80);
	
}

void LCD1602_ShowStr(uint8_t x,uint8_t y,uint8_t *str,uint8_t len)
{
	LCD1602_SetCursor(x,y);
	while(len--)
	{
		LCD1602_WriteDat(*str++);
	}
}

void LCD1602_ShowChar(uint8_t x,uint8_t y,uint8_t str)
{
	LCD1602_SetCursor(x,y);
	LCD1602_WriteDat(str);
}

void LCD1602_Init(void)
{
	LCD1602_WriteCmd(0x38);//16*2显示,5*7点阵
	LCD1602_WriteCmd(0x0C);//显示器开，光标关闭
	LCD1602_WriteCmd(0x06);//文字不动，地址自动
	LCD1602_WriteCmd(0x01);//清屏
}


void LCD1602_Clear(void)
{
	LCD1602_WriteCmd(0x01);
}

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LCD1602_E_Pin|LCD1602_RW_Pin|LCD1602_RS_Pin|DB0_Pin 
                          |DB1_Pin|DB2_Pin|DB3_Pin|DB4_Pin 
                          |DB5_Pin|DB6_Pin|DB7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PAPin PAPin PAPin PAPin 
                           PAPin */
  GPIO_InitStruct.Pin = KEY0_Pin|KEY1_Pin|KEY2_Pin|KEY3_Pin 
                          |KEY4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PC4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PDPin PDPin PDPin PDPin 
                           PDPin PDPin PDPin PDPin 
                           PDPin PDPin PDPin */
  GPIO_InitStruct.Pin = LCD1602_E_Pin|LCD1602_RW_Pin|LCD1602_RS_Pin|DB0_Pin 
                          |DB1_Pin|DB2_Pin|DB3_Pin|DB4_Pin 
                          |DB5_Pin|DB6_Pin|DB7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
uint8_t keyscan(uint8_t kk)
{
	static uint8_t keyflag = 1;
	if(kk) keyflag = 1;
	if(keyflag && (KEY0==0||KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		HAL_Delay(5);
		keyflag = 0;
		if(KEY0==0)      return 1;
		else if(KEY1==0) return 2;
		else if(KEY2==0) return 3;
		else if(KEY3==0) return 4;
		else if(KEY4==0) return 5;
	}
	else if(KEY0==1&&KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)
		keyflag = 1;
	return 0;
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
