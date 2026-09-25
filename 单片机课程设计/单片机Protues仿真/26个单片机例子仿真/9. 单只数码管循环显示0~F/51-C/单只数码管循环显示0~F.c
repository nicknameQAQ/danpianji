/*
*单只数码管循环显示0~F*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

code uint8 LED_CODE[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
						 0x80,0x90,0x88,0x83,0xA7,0xA1,0x86,0x8E};

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

void main()
{
	uint8 i = 0;
	while(1)
	{
		for(i = 0; i < 16; i ++)   //i = (i + 1) % 16
		{	
			P0 = LED_CODE[i];
			delay(500);
		}
	}
}