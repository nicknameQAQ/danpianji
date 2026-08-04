#ifndef __KEYSCAN_H
#define __KEYSCAN_H
#include <reg52.h>
#define uchar unsigned char

sbit Key1=P2^3;  //频率增大
sbit Key2=P2^4;  //频率减小
sbit Key3=P2^5;  //幅度增大
sbit Key4=P2^6;  //幅度减小
sbit Key5=P2^7;

void keyscan(void);

#endif
