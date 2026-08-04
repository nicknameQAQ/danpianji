#include <reg52.h>

sbit ST=P0^0;
sbit EOC=P0^1;
sbit OE=P0^2;
sbit AD_CLK=P0^3;

sbit Feng=P2^3;
uchar k=0x00;
//ADC0809读取信息
uchar ADC0809()
{
	uchar temp_=0x00;
	//初始化高阻太
	OE=0;
	//转化初始化
	ST=0;
	//开始转换
	ST=1;
	ST=0;
	//外部中断等待AD转换结束
	Feng=0;
	while(EOC==0)
	Feng=1;
	OE=1;
	//temp_=AD_data;
	OE=0;
	return temp_;
}
void main()
{
	while(1)
	{
		ADC0809();
	}
}