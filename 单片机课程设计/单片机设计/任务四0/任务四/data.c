#include"data.h"
#include"ATC24C02.h"
#include "dalay.h"
#include "beep.h"
#include "judge.h"

int maxday;		   //maxday:每个月最大的天数
int h=8,min=0;
int h1=6,min1=30,h2=8,min2=30;	//闹钟的初始化时间
int year0=2019,month0=6,day0=8,maxday0=30; //暂且定义maxday0=30	 
int year=2019,month=7,day=9;  //前期初始化时间
int p=7003;	

int t[14];
int c[16];


void delay5ms(void)   //误差 0us
{
    unsigned char a,b;
    for(b=19;b>0;b--)
        for(a=130;a>0;a--);
}

void process()			  //时间换算函数
{
	if(sec>=60)
	{
		min++;
		sec=0;
	}
	if(min>=60)
	{
		h++;
		min=0;
	}
	if(min<0)
	{
		min=0;
	}
	if(h>=24)
	{
		p++;
		h=0;
		day++;
		day0++;
	}
	if(h<0)
	{
		h=0;
	}
	if(h1>=24||h1<0)
	{
		h1=0;
	}
	if(h2>=24||h2<0)
	{
		h2=0;
	}
	if(min1>=60)
	{
	   h1++;
	   min1=0;
	}
	if(min2>=60)
	{
	   h2++;
	   min2=0;
	}
	if(min1<0)
	{
	   min1=0;
	}
	if(min2<0)
	{
	   min2=0;
	}
	t[5]=sec%10;
	t[4]=sec/10;
	t[3]=min%10;
	t[2]=min/10;
	t[1]=h%10;
	t[0]=h/10;
		
}
void years()				  //日期换算函数
{
	
	if(day>maxday)
	{
		month++;
		day=1;
		
	}
	if(day<=0)
	{
		day=1;
	}
	if(day>31)
	{
		day=1;
	}
	if(month>=12)
	{
		year++;
		month=1;
		
	}
		if(month<=0)
	{
		month=1;
	}

	if(day0>maxday0)
	{
		month0++;
		day0=1;
		
	}
	if(day0<=0)
	{
		day0=1;
	}
	if(day0>31)
	{
		day0=1;
	}
	if(month0>=12)
	{
		year0++;
		month0=1;
		
	}
		if(month0<=0)
	{
		month0=1;
	}

	c[15]=day0%10;
	c[14]=day0/10;
	c[13]=month0%10;
	c[12]=month0/10;
	c[11]=year0%10;
	c[10]=year0%100/10;
	c[9]=year0%1000/100;
	c[8]=year0/1000;
	c[7]=day%10;
	c[6]=day/10;
	c[5]=month%10;
	c[4]=month/10;
	c[3]=year%10;
	c[2]=year%100/10;
	c[1]=year%1000/100;
	c[0]=year/1000;

	At24c02Write(1,sec);			//储存时间					  
	delay5ms();						//5ms
	At24c02Write(2,min);
	delay5ms();
	At24c02Write(3,h);
	delay5ms();
	At24c02Write(4,day);
	delay5ms();
	At24c02Write(5,month);
	delay5ms();
	At24c02Write(6,year%100);
	delay5ms();
	At24c02Write(7,year/100);
	delay5ms();
	At24c02Write(8,min1);								  
	delay5ms();						
	At24c02Write(9,h1);
	delay5ms();
	At24c02Write(10,day0);
	delay5ms();
	At24c02Write(11,month0);
	delay5ms();
	At24c02Write(12,year0%100);
	delay5ms();
	At24c02Write(13,year0/100);
	delay5ms();
}

void reading()			//读取时间函数
{
	sec=At24c02Read(1);
	delay5ms();
	min=At24c02Read(2);
	delay5ms();
	h=At24c02Read(3);
	delay5ms();
	day=At24c02Read(4);
	delay5ms();
	month=At24c02Read(5);
	delay5ms();
	year=At24c02Read(6)+At24c02Read(7)*100;
	delay5ms();
	min1=At24c02Read(8);
	delay5ms();
	h1=At24c02Read(9);
	delay5ms();
	day0=At24c02Read(10);
	delay5ms();
	month0=At24c02Read(11);
	delay5ms();
	year0=At24c02Read(12)+At24c02Read(13)*100;
	delay5ms();
}


void colock()	 //闹钟时间换算
{
	if(h1>=24)
	{
		h1=0;
	}
	if(min1>=60)
	{
		h1++;
		min1=0;
	}
	if(h2>=24)
	{
		h2=0;
	}
	if(min2>=60)
	{
		h2++;
		min2=0;
	}
	t[6]=h1/10;
	t[7]=h1%10;
	t[8]=min1/10;
	t[9]=min1%10;
	t[10]=h2/10;
	t[11]=h2%10;
	t[12]=min2/10;
    t[13]=min2%10;
	}
	
