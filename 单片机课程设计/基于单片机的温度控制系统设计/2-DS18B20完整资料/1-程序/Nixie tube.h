#ifndef _Nixietube_H_
#define _Nixietube_H_
#define uchar unsigned char 
#define uint unsigned int 

#define Nixietube_dat P0

sbit bit1=P2^0;
sbit bit2=P2^1;
sbit bit3=P2^2;
sbit bit4=P2^3;




void tube_delay(uint T)					  //ÑÓÊ±º¯Êı
{
	while(T--);
}

void Nixietube(char dat1,dat2,dat3,dat4)
{
	Nixietube_dat=dat1;
	bit1=0;
	tube_delay(50);
	Nixietube_dat=0xff;
	bit1=bit2=bit3=bit4=1;
		
	Nixietube_dat=dat2;
	bit2=0;
	tube_delay(50);
	Nixietube_dat=0xff;
	bit1=bit2=bit3=bit4=1;
		
	Nixietube_dat=dat3;
	bit3=0;
	tube_delay(50);
	Nixietube_dat=0xff;
	bit1=bit2=bit3=bit4=1;
		
	Nixietube_dat=dat4;
	bit4=0;
	tube_delay(50);
	Nixietube_dat=0xff;
	bit1=bit2=bit3=bit4=1;
		



}

#endif