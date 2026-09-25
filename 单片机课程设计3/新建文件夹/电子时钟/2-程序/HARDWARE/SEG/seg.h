#ifndef __SEG_H
#define __SEG_H
#include "stm32f4xx.h" 


#define de 100

extern u8 h,m,s;

extern u8 date[8];
extern u16 address[8];
extern u8 exit_flag;
extern u8 key_Scan(u8 x);

void gpio_init(void);
void seg_disp1(u8 x);
void seg_disp2(void);
void seg_chag(void);

	 				    
#endif
