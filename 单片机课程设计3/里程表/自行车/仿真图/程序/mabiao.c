#include<reg52.h>

#define uchar unsigned char
#define uint unsigned int

sbit COUNT_IN=P3^2;

//定义1602相关管脚
sbit rs=P2^7;
sbit en=P2^6;

uint count;
unsigned long Velocity,Mileage;
bit flag;
uchar code tab1[]={"  Speed:      km/h"}; 	
uchar code tab2[]={"Mileage:  .   km"};	

void delay(uint x)
{
	uint i,j;
	for(i=0;i<x;i++)
	for(j=0;j<110;j++);
}
void init()
{
	IT0=1;	//INT0负跳变触发	
    TMOD=0x01;//定时器工作于方式1
	TH0=0x3c;	  //50ms
	TL0=0xb0;
	EA=1; //CPU开中断总允许
	ET0=1;//开定时中断
	EX0=1;//开外部INTO中断
    TR0=1;//启动定时
}
/********液晶写入指令函数与写入数据函数，以后可调用**************/

void write_1602com(uchar com)//****液晶写入指令函数****
{
	rs=0;//数据/指令选择置为指令
	P0=com;//送入数据
	delay(1);
	en=1;//拉高使能端，为制造有效的下降沿做准备
	delay(1);
	en=0;//en由高变低，产生下降沿，液晶执行命令

}


void write_1602dat(uchar dat)//***液晶写入数据函数****
{
	rs=1;//数据/指令选择置为数据
	P0=dat;//送入数据
	delay(1);
	en=1; //en置高电平，为制造下降沿做准备
	delay(1);
	en=0; //en由高变低，产生下降沿，液晶执行命令
}


void lcd_init()//***液晶初始化函数****
{
	uchar a;
	write_1602com(0x38);//设置液晶工作模式，意思：16*2行显示，5*7点阵，8位数据
	write_1602com(0x0c);//开显示不显示光标
	write_1602com(0x06);//整屏不移动，光标自动右移
	write_1602com(0x01);//清显示

	write_1602com(0x80);//显示固定符号从第一行第1个位置之后开始显示
	for(a=0;a<16;a++)
	{
		write_1602dat(tab1[a]);//向液晶屏写固定符号部分
	}
	write_1602com(0x80+0x40);//显示固定符号写入位置，从第2个位置后开始显示
	for(a=0;a<16;a++)
	{
		write_1602dat(tab2[a]);//写显示固定符号
	}
}

void display()
{
		//			1km/h=100m/0.1h	  	 360s
		//			10km/h=100m/0.01h 	 36s
		//			100km/h=100m/0.001h  3.6s

		write_1602com(0x80+9);
		if(Velocity/100==0)
		write_1602dat(' ');
		else
		write_1602dat(0x30+Velocity/100);
		if((Velocity/100+Velocity%100/10)==0)
		write_1602dat(' ');
		else
		write_1602dat(0x30+Velocity%100/10);
		write_1602dat(0x30+Velocity%10);//数字+30得到该数字的LCD1602显示码

		write_1602com(0x80+0x40+8);
		write_1602dat(0x30+Mileage/1000000);//数字+30得到该数字的LCD1602显示码
		write_1602dat(0x30+Mileage%1000000/100000);//数字+30得到该数字的LCD1602显示码 
		write_1602com(0x80+0x40+11);
		write_1602dat(0x30+Mileage%100000/10000);//数字+30得到该数字的LCD1602显示码
		write_1602dat(0x30+Mileage%10000/1000);//数字+30得到该数字的LCD1602显示码
		
		write_1602dat(0x30+Mileage%1000/100);//数字+30得到该数字的LCD1602显示码
}

void main()
{
	init();
	lcd_init();
	while(1)
	{
		display();
	}
}

void EXINT0() interrupt 0
{
	count++;
}

void time0() interrupt 1
{
	uchar m;
	TH0=0x3c;
	TL0=0xb0;	 //50ms
	m++;
	if(m>=10)
	{
		m=0;
		Mileage=Mileage+Velocity*500/36;		 //里程m=速度km/h*1000*5/3.6
		Velocity=count/5;//将100ms内的脉冲数
		count=0;		
	}
}




















