#ifndef _1602_H_
#define _1602_H_
 
#include<reg52.h>
/***********************LCD1602接线引脚定义**************************/
#define LCD_IO P0 //定义P2口与LCD1602的数据口相接
sbit LCD_RS = P2^6; //数据、命令选择端 1为数据
sbit LCD_RW = P2^5; //读、写选择端 1为读
sbit LCD_EN = P2^7; //使能控制
sbit LCD_BUSY=LCD_IO^7; //忙碌标志位
/**********************另外相关的定*********************************/
#define HIGH 1
#define LOW 0 
#define TURE 1
#define FALSE 0
#define uchar unsigned char
#define uint unsigned int
/**********************输入方式设置**************************** */
#define LCD_AC_AUTO_INCREMENT 0x06 //数据读、写操作后,AC自动增一
#define LCD_AC_AUTO_DECREASE 0x04 //数据读、写操作后,AC自动减一
#define LCD_MOVE_ENABLE 0x05 //数据读、写操作,画面平移
#define LCD_MOVE_DISENABLE 0x04 //数据读、写操作,画面不动
#define LCD_GO_HOME 0x02 //AC=0,光标、画面回HOME位
//设置显示、光标及闪烁开、关
#define LCD_DISPLAY_ON 0x0C //显示开
#define LCD_DISPLAY_OFF 0x08 //显示关
#define LCD_CURSOR_ON 0x0A //光标显示
#define LCD_CURSOR_OFF 0x08 //光标不显示
#define LCD_CURSOR_BLINK_ON 0x09 //光标闪烁
#define LCD_CURSOR_BLINK_OFF 0x08 //光标不闪烁
//光标、画面移动,不影响DDRAM
#define LCD_LEFT_MOVE 0x18 //LCD显示左移一位
#define LCD_RIGHT_MOVE 0x1C //LCD显示右移一位
#define LCD_CURSOR_LEFT_MOVE 0x10 //光标左移一位
#define LCD_CURSOR_RIGHT_MOVE 0x14 //光标右移一位
//工作方式设置
#define LCD_DISPLAY_DOUBLE_LINE 0x38 //两行显示，8位数据端口
#define LCD_DISPLAY_SINGLE_LINE 0x30 //单行显示
#define LCD_CLEAR_SCREEN 0X01 //清屏
/***********************LCD1602地址相关******************************/
#define LINE1_HEAD 0x80 //第一行DDRAM起始地址 
#define LINE2_HEAD 0xc0 //第二行DDRAM起始地址
#define LINE1 0 //第一行
#define LINE2 1 //第二行
#define LINE_LENGTH 16 //每行的最大字符长度


/*************************以下是函数的申明部*************************/
extern void LCD_Init(void); //LCD1602初始化
extern void LCD_Write_Command(uchar); // 写命令
extern void LCD_Write_Data(uchar); //写数据

extern void LCD_Flash(uchar); //LCD 画面闪烁
extern void LCD_Char(uchar, uchar, uchar); //在第几行第几列显示一个字符
extern void LCD_Str(uchar, uchar, char *); //从第几行第几列开始显示一个字符串

extern void DelayMs(uchar); //延迟 毫
extern void DelayUs(uchar); //延迟 微妙
extern void LCD_Check_Busy(void); //LCD 忙碌检测


/******************************************************************/
#endif
