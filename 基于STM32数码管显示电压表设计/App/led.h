#ifndef _led_H
#define _led_H

#include "stm32f10x.h"

/*  LED时钟端口、引脚定义 */
#define LED_PORT 			GPIOB  
#define LED_PIN 			GPIO_Pin_12|GPIO_Pin_13
#define LED_PORT_RCC		RCC_APB2Periph_GPIOB

#define led1          PBout(12)
#define led2          PBout(13)

void LED_Init(void);


#endif
