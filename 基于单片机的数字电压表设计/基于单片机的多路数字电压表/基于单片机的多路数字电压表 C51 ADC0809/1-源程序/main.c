#include<reg52.h>				   //包含头文件
#include <math.h>
#include "uart.h"
#include "display.h"
#define uint unsigned int 
#define uchar unsigned char		   //宏定义
sbit ale=P2^6;
sbit start=P2^7;
sbit eoc=P3^2;
sbit oe=P3^3;					   //定义adc0809的控制引脚
sbit adda=P2^0;
sbit addb=P2^1;
sbit addc=P2^2;					   //adc0809的地址引脚
//sbit rs=P2^3;
//sbit rw=P2^4;
//sbit en=P2^5;					   //液晶的控制引脚
sbit key1=P3^4;
sbit key2=P3^5;
unsigned long voltdata[8]; 
uint realvolt,i,j,k,l,t,sum,a[5];	 //定义全局变量
unsigned char CH;
uchar add;									 //地址变量
uchar TxBuf[8];		//四个显示的位
unsigned char t0Count;
unsigned char flagAu=1;
uchar code t1[]={"DC Voltmeter:IN "};
uchar code t2[]={" (0~20V):  .  V "};
uchar code t3[]={"      Hello!    "};
uchar code t4[]={"     Welcome!   "};		 //初始化显示

/*
函数说明
*/ 
void addselect()		//量程选择
{
  CH++;						//通道号加
  if(CH>7)					//最大到7		  符号&：按位与符号，二进制的对应位如果都为1，那么结果的对应位就为1，否则为0
  CH=0;						//清零，一共八个通道0 1 2 3 4 5 6 7，对应二进制是0000.0001.0010.0011.0100.0101.0110.0111
  adda=CH&0x01;				//例如5通道计算方法：将通道对应的二进制0101&0001，结果就是0001，赋值给adda（引脚只能接受最低位的数据，只有1和0），那么adda引脚就是高电平
  addb=(CH&0x02)>>1;		//然后将通道5的二进制0101&0010，结果就是0000（因为所有位都不同），然后整组数据右移一位，赋值给addb，那么addb引脚就是低电平
  addc=(CH&0x04)>>2; 		//然后将通道5的二进制0101&0100，结果就是0100，然后整组数据右移两位，赋值给addc，那么addc引脚就是高电平，最后芯片的通道选择引脚的状态就是101，则选择的就是通道5了
  delay(5);					//短延时
}
uint adtrans()			 //AD转换
{
	addselect();
	sum=0;	             //滤波开始
	for (i=0;i<5;i++)	   
	{
		ale=1;
		ale=0;
		start=1;			 //启动AD转换。
		start=0;
		while(eoc==0);		 //等待转换结束。
		oe=1;
		a[i]=P1;	     //出入转换结果。
		delay(5);		  ////每隔5usAD转换并采集一次数据，将得到的数值存入数组中
	}
	for (j=0;j<4;j++)				
		for(i=0;i<4-j;i++)
			if(a[i]>a[i+1])
			{
				t=a[i];
				a[i]=a[i+1];
				a[i+1]=t;
			}
	for(i=1;i<4;i++)
	{
		sum+=a[i];
	}				  //采用冒泡法比较，取中间3个数值
	voltdata[CH]=sum/3;
	
	delay(1);
	oe=0;
	voltdata[CH]=4*(100*voltdata[CH])/51;		   //处理运算结果。
	//voltdata[CH]是读取到的ad值，乘以100是让小数点后移两位，乘以4是量程扩大四倍也就是5*4=20V，然后除以51（和乘以0.0196一样）得到实际的电压值
	//8位AD最小分辨率就是0.0196V
	return voltdata[CH];
}
void disvolt()				//显示函数	
{
	realvolt=voltdata[add];
	writelcd_cmd(0x80);
	writelcd_dat('1'+add);
	TxBuf[1]=realvolt/1000+0x30;
	TxBuf[2]=realvolt/100%10+0x30;
	TxBuf[4]=realvolt/10%10+0x30;
	TxBuf[5]=realvolt%10+0x30;		 //将电压值分成4个一位数，方便显示
	writelcd_cmd(0x82);	   //显示电压值
	writelcd_dat(TxBuf[1]);
	writelcd_dat(TxBuf[2]);
	writelcd_cmd(0x85);
	writelcd_dat(TxBuf[4]);
	writelcd_dat(TxBuf[5]);
	realvolt=voltdata[add+1];
	writelcd_cmd(0x88);
	writelcd_dat('1'+add+1);
	TxBuf[1]=realvolt/1000+0x30;
	TxBuf[2]=realvolt/100%10+0x30;
	TxBuf[4]=realvolt/10%10+0x30;
	TxBuf[5]=realvolt%10+0x30;		 //将电压值分成4个一位数，方便显示
	writelcd_cmd(0x8a);	   //显示电压值
	writelcd_dat(TxBuf[1]);
	writelcd_dat(TxBuf[2]);
	writelcd_cmd(0x8d);
	writelcd_dat(TxBuf[4]);
	writelcd_dat(TxBuf[5]);

	realvolt=voltdata[add+2];
	writelcd_cmd(0xc0);
	writelcd_dat('1'+add+2);
	TxBuf[1]=realvolt/1000+0x30;
	TxBuf[2]=realvolt/100%10+0x30;
	TxBuf[4]=realvolt/10%10+0x30;
	TxBuf[5]=realvolt%10+0x30;		 //将电压值分成4个一位数，方便显示
	writelcd_cmd(0xc2);	   //显示电压值
	writelcd_dat(TxBuf[1]);
	writelcd_dat(TxBuf[2]);
	writelcd_cmd(0xc5);
	writelcd_dat(TxBuf[4]);
	writelcd_dat(TxBuf[5]);
	realvolt=voltdata[add+3];
	writelcd_cmd(0xc8);
	writelcd_dat('1'+add+3);
	TxBuf[1]=realvolt/1000+0x30;
	TxBuf[2]=realvolt/100%10+0x30;
	TxBuf[4]=realvolt/10%10+0x30;
	TxBuf[5]=realvolt%10+0x30;		 //将电压值分成4个一位数，方便显示
	writelcd_cmd(0xca);	   //显示电压值
	writelcd_dat(TxBuf[1]);
	writelcd_dat(TxBuf[2]);
	writelcd_cmd(0xcd);
	writelcd_dat(TxBuf[4]);
	writelcd_dat(TxBuf[5]);
}
void anjian()				//按键扫描函数
{

	if(key1==0)				//第一个按键按下
	{
	 	delay(20);
		if(key1==0)
		{
		 if(add==0)
		 add=4;
		 else
		 add=0;
		 flagAu=0;
		 while(key1==0);
		}
	}
	if(key2==0)
	{
	 	delay(20);
		if(key2==0)
		{
		 
	     flagAu=1;
		 while(key2==0);
		}
	}
 					//切换档位
	TxBuf[6]=CH+0x31;
 
}
void init()				 	//初始化函数
{
 
	add=0;				
		addselect();
	lcd_init();				//液晶显示初始化
    write_string(0x80,"1:  .  V2:  .  V");
	write_string(0xc0,"3:  .  V4:  .  V");
	TxBuf[0]='$';
	TxBuf[3]='.';
	TxBuf[7]='\0';
}
void main()				  //主程序
{
	init();				  //调用初始化函数
	Uart_init();
	while(1)			  //进入while循环
	{   
		anjian();		  //扫描按键
		adtrans();		  //ad函数
		disvolt();		  //显示电压
	}	 		  
}
 void timer0() interrupt 1
 {
  TH0=0x4c;
  TL0=0x00;
  t0Count++;
  if(t0Count>=40)
  {
   t0Count=0;
   if(flagAu==1)
   {
    if(add==0)
	add=4;
	else
	add=0;
   }
  }
 }