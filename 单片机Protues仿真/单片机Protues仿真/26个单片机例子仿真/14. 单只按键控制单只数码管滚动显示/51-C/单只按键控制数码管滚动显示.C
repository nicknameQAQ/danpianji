/*
*单只按键控制单只数码管滚动显示*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

#define Key()	(P1 & 0x01)

code uint8  LED_CODE[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
						  0x80,0x90,0x88,0x83,0xA7,0xA1,0x86,0x8E,0x00};

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
		 if(Key())
		 {
		 	while(Key());
			P0 = LED_CODE[i];
			i = (i + 1) % 17;
			delay(10);
		 }
	}
}