/*
*Á÷Ë®µÆ×óÒÆ*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

void main()
{
	uint8  j = 0;	
	uint16 i;
	while(1)
	{
		P0 = ~(0x01 << j++);	  		  //00000001 00000010 11111101
		for(i = 0;i< 20000;i++);
		if(8 == j)			    	
		{
			j = 0;				
		}
	}
}