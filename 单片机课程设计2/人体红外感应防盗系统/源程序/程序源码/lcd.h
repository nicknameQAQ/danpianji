
#ifndef __LCD_H_
#define __LCD_H_
/**********************************
当使用的是4位数据传输的时候定义，
使用8位取消这个定义
**********************************/


/**********************************
包含头文件
**********************************/
#include<STC15.h>
#include	"config.h"
#define LCD1602_4PINS

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif


//#define MAIN_Fosc		  22118400L	//定义主时钟
//#define MAIN_Fosc		12000000L	//定义主时钟
//#define MAIN_Fosc		11059200L	//定义主时钟
//#define MAIN_Fosc		 5529600L	//定义主时钟
//#define MAIN_Fosc		24000000L	//定义主时钟
/**********************************
PIN口定义
**********************************/
#define LCD1602_DATAPINS P1
sbit LCD1602_E=P3^7;
sbit LCD1602_RW=P3^5;
sbit LCD1602_RS=P3^4;

/**********************************
函数声明
**********************************/
/*在51单片机12MHZ时钟下的延时函数*/
void Lcd1602_Delay1ms(uint c);   //误差 0us
/*LCD1602写入8位命令子函数*/
void LcdWriteCom(uchar com);
/*LCD1602写入8位数据子函数*/	
void LcdWriteData(uchar dat);
/*LCD1602初始化子程序*/		
void LcdInit();						  

#endif
