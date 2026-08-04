#ifndef _UART_H_
#define	 _UART_H_

#include"reg52.h"

#define uchar unsigned char
#define uint unsigned int

void Uart_init()
{
	
	TMOD= 0X01;		//T1  方式2  ，8位   自动重装   
	TH0=0x4c;
	TL0=0x00;
	ET0=1;
	TR0=1;
	EA=1;		 // 开启中断	
}

#endif
			