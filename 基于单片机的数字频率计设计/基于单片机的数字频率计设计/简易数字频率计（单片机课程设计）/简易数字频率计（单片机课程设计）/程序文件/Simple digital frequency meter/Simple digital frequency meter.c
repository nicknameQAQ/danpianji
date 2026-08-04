#include<reg52.h>
#include<intrins.h>
sbit OE1=P1^0;
sbit LE1=P1^1;
sbit OE2=P1^2;
sbit LE2=P1^3;
sfr16 DPTR=0x82;


    unsigned int a;                //输入信号1S内脉冲次数变量
	unsigned int i;
	unsigned int j;	
    unsigned int t;
	unsigned int k;

code unsigned char Segment[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
code unsigned char Select[]={0x00,0x80,0x40,0x20,0x10,0x08,0x04};
//void DigOutput(unsigned char SegmentByte) //段码输出程序
//void Delay(void)
//{
//	for(i=0;i<10;i++)
//	  {
	  
//	  }
	
//}
void TimerInit(void)  //定时器0与计数器1初始化
{
	TMOD=0x15;			  //非门控方式 定时器1 16位定时模式 	定时器0 16位计数模式
	TH1=0x3c;  //定时50ms初值装入
	TL1=0xaf;
	TH0=0x00;        //装入计数初值
	TL0=0x00;
	EA=1;
	ET0=1;
	TR0=1;
	ET1=1;
	TR1=1;
}
void Time1(void) interrupt 3	//定时器1中断服务程序
{	
	TH1=0x3c;
	TL1=0xaf;
	t++;
	if(t==20)
	{
		t=0;
		EA=0;
	}
	
}
void Time0(void) interrupt 1  //计数器0中断服务程序
{
	TH0=0x00;
	TL0=0x00;
	a++;
}

//void Int0Init(void)  //外部中断0初始化
//{
//	IT0=1;	//IT0下降沿触发
//	EX0=1;	//开外部中断0
//	EA=1;   //开总中断
//}
//void Int0(void) interrupt 0   //外部中断0服务程序
//{
//	a++;
//}
void Delay(unsigned int l)
{
	unsigned int f;
	for(f=0;f<l;f++);	
}
void main(void)
{	
	a=0;
	t=0;
	  
	TimerInit();
	OE1=0;
	LE1=1;
	OE2=0;
	LE2=1;
    while(1)
	{	
		if(!EA)
		{
		 	 
		 unsigned int Ament[7]={0,0,0,0,0,0,0};
		 DPL=TL0;
		 DPH=TH0;
		 k=a*65535+DPTR;
		 Ament[6]=k/100000;
		 Ament[5]=(k%100000)/10000;
		 Ament[4]=(k%10000)/1000;
		 Ament[3]=(k%1000)/100;
		 Ament[2]=(k%100)/10;
		 Ament[1]=k%10;
		 while(1)
		 {
			 for(i=1;i<7;i++)
			 {
				P2=Select[i];
				LE2=0;
				LE2=1;
				P0=Segment[Ament[i]];
				LE1=0;
				LE1=1;
				Delay(129);
			}
		 }
		
	}
  } 
}