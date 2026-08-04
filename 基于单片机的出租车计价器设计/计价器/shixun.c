#include<reg51.h>
#include"lcd1602.h"
#include"ds1302.h"
unsigned char time_add[7];
void LcdDisplay();
void keyscan();
uint n=0,s=0;
uint j=0,k=0;
uint t=0;
uint tx=0,ty=0,tz=0;
uint s18=0,n18=0,t18=0,tx18=0,ty18=0,tz18=0,j18=0,k18=0;
uint l=0;
uint lx=0,ly=0;
uint count;
sbit SW=P2^1;
sbit LED=P2^0;
sbit add_hour=P1^2;   
sbit add_min=P1^1;    
sbit add_sec=P1^0;    


void main()
{
	LcdInit();
	Ds1302Init();
	TMOD = 0x01;                    
    TL0 = 0X60;                    
    TH0 = 0XEA;                                        
    ET0 = 1;                                                
    count = 5400; 
	INT0 = 1;
    IT0 = 1;                   
    EX0 = 1;                    
    EA = 1;

	while(1)
	  {	
		Ds1302ReadTime(); 	
		LcdDisplay();	
		keyscan(); 
		if(SW==0)
		{
			TR0 = 1;
			EX0=0;			
			do
			{	
				
				lx=l/10;
				ly=l%10;
				Ds1302ReadTime();
				LcdWriteCom(0x80+0X40);
				LcdWriteData('T');			    	
				LcdWriteData(':');			    	
				LcdWriteData('0'+TIME[2]/16);		
				LcdWriteData('0'+(TIME[2]&0x0f));				 
				LcdWriteData('-');
				LcdWriteData('0'+TIME[1]/16);		
				LcdWriteData('0'+(TIME[1]&0x0f));	
				LcdWriteData('-');
				LcdWriteData('0'+TIME[0]/16);		
				LcdWriteData('0'+(TIME[0]&0x0f));
				LcdWriteCom (0x80);
		 		LcdWriteData('D');
		 		LcdWriteData(':');
				LcdWriteData('1');
		 		LcdWriteData('.');
		 		LcdWriteData('0');
		 		LcdWriteData(' ');
		 		LcdWriteData('S');
				LcdWriteData(':');		   
		 		LcdWriteData('0'+j);
		 		LcdWriteData('0'+k);
				LcdWriteData(' ');
				LcdWriteData('T');
				LcdWriteData(':');
				LcdWriteData(' ');
				LcdWriteData('0'+lx);
				LcdWriteData('0'+ly);
			
				
			}while(SW==0);
		
		}
		else
			{TR0=0;
			 EX0=1;
			}

	   }
	}

void keyscan()								
{
   Ds1302Write(0x8E,0x00);	                    
   if( add_sec==0)		                        
   { 
    time_add[0]=(TIME[0]>>4)*10+(TIME[0]&0x0F);
                                               
    time_add[0]=(time_add[0]+1)%60 ;  
    TIME[0]=(time_add[0]/10<<4)|(time_add[0]%10); 
                                               
    Ds1302Write(WRITE_RTC_ADDR[0], TIME[0]);	 
                                               
    }
	if( add_min==0)		                          
  { time_add[1]=(TIME[1]>>4)*10+(TIME[1]&0x0F); 
                                             
    time_add[1]=(time_add[1]+1)%60 ;   
    TIME[1]=(time_add[1]/10<<4)|(time_add[1]%10); 
                                              
    Ds1302Write(WRITE_RTC_ADDR[1], TIME[1]);	
                                             
   }
if( add_hour==0)		                   
  { 
    time_add[2]=(TIME[2]>>4)*10+(TIME[2]&0x0F); 			  
                                          
    time_add[2]=(time_add[2]+1)%24 ;  
    TIME[2]=(time_add[2]/10<<4)|(time_add[2]%10); 
                                      
    Ds1302Write(WRITE_RTC_ADDR[2], TIME[2]);
                                       
   }

}

void LcdDisplay()
{
	LcdWriteCom(0x80+0X40);
	LcdWriteData('T');			    	
	LcdWriteData(':');			    	
	LcdWriteData('0'+TIME[2]/16);		
	LcdWriteData('0'+(TIME[2]&0x0f));				 
	LcdWriteData('-');
	LcdWriteData('0'+TIME[1]/16);		
	LcdWriteData('0'+(TIME[1]&0x0f));	
	LcdWriteData('-');
	LcdWriteData('0'+TIME[0]/16);		
	LcdWriteData('0'+(TIME[0]&0x0f));
	LcdWriteData('W');
	LcdWriteData(':');
	LcdWriteData('0'+tz18);
	LcdWriteData('0'+tx18);
	LcdWriteData('.');
	LcdWriteData('0'+ty18);
	


    if(time_add[2]>=18&&time_add[2]<=24)
	{	
			 t18=1.8*s18*10;
		 	 tx18=t18/10; 
			 ty18=t18%10;
			 tz18=t18/100;
			 if(tx18>=10)
			 {
				tx18=t18%100/10;
			 }

			LcdWriteCom(0x80);
			LcdWriteData('D');
			LcdWriteData(':');
			LcdWriteData('1');
			LcdWriteData('.');
			LcdWriteData('8');
			LcdWriteData(' ');
			LcdWriteData('S');
			LcdWriteData(':');		   
			LcdWriteData('0'+j18);
			LcdWriteData('0'+k18);
			LcdWriteData('T');
		 	LcdWriteData(':');
		 	LcdWriteData('0'+tz);
			LcdWriteData('0'+tx);
		 	LcdWriteData('.');
			LcdWriteData('0'+ty);
		 


	}
	else
	{	 
		 
		 t=1.2*s*10;
		 tx=t/10;
		 ty=t%10;
		 tz=t/100;
		 if(tx>=10)
		 {
			tx=t%100/10;
		 }
		 
		 LcdWriteCom(0x80);
		 LcdWriteData('D');
		 LcdWriteData(':');
		 LcdWriteData('1');
		 LcdWriteData('.');
		 LcdWriteData('2');
		 LcdWriteData(' ');
		 LcdWriteData('S');
		 LcdWriteData(':');		   
		 LcdWriteData('0'+j);
		 LcdWriteData('0'+k);
		 LcdWriteData('T');
		 LcdWriteData(':');
		 LcdWriteData('0'+tz);
		 LcdWriteData('0'+tx);
		 LcdWriteData('.');
		 LcdWriteData('0'+ty);
		 	
	}		 
}
void INT0_ISR() interrupt 0
{

	LED=!LED;
	n++;
	s=(n*2)/10;
	j=s/10;
	k=s%10;
	if(time_add[2]>=18&&time_add[2]<=24)
	{
		n18++;
		s18=(n18*2)/10;
		j18=s18/10;
		k18=s18%10;

	}
	
}
void tm0_isr() interrupt 1 
{
    TL0 =0x60;                     
    TH0 =0xea;                
     if (count-- == 0)               
    {
        count =5400;	
		l++;	
    }
}

		 		 