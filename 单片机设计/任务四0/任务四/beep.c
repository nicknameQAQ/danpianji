#include "beep.h"		
#include "data.h"
#include "key.h"
#include "keypro.h"
#include"cycle.h"	

int y=2;

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

sbit beep=P1^5;	   


/*******************************************************************************
* 函 数 名         : delay
* 函数功能		   : 延时函数，i=1时，大约延时10us
*******************************************************************************/

void same()								 //闹钟函数
{  int i;
	colock();
	if((h==h1&&min==min1)||(y==n&&h==h2&&min==min2))		//条件
	{  
	for(i=0;i<10000;i++)
	{
		beep=~beep;
		delay(10);
		}
	}
}

void weeks()   	//星期几的限制范围
{
if(y>6||y<0)
{
	y=0;
}
}