/*
*花样流水灯*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

code uint8 LED []= 
{
0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F,
0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0xFE,0xFF,
0x7E,0xBD,0xDB,0xE7,0xDB,0xBD,0x7E,0xFF
};

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

void main()
{
	uint8 i;
	while(1)
	{
		for(i = 0; i < 24; i ++)    //循环显示所有花样字节
		{
			P0 = LED[i];			//发字节给P0口
			delay(200);
		}
	}	
}