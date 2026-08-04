#include<reg52.h>
#define uint unsigned int
#define uchar unsigned char


sbit dscs=P2^4;
sbit dsas=P2^5;
sbit dsrw=P2^6;
sbit dsds=P2^7;
sbit irq=P3^3;


void writeds(uchar add,uchar dat)	// 向ds12cr887指定地址和写入数据
{
 dscs=0;
 dsas=1;
 dsds=1;
 dsrw=1;
 P1=add;
 dsas=0;
 dsrw=0;
 P1=dat;
 dsrw=1;
 dsas=1;
 dscs=1;
}

uchar readds(uchar add)	//访问12887寄存器的函数
{
 uchar dsdata;
 dsas=1;
 dsds=1;
 dsrw=1;
 dscs=0;
 P1=add;
 dsas=0;
 dsds=0;
 P1=0xff;
 dsdata=P1;
 dsas=1;
 dsds=1;
 dscs=1;
 return dsdata;

}

void initds()
{
writeds(0,0);
writeds(1,02);
writeds(2,30);
writeds(3,30);
writeds(4,15);
writeds(5,15);
writeds(6,4);
writeds(7,22);
writeds(8,11);
writeds(9,12);
writeds(0x0A,0x20);//打开振荡器（不打开12887内部的振荡器，时钟将不会运行）
writeds(0x0B,0x26);//打开闹钟中断使能，设置24小时模式，数据二进制格式
readds(0x0c);//读C寄存器将IRQF清除
}

void writealarm(uchar ahour,uchar aminute,uchar asecond) //将预定时的数据写进12887内的寄存器函数
{
writeds(1,asecond);
writeds(3,aminute);
writeds(5,ahour);
}
