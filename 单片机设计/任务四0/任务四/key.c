#include "key.h"
#include"data.h"
#include "dalay.h"
#include "judge.h"
#include "delay.h"

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

#define GPIO_DIG P0
#define GPIO_KEY P1



u8 KeyValue;	//用来存放读取到的键值

/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/


/*******************************************************************************
* 函 数 名         : KeyDown
* 函数功能		   : 检测有按键按下并读取键值
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void KeyDown(void)
{
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		delay(1000);//延时10ms进行消抖
		if(GPIO_KEY!=0x0f)//再次检测键盘是否按下
		{	
			//测试列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//测试行
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			while((a<50)&&(GPIO_KEY!=0xf0))	 //检测按键松手检测
			{
				delay(1000);
				a++;
			}
		}
	}
	else   KeyValue=20;
}


void key()			    //按键判断函数
{
	keyDown();	
	if(KeyValue==0) key0();
	if(KeyValue==1) key1();
	if(KeyValue==2) key2();
	if(KeyValue==3) key3();
	if(KeyValue==4) key4();
	if(KeyValue==5) key5();
	if(KeyValue==6) key6();
	if(KeyValue==7) key7();
	if(KeyValue==8) key8();
	if(KeyValue==9) key9();
	if(KeyValue==10) key10();
	if(KeyValue==11) key11();
	if(KeyValue==12) key12();
	if(KeyValue==13) key13();
	if(KeyValue==14) key14();
	if(KeyValue==15) key15();


}
								 //不同的按键实现不同的功能
void key0()
{
	h++;
}
void key1()
{
	min++;
}
void key2()
{
	h1++;
}
void key3()
{
	min1++;
}
void key4()
{
	h--;
}
void key5()
{
	min--;
}
void key6()
{
	h1--;
}
void key7()
{
	min1--;
}
void key8()
{
	h2++;		   
}
void key9()
{
	h2--;	  
}
void key10()
{
	year=year+1;
	p=p+number;	
}
void key11()
{
	year=year-1;
	p=p-number;
}
void key12()
{
	month++;
	p=p+maxday;
}
void key13()
{
	month--;
	p=p-maxday;
}
void key14()
{
	day++;
	day0++;
	p++;
}
void key15()
{
	day--;
	day0--;
	p--;
}

