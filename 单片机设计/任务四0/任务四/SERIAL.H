#ifndef __SERIAL_H_
#define __SERIAL_H_
#include<reg52.h>

extern int year,year0,month,month0,day,day0,h,min,h1,min1,h2,min2,y,p,maxday,number,sec; 

void UsartInit();
void pointer();
void sends();
void send();

#endif