#ifndef __LCD_H_
#define __LCD_H_

#include<reg51.h>

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

typedef unsigned char uint8;
typedef unsigned int uint16;
sbit rs=P2^5;	 // 数据命令选择
sbit rw=P2^4;	 //读写选择
sbit e=P2^3;	  //使能

void Lcd1602_Delay1ms(uint16 i);

void LcdWriteCom(uchar com);
	
void LcdWriteData(uchar dat);
	
void LcdInit();	

void LCDWrite_String(uchar x, uchar y,uchar z, uchar *s);

void LCD_set_xy(uchar x, uchar y, uchar z);					  

#endif
