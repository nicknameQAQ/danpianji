#ifndef __KEYPRO_H_
#define __KEYPRO_H_
#include<reg52.h>



extern int y;
extern int min2;
extern int moment;

void keypros();

sbit k1=P3^1;	 //定义P31口是k1
sbit k2=P3^0;



#endif	