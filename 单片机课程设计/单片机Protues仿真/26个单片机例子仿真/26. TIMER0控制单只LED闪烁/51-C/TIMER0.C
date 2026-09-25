/*
*INT1中断5位计数*
*/
#include<reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

sbit LED = P0^0;

uint16 T_Count = 0;

void main()
{
	TMOD = 0x01;
	TH0 = 0xFC;
	TL0 = 0x67;
	IE   = 0x82;
	TR0  = 1;
	while(1);
}

void LED_Flash() interrupt 1
{
 	TMOD = 0x01;
	TH0 = 0xFC;
	if(++T_Count == 100)
	{
	 	LED = ~LED;	 //!LED
		T_Count = 0;
	}
}