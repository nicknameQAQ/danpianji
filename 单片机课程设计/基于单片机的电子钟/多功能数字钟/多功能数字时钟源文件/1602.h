#ifndef __1602_H__
#define __1602_H__
void writelcd_cmd(unsigned char cmd);
void writelcd_dat(unsigned char dat);
void lcd_init();
void delay(unsigned int t);
#endif