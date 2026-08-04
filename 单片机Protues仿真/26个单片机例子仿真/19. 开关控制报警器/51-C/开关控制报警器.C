/*
*开关控制报警器*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

#define K1_DOWN()	 	    ((P1  & 0x08) == 0x08)			
#define SPK()				(P3 ^= 0x01)			

void BUZZER(uint8 t)
{
	uint8 i,j;
	for(i = 0; i < 200; i ++)
	{
		SPK();
		for(j=0;j<t;j++);
	}
}

void main()
{
	while(1)
	{
		if(K1_DOWN())
		{
			BUZZER(90);
			BUZZER(120);
		}
	}
}