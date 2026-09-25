#ifndef _TIMER_H
#define _TIMER_H
#include "stm32f4xx.h" 


extern u8 h,m,s,bs;
extern u8 date[8];


void TIM3_Int_Init(u16 arr,u16 psc);
void get_time(void);
#endif
