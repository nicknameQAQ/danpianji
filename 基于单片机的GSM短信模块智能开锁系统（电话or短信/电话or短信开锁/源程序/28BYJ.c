#include <reg52.h>
#define uint unsigned int
#define uchar unsigned char
uchar code ccw[4]={0xfc,0xf6,0xf3,0xf9};
uchar code cw[4]={0xf9,0xf3,0xf6,0xfc};

void delayms_motor()//步进机延时程序
{
	uint i;
	for(i=0;i<200;i++)//快点好
	;

}
void delayms(uchar x)//延时程序
{
	uchar i;
	while(x--)
	for(i=0;i<120;i++)
	;
}
void forward()//正转程序
{
	uchar i;
	for(i=0;i<4;i++)
	{
		P2=cw[i];
		delayms_motor();
	}
} 
void backward()//反转程序
{
	uchar i;
	for(i=0;i<4;i++)
	{
		P2=ccw[i];
		delayms_motor();
	}

}
void stop()
{
	P2=0xff;
}