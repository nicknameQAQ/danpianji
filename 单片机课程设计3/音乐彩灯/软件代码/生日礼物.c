#include<reg52.h>
#include<present.h>

sbit Buzz=P3^7;

uchar flag=16,sign=1;
uchar time=0; 		//流水灯定时
uint *pf=Happy_Birthday;
uint *p=Happy_Birthday;

void init_devices(void)
{
	TMOD=0x11;
	TH1=0x71;
	TL1=0x48;	//40ms的定时
	TH0=0x00;
	TL0=0x00;	//初始化
	IT1=1;
	EA=1;
	EX1=1;
	ET0=1;
	ET1=1;	
}

void int1() interrupt 2 using 0
{
	sign++;	//下一曲
	switch(sign)
	{	case 1:TR0=1;pf=Happy_Birthday;flag=16;break;
		case 2:pf=Legend;flag=8;break;
		case 3:pf=Childhood;flag=8;break;
		case 4:TR0=0;sign=0;Buzz=1;break;//停唱Buzz=1,三极管关断，防止大电流
		default :pf=Happy_Birthday;flag=16;break;	
	}
    p=pf;
}

//定时器0产生频率
void timer0() interrupt 1 using 1
{
	uint temp=65536-*p;
	TH0=(uchar)(temp>>8);
	TL0=(uchar)temp; 
	if(*p) Buzz^=1;
	else Buzz=1;	//Buzz=1,三极管关断，防止大电流

}

//定时器1产生40ms延时
void timer1() interrupt 3 using 2
{		
	TH1=0x71;
	TL1=0x48;	//40ms的定时
		  
	if(flag==0)
	{	
		flag=*(p+1);
		if(flag==0) 
		{
			p=pf;
			flag=*(p+1);
		}
		else 
		{	p++;
			p++;
		}
	}
	else flag--; 
	time++;		//流水灯定时
	if(time%5==0)
	{  switch(time)
	   {case 5:P0=0x01;break;
	    case 10:P0=0x02;break;
	  	case 15:P0=0x04;break;
		case 20:P0=0x08;break;
	    case 25:P0=0x10;break;
	  	case 30:P0=0x20;break;
		case 35:P0=0x40;break;
		case 40:P0=0x80;time=0;break;
		default :time=0;break;	   
	   }
	}
}

void delay(uint i)
{	
	while(i--);
}
void word(uchar *ps)	 //显示单个字符
{	uchar i,j;
	uchar _bit=0X80;
	for(i=0;i<200;i++)
	{		
		for(j=0;j<8;j++)
		{  P1=0xff;
		   P2=~_bit;
		   P1=*(ps+j);
		   delay(100);
		   _bit>>=1;
		}
		_bit=0X80;
	}
}
void display(uchar *ps)	 //显示移动字符
{	uchar i,j;
	uchar _bit=0X80;
	while(*(ps+8)!=0xaa)
	{		
		for(i=0;i<20;i++)
		{		
				for(j=0;j<8;j++)
				{  P1=0xff;
				   P2=~_bit;
				   P1=*(ps+j);
				   delay(100);
				   _bit>>=1;
				}
			_bit=0x80;
		}
		ps++;
	}
}

void main()
{	uchar i;
	init_devices();
	for(i=0;i<6;i++) word(figure[i]);	
	word(arrow);
	word(arrow);
	TR0=1;
	TR1=1;
	while(1)
	{
		display(str1);
		display(str1);	
		display(str2);
	}
}