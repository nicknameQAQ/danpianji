#include<reg51.h>
#include<stdio.h>
#include<intrins.h>
typedef unsigned char uchar;
typedef unsigned int  uint;
sbit RS=P2^5;
sbit RW=P2^6;
sbit E=P2^7;
sbit CLK=P2^0;
sbit DO=P2^1;
sbit DI=P2^1;
sbit CS=P2^2;
sbit CLR=P3^0;
sbit GATE=P3^1;
sbit K1=P3^7;
uint  num=0;
uint  c=0;
float d=0;
uchar e=1,f=1;
unsigned long int aaa=0,bbb=0;
uchar LCD_wait()//LCD1602内部等待函数
{
	RS=0;
	RW=1;	
	_nop_();
	E=1;
	_nop_();
	E=0;
	return P0;		
}
void LCD_write(bit aa,uchar bb)//向LCD写入命令或数据
{
	E=0;
	RS=aa;
	RW=0;		
	_nop_();
	P0=bb;	
	_nop_();//注意顺序
	E=1;		
	_nop_();//注意顺序
	E=0;		
	_nop_();
	LCD_wait();//LCD1602内部等待函数	
}
void LCD_display(uchar temp)
{
	LCD_write(0,0x08|temp);	
}
void LCD_input(uchar cc)
{
	LCD_write(0,0x04|cc);
}
void LCD_init()//初始化LCD
{
	//RW=0;
	LCD_write(0,0x38);//8位数据端口,2行显示,5*7点阵
	LCD_write(0,0x38);
	
	LCD_write(0,0x0C);//开启显示, 无光标
	LCD_write(0,0x01);//清屏
	LCD_write(0,0x0C);//AC递增,画面不动
	
}
void LCD_sfj1(uchar x,uchar y)
{
	if(y==0)
		LCD_write(0,0x80|x);
	if(y==1)
		LCD_write(0,0x80|(x-0x40));
}
void LCD_print(uchar *p)
{
	while(*p!='\0')
	{
		LCD_write(1,*p);
		p++;
	}
}
void delay1(uchar x)//ms延时子函数
{ 		   
    uchar i;
    for(i=0;i<x;i++); //延时，脉冲一位持续的时间
}
uchar start(bit temp) //把模拟电压值转换成8位二进制数并返回,dat为通道选择。 
{ 	 
	uchar i,aa=0;
    CS=0;
    DO=0;//片选，DO为高阻态
    delay1(10);
    CLK=0;
    delay1(2);
    DI=1;	 
    CLK=1;	 
    delay1(2);//第一个脉冲，起始位 
    CLK=0;					  
    delay1(2);					  
    DI=1;					  
    CLK=1;					  
    delay1(2);//第二个脉冲，DI=1表示双通道单极性输入	 
    CLK=0;										 
    delay1(2);										 
    DI=temp;	/*******************************/											 
    CLK=1;
    delay1(2);//第三个脉冲，DI=1表示选择通道1（CH2）	  
    DI=0;										   
    DO=1;//DI转为高阻态，DO脱离高阻态为输出数据作准备
    CLK=1;									   													 
    delay1(2);									   
    CLK=0;									   
    delay1(2);//经实验，这里加一个脉冲AD便能正确读出数据， 
        //不加的话读出的数据少一位（最低位d0读不出）		  
	for(i=0;i<8;i++)									  
    { 
    	CLK=1;
       	delay1(2);
       	CLK=0;
       	delay1(2);
       	aa=(aa<<1)|DO;//在每个脉冲的下降沿DO输出一位数据，最终ch为8位二进制数
	}
    CS=1;//取消片选，一个转换周期结束
    return(aa);//把转换结果返回
}
uint fenjie(bit cc)
{
	uchar dd;
	uint  ee;
	dd=start(cc); 
	ee=(uint)dd*100/51; //51是由5/255的倒数得来的，5是5V，255是8位AD。
	return(ee);
}	
void delay2(uint z)//按键消抖
{
	uint x;
	uchar y;
	for(x=0;x<z;x++)
		for(y=0;y<110;y++);
}
void delay3()//1s延时子函数	    
{
	uchar h,i,j,k;
	for(h=5;h>0;h--)
		for(i=4;i>0;i--)
			for(j=116;j>0;j--)
				for(k=214;k>0;k--);
}
//定时计数初始化
void init()	      
{
	TMOD=0x25;				// T0计数，T1定时
	TH0=0x00;	
	TL0=0x00;
	TH1=0x06;
	TL1=0x06;
	CLR=1;
	CLR=0;
	GATE=0;
	GATE=1;
	ET0=1;
	ET1=1;
	EA=1;
	TR0=1;
	TR1=1;
}
void main()//主函数
{	 
	unsigned long a=0;
	bit b=0;
	uchar tab[10];
	uint temp=0;
	LCD_init();
	init();
	LCD_sfj1(0,0);
	LCD_print("Welcome to use ");
	LCD_sfj1(0,1);
	LCD_print("Please Wating...");
	delay3();
	LCD_write(0,0x01); 
	while(1)
	{
		if(K1==0)
		{	
			delay2(10);
			if(K1==0)	
			{
				while(K1==0);
				b=~b;
				LCD_write(0,0x01); 
			}
		}
		temp=fenjie(0);
		if(temp>c)
		c=temp; 
		if(temp>c>>3&&temp<c>>1) 
			f=0;
		if(b==0)
		{
			a=(aaa*256)+bbb;
			if(a==0)
			{
				LCD_sfj1(0,0);
				LCD_print("F= 0000000000 Hz");	
				LCD_sfj1(0,1);
				LCD_print("F= 0000000000 Hz"); 	
			}
			else
			{		
				LCD_sfj1(0,0);
				LCD_print("F=            Hz");
				sprintf(tab,"%ld",a);
				LCD_sfj1(2,0);
				LCD_print(tab);
				LCD_sfj1(0,1);
				LCD_print("P=            ms");
				sprintf(tab,"%0.6f",1000.0/a);
				LCD_sfj1(2,1);
				LCD_print(tab);	
			} 
		}
		else
		{   
			sprintf(tab,"%bd",e);
			LCD_sfj1(0,0);
			LCD_print(tab);
			sprintf(tab,"Upp=%0.3fV",d);
			LCD_sfj1(0,1);
			LCD_print(tab);
		}
	}
}
void t0() interrupt 1 using 1
{

}
void t1() interrupt 3 using 3//定时器T1中断服务程序
{	  
	if(num==3999)
	{
		GATE=0;
		TR0=0;
		TR1=0;
		aaa=TH0*256+TL0;
		bbb=P1;
		d=c/100.0;
		c=0;
		e=f;
		f=1;
		CLR=1;
		CLR=0;
		TH0=0x00;	
		TL0=0x00;
		TH1=0x06;
		TL1=0x06;
		num=0;
		GATE=1;
		TR0=1;
		TR1=1;
	}
	else 
		num++;
}



 



