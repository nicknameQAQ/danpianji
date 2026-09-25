#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char
sbit s1=P3^7;
sbit s2=P3^6;
sbit s3=P3^5;
uchar ID;
uchar key;//记录按键值

void delayms(uint x)//延时程序
{
	uint i;
	while(x--)
	for(i=0;i<12;i++)
	;
}

void Key_Init()
{
  	EA=1;
	TMOD=0x01;//开定时器T0模式1
	ET0=1;//定时器T0中断允许
	TR0=1;//打开定时器T0
	TH0=(65536-500)/256;
	TL0=(65536-500)%256;
	key=0;
	ID=0;
 	while(1);

}
	
void time0()interrupt 1	using 1
{
	TR0=0;//关闭定时器
	if((P3&0xf0)!=0xf0)//检测有按键按下
	{
	  	delayms(100);//延时消抖
		if((P3&0xf0)!=0xf0)
		{
		 	if(s1==0)
			key=3;
			if(s2==0)
			key=1;
			if(s3==0)
			key=2;	
		}	   	
	}
	TH0=(65536-500)/256;
	TL0=(65536-500)%256;
	TR0=1;
}