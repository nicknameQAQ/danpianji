/*
*INT0和INT1控制条形LED*
*/
#include<reg51.h>						//头文件

void main(void)						    //主函数
{
	IP=0x05;							//外部中断0和外部中断1设置为高优先级
	IT0=1;								//外部中断0为下降沿触发
	IT1=1;								//外部中断1为下降沿触发
	EX0=1;								//开EX0中断
	EX1=1;								//开EX1中断
	EA=1;
	while(1);							//主循环
}

void ISR0(void) interrupt 0			//外部中断0服务例程
{
	P0=~P0;							//P1端口反相
}

void ISR1(void) interrupt 2			//外部中断1服务例程
{
	P1=~P1;							//P1端口反相
}