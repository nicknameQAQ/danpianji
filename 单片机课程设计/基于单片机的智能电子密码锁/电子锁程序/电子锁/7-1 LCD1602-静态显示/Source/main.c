/*******************************************************


程序功能：LCD1602液晶静态显示

开发环境：Keil3

硬件环境：CEPARK多功能开发学习板/实验箱（2013版），STC12C5A60S2，11.092M晶振
																							   
接线说明：单片机P1口与底板JP33排针相连，分别为：P10-D0, P11-D1，P12-D2，P13-D3，P14-D4，P15-D5，P16-D6，P17-D7；
          单片机P35~P37口与底板JP31相连，分别为：P35-RS,P36-RW,P37-EN;	

跳线说明：无

实验现象：在液晶上显示两行字符
		  第一行显示“1A2B3C4D5E6F7G8H”
		  第二行显示“ bbs.cepark.com ”
          
									 
论坛支持：http://bbs.cepark.com

淘宝店铺：http://cepark.taobao.com

作者：cepark 

时间：2013-01-12

*******************************************************/

#include "STC12C5A60S2.h"

sbit LCD1602_RS = P2^2;	//位定义，液晶的数据/命令选择
sbit LCD1602_RW = P2^3;	//位定义，液晶的读写选择
sbit LCD1602_EN = P3^7;	//位定义，液晶使能信号

#define LCDPORT P0	  //液晶的数据口

unsigned char code ucForum[]=" bbs.cepark.com ";	  //在CODE区定义一个用于显示的常量字符串
unsigned char code zifuchuan[]=" abcdefg ";	  //在CODE区定义一个用于显示的常量字符串
void Delay(unsigned int uiCount);	//延时函数
void LCD1602_CheckBusy(void);	//液晶忙检测
void LCD1602_WriteInformation(unsigned char ucData,bit bComOrData);	//在液晶上写数据或者写命令，0为命令，1为数据
void LCD1602_Init(void);	//液晶初始化
void LCD1602_MoveToPosition(unsigned char x,unsigned char y);	//液晶的坐标移动到指定位置
void LCD1602_DisplayOneCharOnAddr(unsigned char x,unsigned char y,unsigned char ucData);	//在液晶指定位置显示字符
void LCD1602_DisplayString(unsigned char *ucStr);	//在液晶上显示字符串

/******************************************************************************
函数名称：main
函数功能：程序主函数
入口参数：无
返回值：无
备注：无
*******************************************************************************/
void main(void)
{
	LCD1602_Init();	  //液晶初始化
	
	while(1)		 //程序循环
	{
//		LCD1602_DisplayOneCharOnAddr(0,0,'1');	//在液晶的第1个位置显示1
//		LCD1602_DisplayOneCharOnAddr(0,1,'A');	//在液晶的第2个位置显示A
//		LCD1602_DisplayOneCharOnAddr(0,2,'2');	//在液晶的第3个位置显示2
//		LCD1602_DisplayOneCharOnAddr(0,3,'B');	//在液晶的第4个位置显示B
//		LCD1602_DisplayOneCharOnAddr(0,4,'3');	//在液晶的第5个位置显示3
//		LCD1602_DisplayOneCharOnAddr(0,5,'C');	//在液晶的第6个位置显示C
//		LCD1602_DisplayOneCharOnAddr(0,6,'4');	//在液晶的第7个位置显示4
//		LCD1602_DisplayOneCharOnAddr(0,7,'D');	//在液晶的第8个位置显示D
//		LCD1602_DisplayOneCharOnAddr(0,8,'5');	//在液晶的第9个位置显示5
//		LCD1602_DisplayOneCharOnAddr(0,9,'E');	//在液晶的第10个位置显示E
//		LCD1602_DisplayOneCharOnAddr(0,10,'6');	 //在液晶的第11个位置显示6
//		LCD1602_DisplayOneCharOnAddr(0,11,'F');	 //在液晶的第12个位置显示F
//		LCD1602_DisplayOneCharOnAddr(0,12,'7');	 //在液晶的第13个位置显示7
//		LCD1602_DisplayOneCharOnAddr(0,13,'G');	 //在液晶的第14个位置显示G
		LCD1602_DisplayOneCharOnAddr(0,14,'8');	 //在液晶的第15个位置显示8
		LCD1602_DisplayOneCharOnAddr(0,15,'H');	 //在液晶的第16个位置显示H
		
  		LCD1602_MoveToPosition(0,0);	//显示位置移动到指定位置
		LCD1602_DisplayString(zifuchuan);	//显示的内容
		LCD1602_MoveToPosition(1,0);	//显示位置移动到指定位置
		LCD1602_DisplayString(ucForum);	//显示的内容
		while(1);
	}	
}

/******************************************************************************
函数名称：LCD1602_CheckBusy
函数功能：忙检测
入口参数：无
返回值：无
备注：忙检测采用了多加一个延时条件的办法，放置程序卡在检测函数中
*******************************************************************************/
void LCD1602_CheckBusy(void)		
{
	unsigned char i = 255;
	LCDPORT = 0xFF;  //读之前先置位，准备读取IO口数据
	LCD1602_RS = 0;
	LCD1602_RW = 1;	 //使液晶处于读数据状态
	LCD1602_EN = 1;	 //使能液晶，高电平有效
	while((i--) && (LCDPORT & 0x80));	//忙检测
	LCD1602_EN = 0;
}

/******************************************************************************
函数名称：LCD1602_WriteInformation
函数功能：向LCD1602液晶写入数据或者命令
入口参数：ucData-要写入的数据或者命令参数
		  bComOrData-为0时写入的时命令，为1时写入的时数据
返回值：无
备注：无
*******************************************************************************/
void LCD1602_WriteInformation(unsigned char ucData,bit bComOrData)	 
{
	LCD1602_CheckBusy();	 //在写入数据或者命令前先进行忙检测
	LCDPORT = ucData;		 //先将数据或者命令送至IO
	LCD1602_RS = bComOrData;	//确定是写入数据还是写命令
	LCD1602_RW = 0;		//使液晶处于写入信息状态
	LCD1602_EN = 1;		//使能液晶，高电平有效
	LCD1602_EN = 0;
}

/******************************************************************************
函数名称：LCD1602_Init
函数功能：液晶初始化函数
入口参数：无
返回值：无
备注：无
*******************************************************************************/
void LCD1602_Init(void)		 
{
	LCD1602_WriteInformation(0x38,0);
	Delay(300);
	LCD1602_WriteInformation(0x38,0);
	Delay(100);
	LCD1602_WriteInformation(0x38,0);
	Delay(100);
	LCD1602_WriteInformation(0x38,0); 	//写入命令，5x7点阵工作方式，8位数据接口
	Delay(100);
	LCD1602_WriteInformation(0x0c,0);	//显示设置，开显示，光标不显示，不闪烁
	Delay(20);
	LCD1602_WriteInformation(0x01,0);	//清屏指令
	Delay(20);
}

/******************************************************************************
函数名称：LCD1602_MoveToPosition
函数功能：将液晶的光标移动到指定的位置
入口参数：x-液晶显示的行数，范围0-1
			x = 0:在液晶的第一行
			x = 1:在液晶的第二行
		  y-液晶显示的列数，范围0-15
		    y = 0:在液晶的第一列
			y = 1:在液晶的第二列
			......
			y = 15:在液晶的第十六列
返回值：无
备注：通过指定x,y的值可以将液晶的光标移动到指定的位置
*******************************************************************************/
void LCD1602_MoveToPosition(unsigned char x,unsigned char y)	
{
	if(0 == x)
		LCD1602_WriteInformation((0x80 | y),0);	   //光标定位到第一行的y列
	if(1 == x)
		LCD1602_WriteInformation((0xC0 | y),0);	   //光标定义到第二行的y列
}

/******************************************************************************
函数名称：LCD1602_DisplayOneCharOnAddr
函数功能：在指定的位置上显示指定的字符
入口参数：x-液晶显示的行数，范围0-1
			x = 0:在液晶的第一行
			x = 1:在液晶的第二行
		  y-液晶显示的列数，范围0-15
		    y = 0:在液晶的第一列
			y = 1:在液晶的第二列
			......
			y = 15:在液晶的第十六列
		  ucData-要显示的字符数据
返回值：无
备注：确保x,y的取值要在指定的范围内
*******************************************************************************/
void LCD1602_DisplayOneCharOnAddr(unsigned char x,unsigned char y,unsigned char ucData)
{
	LCD1602_MoveToPosition(x,y);   //光标位置
	LCD1602_WriteInformation(ucData,1);	  //写入数据
}

/******************************************************************************
函数名称：LCD1602_DisplayString
函数功能：显示字符串
入口参数：ucStr-字符串的首地址
返回值：无
备注：无
*******************************************************************************/
void LCD1602_DisplayString(unsigned char *ucStr)	
{
	while(*ucStr != '\0')	   //字符串结束之前，循环显示
	{
		 LCD1602_WriteInformation(*ucStr,1);	 //依次写入每一个字符
		 ucStr++;								 //指针增加
	}
}

/******************************************************************************
函数名称：Delay
函数功能：延时函数
入口参数：uiCount-延时参数，每加1增加0.5ms
返回值：无
备注：无
*******************************************************************************/
void Delay(unsigned int uiCount)
{
	unsigned char j = 244;
	for(;uiCount > 0;uiCount--) while(--j);	
}
