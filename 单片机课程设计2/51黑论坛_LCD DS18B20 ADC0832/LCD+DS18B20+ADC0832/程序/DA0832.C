#include"DAC0832.h"
#include<intrins.h>
#define NOP _nop_()
void DacInit(void)
{
	CS1=1;
	NOP;
	WR1=1;
}

void WriteToDac0832(unsigned char Dat)
{
	DacInit();
	DatPort=Dat;
	CS1=0;
	NOP;
	NOP;
	WR1=0;
	NOP;
	NOP;
	WR1=1;
	CS1=1;				
}