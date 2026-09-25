#include "LCD1602.h"
#include <STC8.h>

sbit RS=P2^1;
sbit RW=P2^2;
sbit E=P2^4;

void LCD1602_operation(unsigned char Dat){//²Ù×÷1602
	unsigned int delay;
	RW=0;
	E=1;
	P0=Dat;
	E=0;
	for(delay=800;--delay;);
}

void LCD1602_Write_Reg(unsigned char Dat){//1602Ð´ÃüÁî
	RS=0;
  LCD1602_operation(Dat);
}

void LCD1602_Write_Data(unsigned char Dat){//1602Ð´Êý¾Ý
	RS=1;
  LCD1602_operation(Dat);
}