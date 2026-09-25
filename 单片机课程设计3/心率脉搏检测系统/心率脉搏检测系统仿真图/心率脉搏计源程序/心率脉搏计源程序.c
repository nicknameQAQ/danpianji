#include <reg52.h>
#include "lcd1602.h"

#define uchar unsigned char 
#define uint  unsigned int

unsigned char num=0,timecount=0,displayOK=0,rate=0;
unsigned int time[6]={0};

/***********************1ms延时函数*****************************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<110;j++);
}


void main()
{	
        init_1602();
        TCON=0x01;//设置外部中断0
		EX0=1;
		TMOD=0x01;//定时器0初始化
		TH0=(65536-50650)/256;//实测每50ms中断的定时值
		TL0=(65536-50650)%256;
		ET0=1;//开定时器中断
		//显示基本文字
		write_string(1,0,"   Heart Rate   ");
		write_string(2,0,"        /min    ");
		TR0=0;//定时器停止
		EA=1;//开总中断                    
	   while(1)
	   {
		
		   if(displayOK==1)
		   {
			rate=60000/(time[1]/5+time[2]/5+time[3]/5+time[4]/5+time[5]/5);
			write_sfm3_18B20(2,5,rate);

	      	}	
			else
			{
			   	  write_string(2,5,"   ");
			}
		   delay_1ms(300);
		   						
	
	}
}

void ex0() interrupt 0
{
		EX0=0;//暂时关外部中断
		if(timecount<8)   //当连续两次检测时间间隔小于8*50ms=400ms不处理
		{
				TR0=1;//开定时器
		}
		else
		{
				time[num]=timecount*50+TH0*0.256+TL0/1000;//算出间隔时间
				TL0=(65536-50650)%256;//重新设置定时器
				TH0=(65536-50650)/256;
				timecount=0;//50ms计数清零
				num++;
				if(num==6)//记录到超过等于6次时间
				{
						num=1;//计数从1开始
						displayOK=1;    //测得5次开始显示
				}	
				
		}
		EX0=1;
}
void et0() interrupt 1
{
		TL0=(65536-50650)%256;
		TH0=(65536-50650)/256;
			
		timecount++;//每50ms一次计数
		if(timecount>25)     //当超过25*50ms=1.5s没有检测到信号停止显示
		{
				num=0;//数据个数清零
				timecount=0;//50ms计数清零
				displayOK=0;//显示关
				TR0=0;//定时器关
				TH0=(65536-50650)/256;
				TL0=(65536-50650)%256;
		}
}