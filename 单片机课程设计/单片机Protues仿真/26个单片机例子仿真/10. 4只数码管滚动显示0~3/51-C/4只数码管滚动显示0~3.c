/*
*4只数码管循环显示0~3*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

code uint8 LED_CODE[] = {0xC0,0xF9,0xA4,0xB0};

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
		for(i = 0; i < 4; i ++)
		{
			P3 = 0x01 << i;    
			P0 = LED_CODE[i];
			delay(250);
		}
	}
}