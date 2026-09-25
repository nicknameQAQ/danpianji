#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f4xx.h" 

u8 iskeyinput(void);
u8 key_identify(void);
void wait_key_release(u8 x);
u8 key_Scan(u8 x);
void key_init(void);

extern void seg_disp1(u8 x);
extern void seg_disp2(void);

extern int8_t f;
	
#define KEYPORT GPIO_ReadInputData(GPIOE)

#define line0 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)
#define line1 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)
#define line2 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)
#define line3 GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)

#endif
