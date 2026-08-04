//宏定义
#define uint unsigned int 
#define uchar unsigned char
 
#include<intrins.h>
//LCD管脚声明
sbit lcd1602_rs= P2^3;
sbit lcd1602_en= P2^5;
sbit lcd1602_rw=P2^4; //液晶的控制引脚
//sbit busy=P0^7;
//void busy_lcd(void)
//{
////   busy=1;      //把忙标志位置1
//   lcd1602_en=0;       //把使能端置0
//   lcd1602_rs=0;      //把rs端置0，为指令操作
//   lcd1602_rw=1;      //为读操作
//   lcd1602_en=1;       //拉高使能端
////   _nop_();      //等待
////   while(busy==1);    //检测忙标志位，
//   lcd1602_en=0;       //忙标志位为0，则把使能拉低
//}
//----------------延时函数，后面经常调用----------------------
void delay(uint xms)//延时函数，有参函数
{
	uint x,y;
	for(x=xms;x>0;x--)
	 for(y=110;y>0;y--);
}

//--------------------------写指令---------------------------
void writelcd_cmd(uchar com)//****液晶写入指令函数****
{
//	  busy_lcd();      //忙标志检测
  lcd1602_rs=0;        //指令操作
  lcd1602_rw=0;        //写入操作
  lcd1602_en=1;        //拉高使能端
  P0=com;      //放上数据
  delay(1);       //等待
  lcd1602_en=0;        //拉低使能端，数据写入液晶
}

//-------------------------写数据-----------------------------
void writelcd_dat(uchar dat)//***液晶写入数据函数****
{
//  busy_lcd();      //忙标志检测
  lcd1602_rs=1;        //数据操作
  lcd1602_rw=0;        //写入操作
  lcd1602_en=1;        //拉高使能端
  P0=dat;      //放上数据
  delay(1);       //等待
  lcd1602_en=0;        //拉低使能端，数据写入液晶
}
 void write_string(uchar a_b,uchar *p)
{ 
  if(a_b!=0)
  writelcd_cmd(a_b);
  while(*p!='\0')
  {
  writelcd_dat(*p);
  p++;
  }
}

 
  
 
//-------------------------初始化-------------------------
void lcd_init(void)
{
	 writelcd_cmd(0x38);//设置液晶工作模式，意思：16*2行显示，5*7点阵，8位数据
	 writelcd_cmd(0x0c);//开显示不显示光标
	 writelcd_cmd(0x06);//整屏不移动，光标自动右移
	 writelcd_cmd(0x01);//清显示
	 writelcd_cmd(0x80);
}

 

