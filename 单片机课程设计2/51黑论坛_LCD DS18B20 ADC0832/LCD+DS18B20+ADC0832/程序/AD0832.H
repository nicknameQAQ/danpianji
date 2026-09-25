#ifndef AD0832_h
#define AD0832_h
#include<reg52.h>
sbit ADCS=P1^0;
sbit ADCLK=P1^2;
sbit ADDI=P1^1;
sbit ADDO=P1^1;
extern unsigned char ReadAdc0832(unsigned char channel);
#endif