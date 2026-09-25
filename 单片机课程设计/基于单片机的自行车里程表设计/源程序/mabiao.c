#include<reg52.h>

#define uchar unsigned char
#define uint unsigned int

#include "Data.h"
#include "DS1302.h"
#include "AT24C02.h"

sbit COUNT_IN=P3^2;

//定义1602相关管脚
sbit rs=P1^4;
sbit en=P1^0;

//键盘定义
sbit K1=P3^4;	//设置时间
sbit K3=P3^6;	//减按键
sbit K2=P3^5;	//加按键
sbit K4=P3^7;	//设置半径安全距离
sbit BEEP=P3^0;

uint count;
unsigned long Velocity,Mileage;

uchar code tab1[]={"  /  /     :    "}; //14/09/10 16:34 3   	
uchar code tab2[]={"  0.000km 00km/h"};	//000.000km 00km/h
uchar code tab3[]={"Wheel Radius  cm"};
uchar code tab4[]={"Safe Speed  km/h"};
uchar code tab5[]={"Sec :           "};
			   			
uchar Mode=0;
uchar bike_set=0;
uchar a;
char RADIUS,SAFE_SPEED;
bit LED_SEC;
uchar before_sec;	

//自定义字符
uchar code num[]={
						0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x00,//1	 
						0x1f,0x01,0x01,0x1f,0x10,0x10,0x1f,0x00,//2
						0x1f,0x01,0x01,0x1f,0x01,0x01,0x1f,0x00,//3	 
						0x11,0x11,0x11,0x1f,0x01,0x01,0x01,0x00,//4
						0x1f,0x10,0x10,0x1f,0x01,0x01,0x1f,0x00,//5	 
						0x1f,0x10,0x10,0x1f,0x11,0x11,0x1f,0x00,//6
						0x1f,0x01,0x01,0x01,0x01,0x01,0x01,0x00,//7 	
};
void READS();
void SETS();
void delay(uint x)
{
	uint i,j;
	for(i=0;i<x;i++)
	for(j=0;j<110;j++);
}
void init()
{
	IT0=1;	//INT0负跳变触发	
    TMOD=0x01;//定时器工作于方式1
	TH0=0x3c;	  //50ms
	TL0=0xb0;
	EA=1; //CPU开中断总允许
	ET0=1;//开定时中断
	EX0=1;//开外部INTO中断
    TR0=1;//启动定时
}
/********液晶写入指令函数与写入数据函数，以后可调用**************/

void write_1602com(uchar com)//****液晶写入指令函数****
{
	rs=0;//数据/指令选择置为指令
	P0=com;//送入数据
	delay(1);
	en=1;//拉高使能端，为制造有效的下降沿做准备
	delay(1);
	en=0;//en由高变低，产生下降沿，液晶执行命令

}


void write_1602dat(uchar dat)//***液晶写入数据函数****
{
	rs=1;//数据/指令选择置为数据
	P0=dat;//送入数据
	delay(1);
	en=1; //en置高电平，为制造下降沿做准备
	delay(1);
	en=0; //en由高变低，产生下降沿，液晶执行命令
}
//自定义字符集
void Lcd_ram()      
{ 
	uint i,j,k=0,temp=0x40; 
	for(i=0;i<7;i++)
	{
	   for(j=0;j<8;j++)
	   {
	    write_1602com(temp+j);
	    write_1602dat(num[k]);
	    k++;
	   }
	   temp=temp+8;
	}
}

void lcd_init()//***液晶初始化函数****
{
	Lcd_ram();
	write_1602com(0x38);//设置液晶工作模式，意思：16*2行显示，5*7点阵，8位数据
	write_1602com(0x0c);//开显示不显示光标
	write_1602com(0x06);//整屏不移动，光标自动右移
	write_1602com(0x01);//清显示

	write_1602com(0x80);//显示固定符号从第一行第1个位置之后开始显示
	for(a=0;a<16;a++)
	{
		write_1602dat(tab1[a]);//向液晶屏写固定符号部分
	}
	write_1602com(0x80+0x40);//显示固定符号写入位置，从第2个位置后开始显示
	for(a=0;a<16;a++)
	{
		write_1602dat(tab2[a]);//写显示固定符号
	}
}

void display()
{
	//			1km/h=100m/0.1h	  	 360s
	//			10km/h=100m/0.01h 	 36s
	//			100km/h=100m/0.001h  3.6s
	if(Mode==0&&bike_set==0)
	{
		//读时间
			Ds1302_Read_Time();
			//显示时间
			write_1602com(0x80);
			write_1602dat(0x30+time_buf1[1]/10);
			write_1602dat(0x30+time_buf1[1]%10);
			write_1602com(0x80+3);
			write_1602dat(0x30+time_buf1[2]/10);
			write_1602dat(0x30+time_buf1[2]%10);
			write_1602com(0x80+6);
			write_1602dat(0x30+time_buf1[3]/10);
			write_1602dat(0x30+time_buf1[3]%10);
			write_1602com(0x80+9);
			write_1602dat(0x30+time_buf1[4]/10);
			write_1602dat(0x30+time_buf1[4]%10);
			write_1602com(0x80+12);
			write_1602dat(0x30+time_buf1[5]/10);
			write_1602dat(0x30+time_buf1[5]%10);
			write_1602com(0x80+15);
			write_1602dat(time_buf1[7]-1);		

			if(before_sec!=time_buf1[6])
			{
				before_sec=time_buf1[6];
				write_1602com(0x80+11);
				write_1602dat(':');
				LED_SEC=1;
			}
			if(LED_SEC==0)
			{
				write_1602com(0x80+11);
				write_1602dat(' ');	
			}

			write_1602com(0x80+0x40);
			if(Mileage/1000000==0)
			write_1602dat(' ');
			else
			write_1602dat(0x30+Mileage/1000000);//数字+0x30得到该数字的LCD1602显示码
			if(Mileage%1000000/100000==0)
			write_1602dat(' ');
			else	
			write_1602dat(0x30+Mileage%1000000/100000);//数字+0x30得到该数字的LCD1602显示码
			write_1602dat(0x30+Mileage%1000000%100000/10000);//数字+0x30得到该数字的LCD1602显示码 
			write_1602com(0x80+0x40+4);
			write_1602dat(0x30+Mileage%1000000%100000%10000/1000);//数字+30得到该数字的LCD1602显示码
			write_1602dat(0x30+Mileage%1000000%100000%10000%1000/100);//数字+30得到该数字的LCD1602显示码
			write_1602dat(0x30+Mileage%1000000%100000%10000%1000%100/10);//数字+30得到该数字的LCD1602显示码
			SETS();
	
			write_1602com(0x80+0x40+10);
			write_1602dat(0x30+Velocity/10);
			write_1602dat(0x30+Velocity%10);//数字+30得到该数字的LCD1602显示码
	}
	else if(Mode!=0)
	{
		switch(Mode)
		{
			case 1:	
				write_1602com(0x80+0x40);//显示固定符号写入位置
				for(a=0;a<16;a++)
				{
					write_1602dat(tab5[a]);//写显示固定符号
				}
				write_1602com(0x80+0x40+14);
				write_1602dat(0x30+time_buf1[6]/10);
				write_1602dat(0x30+time_buf1[6]%10);	
				write_1602com(0x0F);	 //打开闪烁
				write_1602com(0x80+1);
				break;						  
			case 2:
				write_1602com(0x80+4);
				break;
			case 3:
				write_1602com(0x80+7);
				break;
			case 4:
				write_1602com(0x80+10);
				break;
			case 5:
				write_1602com(0x80+13);
				break;
			case 6:
				write_1602com(0x80+0x40+15);
				break;
			case 7:
				write_1602com(0x80+15);
				break;
			case 8:
				write_1602com(0x0c);
				write_1602com(0x80);//显示固定符号从第一行第1个位置之后开始显示
				for(a=0;a<16;a++)
				{
					write_1602dat(tab1[a]);//向液晶屏写固定符号部分
				}
				write_1602com(0x80+0x40);//显示固定符号写入位置，从第2个位置后开始显示
				for(a=0;a<16;a++)
				{
					write_1602dat(tab2[a]);//写显示固定符号
				}
				break;
		}
	}
	else if(bike_set!=0)
	{
		switch(bike_set)
		{
			case 1:	
				write_1602com(0x80);//显示固定符号写入位置
				for(a=0;a<16;a++)
				{
					write_1602dat(tab3[a]);//写显示固定符号
				}
				write_1602com(0x80+0x40);//显示固定符号写入位置
				for(a=0;a<16;a++)
				{
					write_1602dat(tab4[a]);//写显示固定符号
				}
				write_1602com(0x80+12);
				write_1602dat(0x30+RADIUS/10);		   //车轮半径
				write_1602dat(0x30+RADIUS%10);
				write_1602com(0x80+0x40+10);
				write_1602dat(0x30+SAFE_SPEED/10);		   //安全速度
				write_1602dat(0x30+SAFE_SPEED%10);	
				write_1602com(0x0F);	 //打开闪烁
				write_1602com(0x80+13);
				break;						  
			case 2:
				write_1602com(0x80+0x40+11);
				break;
			case 3:
				write_1602com(0x0c);
				write_1602com(0x80);//显示固定符号从第一行第1个位置之后开始显示
				for(a=0;a<16;a++)
				{
					write_1602dat(tab1[a]);//向液晶屏写固定符号部分
				}
				write_1602com(0x80+0x40);//显示固定符号写入位置，从第2个位置后开始显示
				for(a=0;a<16;a++)
				{
					write_1602dat(tab2[a]);//写显示固定符号
				}
				break;
		}
	}	
}

void KEY()
{	
	if(bike_set==0&&K1==0)
	{
		delay(20);
		if(bike_set==0&&K1==0)
		{
			BEEP=0;
			delay(50);
			BEEP=1;
			Mode++;
			display();
			if(Mode>=8)
			{
				Mode=0;
				Ds1302_Write_Time();
			}
		}
		while(bike_set==0&&K1==0);
	}
	if(K4==0&&Mode==0)
	{
		delay(20);
		if(K4==0&&Mode==0)
		{
			BEEP=0;
			delay(50);
			BEEP=1;
			bike_set++;
			display();
			if(bike_set>=3)
			{
				bike_set=0;
				SETS();
			}
		}
		while(Mode==0&&K4==0);
	}

	//+
	if(K2==0&&(Mode!=0||bike_set!=0))
	{
		delay(20);
		//调时
		if(K2==0&&(Mode!=0||bike_set!=0))
		{
			BEEP=0;
			delay(50);
			BEEP=1;	
			switch(Mode)
			{
				case 1:
					time_buf1[1]++;
					if(time_buf1[1]>=100)
						time_buf1[1]=0;
					write_1602com(0x80);
					write_1602dat(0x30+time_buf1[1]/10);
					write_1602dat(0x30+time_buf1[1]%10);
					write_1602com(0x80+1);
					break;
				case 2:
					time_buf1[2]++;
					if(time_buf1[2]>=13)
						time_buf1[2]=1;
					write_1602com(0x80+3);
					write_1602dat(0x30+time_buf1[2]/10);
					write_1602dat(0x30+time_buf1[2]%10);
					write_1602com(0x80+4);
					break;
				case 3:
					time_buf1[3]++;
					if(time_buf1[3]>=YDay(time_buf1[1],time_buf1[2])+1)
						time_buf1[3]=1;
					write_1602com(0x80+6);
					write_1602dat(0x30+time_buf1[3]/10);
					write_1602dat(0x30+time_buf1[3]%10);
					write_1602com(0x80+7);
					break;
				case 4:
					time_buf1[4]++;
					if(time_buf1[4]>=24)
						time_buf1[4]=0;
					write_1602com(0x80+9);
					write_1602dat(0x30+time_buf1[4]/10);
					write_1602dat(0x30+time_buf1[4]%10);
					write_1602com(0x80+10);
					break;
				case 5:
					time_buf1[5]++;
					if(time_buf1[5]>=60)
						time_buf1[5]=0;
					write_1602com(0x80+12);
					write_1602dat(0x30+time_buf1[5]/10);
					write_1602dat(0x30+time_buf1[5]%10);
					write_1602com(0x80+13);
					break;
				case 6:
					time_buf1[6]++;
					if(time_buf1[6]>=60)
						time_buf1[6]=0;
					write_1602com(0x80+0x40+14);
					write_1602dat(0x30+time_buf1[6]/10);
					write_1602dat(0x30+time_buf1[6]%10);
					write_1602com(0x80+0x40+15);
					break;
				case 7:
					time_buf1[7]++;
					if(time_buf1[7]>=8)
						time_buf1[7]=1;
					write_1602com(0x80+15);
					write_1602dat(time_buf1[7]-1);
					write_1602com(0x80+15);
					break;
			}
			switch(bike_set)
			{
				case 1:
					RADIUS++;
					if(RADIUS>=71)
					RADIUS=0;
					write_1602com(0x80+12);
					write_1602dat(0x30+RADIUS/10);
					write_1602dat(0x30+RADIUS%10);
					write_1602com(0x80+13);
					break;
				case 2:
					SAFE_SPEED++;
					if(SAFE_SPEED>=100)
					SAFE_SPEED=0;
					write_1602com(0x80+0x40+10);
					write_1602dat(0x30+SAFE_SPEED/10);
					write_1602dat(0x30+SAFE_SPEED%10);
					write_1602com(0x80+0x40+11);
					break;
			}
		}
		while(K2==0);
	}

	//-
	if(K3==0&&(Mode!=0||bike_set!=0))
	{
		delay(20);
		//调时
		if(K3==0&&(Mode!=0||bike_set!=0))
		{
			BEEP=0;
			delay(50);
			BEEP=1;
			switch(Mode)
			{
				case 1:
					time_buf1[1]--;
					if(time_buf1[1]<0)
						time_buf1[1]=99;
					write_1602com(0x80);
					write_1602dat(0x30+time_buf1[1]/10);
					write_1602dat(0x30+time_buf1[1]%10);
					write_1602com(0x80+1);
					break;
				case 2:
					time_buf1[2]--;
					if(time_buf1[2]<=0)
						time_buf1[2]=12;
					write_1602com(0x80+3);
					write_1602dat(0x30+time_buf1[2]/10);
					write_1602dat(0x30+time_buf1[2]%10);
					write_1602com(0x80+4);
					break;
				case 3:
					time_buf1[3]--;
					if(time_buf1[3]<=0)
						time_buf1[3]=YDay(time_buf1[1],time_buf1[2]);
					write_1602com(0x80+6);
					write_1602dat(0x30+time_buf1[3]/10);
					write_1602dat(0x30+time_buf1[3]%10);
					write_1602com(0x80+7);
					break;
				case 4:
					time_buf1[4]--;
					if(time_buf1[4]<0)
						time_buf1[4]=23;
					write_1602com(0x80+9);
					write_1602dat(0x30+time_buf1[4]/10);
					write_1602dat(0x30+time_buf1[4]%10);
					write_1602com(0x80+10);
					break;
				case 5:
					time_buf1[5]--;
					if(time_buf1[5]<0)
						time_buf1[5]=59;
					write_1602com(0x80+12);
					write_1602dat(0x30+time_buf1[5]/10);
					write_1602dat(0x30+time_buf1[5]%10);
					write_1602com(0x80+13);
					break;
				case 6:
					time_buf1[6]--;
					if(time_buf1[6]<0)
						time_buf1[6]=59;
					write_1602com(0x80+0x40+14);
					write_1602dat(0x30+time_buf1[6]/10);
					write_1602dat(0x30+time_buf1[6]%10);
					write_1602com(0x80+0x40+15);
					break;
				case 7:
					time_buf1[7]--;
					if(time_buf1[7]<1)
						time_buf1[7]=7;
					write_1602com(0x80+15);
					write_1602dat(time_buf1[7]-1);
					write_1602com(0x80+15);
					break;
			}
			switch(bike_set)
			{
				case 1:
					RADIUS--;
					if(RADIUS<0)
					RADIUS=70;
					write_1602com(0x80+12);
					write_1602dat(0x30+RADIUS/10);
					write_1602dat(0x30+RADIUS%10);
					write_1602com(0x80+13);
					break;
				case 2:
					SAFE_SPEED--;
					if(SAFE_SPEED<0)
					SAFE_SPEED=99;
					write_1602com(0x80+0x40+10);
					write_1602dat(0x30+SAFE_SPEED/10);
					write_1602dat(0x30+SAFE_SPEED%10);
					write_1602com(0x80+0x40+11);
					break;
			}
		}
		while(K3==0);
	}	
	if(K2==0&&K3==0&&Mode==0&bike_set==0)
	{
		BEEP=0;
		delay(100);
		BEEP=1;
		delay(100);
		BEEP=0;
		delay(100);
		BEEP=1;
		delay(100);
		Mileage=0;
		SETS();
		while(K2==0&&K3==0);
	}
}
void BJ_SAFE()
{
	if(Velocity>SAFE_SPEED)
	{
		BEEP=0;
	}
	else
	{
		BEEP=1;
	}
}

void main()
{
	//初始化
	Ds1302_Init();
	lcd_init();
	initeeprom();
	//读取初始参数
	READS();
	//定时器初始化
//	InitTimer0();
	init();
	lcd_init();
	before_sec=time_buf1[6];
	while(1)
	{
		if(Mode==0&&bike_set==0)
		{
			display();
			BJ_SAFE();
		}
		KEY();
	}
}

void EXINT0() interrupt 0
{
	count++;
}

void time0() interrupt 1
{
	uchar m,n;
	TH0=0x3c;
	TL0=0xb0;	 //50ms
	m++;
	if(LED_SEC==1)
	{
		n++;
		if(n>=10)
		{
			n=0;
			LED_SEC=0;
		}
	}
	
	if(m>=10)
	{
		m=0;
		Mileage=Mileage+10*(Velocity/3.6)/2;		 //里程m=里程+速度km/h/3.6/2
		Velocity=count *2*3.14*RADIUS /100000*2*3600  /40;//将500ms的距离经过运算得到km/h，将速度/100，方便显示
		count=0;	
	}
}

//读初值
void READS()
{
	uchar Mileage_H,Mileage_M,Mileage_L;
	delay(10);
	RADIUS=read_add(0x01);
	delay(10);
	SAFE_SPEED=read_add(0x02);

	delay(10);
	Mileage_H=read_add(0x03);
	delay(10);
	Mileage_M=read_add(0x04);
	delay(10);
	Mileage_L=read_add(0x05);

	Mileage=Mileage_H*100000+Mileage_M*1000+Mileage_L*10;
}


//写初值
void SETS()
{
	delay(10);
	write_add(0x01,RADIUS);
	delay(10);
	write_add(0x02,SAFE_SPEED);

/*	Mileage_H=Mileage/10000;			 //123.4560
	Mileage_M=Mileage%10000/100;
	Mileage_L=Mileage%10000%100; */
	delay(10);
	write_add(0x03,Mileage/100000);
	delay(10);
	write_add(0x04,Mileage%100000/1000);
	delay(10);
	write_add(0x05,Mileage%100000%1000/10);
}



















