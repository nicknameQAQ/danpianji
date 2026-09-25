#include<reg52.h>
#include"1602.h"
#include"pwm.h"
#include"xunji.h"

sbit POWER=P3^5;
sbit Key1=P3^3;


const uchar tab[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,}; //段码

void delay()
	{
	uchar j;
 	for(j=250;j>0;j--);
 	}


void keypros()  //独立按键进行显示模式切换
{
	if(Key1==0)		  //检测按键K1是否按下
	{	
		delay();   //消除抖动 一般大约10ms
		if(Key1==0)	 //再次判断按键是否按下
		{
			type++;	  //led状态取反
		}
		while(!Key1);	 //检测按键是否松开
	}		
	if(type>1)
	{
	type=0;
	}
}

void jishu()
{
switch(byte)
{
  case 1: nums=nums+1; break;
	
  case 0:  break;
  break;
}

}






void main (void)
{ 
	nums=5;
	type=0;
	Timer0Init();//初始化
	Timer1Init();
	

 	while(1)
	{
		xunji(800);//寻迹
		
		keypros();//切换
		
		P2=0x00;//显示
    P2=nums;
		
		jishu();
		if(nums>9)
		{
		nums=0;
		}
		
	}
  	
}