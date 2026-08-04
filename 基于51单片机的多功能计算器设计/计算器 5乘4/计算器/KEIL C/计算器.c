/*    最终成果版
5*8键盘输入，lcd1602显示
功能：运算均为单精度浮点数
		共 19 钟运算功能 
		加，减，乘，除，平方，开方，N次方，
		开N次方，正弦函数，余弦函数，正切函数，
		反正弦，反余弦，反正切，对数运算，
		阶乘（N<=34），排列，组合，累加！
功能键有，复位键，回删键，确定键，第二功能键，背光灯按键。
								
*/

#include"intrins.h"
#include"AT89X52.h"
#include"stdio.h"
#include"stdlib.h"
#include"math.h"
#include"yanshi.h"
#include"lcd1602.h"
#include"JP.h"

#define uchar unsigned char
#define uint unsigned int

uchar code  welcome[]={"     welcome    "};	//显示缓存
uchar code welcome2[]={"   calculator   "};

void init()//显示和系统初始化函数
{
	uchar i;
	EA=1;				//开总中断
	ET1=1;				//T1开
	TMOD=0X10;			//设置T1工作方式1
	TH1=0X3C;TL1=0XB0;	//设置计数初置（晶振为12M，计50000个脉冲，50ms）

	P2_3=1;				//背光灯控制引脚，默认开背光
	P3_6=0;				//第二功能键指示灯
	lcd_init();			//液晶显示初始化函数
	write_1602com(0x01);  //显示“welcom calaulator ”
	write_1602com(yh);
	for(i=0;i<16;i++)
	{
		write_1602dat(welcome[i]);
		delay(100);
	}
	write_1602com(er);
	for(i=0;i<16;i++)
	{
		write_1602dat(welcome2[i]);
		delay(100);
	}
	write_1602com(0x01);	//清屏
	write_1602com(0x80);	//显示指针初始化
	TR1=1;					//T0开始工作
}

void main()
{
	init();			//调用初始化函数
	while(1)		//等待中断
	{
	  PCON=0X01;	//设置为待机模式
	}
	
}

t1() interrupt 3
{
	TH1=0X3C;TL1=0XB0;	//计数初值重新载入
	keypad();			//键盘检测
}
