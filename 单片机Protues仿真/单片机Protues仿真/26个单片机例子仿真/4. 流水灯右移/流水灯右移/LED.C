/*
*Á÷Ë®µÆÓÒÒÆ*
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
void main()
{	
	uint8 j = 0;
	while(1)
	{
		P0 = ~(0x80 >> j++);
		delay(200);
		if(j == 8)
		{
			j = 0;
		}
	}
}