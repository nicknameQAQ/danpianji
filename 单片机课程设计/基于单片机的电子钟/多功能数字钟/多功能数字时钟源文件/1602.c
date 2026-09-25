#include<reg52.h>
#define uint unsigned int
#define uchar unsigned char
sbit en=P3^4;
sbit rs=P3^5;
sbit rw=P3^6;

uchar code t1[]={" 20  -  -       "}; //1602液晶初始画面
uchar code t2[]={"      :  :     "};  //1602液晶初始画面

void delay(uint t)	 //粗略延时1ms函数
{
uint i,j;
for(i=t;i>0;i--)
for(j=110;j>0;j--);
}


void writelcd_cmd(uchar cmd)//向1602液晶显示器里写入命令的函数
{
en=0;
rs=0;
rw=0;
delay(1);
P0=cmd;
en=1;
delay(1);
en=0;
}

void writelcd_dat(uchar dat)//向1602液晶显示器里写入数据的函数
{
en=0;
rs=1;
rw=0;
delay(1);
P0=dat;
en=1;
delay(1);
en=0;
}

void lcd_init()	 //初始化1602液晶显示器的函数
{
uint i,j;
writelcd_cmd(0x38);
delay(5);
writelcd_cmd(0x38);
delay(5);
writelcd_cmd(0x38);
writelcd_cmd(0x08);
writelcd_cmd(0x01);
writelcd_cmd(0x06);
writelcd_cmd(0x0c);
writelcd_cmd(0x80);
for(i=0;i<16;i++)
{
writelcd_dat(t1[i]);
delay(5);
};
writelcd_cmd(0xc0);
for(j=0;j<16;j++)
{
writelcd_dat(t2[j]);
delay(5);
}
}
