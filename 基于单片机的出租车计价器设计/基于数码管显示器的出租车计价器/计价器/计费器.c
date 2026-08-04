#include<reg51.h>
#define uint unsigned int
#define uchar unsigned char
sbit duan=P2^6;  	//定义段选
sbit wei=P2^7;		//定义位选
sbit dain=P0^7;		//定义小数点开启位
uchar code table[]={
0x3f,0x06,0x5b,0x4f,0x66,
0x6d,0x7d,0x07,0x7f,0x6f};
uint S,H,jin_e;			//定义里程数和金额
uchar x,y;
void delay(uint z);
void init();
void xianshi();
void main()
{
	init();//初始化 
	while(1)
	{		
		xianshi();
	}
	
}
void delay(uint z)//延时函数
{
	
	for(x=z;x>0;x--)
	  {
		for(y=110;y>0;y--)
			{
			}
	  }
}
void init()         //初始化函数
{
	S=0;
	H=0;
	jin_e=50;
	duan=0;
	wei=0;
	TMOD=0x50;        //设T1工作方式1计数；
	TH1=(65536-56)/256;
	TL1=(65536-56)%256;
	ET1=1;			//开启T1中断
	EA=1;				//开启总中断
	TR1=1;			//开启定时
}
void zhongduan() interrupt 3    //中断服务子程序
{
	S++;
	TH1=(65536-56)/256;
	TL1=(65536-56)%256;
	if(S<=20)//判断是否超过两公里
		jin_e=50;
	else
		if(S%5==0)//判断是否行使了500m
	   {	
			  H=(S-20)/5;
				jin_e=50+(8*H);
	   }
	
	
}
void xianshi()
{

  uchar qian,bai,shi,ge,baiyuan,shiyuan,yuan,jiao;
	//里程数分解
	qian=S/1000;
	bai=(S/100)%10;
	shi=(S%100)/10;
	ge=S%10;
	//价格分解
	baiyuan=jin_e/1000;
	shiyuan=(jin_e/100)%10;
	yuan=(jin_e%100)/10;
	jiao=jin_e%10;
	
	//duan=0;wei=0;
	//显示里程数
	P0=0xfe;
	wei=1;
	wei=0;
	P0=table[qian];
	duan=1;
	duan=0;
	delay(1);
	
	P0=0xfd;
	wei=1;
	wei=0;
	P0=table[bai];
	duan=1;
	duan=0;
	delay(1);
	
	P0=0xfb;
	wei=1;
	wei=0;
	P0=table[shi];
	dain=1;
	duan=1;
	duan=0;
	delay(1);
	
	P0=0xf7;
	wei=1;
	wei=0;
	P0=table[ge];
	duan=1;	
	duan=0;
	delay(1);
	
	//显示价格
	P0=0xef;
	wei=1;
	wei=0;
	P0=table[baiyuan];
	duan=1;
	duan=0;
	delay(1);
	
	P0=0xdf;
	wei=1;
	wei=0;
	P0=table[shiyuan];
	duan=1;	
	duan=0;
	delay(1);
	
	P0=0xbf;
	wei=1;
	wei=0;
	P0=table[yuan];
	dain=1;
	duan=1;	
	duan=0;
	delay(1);
	
	P0=0x7f;
	wei=1;
	wei=0;
	P0=table[jiao];	
	duan=1;	
	duan=0;
	delay(1);
}