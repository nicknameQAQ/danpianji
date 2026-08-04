#include"reg52.h"
#include<intrins.h>
#define uint unsigned int
#define uchar unsigned char

sbit cs = P3^2;
long int read=0;
sbit wela = P2^7;
sbit dula = P2^6;
sbit lcden=P3^4;
sbit lcdrs=P3^5;

uchar bai,shi,ge,num;

uchar table[] ="123456789";

void delayms(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y-- );
}

void write_com(uchar com)
{	
	lcdrs=0;
	P0=com;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}

void write_data(uchar date)
{
	lcdrs=1;
	P0=date;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}

void init()
{
	dula=0;
	wela=0;
	lcden=0;
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
}
void delay(uint a)	   //  a ¸ö1usÑÓÊ±
{
	for(;a > 0;a --)
	_nop_();_nop_();
}

void T0_time() interrupt 1
{
	cs = 0;
	RD = 0;
	read = P1;
	INT0 = 1;
}
uint adc0804()
{
TMOD=0X01;
TH0=(65536-45872)/256;
TL0=(65536-45872)/256;
EA=1;
ET0=1;
TR0=1;

	INT0 = 1;
	cs = 0;
	WR = 0;
	delay(255);
	cs = 1;
	WR = 1;
	delay(200);		
	read = read * 196 /100;
	return read;
	
}

void math(uint x) 
{
	bai = x / 100; 
	shi = x /10 % 10;
	ge = x % 10;
}/* 
void display()
{	 
	 
	math(adc0804());
	
	dula=1;
	P0 = table[bai+10];
	dula=0;
	P0=0xff;

	wela = 1; 
	P0 = 0xfe;
	wela=0;
	delay(7000000);

		
	dula=1;
	P0 = table[shi];
	dula=0;
	P0=0xff;

	wela = 1; 
	P0 = 0xfd;
	wela=0;
	delay(7000000);

		
	dula=1;
	P0 = table[ge];
	dula=0;
	P0=0xff;

	wela = 1; 
	P0 = 0xfb;
	wela=0;
	delay(7000000);
	
}

   */

void main()
{
    cs=0;
		init();   	


	   	while(1)
		{
		

	    math(adc0804());
		

		write_com(0x80+5);
	
		write_data(table[bai]);
		write_data(table[shi]);
		write_data(table[ge]);
			delayms(500);
		
		}
	
}