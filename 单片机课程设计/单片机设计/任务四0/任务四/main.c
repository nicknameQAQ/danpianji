 #include "reg52.h"			 //此文件中定义了单片机的一些特殊功能寄存器
#include "LCD.h"				//12864
#include "dalay.h"				 //定时器
#include "key.h"				 //矩阵按键
#include "keypro.h"				 //独立按键
#include "data.h"				 //时间处理
#include "judge.h"				  //判断
#include "ATC24C02.h"
#include "cycle.h"				  //循环（星期、年份）
#include "beep.h"				   //蜂鸣器
#include "serial.h"				   //串口通信
#include "IR.h"					   //红外遥控



/*******************************************************************************
* 函 数 名         : main
* 函数功能		   : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void main(void)
{	
	UsartInit(); 	//串口初始化
	LCD12864_Init(); //12864初始化
	Timer0Init(); //初始化
	IrInit();		//红外线初始化
	reading();		//读取数据 

	while(1)
	{
		key();	//矩阵键盘
		keypros(); //独立按键
		process(); //数据处理
		time();		//	 判断不同的日期
		years();	// 	 数据处理
		week();   //判断星期几
 		lcd0();		//显示函数
		same();   //闹钟函数
		colock();		//时间进制函数
		weeks();		//限定星期几的函数
		infrared();		//红外遥控函数
		lunar();		 //年份函数
		send();			 //串口发送实时时间
	
	}

				
}
