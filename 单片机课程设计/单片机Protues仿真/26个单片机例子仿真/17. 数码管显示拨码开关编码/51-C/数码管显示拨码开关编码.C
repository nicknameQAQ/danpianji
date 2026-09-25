/*
*数码管显示4X4键盘矩阵按键*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

code uint8 LED_CODE[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

uint8 DIS_BUF[3] = {0,0,0};

void main()
{
	uint8 i,Num;
	while(1)
	{
		Num = P1;
		DIS_BUF[0] = Num / 100;
		DIS_BUF[1] = Num / 10 % 10;
		DIS_BUF[2] = Num % 10;
		for(i = 0; i < 3; i ++)
		{
			P3 = 0x01 << i;
			P0 = LED_CODE[DIS_BUF[i]];
			delay(10);
		}
	}
}