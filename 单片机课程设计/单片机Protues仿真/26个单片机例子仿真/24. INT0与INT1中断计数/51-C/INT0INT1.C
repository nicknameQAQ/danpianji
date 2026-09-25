/*
*INT0与INT1中断计数*
*/
#include<reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

sbit K3 = P3^4;
sbit K4 = P3^5;

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}


code uint8  LED_CODE[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF};

code uint8 Scan_BITs[]=	{0x20,0x10,0x08,0x04,0x02,0x01};

uint8 Buffer_Counts[]={0,0,0,0,0,0};

uint16 Count_A=0, Count_B=0;

void Show_Counts()
{
 	uint8 i;
	Buffer_Counts[2] = Count_A / 100;
	Buffer_Counts[1] = Count_A % 100 /10;
	Buffer_Counts[0] = Count_A % 10;	
	if(Buffer_Counts[2] == 0)
	{
	 	Buffer_Counts[2] = 10;
		if(Buffer_Counts[1] == 0)	Buffer_Counts[1] = 10;		
	}

	Buffer_Counts[5] = Count_B / 100;
	Buffer_Counts[4] = Count_B % 100 /10;
	Buffer_Counts[3] = Count_B % 10;	
	if(Buffer_Counts[5] == 0)
	{
	 	Buffer_Counts[5] = 10;
		if(Buffer_Counts[4]==0)	Buffer_Counts[4] = 10;
	}
	for(i = 0;i < 6;i ++)
	{
		P0 = 0xFF;
	 	P1 = Scan_BITs[i];
		P0 = LED_CODE[Buffer_Counts[i]];
		delay(2);
	}
}

void main()
{
 	IP  = 0x05;
	IT0 = 1;
	IT1 = 1;
	IE  = 0x85;
	while(1)
	{
	 	if(K3 == 0) Count_A = 0;
		if(K4 == 0) Count_B = 0;
		Show_Counts();
	}
}

void ISR0() interrupt 0
{
 	Count_A++;
}

void ISR1() interrupt 2
{
 	Count_B++;
}