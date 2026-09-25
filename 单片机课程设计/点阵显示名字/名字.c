#include<reg51.h>
void delay(unsigned int time)
{
	unsigned int count;
	for(;time>0;time--)
		for(count=125;count>0;count--)	;
			
}

void main(void)
{
	unsigned char p1[] ={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};//选通位
	
	unsigned char p00[]=       //姓氏（取模器取模）
	unsigned char p20[]=
	
	unsigned char p01[]=     //名字（取模器取模）
	unsigned char p21[]=

	unsigned char p02[]=    //名字
	unsigned char p22[]=

	unsigned char a,b;
	while(1)
	{
		for(b=0;b<5;b++)
		{
			for(a=0;a<16;a++)
			{
				P1=p1[a]; //R0选通
				P0=p00[a];//姓氏
				P2=p20[a];
				delay(1);
				
			}
			P0=0xff;//消影
			P2=0xff;
		}

		

		delay(500);
		for(b=0;b<5;b++)
		{
			for(a=0;a<16;a++)
			{
				P1=p1[a]; //R0选通
				P0=p01[a];//名
				P2=p21[a];
				delay(1);
				
			}
			P0=0xff;//消影
			P2=0xff;
		}
		delay(500);
		
		for(b=0;b<5;b++)
		{
			for(a=0;a<16;a++)
			{
				P1=p1[a]; //R0选通
				P0=p02[a];//名
				P2=p22[a];
				delay(1);
				
			}
			P0=0xff;//消影
			P2=0xff;
		}
		delay(500);
		


		
	}
}