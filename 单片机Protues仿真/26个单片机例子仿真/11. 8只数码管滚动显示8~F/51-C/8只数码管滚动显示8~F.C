/*
*8只数码管循环显示8~F*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

code uint8 LED_CODE[] = {0x80,0x90,0x88,0x83,0xA7,0xA1,0x86,0x8E};

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
			delay(250);
		}
	}
}