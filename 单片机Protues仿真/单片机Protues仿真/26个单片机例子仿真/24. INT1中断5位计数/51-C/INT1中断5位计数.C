/*
*INT1中断5位计数*
*/
#include<reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

sbit K2 = P3^4;

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}


code uint8  LED_CODE[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF};

code uint8 Scan_BITs[]=	{0x20,0x10,0x08,0x04,0x02};

uint8 Buffer_Counts[]={0,0,0,0,0};

uint16 Count=0;

void Show_Counts()
{
 	uint8 i;
	Buffer_Counts[4] = Count / 10000;
	Buffer_Counts[3] = Count % 10000 / 1000;
	Buffer_Counts[2] = Count % 1000 / 100;
	Buffer_Counts[1] = Count % 100 /10;
	Buffer_Counts[0] = Count % 10;	
	if(Buffer_Counts[4] == 0)
	{
	 	Buffer_Counts[4] = 10;
		if(Buffer_Counts[3] == 0)
		{	
			Buffer_Counts[3] = 10;
			if(Buffer_Counts[2] == 0)
			{	
				Buffer_Counts[2] = 10;
				if(Buffer_Counts[1] == 0)  Buffer_Counts[1] = 10;
			}
		}		
	}

	for(i = 0;i < 5;i ++)
	{
		P0 = 0xFF;
	 	P1 = Scan_BITs[i];
		P0 = LED_CODE[Buffer_Counts[i]];
		delay(2);
	}
}

void main()
{
 	IP  = 0x04;
	IT1 = 1;
	IE  = 0x84;
	while(1)
	{
	 	if(K2 == 0) Count = 0;
		Show_Counts();
	}
}

void ISR1() interrupt 2
{
 	Count++;
}