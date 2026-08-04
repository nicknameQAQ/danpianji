/*
*°´¼ü·¢Òô*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

sbit K1 = P1^0;
sbit K2 = P1^1;
sbit K3 = P1^2;
sbit K4 = P1^3;
sbit BUZZER = P3^0;
			 
void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

void Play(uint8 t)
{
	uint8 i;
	for(i = 0; i < 100; i ++)
	{
		BUZZER = ~BUZZER;delay(t);
	}
}

void main()
{
	P1 = 0xFF;
	while(1)
	{
		if(K1==0) Play(1);
		if(K2==0) Play(2);
		if(K3==0) Play(3);
		if(K4==0) Play(4);
	}
}