#include <reg52.h>

sbit ST=P0^0;
sbit EOC=P0^1;
sbit OE=P0^2;
sbit AD_CLK=P0^3;

sbit Feng=P2^3;
uchar k=0x00;

extern void Init();
void main()
{
	Init();
	while(1)
	{
	}
}
//晶振为12MHz 所以经过12分频之后的时钟周期为1MHz 那么想要得到500Hz 的波形
//就是说半个周期为1/500s 而定时器加1的时间是1/5000000s 所以要这个时间乘以
//100000才能得到1/500s 这也就是TH0=(65536-10000)/256;TL0=(65536-10000)%256;
//初始化
void Init()
{
	AD_CLK=0;//产生ADC所要方波用
	EA = 1;  //总中断打开
	TMOD=0x01;//设置定时器0为工作方式1
	TH0=(65536-10000)/256;
	TL0=(65536-10000)%256;
	ET0=1;//开定时器0中断
	TR0=1;//启动定时器0
}


//中断函数
void exter0() interrupt 1  
{
 TH0=(65536-10000)/256;
 TL0=(65536-10000)%256;//定时器重新赋初值
 k++;
 if(k==1)
 {
  AD_CLK=~AD_CLK;//波形翻转
  k=0;
 }
}

