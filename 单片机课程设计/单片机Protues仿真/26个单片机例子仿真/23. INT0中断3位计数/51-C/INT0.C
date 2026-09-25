 /*
*INT0ÖÐ¶Ï¼ÆÊý*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

sbit K2 = P3^5;

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

code uint8  LED_CODE[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0xFF};


uint8 Buffer_Counts[]={0,0,0};

uint16 Count=0;

void Show_Counts()
{
 	uint8 i;
	Buffer_Counts[2] = Count / 100;
	Buffer_Counts[1] = Count % 100 /10;
	Buffer_Counts[0] = Count % 10;	
	if(Buffer_Counts[2] == 0)
	{
	 	Buffer_Counts[2] = 10;
		if(Buffer_Counts[1] == 0)	Buffer_Counts[1] = 10;		
	}
	for(i = 0;i < 3;i ++)
	{
		P0 = 0xFF;
	 	P2 = 0x80 >> i;
		P0 = LED_CODE[Buffer_Counts[i]];
		delay(2);
	}
}

void main()
{
 	IP  = 0x01;
	IT0 = 1;
	IE  = 0x81;
	while(1)
	{
		if(K2 == 0) Count = 0;
		Show_Counts();
	}
}

void ISR0() interrupt 0
{
 	Count++;
}