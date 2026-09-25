/*
*4个独立式按键控制LED开关*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

void Move_LED()
{
	if	   ((P1 & 0x01) == 0x00) P0 ^= 0x01; 
	else if((P1 & 0x02) == 0x00) P0 ^= 0x02;
	else if((P1 & 0x04) == 0x00) P0 ^= 0x04; 
	else if((P1 & 0x08) == 0x00) P0 ^= 0x08; 
	 
}

void main()
{
	uint8 Key = 0x00;
	while(1)
	{
		 if(P1 != Key)
		{
			Key = P1;
			Move_LED();
			delay(10);
		}
	}
}