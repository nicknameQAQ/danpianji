#include"DS18B20.h"
#include<stdio.h>
#include<intrins.h>

void delayNus(int dly)//11us
{
	while(dly--);
}


 unsigned char Rest18b20(void)
{
 	char x,i=255;
 	DQ=1;
	delayNus(2);
	DQ=0;
	 delayNus(60); 
	 DQ=1;
	 delayNus(2);
	 while(DQ&&x--);
	 x=DQ;
	 delayNus(15);
	 if(x)
	 {
	 return 0;
	 }
	 else
	 {
	 return 1;
	 }              
}

void WriteOneByte(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
		{
			DQ=0;
			DQ=dat&0x01;
			delayNus(2);
			DQ=1;
			dat>>=1;
		}
		delayNus(4);
}

unsigned char ReadOneByte(void)
{	
	unsigned char i,dat;
	for(i=0;i<8;i++)
	{
		DQ=0;
		dat>>=1;
		DQ=1;
		if(DQ)
		dat|=0x80;
		delayNus(4);
	}
	return dat;
}

unsigned char ReadRomSerial(unsigned char *s)
{	unsigned char i;

		if(Rest18b20())
		{
		delayNus(1);
		WriteOneByte(0x33);
		for(i=0;i<8;i++)
		{
			*s=ReadOneByte();
			s++;
		}	
	   }
   	return 1;

			
}

unsigned char MatchRom(unsigned char *s,unsigned char Num)
{
	unsigned char i;
	if(Num>0)
	{
		if(Rest18b20())
		{
			delayNus(1);
			WriteOneByte(0x55);
			for(i=0;i<8;i++)
			{
				s+=8*(Num-1);
				WriteOneByte(*s);
				s++;	
			}
		}
		return 1;
	}
 	return 0;
}

 unsigned char ReadTempreture(unsigned char *s,unsigned char Num )
{
	unsigned char a,b;
	Rest18b20();
	WriteOneByte(0xcc);
	WriteOneByte(0x44);
	delayNus(300);
	Rest18b20();
	if(Num>8)
	WriteOneByte(0xcc);
	else
	MatchRom(s,Num);
	WriteOneByte(0xbe);
	delayNus(20);
	a=ReadOneByte();
	b=ReadOneByte();
	b<<=4;
	b+=(a&0xf0)>>4;
	return b;
}

void SendByte(unsigned char dat)
{
SBUF=dat;
while(!TI);
TI=0;
}

void SendString(unsigned char *s)
{
	while(*s!='\0')
	{
		SendByte(*s);
		s++;	
	}
}

void Init18b20(void)
{
	if(!Rest18b20())
	{
	WriteOneByte(0xcc);
	WriteOneByte(0x4e);
	WriteOneByte(0x20);
	WriteOneByte(0x00);
	WriteOneByte(0x7f);
	}

}


void Initboard(void)
{
TMOD=0x20;
PCON&=0x7f;
SCON=0x50;
TH1=0xfd;
TL1=0;
TR1=1;
TI=1;
}