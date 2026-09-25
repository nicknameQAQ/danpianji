#include<reg51.h>
#define uchar unsigned char
#define uint unsigned  int
sbit sw1=P3^0;
sbit sw2=P3^1;
sbit sw3=P3^2;
sbit key_ew=P3^3;
sbit key_sn=P3^4;
sbit key0=P3^5;
sbit BB=P3^6;
#include <REGX51.H>
uchar TH,TL; 
uchar SN=23,EW=20,NN=56;
uint tt,ii,jj,kk,GG;
uchar code DisCode[]
={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
uchar Disbuff0[2];
uchar Disbuff1[2];
void Delay(uint x)//MS延时
{
 	uchar i;
	while(x--)for(i=0;i<123;i++);
}  
void init()
{
 TMOD=0x01;	 
 TH=(65536-4000)/256,
 TL=(65536-4000)%256;
 TH0=TH;
 TL0=TL;
 EA=1;
 ET0=1;
 TR0=1;
}
void fenli0()
{
 Disbuff0[0]=SN/10;
 Disbuff0[1]=SN%10;
}
void fenli1()
{
  Disbuff1[0]=EW/10;
  Disbuff1[1]=EW%10;
}

void Display()
{
   uchar ii;
   if(ii==0)
   {   
     P0=0x80;
	 P2=0x80;
     P2=DisCode[Disbuff0[0]]|0x80;
	 P0=DisCode[Disbuff1[0]]&0x7f;
	}
	 else
	 { 
	  P0=0x80;
	  P2=0x80;
	  P2=DisCode[Disbuff0[1]]&0x7f;
	  P0=DisCode[Disbuff1[1]]|0x80;
	 }								 
	 ii=~ii;
}
void fuzhi()//赋值
{
  if(NN==56)
  {
    SN=23;
	EW=20;
	P1=0x5E;
  }

  if(EW==0&&NN==36)
  {
    EW=3;
	P1=0x6E;
  }
  if(NN==33)
   {
    SN=30;
	EW=33;
	P1=0xB3;
   }
   if(SN==0&&NN==3)
   {
     SN=3;
	 P1=0xB5;
   }
}
void LED()//LED
{
  if(NN<=39&&NN>36)//
   {
    if(kk)
	{
      P1=0x5E;
	  BB=0;
	}
    else 	  
      P1=0xfE;
	   BB=0;
	   while(NN<=36);
	   	BB=1;
	}
	
    
   if(NN<=36&&NN>33)
    {
	 if(kk)
	 {
	  P1=0x6E;
	  BB=0;
	 }
	 else 
	 P1=0xFE;
	  BB=0;
	   while(NN<=33);
	   	BB=1;
	   
	 
	}
   if(NN<=6&&NN>3)
   {
    if(kk)
	{
      P1=0xB3;
	  BB=0;
	}
	else  
	P1=0xF7;
	BB=0;
	   while(NN<=3);
	   	BB=1;
	 

   }
   if(NN<=3)
    {
	 if(kk)
	 {
	  P1=0xB5;
	  BB=0;
	 }
	 else  
	  P1=0xF7;
	  BB=0;
	   while(NN<=0);
	   	BB=1;
	    
	  
	}
}

void qiangzhi_sw()//强制通行
{	  
    if(key_ew==0)
	  {
	    TR0=0;
		P0=0x3f;
		P2=0x3f;
		P1=0x5e;
		while(!key_ew);		 
	  	TR0=1;
	  }
	 if(key_sn==0)
	  {
	    TR0=0;
		P0=0x3f;
		P2=0x3f;
		P1=0xb3;
		while(!key_sn);		 
	  	TR0=1;
	  }
	  if(key0==0)
	  {
	    TR0=0;
		P0=0x3f;
		P2=0x3f;
		P1=0xB6;
		while(!key0);		 
	  	TR0=1;
	  }
}
void key1()//调时
{
   uchar m=0,num=0,n=0;
   uchar sw11,sw22=1,sw33=1;
 	if(sw1==0)
	 {  
	   Delay(10);
	   if(sw1==0)
	    {  		  
		  while(!sw1);
		     num++;
		     TR0=0;
		  while(!m)
		   {  
		      if(n==1)
			    { 
				   if(sw11==1&&sw1==0)
					{
					   Delay(10);  
			 		 if(sw11==1&&sw1==0)
					  {
						num++;
					  }
					}
				   if(num==3)
				    {
				       while(!sw1);
				    } 
				   sw11=sw1;	 	   
			    } 	
				n=1; 	 
				Delay(3); 
			   switch (num)
			    {
				  
				  case 1:
				         {						     												
								 if(sw22==1&&sw2==0)
								   {
								     SN++;
									 EW++;
									 if(EW==100)
									 {
									  EW=5;
									  SN=0;
									 }
									 if(SN==100)
									 {
									  SN=5;
									  EW=0;
									 }
									 fenli0();
									 fenli1();
								   }	
								  sw22=sw2;
								  if(sw33==1&&sw3==0)
								  {
								    SN--;
									EW--;
								    if(EW==255)
									{
								      EW=94;
									  SN=99;
									}
								    if(SN==255)
									{
								     SN=94;
									 EW=99;
									}
								    fenli0();
								    fenli1();
								  }
								  sw33=sw3;
								  Display();	
					                 
						  } break;
				case 2:
				         {
						    TR0=1;
						    m=1;
							while(!sw1);
							Delay(5);
							while(!sw1);											   
						 } break;
								
			}				 
		  }		  			   
	   }			 
  	}	
}

void main()
{
 init();
 fenli0();
 fenli1();
 while(1)
 {
  qiangzhi_sw(); 
  key1();
 }

}
void timer0() interrupt 1
 {
  TH0=TH;
  TL0=TL;
  tt++;
  jj++;
 if(tt==250)
  {
    tt=0;

	 fuzhi();
	 EW--;
     SN--;
	 if(SN==30)
	   {   
		NN=57;   
	   }
	 if(EW==33)
	   {   
		NN=33;   
	   }
  
   if(EW<31||SN<34)
   {
	 
     NN--;
	   if(NN==0)
	   NN=56;
   }
  
   fenli0();
   fenli1();    
  }	 
  Display();
  LED();
  if(jj==50)
  {
    jj=0;
	GG=~GG; 
    kk=~kk; 
   
  }
 }	   
