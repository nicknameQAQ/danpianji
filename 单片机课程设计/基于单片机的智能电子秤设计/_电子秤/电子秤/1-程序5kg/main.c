#include <reg52.h>
#include <intrins.h>
#include <string.h>

bit bdata flag_key;
#include "main.h"
#include "LCD1602.h"
#include "HX711.h"
#include "keyboard.h"
#include "eeprom52.h"

#define uchar unsigned char
#define uint  unsigned int

unsigned long HX711_Buffer = 0;
unsigned long Weight_Maopi = 0;
unsigned long Weight_Maopi_0 = 0;
unsigned int qupi=0;
long Weight_Shiwu = 0;
//键盘处理变量
unsigned char keycode;
unsigned char DotPos;				   //小数点标志及位置

uint GapValue,GapValue1;

unsigned char idata price;     //单价，长整型值，单位为分   
unsigned char idata money;     //总价，长整型值，单位为分
//定义标识
volatile bit FlagTest = 0;		//定时测试标志，每0.5秒置位，测完清0
volatile bit FlagKeyPress = 0;  //有键按下标志，处理完毕清0
//校准参数
//因为不同的传感器特性曲线不是很一致，因此，每一个传感器需要矫正这里这个参数才能使测量值很准确。
//当发现测试出来的重量偏大时，增加该数值。
//如果测试出来的重量偏小时，减小改数值。
//该值可以为小数
//#define GapValue 349
sbit LED=P1^1;

volatile bit ClearWeighFlag = 0; //传感器调零标志位，清除0漂

/******************把数据保存到单片机内部eeprom中******************/
void write_eeprom()
{
	SectorErase(0x1000);
	GapValue1=GapValue&0x00ff;
	byte_write(0x2000, GapValue1);
	GapValue1=(GapValue&0xff00)>>8;
	byte_write(0x2001, GapValue1);
	byte_write(0x2060, a_a);	
}

/******************把数据从单片机内部eeprom中读出来*****************/
void read_eeprom()
{
	GapValue   = byte_read(0x2001);
	GapValue   = (GapValue<<8)|byte_read(0x2000);
	a_a      = byte_read(0x2060);
}

/**************开机自检eeprom初始化*****************/
void init_eeprom() 
{
	read_eeprom();		//先读
	if(a_a != 1)		//新的单片机初始单片机内问eeprom
	{
		GapValue  = 3500;
		a_a = 1;
		write_eeprom();	   //保存数据
	}	
}
																	
//显示单价，单位为元，四位整数，两位小数
void Display_Price()
{
            LCD1602_write_com(0x8c);
			LCD1602_write_data(price/100 + 0x30);
			LCD1602_write_data(price%100/10 + 0x30);
			LCD1602_write_data('.');
			LCD1602_write_data(price%10 + 0x30);
}

//显示重量，单位kg，两位整数，三位小数
void Display_Weight()
{
            LCD1602_write_com(0x83);
			LCD1602_write_data(Weight_Shiwu/1000 + 0x30);
			LCD1602_write_data('.');
			LCD1602_write_data(Weight_Shiwu%1000/100 + 0x30);
			LCD1602_write_data(Weight_Shiwu%100/10 + 0x30);
			LCD1602_write_data(Weight_Shiwu%10 + 0x30);
}

//显示总价，单位为元，四位整数，两位小数
void Display_Money()
{
  // unsigned int i,j;

   if (money>9999) 	//超出显示量程
   {
     LCD1602_write_com(0x80+0x40+6);
     LCD1602_write_word("---.-");
      return;       
   } 
   if (money>=1000)
   {
       LCD1602_write_com(0x80+0x40+6);
	   LCD1602_write_data(money/1000 + 0x30);
	   LCD1602_write_data(money%1000/100 + 0x30);
	   LCD1602_write_data(money%100/10 + 0x30);
	   LCD1602_write_data('.');
	   LCD1602_write_data(money%10 + 0x30);
   }
   else if (money>=100)
   {
       LCD1602_write_com(0x80+0x40+6);
	   LCD1602_write_data(0x20);
	   LCD1602_write_data(money%1000/100 + 0x30);
	   LCD1602_write_data(money%100/10 + 0x30);
	   LCD1602_write_data('.');
	   LCD1602_write_data(money%10 + 0x30);
   }
    else if(money>=10)
   {
       LCD1602_write_com(0x80+0x40+6);
	  LCD1602_write_data(0x20);
	    LCD1602_write_com(0x80+0x40+7);
	   LCD1602_write_data(0x20);
	   LCD1602_write_data(money%100/10 + 0x30);
	   LCD1602_write_data('.');
	   LCD1602_write_data(money%10+ 0x30);
   }   
   else 
     {
       LCD1602_write_com(0x80+0x40+6);
	   LCD1602_write_data(0x20);
	   LCD1602_write_com(0x80+0x40+7);
	   LCD1602_write_data(0x20);
	   LCD1602_write_com(0x80+0x40+8);
	   LCD1602_write_data(0 + 0x30);
	   LCD1602_write_data('.');
	   LCD1602_write_data(money%10 + 0x30);
   }         
}

//数据初始化
void Data_Init()
{
   price = 0;
   DotPos = 0;
}
//定时器0初始化
void Timer0_Init()
{
	ET0 = 1;        //允许定时器0中断
	TMOD = 1;       //定时器工作方式选择
	TL0 = 0xb0;     
	TH0 = 0x3c;     //定时器赋予初值
	TR0 = 1;        //启动定时器
}

//定时器0中断
void Timer0_ISR (void) interrupt 1 using 0
{
uchar Counter;
	TL0 = 0xb0;
	TH0 = 0x3c;     //定时器赋予初值

	//每0.5秒钟刷新重量
    Counter ++;
    if (Counter >= 10)
    {
       FlagTest = 1;
	   Counter = 0;
    }
}


//按键响应程序，参数是键值
//返回键值：
//         1    2    3    10	   //10:清零重量，兼去皮功能	
//         4    5    6    11	   //11:清除单价
//         7    8    9    12	   //12:显示数值偏大调节按键  
//         14   0    15   13	   //14:无功能（用于扩展）15：小数点   13：显示数值偏小调节按键

void KeyPress(uchar keycode)
{
	switch (keycode)
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
		case 9:	//目前在设置整数位，要注意price是整型，存储单位为分
			if (DotPos == 0)
			{			//最多只能设置到千位
				if (price<100)
				{
					price=price*10+keycode*10;
					/*输入第一个数字时，price是等于0的，当第一次输入一个数字，例如输入5，那么计算：price=price*10+5*10=50
					我们看到液晶上显示的是05.0，但是在程序里直接处理小数显示是比较麻烦的，所以我们是将这个数乘以10，扩大10倍后处理的
					那么当第二次输入数字时，例如输入4，那么计算：price=50*10+4*10=540，液晶显示的就是54.0
					*/
				}
			}//目前在设置小数位
			else if (DotPos==1)  //小数点后第一位
			{
				price=price+keycode;
				DotPos=2;
			}
			Display_Price();
			break;
		case 10:   //去皮键
			if(qupi==0)
				qupi=Weight_Shiwu;
			else
				qupi=0;
			Display_Price();
			//		 FlagSetPrice = 0;
			DotPos = 0;
			break;
		case 11:	//删除键
			price=0;
			DotPos=0;
			Display_Price();
			break;
		case 12:	   //加
			if(GapValue<10000)
			GapValue++;	
			
			break;
		case 13:   //减
			if(GapValue>1)
			GapValue--;
			break;
		case 15:   //小数点按下 
			DotPos = 1;      //小数点后第一位
			break;

   }
}
//****************************************************
//主函数
//****************************************************
void main()
{
	init_eeprom();  //开始初始化保存的数据
	Init_LCD1602();									//初始化LCD1602
   EA = 0;
   Data_Init();
   Timer0_Init();
   //初中始化完成，开断
   EA = 1;
	
//	Get_Maopi();
	LCD1602_write_com(0x80);						//指针设置
   LCD1602_write_word(" Welcome To Use ");	//  
   LCD1602_write_com(0x80+0x40);						//指针设置
   LCD1602_write_word("Electronic Scale");
//   Delay_ms(2000);
   Get_Maopi();
   LCD1602_write_com(0x80);						//指针设置
   LCD1602_write_word("WE:0.000 PR:00.0");
   LCD1602_write_com(0x80+0x40);				//指针设置
   LCD1602_write_word("MONEY:  0.00    ");
   Display_Price();
//	Get_Maopi();				//称毛皮重量

	while(1)
	{
//每0.5秒称重一次
	  if (FlagTest==1)
		{
			Get_Weight();
			FlagTest = 0;
		}			
	  	
	  keycode = Getkeyboard();
	  //有效键值0-15
	  if (keycode<16)
	  {
		 KeyPress(keycode);
		 Buzzer=0;
		 Delay_ms(100);
		 Buzzer=1;
		 while(keycode<16)
		 {
			if(keycode==12||keycode==13)
			{
				Buzzer=0;
				 Delay_ms(10);
				 Buzzer=1;
				 KeyPress(keycode);
				 Get_Weight();
				 flag_key=1;
			}
			keycode = Getkeyboard();
		 }
		 write_eeprom();			   //保存数据
	  }
	}
}
//****************************************************
//称重
//****************************************************
void Get_Weight()
{
	Weight_Shiwu = HX711_Read();
	Weight_Shiwu = Weight_Shiwu - Weight_Maopi;		//获取净重
	
	Weight_Shiwu = (unsigned int)((float)Weight_Shiwu*10/GapValue)-qupi; 	//计算实物的实际重量																
	if(Weight_Shiwu > 5000)		//超重报警
	{
		Buzzer = !Buzzer;	
		LED=!LED;
		LCD1602_write_com(0x83);
	   LCD1602_write_word("-.---");
	}
	else
	{
		if(Weight_Shiwu==0)
		LED=1;
		else if(Weight_Shiwu>0)
		LED=0; 
		Buzzer = 1;
		Display_Weight();
		money = Weight_Shiwu*price/1000;  //money单位为分
	       //显示总金额
	       Display_Money();
	}
}

//****************************************************
//获取毛皮重量
//****************************************************
void Get_Maopi()
{
	unsigned char clear;
mm:	Weight_Maopi_0 = HX711_Read();
	for(clear=0;clear<10;clear++)
	{
		Buzzer=1;
		LED=1;
		Delay_ms(100);
		LED=0;
		Delay_ms(100);	
	}
	Weight_Maopi = HX711_Read();
	if(Weight_Maopi/GapValue!=Weight_Maopi_0/GapValue)
	goto mm;
	Buzzer=0;
	Delay_ms(500);
	Buzzer=1;
} 

//****************************************************
//MS延时函数(12M晶振下测试)
//****************************************************
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}