#include <reg52.h>
#include <Delay.h>
sbit fmq=P1^5;
void FMQ_0(void)
{	
	fmq=0;
}
void FMQ_1(void)
{	
	fmq=1;
	
}