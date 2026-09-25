#include <stdio.h>
#include <intrins.h>
#include "PCA.h"
#include "STC12C5A60S2.h"
extern unsigned int  T1count;		//T1定时器，频率计数
extern unsigned int  PCA0loop;		//PCA0循环初值,25ms
extern unsigned int  PCA1loop;		//PCA1循环初值,500us(1KHz方波)
extern unsigned int  PCA0count;		//PCA0进入中断次数（计时累计）
extern unsigned int  PCA1count;		//PCA1循环递增值，输出的方波周期/2.5
extern unsigned long freqin;					//示波器输入的信号频率,单位Hz

void PCAinit(void)
{
	AUXR1&=0xbf;					//PCA在P1口
	//CMOD=0x08;					//PCA时钟为系统时钟（30MHz）不分频，计数溢出中断失能
	CMOD=0x00;						//PCA时钟为系统时钟（30MHz）12分频，计数溢出中断失能
	CCAPM0=0x49;					//PCA0用于软件计时模式，配合T1计数器测量频率
	//CCAPM1=0x4d;					//P1.4或P4.3输出高速脉冲
	CH=0;CL=0;						//PCA计数器清零
	CCAP0L=PCA0loop;
	CCAP0H=PCA0loop>>8;
	CCAP1L=PCA1loop;
	CCAP1H=PCA1loop>>8;
	CF=0;							//PCA溢出标志位清零
	CCF0=0;							//清捕获中断标志
	CCF1=0;							
	CR=1;							//启动PCA
}

void T1_interrupt(void) interrupt 3
{
	TH1=0x3c;						//T1测频率，计数50000进一次中断
	TL1=0xb0;
	T1count++;
}
void PCAinterrupt(void) interrupt 7
{				
	if(CCF0==1)						//软件计时，配合T1测频率
	{
		CCF0=0;						//清捕获中断标志
		PCA0loop+=50000;			//不分频时PCA0loop+=62500;
		CCAP0L=PCA0loop;
		CCAP0H=PCA0loop>>8;
		PCA0count++;
		if(PCA0count>=50)			//1s时间到  //不分频时if(PCA0count>=480)
		{
			ET1=0;TR1=0;
			freqin=T1count*50000+((((unsigned int)TH1<<8)+TL1)-0x3cb0);
			TH1=0x3c;				//T1测频率，计数50000进一次中断
			TL1=0xb0;
			T1count=0;
			PCA0count=0;
			ET1=1;TR1=1;
		}
	}	
/*	if(CCF1==1)						//方波输出,需同时修改PCA初始化令CCAPM1=0x4d;
	{
		CCF1=0;	
		PCA1loop=PCA1loop+PCA1count;
		CCAP1L=PCA1loop;
		CCAP1H=PCA1loop>>8;
	} */		
}
