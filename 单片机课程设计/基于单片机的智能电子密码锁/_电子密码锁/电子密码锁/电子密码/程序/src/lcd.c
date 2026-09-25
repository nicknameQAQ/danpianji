#include"lcd.h"

void LCDWrite_String(uchar x, uchar y, uchar z, uchar *s)	//x 0~15 , y 0,1 
{
	LCD_set_xy(x,y,z);
	while(*s)
	{
		LcdWriteData(*s);
		s++;
	}
}


void LCD_set_xy(uchar x,uchar y,uchar z)
{
	uchar i;
	if(y==0)
	{
		LcdWriteCom(0x80+x);
		for(i=0;i<z;i++)
		{
			LcdWriteData(' ');
		}
		LcdWriteCom(0x80+x);
	}
	if(y==1)
	{
		LcdWriteCom(0xc0+x);
		for(i=0;i<z;i++)
		{
			LcdWriteData(' ');
		}
		LcdWriteCom(0xc0+x);
	}
}


void Lcd1602_Delay1ms(uint16 i)
{
	while(i--);
}
void LcdWriteCom(uint8 c)	 //Ð´ÃüÁî
{
	Lcd1602_Delay1ms(1000);
	rs=0;
	rw=0;
	e=0;
	P1=c;
	e=1;
	Lcd1602_Delay1ms(10);
	e=0;
}
void LcdWriteData(uint8 dat)	  //Ð´Êý¾Ý
{
	Lcd1602_Delay1ms(1000);
	rs=1;
	rw=0;
	e=0;
	P1=dat;
	e=1;
	Lcd1602_Delay1ms(10);
	e=0;
	rs=0;
}
void LcdInit()
{
	Lcd1602_Delay1ms(1000);
	LcdWriteCom(0x38);
	LcdWriteCom(0x38);
	LcdWriteCom(0x38);
	LcdWriteCom(0x06);
	LcdWriteCom(0x0c);
	LcdWriteCom(0x01);
}

