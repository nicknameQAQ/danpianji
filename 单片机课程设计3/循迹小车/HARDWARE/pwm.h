#ifndef  __PWM_H__
#define  __PWM_H__

#define uchar unsigned char
#define uint unsigned int

extern uchar high_time1,nums,type,byte;

extern uint zkb1,zkb2;
void Timer0Init(void);
void Timer1Init(void);
		 
void Pwm_set1(uint zkb);
void Pwm_set2(uint zkb);


#endif