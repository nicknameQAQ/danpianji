/*
*左右来回的流水灯*
*/
#include<reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

void delay(uint16 x)
{
	uint8 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

void main()
{
	uint8 b = 0, way = 0;			  //移动位数变量及移动方向变量 
	while(1)
	{
		 if(way == 0)  				  //根据way选择左移还是右移
		 	P0 = ~(0x01 << b);		  //最低位的0被左移b位
		 else
		 	P0 = ~(0x80 >> b);		  //最高位的0被右移b位

		 if( ++b == 8) 				  //如果移动到左端或右端
		 {
		 	b = 0;					  //b归0
			way = !way;				  //改变方向
		 }
		 delay(200);
	}
}