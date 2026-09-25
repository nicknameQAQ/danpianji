/*
*开关控制报警器*
*/
#include <reg51.h>
#define uint8 unsigned char
#define uint16 unsigned int
sbit SPK = P3^0;
sbit K1 = P1^3;

void Alarm(uint8 t)
{
 	uint8 i,j;
	for(i=0;i<200;i++)
	{
	 	SPK = ~SPK;
		for(j=0;j<t;j++);
	}
}

void main()
{
 	while(1)
	{
	 	if(K1==1)
		{
		 	Alarm(90);
			Alarm(120);
		}
	}
}