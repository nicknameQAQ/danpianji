#include<reg52.h>
#include"1602.h"
#include"pwm.h"
#include"xunji.h"

sbit POWER=P3^5;
sbit Key1=P3^3;
sbit P20=P2^0;


int coul;

const uchar tab[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,}; /*显示0～9*/

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

void InitTimer0(void)
{
    TMOD = 0x01;
    TH0 = 0x0D8;
    TL0 = 0x0F0;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
}


void main (void)
{ 
	IT0=1;
  IT1=1;
  EX0=1;
  EX1=1;
  EA=1;
  PX1=1;
	
	nums=0;
	type=0;
	Timer0Init();//初始化
	Timer1Init();
	InitTimer0();

 	while(1)
	{
		xunji(800);//寻迹
		
		keypros();//切换
		
		jishu();
		if(nums>9)
		{
		nums=0;
		}
		coul=nums%10;
		
		P0=tab[coul];
    P20=1;
    delay();  //扫描时间间隔
    P20=0;

		
		
		
	}
  	
}