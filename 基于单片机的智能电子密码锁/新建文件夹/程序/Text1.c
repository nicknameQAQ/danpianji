 #include<reg51.h>
  unsigned char lie[]={0xff,0x07,0x0b,0x0d};
 unsigned char L,L1=0,L2,L3=0,L4,L5,L6,L7,m,n,a,s,i;
 unsigned char shu[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
  sbit P25=P1^5;
 

  void delay();
	
	 void delay()
	 {
	 unsigned char i;
	 for(i=0;i<200;i++);
	 }

  void main()
 { 
 while(1)
 { 	
 P2=0X00;	
 P3=0xff;
 if(P3!=0xff)
 {
 delay();
 L=0;  L2=0;P25=1;
 }
 else
 {
 L=1; 
 }
 

 if(L==0)
 {	 	 
    for(i=1;i<4;i++)
	{
	delay(); delay(); delay(); 
	
	  P2=lie[i];
	if(P3!=0xff)
	{
	  
	 		 
	 

	if(L1==0)
	{if(P3==0xfe){m=0+i;}
	if(P3==0XFd){m=3+i;}
	if(P3==0xfb){m=6+i;}
	}
	if(L1==1)	 
	{
	if(P3==0xfe){n=0+i;}
	if(P3==0xfd){n=3+i;}
	if(P3==0xfb){n=6+i;}
	}
	if(L1==2)
	{	
	if(P3==0xfe){a=0+i;}
	if(P3==0xfd){a=3+i;}
	if(P3==0xfb){a=6+i;}
	 }
	  if(L1==3)
	  {
	  if(P3==0xfe){s=0+i;}
	if(P3==0xfd){s=3+i;}
	if(P3==0xfb){s=6+i;}
	  }	   
	  while(P3!=0Xff);
	  L1++;
	  if(L1>3){L1=0;}

	  if(m==8){a=s=m=n=L1=0;L2=0;}
	  if(n==8){m=n=a=s=L1=0;L2=0;}
	  if(a==8){m=n=a=s=L1=0;L2=0;}
	  if(s==8){m=n=a=s=L1=0;L2=0;}						   
	 
	   if(m==9){L2=1;a=s=m=n=L1=0;}
	   if(n==9){L2=1;a=s=m=n=L1=0;}
	   if(a==9){L2=1;a=s=m=n=L1=0;}
	   if(s==9){L2=1;a=s=m=n=L1=0;}
	 
	  }
	  }
	  }
		



	   if(m>0)
	   {
	   P2=0x80;
	   P0=shu[m];
	   delay();
	   
	   }
		else
		{
		P2=0X80;
		
		P0=0Xbf;
		delay();
		
		}
		if(n>0)
		{
		P2=0X40;
		P0=shu[n];
		delay();
		
		}
		else
		{
		P2=0X40;
		P0=0xbf;
		delay();
	
		}
		if(a>0)
		{
		P2=0X20;
		P0=shu[a];
		delay();
	
		}
		else
		{
		P2=0X20;
		P0=0Xbf;
		delay();
		
		}
		if(s>0)
		{
		P2=0x10;
		P0=shu[s];
		delay();
	
		}
		else
		{
		P2=0X10;
		P0=0Xbf;
		delay();
	
		}
	   if(L2==0){L7=L6=L5=L4=L3=0;}
	   if(m==2)
	   { L4=L3+1;}
	   if(n==5)
	   { L5=L4+1;}
	   if(a==6)
	   { L6=L5+1;}
	   if(s==5)
	   { L7=L6+1; }
	   	
	   if(L7==4) 
	   { 
	   if(L2==1) 
	    {P25=0;} } 
	   }
	   }
		





  
