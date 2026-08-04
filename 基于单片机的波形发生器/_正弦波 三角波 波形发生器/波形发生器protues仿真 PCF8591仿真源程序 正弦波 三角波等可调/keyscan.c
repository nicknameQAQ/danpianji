  
#include "keyscan.h"
#include "PCF8591.h"
#include "delay.h"
extern uchar time1;
extern uchar range; 
 
 void keyscan(void)
 {
   if(Key1==0)
	{
		time1+=5;
		while(!Key1);
	}
	if(Key2==0)
	{
		time1-=5;
		while(!Key2);
	}
	if(Key3==0)
	{
		range+=5;
		while(!Key3);
	}
	if(Key4==0)
	{
		range-=5;
		while(!Key4);
	}

 }
