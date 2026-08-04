#ifndef __DS1302_h_
#define __DS1302_h_

#include <reg52.h>
#include <intrins.h>

sbit DSIO=P2^1;			//∂®“Âds1302
sbit RST=P2^2;
sbit SCLK=P2^0;

extern unsigned char code READ_RTC_ADDR[7];
extern unsigned char code WRITE_RTC_ADDR[7];
extern unsigned char Time_1[7];

void Ds1302Write(unsigned char addr, unsigned char dat);
unsigned char Ds1302Read(unsigned char addr);
void Ds1302Init();
void Ds1302ReadTime();

#endif