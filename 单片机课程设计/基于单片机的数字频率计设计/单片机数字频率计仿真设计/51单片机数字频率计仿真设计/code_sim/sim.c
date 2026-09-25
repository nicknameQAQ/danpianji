// 基于51单片机的简易数字频率计
#include<reg52.h>
#define uchar unsigned char 
#define uint	unsigned int 

sbit RS = P2^0;
sbit RW = P2^1;
sbit EN = P2^2;
sbit led = P3^6;

uint jici=0,fre=0;
uchar display_num[6];

void delayms(uint z)
{
 uint x,y;
 for(x=z;x>0;x--)
	for(y=110;y>0;y--);
}

void delay_20us()
{ 
	uchar a ;
	for(a=0;a<500;a++);
}

int delay(unsigned char xms)
{
    unsigned char i,j;
    for(i=xms;i>0;i--)
        for(j=110;j>0;j--);
    return 0;
}

int write_com(unsigned char com)
{
    RS = 0;
    RW = 0;
    P0 = com;
    delay(5);
    EN = 1;
    delay(5);
    EN = 0;
    return 0;        
}

int write_date(unsigned char date)
{
    RS = 1;
    RW = 0;
    P0 = date;
    delay(5);
    EN = 1;
    delay(5);
    EN = 0;
    return 0;        
}

void LCD1602_Write_String(uchar x,uchar y,uchar *s) 
{     
	if (y == 0) 
	{     
		write_com(0x80 + x);    
	}
	else 
	{      
		write_com(0xC0 + x);      
	}        
	while (*s != '\0') 
	{     
		write_date(*s++); 
		delay(150);		
	}
}

int init()//1602初始化
{
    EN = 0;
    write_com(0X38);
    write_com(0X0C);
    write_com(0X06);
    write_com(0X01);
    return 0;
}

int main(void)
{   
	init();	  
  TMOD=0x51;//定时器1计数模式 定时器0定时模式 工作方式1
	TH0=(65536-46080)/256;//装定时50ms初值
	TL0=(65536-46080)%256;
	TR0=1; //定时器0模式开启
	ET0=1;// 定时器0中断
	EA=1;//全局中断开启
	TH1=0;
	TL1=0;
	TR1=0; 
	LCD1602_Write_String(0,0,"frequence:");
	while(1)
	{
		display_num[0] = fre / 100 + '0';//分位提取
		display_num[1] = fre % 100 /10+ '0';
		display_num[2] = fre % 10+ '0';
		display_num[3] = 'H';
		display_num[4] = 'z';
		display_num[5] = '\0';
		if(pinlv > 1000)
			LCD1602_Write_String(0,1,"flow!");//大于1000则超出提示
		else
			LCD1602_Write_String(0,1,display_num);//显示频率
	}
}

void timer0() interrupt 1//定时器0溢出中断
{
	jici++;
	TH0=(65536-46080)/256;;
	TL0=(65536-46080)%256;;
	if(jici == 20) //20次为1s
	{
		TR0=TR1=0; 
		fre=TL1+TH1*256; //提取计数数值  计算为频率
		TL1=TH1=0; 
		led=~led; 
		jici=0;
		TR0=TR1=1; 
	}
}

