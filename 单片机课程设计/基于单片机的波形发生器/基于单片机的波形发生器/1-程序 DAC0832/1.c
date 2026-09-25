#include<reg52.h>					   //包含头文件
#include<intrins.h>
#define uchar unsigned char			   //宏定义
#define uint unsigned int

sbit s1=P3^5; 						   //定义按键的接口
sbit s2=P3^6;
sbit s3=P3^7;
sbit s4=P3^4;

sbit led0=P3^0;
sbit led1=P3^1;
sbit led2=P3^2;
sbit led3=P3^3;
									   
sbit lcdrs=P2^7;					   //液晶控制位
sbit lcden=P2^6;

char num,boxing,u;					   //定义全局变量
int pinlv=100,bujin=1,bujin1=1;
uchar code table[]="0123456789";		//定义显示的数组
uchar code table1[]="Fout=     Wave form:";
unsigned long int m;
int a,b,h,num1;

//自定义字符
uchar code zifu[]={
						0x0e,0x11,0x11,0x00,0x00,0x00,0x00,0x00,
						0x00,0x00,0x00,0x00,0x11,0x11,0x0e,0x00,	  //正弦波	   0  1

						0x00,0x07,0x04,0x04,0x04,0x04,0x1c,0x00,
						0x00,0x1c,0x04,0x04,0x04,0x04,0x07,0x00,	  //矩形波	   2  3

						0x00,0x01,0x02,0x04,0x08,0x10,0x00,0x00,
						0x00,0x10,0x08,0x04,0x02,0x01,0x00,0x00,	  //三角波	   4  5

						0x00,0x01,0x03,0x05,0x09,0x11,0x00,0x00,  	  //锯齿波	   6
};
uchar code sin[64]={
135,145,158,167,176,188,199,209,218,226,234,240,245,249,252,254,254,253,251,247,243,237,230,222,213,204,193,182,170,158,
146,133,121,108,96,84,72,61,50,41,32,24,17,11,7,3,1,0,0,2,5,9,14,20,28,36,45,55,66,78,90,102,114,128
};					  //正弦波取码
uchar code juxing[64]={
255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
255,255,255,255,255,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};					  //矩形波取码

uchar code sanjiao[64]={
0,8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128,136,144,152,160,168,176,184,192,200,208,216,224,232,240,248,
248,240,232,224,216,208,200,192,184,176,168,160,152,144,136,128,120,112,104,96,88,80,72,64,56,48,40,32,24,16,8,0
};						//三角波取码
uchar code juchi[64]={
0,4,8,12,16,20,24,28,32,36,40,45,49,53,57,61,65,69,73,77,81,85,89,93,97,101,105,109,113,117,121,125,130,134,138,142,
146,150,154,158,162,166,170,174,178,182,186,190,194,198,202,206,210,215,219,223,227,231,235,239,243,247,251,255
};					  //锯齿波取码

void delay(uint xms)	 //延时函数
{
	int a,b;
	for(a=xms;a>0;a--)
		for(b=110;b>0;b--);
}
void write_com(uchar com)		//写命令函数
{
	lcdrs=0;
	P0=com;	
	delay(1);
	lcden=1;
	delay(1);						 			  
	lcden=0;
}

void write_date(uchar date)		//写数据函数
{
	lcdrs=1;
	P0=date;
	delay(1);
	lcden=1;
	delay(1);
	lcden=0;
}

//自定义字符集
void Lcd_ram()      
{ 
	uint i,j,k=0,temp=0x40; 
	for(i=0;i<7;i++)
	{
	   for(j=0;j<8;j++)
	   {
	    write_com(temp+j);
	    write_date(zifu[k]);
	    k++;
	   }
	   temp=temp+8;
	}
}

void init_lcd()					//初始化函数
{
	uchar i;								  
	lcden=0;					//默认开始状态为关使能端，见时序图  
	Lcd_ram();
	write_com(0x0f);
	write_com(0x38);			//显示模式设置，默认为0x38,不用变。
	write_com(0x01);			//显示清屏，将上次的内容清除，默认为0x01. 
	write_com(0x0c);			//显示功能设置0x0f为开显示，显示光标，光标闪烁；0x0c为开显示，不显光标，光标不闪
	write_com(0x06);			//设置光标状态默认0x06,为读一个字符光标加1. 
	write_com(0x80);			//设置初始化数据指针，是在读指令的操作里进行的
	for(i=10;i<20;i++)			   //显示初始化
	{
		write_date(table1[i]);
	}
	write_com(0x80+0x40);
	for(i=0;i<9;i++)
	{
		write_date(table1[i]);
	}
	write_com(0x80+10);
	write_date(0);
	write_date(1);
	write_date(0);
	write_date(1);
	write_date(0);
	write_date(1);
	write_com(0x80+0x40+0x09);
	write_date(' ');
	write_date('1');
	write_date('0');
	write_date('.');
	write_date('0');
	write_date('H');
	write_date('z');
}
void initclock()			   //时钟初始化
{
	TMOD=0x01;
	TH0=a;
	TL0=b;
	EA=1;
	ET0=1;
	TR0=1;
}
void display()				   //显示函数
{
	uchar qian,bai,shi,ge;
	qian=pinlv/1000;
	bai=pinlv%1000/100;
	shi=pinlv%1000%100/10;
	ge=pinlv%1000%100%10;
	write_com(0x80+0x40+0x09);

   	if(qian==0)
	write_date(' ');
	else
	write_date(table[qian]);
	if(qian==0&&bai==0)
	write_date(' ');
	else
	write_date(table[bai]);
	write_date(table[shi]);
	write_date('.');
	write_date(table[ge]);
	write_date('H');
	write_date('z');
	if(boxing==0)
	{
		write_com(0x80+10);
		write_date(0);
		write_date(1);
		write_date(0);
		write_date(1);
		write_date(0);
		write_date(1);
		led3=1;
		led0=0;
	}
	if(boxing==1)
	{
		write_com(0x80+10);
		write_date(2);
		write_date(3);
		write_date(2);
		write_date(3);
		write_date(2);
		write_date(3);
		led0=1;
		led1=0;
	}


	if(boxing==2)
	{
		write_com(0x80+10);
		write_date(4);
		write_date(5);
		write_date(4);
		write_date(5);
		write_date(4);
		write_date(5);
		led1=1;
		led2=0;
	}
	if(boxing==3)
	{
		write_com(0x80+10);
		write_date(6);
		write_date(6);
		write_date(6);
		write_date(6);
		write_date(6);
		write_date(6);
		led2=1;
		led3=0;
	}

}
void keyscan()					//键盘检测函数
{
	if(s1==0)
	{	 
		EA=0;
		delay(2);
		if(s1==0)
		{
			while(!s1);
			pinlv+=bujin;
			if(pinlv>1000)
			{
				pinlv=100;
			}
			display();
			m=65536-(150000/pinlv);
			a=m/256;
			b=m%256;
			EA=1;
		}
	}
	if(s2==0)
	{	 
		
		delay(5);
		if(s2==0)
		{
			EA=0;	
			while(!s2);
			pinlv-=bujin;
			if(pinlv<100)
			{
				pinlv=1000;
			}
			display();
			m=65536-(150000/pinlv);
			a=m/256;
			b=m%256;	
			EA=1;
		}
	}
	if(s3==0)
	{	 
		delay(5);
		if(s3==0)
		{
			EA=0;
			while(!s3);
			boxing++;
			if(boxing>=4)
			{
				boxing=0;
			}
			display();
			EA=1;
		}
	}	
}
void bujindisplay()
{
	uint bai,shi,ge;
	bai=bujin1/100;
	shi=bujin1%100/10;
	ge=bujin1%100%10;
	write_com(0x80+11);
	if(bai==0)
	write_date(' ');
	else
	write_date(table[bai]);
	write_date(table[shi]);
	write_date('.');
	write_date(table[ge]);
}
void bujinjiance()
{
	if(s4==0)
	{
		delay(5);
		if(s4==0)
		{
			 while(!s4);
			 h++;
			 if(h==1)
			 {
			 	 write_com(0x01);
				 write_com(0x80);
				 write_date('S');delay(1);	  //step value
				 write_date('t');delay(1);
				 write_date('e');delay(1);
				 write_date('p');delay(1);
				 write_date(' ');delay(1);
				 write_date('v');delay(1);
				 write_date('a');delay(1);
				 write_date('l');delay(1);
				 write_date('u');delay(1);
				 write_date('e');delay(1);
				 write_date(':');delay(1);
				 bujin1=bujin;
				 
				 bujindisplay();
			 }
			 if(h==2)
			 {
			 	h=0;
				bujin=bujin1;
				
				init_lcd();
				initclock();
				display();
			 }
		}
	}
	if(h==1)
	{
		if(s1==0)
		{
			delay(5);
			if(s1==0)
			{
				while(!s1);
				bujin1++;
				if(bujin1>=101)
				{
					bujin1=1;
				}
				bujindisplay();
			}
		}
		if(s2==0)
		{
			delay(5);
			if(s2==0)
			{
				while(!s2);
				bujin1--;
				if(bujin1<=0)
				{
					bujin1=100;
				}
				bujindisplay();
			}
		}
	}
}
void main()					 //主函数
{	
	init_lcd();
	m=65536-(150000/pinlv);
	a=m/256;
	b=m%256;
	initclock();
	led0=0;
	while(1)
	{
		if(h==0)
		{
			keyscan();
		//	display();
		}
		 
		bujinjiance();

		switch(boxing)
		{
			case 0 : P1=sin[u]; break;
			case 1 : P1=juxing[u]; break;
			case 2 : P1=sanjiao[u]; break;
			case 3 : P1=juchi[u]; break;
		}
	}
}
void T0_time()interrupt 1		   //定时器
{
	TH0=a;
	TL0=b;
	u++;
	if(u>=64)
	u=0;
}