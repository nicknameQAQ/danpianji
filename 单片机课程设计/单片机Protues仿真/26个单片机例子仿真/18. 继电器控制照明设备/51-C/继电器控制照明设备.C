/*
*继电器控制照明设备*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

#define K1_DOWN	 	    P1  & 0x01			//K1按键定义
#define RELAY_SWITCH()	P2 ^= 0x10			//继电器开关切换控制
#define LED_Light()		P2 ^= 0x04

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

int main()
{									
	while(1)
	{
		if(K1_DOWN)								
		{
			while (K1_DOWN);					
			RELAY_SWITCH();						
			LED_Light();
			delay(10);							  						
		}
	}
}