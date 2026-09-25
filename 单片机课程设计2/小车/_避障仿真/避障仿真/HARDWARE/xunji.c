#include"xunji.h"
#include"1602.h"
#include"pwm.h"


sbit HZ = P3^6;
sbit HY = P3^7;

sbit Z1 = P1^2;
sbit Z2 = P1^3;
sbit Y1 = P1^4;
sbit Y2 = P1^5;


void go (uchar S)
{ 		    
	Pwm_set1(S);
	Pwm_set2(S);
	Z1=1;
	Z2=0;
	Y1=1;
	Y2=0;
    LCD_Str(5,1,"GO !  ");
}
//void back(void)
//{	    	
//	Pwm_set1(80);
//	Pwm_set2(80);
//	Z1=0;
//	Z2=1;
//	Y1=0;
//	Y2=1;
//	LCD_Str(5,1,"BACK !");
//}	
void right(void)
{
	Pwm_set1(80);
	Pwm_set2(30);
	Z1=1;
	Z2=0;
	Y1=1;
	Y2=0;
	LCD_Str(5,1,"TURN R");
}
void left(void)
{
	Pwm_set1(30);
	Pwm_set2(80);
	Z1=1;
	Z2=0;
	Y1=1;
	Y2=0;
	LCD_Str(5,1,"TURN L");
}
void stop(void)
{
	Pwm_set1(0);
	Pwm_set2(0);
	Z1=1;
	Z2=1;
	Y1=1;
	Y2=1;
	LCD_Str(5,1,"stop  ");
	LCD_Str(0,0,"***smart car***");	
}
void xunji(uchar vx)   
{
	if( (HZ==0)&&(HY==0) )
          {
		  go(vx); 
		  }
	else if ( (HZ==1)&&(HY==0) )
		  {
		  left( );		  
		  }
	else if ( (HZ==0)&&(HY==1) )
		  {
		  right();		  
		  }	
	else if ( (HZ==1)&&(HY==1) )
		  {
		   stop();		  
		  }		 					
}
