#include "keypro.h"
#include"data.h"
#include "beep.h"
#include "delay.h"
#include "LCD.H"


void keypros()
{
	if(k1==0)		  //检测按键K1是否按下
	{
		delay(1000);   //消除抖动 一般大约10ms
		if(k1==0)
			{
				y++;  
			}
		while(!k1);	 //检测按键是否松开
	}

	if(k2==0)		  //检测按键K2是否按下
	{
		delay(1000);   //消除抖动 一般大约10ms
		if(k2==0)
			{
			moment=0;  
			}
		while(!k2);	 //检测按键是否松开
	}

	if(k4==0)		  //检测按键K4是否按下
	{
		delay(1000);   //消除抖动 一般大约10ms
		if(k4==0)
		{
		moment=1;		 
		}
		while(!k4);	 //检测按键是否松开	
	}
}
	
	