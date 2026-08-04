#ifndef __PCF8591_H
#define __PCF8591_H

#include <reg52.h>
#define uchar unsigned char
#define  PCF8591 0x90    //PCF8591 µÿ÷∑  
void start();
void stop();
void respons();
void init();
void write_byte(uchar date);
void write_add(uchar control,uchar date);
#endif
