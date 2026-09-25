
#ifndef __nugpio__
#define __nugpio__
#define KEY1 PE2
#define KEY2 PA8
#define KEY3 PB6
#define KEY4 PB7
#define LED0 PB5
#include "M451Series.h"
extern void Input_init(void);
extern unsigned char Get_Key_Input(void);
extern   void Output_init(void);
extern   void gpio_int(void);
#endif //__CAN_H__

