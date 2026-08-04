#include <reg52.h>
#include <intrins.h>
#include <absacc.h>
#include <math.h>  
#include "0832.h"
#define uchar unsigned char
#define uint unsigned int
#include "eeprom52.h"

sbit DATA = P3^7;  //温湿度
sbit E=P2^5;		//1602使能引脚
sbit RS=P2^7;		//1602数据/命令选择引脚
sbit RW=P2^6;
sbit K1=P1^3;//设置键
sbit K2=P1^4;//加
sbit K3=P1^5;//减
sbit LED1=P3^1;	 //烟雾报警
sbit LED2=P3^2;			   //温度上限
sbit LED3=P3^3;				//温度下限
sbit LED4=P3^4;				 //湿度上限
sbit LED5=P3^5;					   //湿度下限
sbit alarm=P3^0;		 //蜂鸣器
uchar U8FLAG,U8temp,U8comdata,U8RH_data_H_temp,U8RH_data_L_temp,U8T_data_H_temp,U8T_data_L_temp,U8checkdata_temp;
uchar U8RH_data_H,U8RH_data_L,U8T_data_H,U8T_data_L,U8checkdata;
uchar Mode,humidity,temperature,smog;
bit BJ_LED1=1,BJ_LED2=1,BJ_LED3=1,BJ_LED4=1,BJ_LED5=1,BJ_LED6=1;	  //定义LED报警变量
bit bdata FlagStartRH;
uchar flag ;//记录当前设置状态
uint count;

//定时器0初始化
void Timer0_Init()
{
	ET0 = 1;        //允许定时器0中断
	TMOD = 0x11;       //定时器工作方式选择
	TL0 = 0xFF;     
	TH0 = 0x4B;     //定时器赋予初值
	TH1=0xdc;
	TL1=0x00;
	ET1=1;
	TR0 = 1;        //启动定时器
}

//定时器0中断
void Timer0_ISR (void) interrupt 1 using 0
{
	uchar RHCounter;
	TL0 = 0xFF;
	TH0 = 0x4B;     //定时器赋予初值
	RHCounter++;
    if (RHCounter >= 15)
    {
       FlagStartRH = 1;
	   RHCounter = 0;
    }
}
void Delay1(uint j)
{
    uchar i;
    for(;j>0;j--)
    { 	
		for(i=0;i<27;i++);
    }
} 

void  Delay_10us(void)
{
    uchar i;
    i--;
    i--;
    i--;
    i--;
    i--;
    i--;
}

void delay_ms(uint z)  //延时函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=121;y>0;y--);			//约1ms
} 	

void  COM(void)
{
    uchar i;
    for(i=0;i<8;i++)	   
    {
  	    U8FLAG=2;
	   	while((!DATA)&&U8FLAG++);
		Delay_10us();
		Delay_10us();
		Delay_10us();
  		U8temp=0;
        if(DATA)U8temp=1;
		    U8FLAG=2;
		while((DATA)&&U8FLAG++);
	   	//超时则跳出for循环		  
	   	if(U8FLAG==1)break;
	   	//判断数据位是0还是1	 
	   	   
		// 如果高电平高过预定0高电平值则数据位为 1 
	   	 
		U8comdata<<=1;
	   	U8comdata|=U8temp;        //0
    }//rof
}

//--------------------------------
//-----温湿度读取子程序 ------------
//--------------------------------
//----以下变量均为全局变量--------
//----温度高8位== U8T_data_H------
//----温度低8位== U8T_data_L------
//----湿度高8位== U8RH_data_H-----
//----湿度低8位== U8RH_data_L-----
//----校验 8位 == U8checkdata-----
//----调用相关子程序如下----------
//---- Delay();, Delay_10us();,COM(); 
//--------------------------------
uchar RH(void)
{
    //主机拉低18ms 
    DATA=0;
	Delay1(180);  //原来为5
	DATA=1;
	//总线由上拉电阻拉高 主机延时20us
	Delay_10us();
	Delay_10us();
	Delay_10us();
	Delay_10us();
	//主机设为输入 判断从机响应信号 
	DATA=1;
	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	if(!DATA)		 //T !	  
	{
	    U8FLAG=2;
	    //判断从机是否发出 80us 的低电平响应信号是否结束	 
	    while((!DATA)&&U8FLAG++);
	    U8FLAG=2;
	    //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	    while((DATA)&&U8FLAG++);
	    //数据接收状态		 
	    COM();
	    U8RH_data_H_temp=U8comdata;
	    COM();
	    U8RH_data_L_temp=U8comdata;
	    COM();
	    U8T_data_H_temp=U8comdata;
	    COM();
	    U8T_data_L_temp=U8comdata;
	    COM();
	    U8checkdata_temp=U8comdata;
	    DATA=1;
	    //数据校验 
	    U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
	    if(U8temp==U8checkdata_temp)
	    {
	   	   U8RH_data_H=U8RH_data_H_temp;
	   	   U8RH_data_L=U8RH_data_L_temp;
	   	   U8T_data_H=U8T_data_H_temp;
	   	   U8T_data_L=U8T_data_L_temp;
	   	   U8checkdata=U8checkdata_temp;
	    }
		return 1;
	}
	else   //传感器不响应
	{
	   return 0;
	}
}

/********************************************************************
* 文件名  ： 液晶1602显示.c
* 描述    :  该程序实现了对液晶1602的控制。
***********************************************************************/


/********************************************************************
* 名称 : delay()
* 功能 : 延时,延时时间大概为140US。
* 输入 : 无
* 输出 : 无
***********************************************************************/

void delay()
{
	int i,j;
	for(i=0; i<=10; i++)
	for(j=0; j<=2; j++);
}	  
	
/********************************************************************
* 名称 : enable(uchar del)
* 功能 : 1602命令函数
* 输入 : 输入的命令值
* 输出 : 无
***********************************************************************/

void write_1602com(uchar del)
{
	P0 = del;
	RS = 0;
	E = 1;
	delay();
	E = 0;
	delay();
}

/********************************************************************
* 名称 : write(uchar del)
* 功能 : 1602写数据函数
* 输入 : 需要写入1602的数据
* 输出 : 无
***********************************************************************/

void write_1602dat(uchar del)
{
	P0 = del;
	RS = 1;
	E = 1;
	delay();
	E = 0;
	delay();
}

/********************************************************************
* 名称 : L1602_init()
* 功能 : 1602初始化，请参考1602的资料
* 输入 : 无
* 输出 : 无
***********************************************************************/
void L1602_init(void)
{
	
	write_1602com(0x38);
	write_1602com(0x0c);
	write_1602com(0x06); 
	write_1602com(0x01); //清屏要放在最后
	delay_ms(2);
}


void display()
{
  write_1602com(0x83);
  write_1602dat(temperature%100/10+0x30);
  write_1602dat(temperature%100%10+0x30);
  write_1602com(0x8c);
  write_1602dat(humidity%100/10+0x30);
  write_1602dat(humidity%100%10+0x30);
    write_1602com(0xc3);
  write_1602dat(smog%100/10+0x30);
  write_1602dat(smog%100%10+0x30);
  	write_1602com(0x85);
	write_1602dat(0xdf);								  
}
void display_1()
{
  write_1602com(0x83);
  write_1602dat(TH%100/10+0x30);
  write_1602dat(TH%100%10+0x30);
  write_1602com(0x8c);
  write_1602dat(TL%100/10+0x30);
  write_1602dat(TL%100%10+0x30);
   write_1602com(0xc3);
  write_1602dat(HH%100/10+0x30);
  write_1602dat(HH%100%10+0x30);
  write_1602com(0xcc);
  write_1602dat(HL%100/10+0x30);
  write_1602dat(HL%100%10+0x30);
}
void write_string(uchar * str)
{
  while( * str)
  {
  write_1602dat(* str);
  str++;
  }
}
/***按键函数***/
void KEY()
{
 if(K1==0)
 {
   Delay1(200);
  if(K1==0)
  {
   	flag++;
	if(flag==1)
    {
	 write_1602com(0x80);
	 write_string("TH:   C  TL:   C");
	 write_1602com(0xc0);
	 write_string("HH:  %   HL:  % ");
	 write_1602com(0x85);
	write_1602dat(0xdf);
	write_1602com(0x8e);
	write_1602dat(0xdf);
	 display_1();
	 write_1602com(0x84);
	 write_1602com(0x0f);
	}
	else if(flag==2)
	 write_1602com(0x8d);
	 else if(flag==3)
	 write_1602com(0xc4);
	 else if(flag==4)
	 write_1602com(0xcd);
	 else if(flag==5)
	 {
	 write_1602com(0xc0);
	 write_string("                ");
	 write_1602com(0x80);
	 write_string("Sg:  mg/m3      ");
	 write_1602com(0x83);
	 write_1602dat(SM/10+0x30);
	 write_1602dat(SM%10+0x30);

	 write_1602com(0x84);
	 }
	 else 
	 {
	 write_1602com(0x80);
	 write_string("Te:   C  Hu:  % ");
	 write_1602com(0xc0);
	 write_string("Sg:  mg/m3      ");
	 write_1602com(0x85);
     write_1602dat(0xdf);
     display();
	 write_1602com(0x0c);
	 flag=0;
	 write_eeprom();
	 }
	 while(!K1);
  }
 }
 if(K2==0)
 {
   Delay1(200);
  if(K2==0)
  {
   switch (flag)
           {
		    case 1: TH++;if(TH>99) TH=0;if(TH<=TL) TH=TL+1;  write_1602com(0x83);write_1602dat(TH/10+0x30);write_1602dat(TH%10+0x30); write_1602com(0x84);break;
		    case 2: TL++;if(TL>98) TL=0; if(TL>=TH) TL=TH-1;    write_1602com(0x8c);write_1602dat(TL/10+0x30);write_1602dat(TL%10+0x30);write_1602com(0x8d); break;
			case 3: HH++;if(HH>99) HH=0; if(HH<=HL) HH=HL+1;write_1602com(0xc3);write_1602dat(HH/10+0x30);write_1602dat(HH%10+0x30); write_1602com(0xc4);break;
			case 4: HL++;if(HL>98) HL=0;   if(HL>=HH) HL=HH-1;  write_1602com(0xcc);write_1602dat(HL/10+0x30);write_1602dat(HL%10+0x30); write_1602com(0xcd);break;
			case 5: SM++;if(SM>9)  SM=0;  write_1602com(0x83);write_1602dat(SM/10+0x30);write_1602dat(SM%10+0x30);write_1602com(0x84); break;
		   }
   while(!K2);
  }
 }
 if(K3==0)
 {
   Delay1(200);
  if(K3==0)
  {
   switch (flag)
   {
   case 1: TH--;if(TH>99) TH=99; if(TH<=TL) TH=TL+1;   write_1602com(0x83);write_1602dat(TH/10+0x30);write_1602dat(TH%10+0x30); write_1602com(0x84);break;
   case 2: TL--;if(TL>99) TL=98;if(TL>=TH) TL=TH-1;   write_1602com(0x8c);write_1602dat(TL/10+0x30);write_1602dat(TL%10+0x30);write_1602com(0x8d); break;
   case 3: HH--;if(HH>99) HH=99; if(HH<=HL) HH=HL+1;   write_1602com(0xc3);write_1602dat(HH/10+0x30);write_1602dat(HH%10+0x30); write_1602com(0xc4);break;
   case 4: HL--;if(HL>99) HL=98;if(HL>=HH) HL=HH-1;   write_1602com(0xcc);write_1602dat(HL/10+0x30);write_1602dat(HL%10+0x30); write_1602com(0xcd);break;
   case 5: SM--;if(SM>99)  SM=9;  write_1602com(0x83);write_1602dat(SM/10+0x30);write_1602dat(SM%10+0x30);write_1602com(0x84); break;
   }
   while(!K3);
  }
 }
}
void Onealarm()
{
  alarm=0;
  Delay1(200);
  alarm=1;
}

void dis_init()
{
	unsigned char pp=0;
	write_1602com(0x80);				//进入设置状态显示
	write_string("Welcome to use  ");	//设置状态第一行显示
	write_1602com(0xc0);				//选中第二行
	write_string("Please waiting..");	//设置状态第二行显示
	P3=0xFE;
	delay_ms(500);
	for(pp=0;pp<8;pp++)
	{
		P3=_crol_(P3,1);
		delay_ms(100);
	}
	P3=0x7F;
	for(pp=0;pp<8;pp++)
	{
		P3=_cror_(P3,1);
		delay_ms(100);
	}
	alarm=1;
	LED1=LED2=LED3=LED4=LED5=0;
	delay_ms(2000);
}

/********************************************************************
* 名称 : Main()
* 功能 : 主函数
***********************************************************************/
void main()
{
    uint testnum;

    EA = 0;
	Timer0_Init();  //定时器0初始化
	EA = 1;
	RW=0;
  
	L1602_init();
	dis_init();
	 write_1602com(0x85);
     write_1602dat(0xdf);
	  	 write_1602com(0x80);
     write_1602dat('T');
     display();	
	 init_eeprom();
	 read_eeprom();
	 	 write_1602com(0x80);
	 write_string("Te:   C  Hu:  % ");
	 write_1602com(0xc0);
	  write_string("Sg:  mg/m3      ");
	while(1)
	{
		 if (FlagStartRH == 1)	 //温湿度转换标志检查
		 {
		     TR0 = 0;
             testnum = RH();
 			 FlagStartRH = 0;
			 TR0 = 1;
			 humidity = U8RH_data_H;	 //读出温湿度，只取整数部分
			 temperature = U8T_data_H;	
			 if(temperature>=TH)
			 {LED2=0;BJ_LED2=0;}
			 else
			 {LED2=1;BJ_LED2=1;}
			 if(temperature<=TL)
			 {LED3=0;BJ_LED3=0;}
			 else
			 {LED3=1;BJ_LED3=1;}
			 if(humidity>=HH)
			 {LED4=0;BJ_LED4=0;}
			 else
			 {LED4=1;BJ_LED4=1;}
			 if(humidity<=HL)
			 {LED5=0;BJ_LED5=0;}
			 else
			 {LED5=1;BJ_LED5=1;}
		}
		convdata(adc0832(0));
		smog=(dis[1]*100+dis[2]*10+dis[3]+1)*10/250;
		if(flag==0)
		display();
		KEY();	
		if(smog>=SM)
		{LED1=0;BJ_LED1=0;}
		else
		{LED1=1;BJ_LED1=1;}
		if(BJ_LED1==0||BJ_LED2==0||BJ_LED3==0||BJ_LED4==0||BJ_LED5==0)	   //有超限
		Onealarm();
		else
		alarm=1;
	}
}