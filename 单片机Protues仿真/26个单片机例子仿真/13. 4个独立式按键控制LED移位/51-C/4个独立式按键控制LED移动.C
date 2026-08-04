/*
*4个独立式按键控制LED移动*
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

uint8 i = 0,j = 0;

void Move_LED()
{
	if	   ((P1 & 0x01) == 0x00) i = (i - 1) & 0x07;
	else if((P1 & 0x02) == 0x00) i = (i + 1) & 0x07;
	else if((P1 & 0x04) == 0x00) j = (j - 1) & 0x07; 
	else if((P1 & 0x08) == 0x00) j = (j + 1) & 0x07; 
	
	P0 = ~(1 << i);
	P3 = ~(1 << j); 
}

void main()
{
	uint8 Key = 0x00;
	P0 = 0xFF;	P3 = 0xFF;
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