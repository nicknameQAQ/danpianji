#include <reg52.h>
#include "delay.h"

unsigned char keynum(void)
{
	 static unsigned char key;
	 P3=0x7f;
	 DelayMS(10);
	 if(P1!=0xff)
	 {
	   key=1;
	 }
	 else
	 {
	   P3=0xbf;
		 DelayMS(10);
		 if(P1!=0xff)
	   {
	     key=2;
	   }
		 else
		 {
		   P3=0xdf;
			 DelayMS(10);
			 if(P1!=0xff)
			 {
				 key=3;
			 }
			 else key=0;
		 }
	 }
	 switch(P1)
	 {
		 case 0xfe : key=key*10+1;break;
		 case 0xfd : key=key*10+2;break;
		 case 0xfb : key=key*10+3;break;
		 case 0xf7 : key=key*10+4;break;
		 case 0xef : key=key*10+5;break;
		 case 0xdf : key=key*10+6;break;
		 case 0xbf : key=key*10+7;break;
		 case 0x7f : key=key*10+8;break;
		 default : break;
	 }
	 return key;
}
