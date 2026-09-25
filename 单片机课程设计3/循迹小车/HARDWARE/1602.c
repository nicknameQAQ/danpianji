#include<reg52.h>
#include "1602.h"
#include<intrins.h>

void LCD_Init()
{
	DelayMs(15); //延迟15ms，等待LCD电源稳定,使其进入工作状态
	LCD_IO = 0x00;
	LCD_Write_Command(LCD_DISPLAY_DOUBLE_LINE); 
	DelayMs(5); 
	LCD_Write_Command(LCD_DISPLAY_DOUBLE_LINE); 
	DelayMs(5); 
	LCD_Write_Command(LCD_DISPLAY_DOUBLE_LINE); //显示模式设置为两行显示，8位数据接口，5*8点阵
	DelayMs(5);
	LCD_Write_Command(LCD_AC_AUTO_INCREMENT | LCD_MOVE_DISENABLE); //数据读、写操作后,AC自动增一,画面不动
	DelayMs(5);
	LCD_Write_Command(LCD_DISPLAY_ON | LCD_CURSOR_OFF); //显示开，光标不显示
	DelayMs(5);
	LCD_Write_Command(LCD_CLEAR_SCREEN); //清除LCD显示内容 
}
/************延迟函数*******************************/
void DelayUs(uchar us)//delay us
{
	unsigned char uscnt;
	uscnt=us>>1; /* Crystal frequency in 12MHz*/
	while(--uscnt);
}
void DelayMs(uchar ms)//delay Ms
{
	while(--ms)
{
	DelayUs(250);
	DelayUs(250);
	DelayUs(250);
	DelayUs(250);
}
}
/************LCD1602写指令*******************************/
void LCD_Write_Command(uchar com)
{
	LCD_Check_Busy();
	LCD_RS = LOW;
	LCD_RW = LOW;
	_nop_(); //一个_nop_();是一个机器周期，是1us
	LCD_EN = HIGH;
	LCD_IO = com;
	LCD_EN = LOW;
}
/********************************************************/
/*****************LCD1602写数据**************************/
void LCD_Write_Data(uchar dat)
{
	LCD_Check_Busy();
	LCD_RS = HIGH;
	LCD_RW = LOW;
	_nop_();
	LCD_EN=HIGH;
	LCD_IO=dat;
	LCD_EN=LOW;
}
/********************************************************/
/**********************显示一个字节**************************/
void LCD_Char(uchar x, uchar line, uchar dat) //从第x开始写一个字节
{
	unsigned char address;
	if (line == LINE1) // line=0，为第一行
	address = LINE1_HEAD + x;
	else // 否则为第二行
	address = LINE2_HEAD + x;
	LCD_Write_Command(address); 
	LCD_Write_Data(dat);
}
/********************************************************/
/******************LCD1602显示字符串*********************/
void LCD_Str(uchar x,uchar line,uchar *Str) //从第line行的第x位置开始显示字符串
{
	uchar i = x;
	
	if (line == LINE1)
	{
	while( *Str != '\0')
		LCD_Char(i++, 0, *Str++);
	}
	else 
	{
	while( *Str != '\0')
	LCD_Char(i++, 1, *Str++);
	}
}
/***************************LCD忙碌状态*******************************/
void LCD_Check_Busy(void) //检测LCD状态,看它是不是还在忙呢
{
	do
	{
	LCD_EN=0;
	LCD_RS=0;
	LCD_RW=1;
	LCD_IO=0xff;
	LCD_EN=1;
	}
	while (LCD_BUSY==1);
	
	LCD_EN=0;
}
/*****************屏幕闪烁********************/
void LCD_Flash(uchar time)
{ 
	//控制停留时间
	LCD_Write_Command(LCD_DISPLAY_OFF); //关闭显示
	DelayMs(time); 
	//延时
	LCD_Write_Command(LCD_DISPLAY_ON); //开显示
	DelayMs(time);
	LCD_Write_Command(LCD_DISPLAY_OFF); //关闭显示
	DelayMs(time); 
	//延时
	LCD_Write_Command(LCD_DISPLAY_ON); //开显示
	DelayMs(time);
}
