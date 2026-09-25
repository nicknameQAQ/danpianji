#include <reg52.h>
#include "key.h"
#include "delay.h"

#define uint unsigned int
#define uchar unsigned char
	
sbit key1=P3^0;
uint code shu[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};  		//共阳极段码  				
uint code wei[]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80}; //位选             
								
uchar mm,ss,pp;

void Timer0Init(void)		  //定时器初始化
{	
	TMOD=0x01;
	TH0=(65536-18000)/256;
	TL0=(65536-18000)%256;					
	TR0=1;
  ET0=1;
  EA=1;	
}
void main()
{
	uchar tt,i,j,aa=1;
	uchar num[30];
	P0=0x00;
	P2=0x00;
	Timer0Init();
	
	while(1)
	{
		tt = keynum();	
		if(tt!=num[i]&&tt!=0)
		     num[++i] = tt;
		if(key1==0)
		{
		  DelayMS(1);
      if(key1==0)
      {
			   while(!key1);
				 aa++;
				 ss=0;
				 mm=0;
			}				
	  }
		if(num[aa]!=0)
		{
      TR0=1;
			P2 = wei[7];
			P0 = shu[num[aa]/10];
			DelayMS(1);
			P2 = wei[5];
			P0 = shu[num[aa]%10];
			DelayMS(1);
		  P2 = wei[2];
			P0 = shu[ss/10];
			DelayMS(1);
			P2 = wei[3];
			P0 = shu[ss%10];
			DelayMS(1);
			P2 = wei[0];
			P0 = shu[mm/10];
			DelayMS(1);
			P2 = wei[1];
			P0 = shu[mm%10];
			if(ss==60)
			{
				ss=0; 
				mm++;
			}
		}
		else
		{
		  P2=0x00;
			ss=0;
			aa=1;
			for(j=0;j<i+1;j++)
			  num[j]=0;
			i=0;
			TR0=0;
		}
	}
}
void T0_time() interrupt 1
{
  TR0=0;
	TH0=(65536-18000)/256;
	TL0=(65536-18000)%256;
	pp++;
	if(pp==40) {ss++;pp=0;}
	TR0=1;
}