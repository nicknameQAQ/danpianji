#ifndef __EXTI_H
#define __EXIT_H	 
#include "stm32f4xx.h" 

void exti_init(void);	//外部中断初始化	

extern u8 h,m,s;
extern void seg_disp1(u8 x);
extern void seg_disp2(void);
extern u8 date[8];

#endif
