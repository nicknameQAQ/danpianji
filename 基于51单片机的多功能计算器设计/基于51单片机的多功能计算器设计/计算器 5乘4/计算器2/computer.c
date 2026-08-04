#include<reg52.h>
#include<intrins.h>
#include<zm.h>
sbit en=P3^0;
sbit rw=P3^1;
sbit rs=P3^2;
sbit cs2=P3^3;
sbit cs1=P3^4;
sbit rst=P3^5;
uchar key,wei=4,hang=0,k=0;
unsigned long a1=0,a2=0,count=0;
void wc(uchar cmd)
{
	_nop_();
	rs=0;
	rw=0;
	en=1;
	P2=cmd;
	_nop_();
	en=0;
}
void wd(uchar dat)
{
	_nop_();
	rs=1;
	rw=0;
	en=1;
	P2=dat;
	_nop_();
	en=0;
}
void delay(uchar ms)
{
	uchar i,j;
	for(i=ms;i>0;i--)
		for(j=113;j>0;j--);
}
void set(uchar line,uchar col)
{
	line=line|0xb8;
	col=col|0x40;
	wc(col);
	wc(line);
}
void clean()
{
	uchar i,j,k=0;
	rst=0;
	delay(10);
	rst=1;
	wc(0xc0);//设置显示起始行11xxxxxx
	wc(0x3f);//00111111显示开，00111110显示关
	cs1=0,cs2=0;
	set(0,0);
	for(i=0;i<8;i++)
		{for(j=0;j<64;j++)
			wd(0x00);
			k++;
			set(k,0);
			}
	a1=0;a2=0;count=0;k=0;
	wei=4;
	hang=0;
}
void write(uchar zi)
{
	uchar i;
	if(wei<63)
	{
	cs1=0,cs2=1;
	set(hang,wei);
	for(i=0;i<6;i++)
		wd(nAsciiDot[zi++]);
	wei=wei+6;
	if(wei>59)
	{cs1=1;
	cs2=0;}
	if(hang>7)
	hang=0;
	}
	else
	{
	set(hang,wei-64);
	for(i=0;i<6;i++)
		wd(nAsciiDot[zi++]);
	wei=wei+6;
	if(wei>123)
	{wei=4;
	hang++;
	cs1=0;
	cs2=1;}
	if(hang>7)
	hang=0;
	}
}
void jis()
{
	switch(k)
	{
		case 0:
			break;
		case 1:
			count=a2*a1;a1=count;break;
		case 2:
			count=a2+a1;a1=count;break;	
		case 3:
			count=a2-a1;a1=count;break;
		case 4:
			count=a2/a1;a1=count;break;
		default:
			break;
	}
}
			
void rel()
{	
	uchar a=P1;
	write(key);
	while((a&0x0f)!=0x0f)
	{
		a=P1;
	}	
	switch(key/6)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			a1=a1*10+key/6;break;
		case 10:
			jis();a2=a1;a1=0;k=1;break;
		case 11:
			jis();a2=a1;a1=0;k=2;break;
		case 12:
			jis();a2=a1;a1=0;k=3;break;
		case 13:
			jis();,                                                                                                                        a2=a1;a1=0;k=4;break;
		case 14:
			{
				switch(k)
				{
					case 1:
						count=a2*a1;break;
					case 2:
						count=a2+a1;break;	
					case 3:
						count=a2-a1;break;
					case 4:
						count=a2/a1;break;
					default:
						break;
				}
				if(count>999999999)
					write(90);
				else
					if(count>99999999)
						{
						 write(count/100000000*6);
						 write(count%100000000/10000000*6);
						 write(count%10000000/1000000*6);
						 write(count%1000000/100000*6);
						 write(count%100000/10000*6);
						 write(count%10000/1000*6);
						 write(count%1000/100*6);
						 write(count%100/10*6);
						 write(count%10*6);
						 }
					else
					if(count>9999999)
						{
						 write(count%100000000/10000000*6);
						 write(count%10000000/1000000*6);
						 write(count%1000000/100000*6);
						 write(count%100000/10000*6);
						 write(count%10000/1000*6);
						 write(count%1000/100*6);
						 write(count%100/10*6);
						 write(count%10*6);
						 }
						 else
					if(count>999999)
						{
						 write(count%10000000/1000000*6);
						 write(count%1000000/100000*6);
						 write(count%100000/10000*6);
						 write(count%10000/1000*6);
						 write(count%1000/100*6);
						 write(count%100/10*6);
						 write(count%10*6);
						 }
						 else
					if(count>99999)
						{
						 write(count%1000000/100000*6);
						 write(count%100000/10000*6);
						 write(count%10000/1000*6);
						 write(count%1000/100*6);
						 write(count%100/10*6);
						 write(count%10*6);
						 }
						 else
					if(count>9999)
						{

						 write(count%100000/10000*6);
						 write(count%10000/1000*6);
						 write(count%1000/100*6);
						 write(count%100/10*6);
						 write(count%10*6);
						 }
						 else
					if(count>999)
						{
						 write(count%10000/1000*6);
						 write(count%1000/100*6);
						 write(count%100/10*6);
						 write(count%10*6);
						 }
						 else
					if(count>99)
						{
						 write(count%1000/100*6);
						 write(count%100/10*6);
						 write(count%10*6);
						 }
						 else
					if(count>9)
						{
						 write(count%100/10*6);
						 write(count%10*6);
						 }
						 else
						{
						 write(count%10*6);
						 }
					a1=count;k=0;
			}break;			
		default:
			break;
	}
}

void keyscan()
{
	uchar a;
	P1=0xef;
	a=P1;
	a=a&0x0f;
	switch(a)
	{case 7:
		key=90;clean();break;
	case 11:
		key=0;rel();break;
	case 13:
		key=84;rel();break;
	case 14:
		key=66;rel();break;
	default:
		break;}
	
	P1=0xdf;
	a=P1;
	a=a&0x0f;
	switch(a)
	{case 7:
		key=6;rel();break;
	case 11:
		key=12;rel();break;
	case 13:
		key=18;rel();break;
	case 14:
		key=72;rel();break;
	default:
		break;}
	
	P1=0xbf;
	a=P1;
	a=a&0x0f;
	switch(a)
	{case 7:
		key=24;rel();break;
	case 11:
		key=30;rel();break;
	case 13:
		key=36;rel();break;
	case 14:
		key=60;rel();break;
	default:
		break;}
	
	P1=0x7f;
	a=P1;
	a=a&0x0f;
	switch(a)
	{case 7:
		key=42;rel();break;
	case 11:
		key=48;rel();break;
	case 13:
		key=54;rel();break;
	case 14:
		key=78;rel();break;
	default:
		break;}
}
void main()
{
	cs1=0;
	cs2=0;
	clean();
	cs1=0;
	while(1)
	{keyscan();
	delay(2);
	}
	}
	