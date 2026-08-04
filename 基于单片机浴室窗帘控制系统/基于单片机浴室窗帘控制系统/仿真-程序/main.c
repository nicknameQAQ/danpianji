#include<reg51.h>
#include"delay.h"
#include"lcd602.h"
#include"adc0832.h"
/*****************************数组定义*******************************/
uchar code tab1[]={"Now_G: "};	 //
uchar code tab5[]={0xfe,0xfc,0xfd,0xf9,0xfb,0xf3,0xff7,0xf6};  //1-2励磁顺序表 //半步 
/************************端口定义**************************/
sbit k1 = P2^0;	// 手-正 
sbit k2 = P2^1;	// 手-反 
sbit k3 = P2^2;	// 手-停
sbit k4 = P2^3;	// 自-上
sbit k5 = P2^4;	// 自-下
sbit led = P3^3; // 自-手-lED
sbit k6 = P3^4; // 自-手
/************************变量定义*****************************/
uint sec,min,h;	//BCD码格式
int second,minute,hour; //十进制
double youliang = 0;
uchar i = 0;
bit flag = 0; //暂停开始标志
bit flag1 = 0; //正反转标志
bit flag2 = 0;
bit flag3 = 0;
bit mode = 1;
/*************************按键*******************************/
void key()
{
	  if(k1==0)
	  {
	  	   delayxms(5);
		   while(k1==0);
		   flag1 = 0; flag = 1;
		   write_zifu(1,15,'Z');
	  }
	  if(k2==0)	
	  {
			delayxms(5);
			while(k2==0);
			flag1 = 1; flag = 1;
			write_zifu(1,15,'F');
	  }
	  if(k3==0)	
	  {
			delayxms(5);
			while(k3==0);
			flag = 0;
	  }
}

void key1()
{
	  if(k4==0)
	  {
	  	   delayxms(5);
		   while(k4==0);
		   flag = 0;
		   write_zifu(1,15,'T');
		   flag2 = 1; flag3 = 0;
	  }
	  if(k5==0)	
	  {
			delayxms(5);
			while(k5==0);
			flag = 0;
			write_zifu(1,15,'T');
			flag2 = 0; flag3 = 1;
	  }
}
/*******************定时器T0初始化********************/
void T0_init()
{
	  TMOD=TMOD | 0x01;              //设T0为方式1
	  TH0=(65535-50000)/256;       //计数50000个
	  TL0=(65535-50000)%256; 
	  EA=1;                        //开启总中断         
	  ET0=1;                       //允许T0中断
	  //TR0=1;                       //启动
}
/**************************************************************
                      主程序
**************************************************************/
void main()
{
  chushihua(); //LCD1602初始化
  write_string(1,0,tab1);
  T0_init();
  while(1)
  {
			    /*自动和手动判断*/
			    if(k6==0)
				{
				  	   delayxms(5);
					   while(k6==0);
					   mode = ~mode;
				}
				/*重力计算 0-100*/
		        youliang = adc0832()/2.55; 
		        write_num3(1,7,youliang); //重力显示
				/*手动模式*/
				if(mode == 1)
				{
				   flag2 = 0; flag3 = 0;
				   led = 1; //LED关
				   key(); //按键检测
				   if(flag == 0)
				   {TR0 = 0; write_zifu(1,15,'T');}
				   else
				   {TR0 = 1;}
				}
				/*自动模式*/
				if(mode == 0)
				{
				   led = 0; //LED开
				   key1(); //按键检测
				   if(flag == 0)
				   {TR0 = 0;}
				   else
				   {TR0 = 1;}
				   /*根据重力判断正反转*/
				   if(youliang > 50 && flag2 == 0) //正转
				   {
				   	  flag = 1; flag1 = 0;
					  write_zifu(1,15,'Z');
				   }
				   if(youliang <= 50 && flag3 == 0) //反转
				   {
				   	  flag = 1; flag1 = 1;
					  write_zifu(1,15,'F');
				   }
				   if(flag == 0)
				   {TR0 = 0;}
				   else
				   {TR0 = 1;}
				}
  }
}
/**************************定时器T0中断服务函数************************************/
void T0_time() interrupt 1   
{
	  TR0 = 0;
      TH0=(65535-50000)/256;       
	  TL0=(65535-50000)%256;     
	  if(i > 7)
	  {i = 0;}
	  if(flag1 == 0)
	  {P1 = tab5[i];} //正转
	  else
	  {P1 = tab5[7-i];} //反转
	  i += 1;
	  TR0 = 1;
}
