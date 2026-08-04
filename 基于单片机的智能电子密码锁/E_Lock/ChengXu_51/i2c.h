#ifndef __I2C_H_
#define __I2C_H_

#include<reg51.h>

sbit SCL=P3^6;
sbit SDA=P3^7;

void I2cStart();
void I2cStop();
unsigned char I2cSendByte(unsigned char dat);
unsigned char I2cReadByte();
void I2cReadRespon();
#endif
