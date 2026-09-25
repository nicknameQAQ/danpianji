#ifndef LCD_h
#define LCD_h
#include<reg52.h>
#define DataPort P0
sbit RS=P2^0;
sbit RW=P2^1;
sbit E=P2^2;
extern 	void WriteCMD(unsigned char cmd);
extern void LCD_Init(void);
extern void WriteDatToLcd(unsigned char x,unsigned char y,unsigned char dat);
extern void WriteStringToLcd(unsigned char x,unsigned char y,unsigned char *s);
extern 	void Write_self(unsigned char *s);
extern void Write_selfString(unsigned char x,unsigned char y,unsigned char Num);
#endif