/*
*8只数码管同时显示不同字符*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

code uint8 LED_CODE[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8};

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

void main()
{
	uint8 i;
	while(1)
	{
		for(i = 0; i < 8; i ++)
		{
			P0 = 0xFF;
			P3 = 0x01 << i;    
			P0 = LED_CODE[i];
			delay(4);
		}
	}
}