#include<reg51.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit rs=P3^3;
sbit rw=P3^4;
sbit en=P3^5;
sbit bflag=P2^7;
 
uchar str1[16]="welcome to: ";
uchar str2[16]="www.hpu.edu.cn  ";
 void delay(unsigned int z)
{
	unsigned int x;
	unsigned char y;
	for(x=z;x>0;x--)
		for(y=20;y>0;y--);	
}
 
void delay1()
	{
	uchar j;
 	for(j=2500;j>0;j--);
 	}
//-------------------液晶模块-----------------//
void en_toggle()
	{
  	en=0;
  //_nop_();
 	delay1();
  	en=1;
 	delay1();
 // _nop_();
  	en=0;//由高电平变成低电平时，执行命令
	}             //注意必须加延时，否则会出问题的
void is_ready()
	{
   P2=0x00;//设置为输入口
   rs=0;
   rw=1;
   en=1;//高电平，读忙标志
   while(bflag);
  	en=0;
	}

void write_cmd(uchar cmd)
{
  is_ready();
  rs=0;
  rw=0;
  P2=cmd;
  en_toggle();//下降沿，写指令代码
}
void write_data(uchar indata)
{
  is_ready();
  rs=1;
  rw=0;
  P2=indata;
  en_toggle();//下降沿，写数据
  _nop_();
}
void write_str(uchar addr,uchar str[16])
{ 
 uchar i; 
 write_cmd(addr);
 for(i=0;i<16;i++)
 {
 write_data(str[i]);
 delay(2000);

 } 

}
 
void init_lcd()
{
 write_cmd(0x38);//8位数据接口，两行显示，5*7点阵字符
 write_cmd(0x0c);//显示开关开
 write_cmd(0x06);//光标移动设置
 write_cmd(0x01);//清屏幕
 delay1();
}
 
//-----------ds1302模块结束-------------------
void main()
	{
 	init_lcd();             //初始化LCD
		write_str(0x80,str1);	  //液晶显示提示信息
                
      write_str(0xc0,str2);	  //液晶显示提示信息
  
  	while(1)
  		{
	 ;
		}
	
	}