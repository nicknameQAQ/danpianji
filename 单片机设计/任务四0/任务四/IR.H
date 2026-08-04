#ifndef __IR_H_
#define __IR_H_
#include<reg52.h>


extern int p,number,maxday,year,month,day,h,min,h1,min1,h2,min2,y;

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

sbit IRIN=P3^2;



void IrInit();				//红外按键初始化
void infrared();				//红外按键

#endif