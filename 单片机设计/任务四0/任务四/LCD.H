#ifndef __LCD_H_
#define __LCD_H_

//---包含头文件---//
#include <reg52.h>

extern int t[14],c[16],year,y;
extern  char w[7];
extern  char d[14];
sbit k4=P3^2;
//#define LCD12864_PICTURE

//---定义使用的IO口---//

#define LCD12864_DATAPORT P0	  //数据IO口

sbit LCD12864_RS  =  P2^6;             //（数据命令）寄存器选择输入 
sbit LCD12864_RW  =  P2^5;             //液晶读/写控制
sbit LCD12864_EN  =  P2^7;             //液晶使能控制
sbit LCD12864_PSB =  P3^2;             //串/并方式控制
sbit LCD12864_RST =  P3^4;			   //复位端


//---声明全局函数---//
void Delay1ms(unsigned int c);
unsigned char LCD12864_Busy(void);
void LCD12864_WriteCmd(unsigned char cmd);
void LCD12864_WriteData(unsigned char dat);
void LCD12864_Init();
void LCD12864_STR(unsigned char X,unsigned char Y,unsigned char *str);
 void LCD12864_char(unsigned char x,unsigned char y);				   //定位显示

void lcd0();
#endif
