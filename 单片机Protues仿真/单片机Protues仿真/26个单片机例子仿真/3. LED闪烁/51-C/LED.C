 /*
*LED…¡À∏*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

void main()
{
	uint16 i;
	while(1)
	{
		for(i = 0; i < 20000; i ++)
		P0 = 0xFE;
		for(i = 0; i < 20000; i ++)
		P0 = 0xFF;
	}
}