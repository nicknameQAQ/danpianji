#include"xunji.h"
#include"1602.h"
#include"pwm.h"


sbit HZ = P3^6;
sbit HY = P3^7;

sbit Z1 = P1^2;
sbit Z2 = P1^3;
sbit Y1 = P1^4;
sbit Y2 = P1^5;

sbit Buzz=P1^7;

void go (uchar S)
{ 		    
	Pwm_set1(S);
	Pwm_set2(S);
	Z1=1;
	Z2=0;
	Y1=1;
	Y2=0;
  Buzz=1;
}

void back (uchar S)
{	    	
	Pwm_set1(S);
	Pwm_set2(S);
	Z1=0;
	Z2=1;
	Y1=0;
	Y2=1; 
	Buzz=1;
}	

void right(void)
{
	Pwm_set1(80);
	Pwm_set2(30);
	Z1=1;
	Z2=0;
	Y1=1;
	Y2=0;	
	Buzz=0;
}
void left(void)
{
	Pwm_set1(30);
	Pwm_set2(80);
	Z1=1;
	Z2=0;
	Y1=1;
	Y2=0;
  Buzz=0;
}
void stop(void)
{
	Pwm_set1(0);
	Pwm_set2(0);
	Z1=1;
	Z2=1;
	Y1=1;
	Y2=1;
Buzz=0;
}




void xunji(uchar vx)   
{

	if(type==0)
	{

	if( (HZ==0)&&(HY==0) )
      {
		  go(vx);
			byte=0;
		  }
	else if ( (HZ==1)&&(HY==0) )
		  {
		  left( );	
      byte++;
			if(byte>1)
{
	byte=2;
}	
	
		  }
	else if ( (HZ==0)&&(HY==1) )
		  {
		  right();
	    byte++;
if(byte>1)
{
	byte=2;
}
		
		  }	
	else if ( (HZ==1)&&(HY==1) )
		  {
		   stop();
       
		  }		 			
	}
	
	
	if(type==1)
	{
	
		if( (HZ==0)&&(HY==0) )
    {
		  back(vx);
			byte=0;
						
		  }
	else if ( (HZ==1)&&(HY==0) )
		  {
		  	right();
       byte++;
			if(byte>1)
{
	byte=2;
}

		  }
	else if ( (HZ==0)&&(HY==1) )
		  {
		  left( );
			byte++;	
if(byte>1)
{
	byte=2;
}
		  }	
	else if ( (HZ==1)&&(HY==1) )
		  {
		   stop();		  
		  }		
	}
	
			
}
