#include<reg52.h>



//自动调速包含案件程序



sbit K1=P3^0;//低速
sbit K2=P3^1;//中速
sbit K3=P3^2;//高速

sbit K4=P3^3;//按下为自动
	
unsigned int i;

////////////延时函数///////////////

void  Delay(unsigned int count) 
{
unsigned char i;
while(count--)
{for(i=0;i<115;i++);}
}



void buzidong()
{
if(K1==0)
{Delay(200);i=7;}
if(K2==0)
{Delay(200);i=6;}
if(K3==0)
{Delay(200);i=5;}

}
void zidong()
{//具体的温度和速度范围自己可以设置
		if(temp>0&&temp<22) //速度1
//		i=6;
	if(temp>22&&temp<24) //速度1
		i=4;
		if(temp>24&&temp<26) //速度2
		i=2;
	if(temp>26&&temp<30) //速度3
		i=0;
	
{
DS18B20PRO();


}
}
void main()
{

while(1)
{


buzidong();

if(K4==0)
{zidong();}
}
}
