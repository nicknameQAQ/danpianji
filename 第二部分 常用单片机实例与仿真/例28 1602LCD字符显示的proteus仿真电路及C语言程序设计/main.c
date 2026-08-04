#include<reg51.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit rs=P3^3;
sbit rw=P3^4;
sbit en=P3^5;
sbit bflag=P2^7;
 
uchar str1[]="welcome to: ";
uchar str2[]="            www.hpu.edu.cn";
 uchar init[]={0x00,0x00,0x21,0x25,0x04,0x06,0x09};//秒，分，时，日，月，星期，年
 
void delay()
	{
	uchar j;
 	for(j=250;j>0;j--);
 	}
//-------------------液晶模块-----------------//
void en_toggle()
	{
  	en=0;
  //_nop_();
 	delay();
  	en=1;
 	delay();
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
void write_str(uchar addr,uchar *p)
{
 uchar i; 
 write_cmd(addr);
 while(p[i]!='\0')
 {write_data(p[i]);
  i++;
 } 

}
/*void write_position(uchar row,uchar col)
{uchar p;
 if(row==1)
 {p=0x80+col-1;
  write_cmd(p);}
 else
 {p=0xc0+col-1;
  write_cmd(p);
   }
}*/
void init_lcd()
{
 write_cmd(0x38);//8位数据接口，两行显示，5*7点阵字符
 write_cmd(0x0c);//显示开关开
 write_cmd(0x06);//光标移动设置
 write_cmd(0x01);//清屏幕
 delay();
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
