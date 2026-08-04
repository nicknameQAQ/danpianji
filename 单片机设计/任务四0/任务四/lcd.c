#include "LCD.H"
#include"data.h"
#include "beep.h"
#include "keypro.h"

char e[2]={'N','R'};
char u[]="农历"  ;
char f[]="年";

int moment=1;


/*******************************************************************************
* 函 数 名         : Lcd1602_Delay1ms
* 函数功能		   : 延时函数，延时1ms
* 输    入         : c
* 输    出         : 无
* 说    名         : 该函数是在12MHZ晶振下，12分频单片机的延时。
*******************************************************************************/


void Delay1ms(unsigned int c)
{
    unsigned char a,b;
	for(; c>0; c--)
	{
	    for(b=100; b>0; b--)
		{
	        for(a=1; a>0; a--);
		}
	}
}

 void LCD12864_char(unsigned char x,unsigned char y)				   //定位显示
{
	char add;
	if(x==1) x=0x80;
	if(x==2) x=0x90;
	if(x==3) x=0x88;
	if(x==4) x=0x98;
	add=x+y;
	LCD12864_WriteCmd(add);
}
/*******************************************************************************
* 函 数 名         : LCD12864_Busy
* 函数功能		   : 检测LCD是否忙
* 输    入         : 无
* 输    出         : 1或0（1表示不忙，0表示忙）
*******************************************************************************/

unsigned char LCD12864_Busy(void)
{
	unsigned char i = 0;

	LCD12864_RS = 0;   	//选择命令
	LCD12864_RW = 1;	//选择读取

	LCD12864_EN = 1;
    Delay1ms(1);

	while((LCD12864_DATAPORT & 0x80) == 0x80)	//检测读取到的值
	{
		i++;
		if(i > 100)
		{
			LCD12864_EN = 0;
			return 0;	   //超过等待时间返回0表示失败
		}
	}

	LCD12864_EN = 0;

	return 1;
}

/*******************************************************************************
* 函 数 名         : LCD12864_WriteCmd
* 函数功能		   : 写命令
* 输    入         : cmd
* 输    出         : 无
*******************************************************************************/

void LCD12864_WriteCmd(unsigned char cmd)
{
	unsigned char i;
	i = 0;
	while( LCD12864_Busy() == 0)
	{
		Delay1ms(1);
		i++;
		if( i>100)
		{
			return;	   //超过等待退出
		}	
	}
	
	LCD12864_RS = 0;     //选择命令
	LCD12864_RW = 0;     //选择写入
	LCD12864_EN = 0;     //初始化使能端

	LCD12864_DATAPORT = cmd;   //放置数据

	LCD12864_EN = 1;		   //写时序
	Delay1ms(120);
	LCD12864_EN = 0;    					
}

/*******************************************************************************
* 函 数 名         : LCD12864_WriteData
* 函数功能		   : 写数据
* 输    入         : dat
* 输    出         : 无
*******************************************************************************/

void LCD12864_WriteData(unsigned char dat)
{
	unsigned char i;
	i = 0;
	while( LCD12864_Busy() == 0)
	{
		Delay1ms(1);
		i++;
		if( i>100)
		{
			return;	   //超过等待退出
		}	
	}

	LCD12864_RS = 1;     //选择数据
	LCD12864_RW = 0;     //选择写入
	LCD12864_EN = 0;     //初始化使能端

	LCD12864_DATAPORT = dat;   //放置数据

	LCD12864_EN = 1;		   //写时序
	Delay1ms(5);
	LCD12864_EN = 0;    								
}


	 
/*******************************************************************************
* 函 数 名         : LCD12864_ReadData
* 函数功能		   : 读取数据
* 输    入         : 无
* 输    出         : 读取到的8位数据
*******************************************************************************/
#ifdef LCD12864_PICTURE

unsigned char LCD12864_ReadData(void)
{
	unsigned char i, readValue;

	i = 0;
	while( LCD12864_Busy() == 0)
	{
		LCD12864_Delay1ms(1);
		i++;
		if( i>100)
		{
			return 0;	   //超过等待退出
		}	
	}

	LCD12864_RS = 1;       //选择命令
	LCD12864_RW = 1;
	LCD12864_EN = 0;
	LCD12864_Delay1ms(1);  //等待

	LCD12864_EN = 1;
	LCD12864_Delay1ms(1);
	readValue = LCD12864_DATAPORT;
	LCD12864_EN = 0;

	return readValue;
}

#endif

/*******************************************************************************
* 函 数 名         : LCD12864_Init
* 函数功能		   : 初始化LCD12864
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void LCD12864_Init()
{
	LCD12864_PSB = 1;	  //选择并行输入
	LCD12864_RST = 1;	  //复位

	LCD12864_WriteCmd(0x30);  //选择基本指令操作
	LCD12864_WriteCmd(0x0c);  //显示开，关光标
	LCD12864_WriteCmd(0x01);  //清除LCD12864的显示内容
}

/*******************************************************************************
* 函 数 名         : LCD12864_STR
* 函数功能		   : 设置在基本指令模式下设置显示坐标,且数据显示。
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
	  

void lcd0()
{
	
	int i=0;
   if(moment==0)
   {
	LCD12864_char(1,0);
	LCD12864_WriteData(c[0]+'0');
	LCD12864_WriteData(c[1]+'0');
	LCD12864_WriteData(c[2]+'0');
	LCD12864_WriteData(c[3]+'0');
	LCD12864_WriteData('/');
	LCD12864_WriteData(c[4]+'0');
	LCD12864_WriteData(c[5]+'0');
	LCD12864_WriteData('/');
	LCD12864_WriteData(c[6]+'0');
	LCD12864_WriteData(c[7]+'0'); 
	LCD12864_char(1,5);
	while(w[i]!='\0')
	{
		LCD12864_WriteData(w[i]);
		i++;
	}  i=0;
   }
if(moment==1)
	{
	LCD12864_char(2,0);
	LCD12864_WriteData(t[0]+'0');
	LCD12864_WriteData(t[1]+'0');
	LCD12864_WriteData(':');
	LCD12864_WriteData(t[2]+'0');
	LCD12864_WriteData(t[3]+'0');
	LCD12864_WriteData(':');
	LCD12864_WriteData(t[4]+'0');
	LCD12864_WriteData(t[5]+'0');

	LCD12864_char(2,4);
if((year%400==0)||((year%4==0)&&(year%100!=0)))
	{
	LCD12864_WriteData(e[1]);
	}
else
	
	LCD12864_WriteData(e[0]);

	LCD12864_char(2,5);
	LCD12864_WriteData(t[6]+'0');
	LCD12864_WriteData(t[7]+'0');
	LCD12864_WriteData(':');
	LCD12864_WriteData(t[8]+'0');
	LCD12864_WriteData(t[9]+'0');

	LCD12864_char(3,0);
	while(d[i]!='\0')
	{
		LCD12864_WriteData(d[i]);
		i++;
	}	
	i=0;

	LCD12864_char(3,3);
	while(f[i]!='\0')
	{
		LCD12864_WriteData(f[i]);
		i++;
	}	
	i=0;
	 
	LCD12864_char(3,4);
	LCD12864_WriteData(y+'0');
	LCD12864_WriteData(0x10);

	LCD12864_char(3,5);
 	LCD12864_WriteData(t[10]+'0');
	LCD12864_WriteData(t[11]+'0');
	LCD12864_WriteData(':');
	LCD12864_WriteData(t[12]+'0');
	LCD12864_WriteData(t[13]+'0');

	LCD12864_char(4,0);
	while(u[i]!='\0')
	{
		LCD12864_WriteData(u[i]);
		i++;
	}	
	i=0;

 	LCD12864_char(4,2);
	LCD12864_WriteData(0x02);

	LCD12864_char(4,3);
	LCD12864_WriteData(c[8]+'0');
	LCD12864_WriteData(c[9]+'0');
	LCD12864_WriteData(c[10]+'0');
	LCD12864_WriteData(c[11]+'0');
	LCD12864_WriteData('/');
	LCD12864_WriteData(c[12]+'0');
	LCD12864_WriteData(c[13]+'0');
	LCD12864_WriteData('/');
	LCD12864_WriteData(c[14]+'0');
	LCD12864_WriteData(c[15]+'0');
		

}
}
