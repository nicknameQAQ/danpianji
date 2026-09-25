#include<reg52.h>
#include<intrins.h>
#include<pwm.h>
#include<1602.h>

uchar count1,high_time1,count2,high_time2,nums,type,byte;
 
sbit Output1=P1^0; 
sbit Output2=P1^1;


void Pwm_set1(uint zkb)
{
   high_time1=zkb;
}

void Pwm_set2(uint zkb)
{
   high_time2=zkb;
}

void Timer0Init(void)		 
{
//	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	EA=1;//开总中断
    ET0=1;//开定时器0中断
}

void Timer1Init(void)		 
{
//	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TMOD |= 0x10;		//设置定时器模式
	TL1 = 0x66;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF0标志
	TR1 = 1;		//定时器0开始计时
	EA=1;//开总中断
    ET1=1;//开定时器1中断
}
void serve_T0() interrupt 1 using 1 
{ 	
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	if(++count1<=(high_time1)) 
	 	{	
	  	Output1=1;
	  	}
	else if(count1<=100) 
		{ 
		Output1=0;
		} 
	else   count1=0; 
} 

void serve_T1() interrupt 3   
{ 	
	TL1 = 0x66;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	if(++count2<=(high_time2)) 
	 	{	
	  	Output2=1;
	  	}
	else if(count2<=100) 
		{ 
		Output2=0;
		} 
	else   count2=0; 
} 



/*
void pwm_init(void)
{
	CCON=0;
	CL=0;
	CH=0;
	CMOD=0x02;
	CCAP0H=CCAP0L=0Xff;
	CCAPM0=0X42;
	CCAP1H=CCAP1L=0xff;
//	PCAPWM1=0X03;
	CCAPM1=0x42;	
	CR=1;
	}	  */
