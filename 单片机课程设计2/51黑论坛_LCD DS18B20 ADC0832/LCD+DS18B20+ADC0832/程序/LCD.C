#include"LCD.h"
#include<intrins.h>
void CheckBusy(void)
{
 	char i;
	while(i&0x80)
	{
	E=1;
	RS=0;
	RW=1;
	i=DataPort;
	E=0;
	}

}

void WriteData(unsigned char dat)
{
	CheckBusy();
	E=1;
	RS=1;
	RW=0;
	DataPort=dat;
	E=0;
}

void WriteCMD(unsigned char cmd)
{
	
	CheckBusy();
	E=1;
	RS=0;
	RW=0;
	DataPort=cmd;
	E=0;
}

void LCD_Init(void)
{		_nop_();
	_nop_();
	_nop_();
	_nop_();
	WriteCMD(0x30);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	WriteCMD(0x30);
	WriteCMD(0x8);
	WriteCMD(0x38);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	WriteCMD(0x01);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	WriteCMD(0x0c);
	WriteCMD(0x06); 
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}

void WriteStringToLcd(unsigned char x,unsigned char y,unsigned char *s)
{	unsigned char i;
	if(y)
	{
	i=0xc0+x;
	WriteCMD(i);
	}
	else
	{
	 i=0x80+x;
	WriteCMD(i);
	}
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
 	_nop_();
	_nop_();
	_nop_();
	_nop_();

	while(*s!='\0')
	{
	WriteData(*s);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	s++;
	}
	 	_nop_();
	_nop_();
	_nop_();
	_nop_();
}

void Write_self(unsigned char *s)
{char i;
WriteCMD(0x40);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
 	_nop_();
	_nop_();
	_nop_();
	_nop_();
for(i=0;i<8;i++)
{
	WriteData(*s);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	s++;
}
 	_nop_();
	_nop_();
	_nop_();
	_nop_();
}

 void Write_selfString(unsigned char x,unsigned char y,unsigned char Num)
 {
 	if(y)
	WriteCMD(0xc0+x);
	else
	WriteCMD(0x80+x);
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
 	_nop_();
	_nop_();
	_nop_();
	_nop_();
	 WriteData(Num);
	 	_nop_();
	_nop_();
	_nop_();
	_nop_();
 }