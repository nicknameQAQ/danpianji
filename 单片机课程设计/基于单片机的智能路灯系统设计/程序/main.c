/******定义头文件**************/
#include<reg52.h> 
#include <INTRINS.H> 
#include"lcd.h"
#include "ds1302.h"
/******类型重定义*************/
typedef unsigned char uchar;
typedef unsigned int  uint;
/******引脚定义***************/
sbit key_1 = P1^0;			//定义4个按键
sbit key_2 = P1^1;
sbit key_3 = P1^2;
sbit key_4 = P1^3;
sbit led_1 = P1^4;
sbit Beep = P2^3;					//定义声光报警
sbit AD_clk = P3^4;					//定义ADC0832 		
sbit AD_do = P3^5; 
sbit AD_di = P3^5;
sbit AD_cs = P3^6; 
sbit red_sr = P3^7;					//红外检测

/******定义变量***************/
char hour,min,sec;						//定义小时、分钟、秒变量
char time;
char work_time_Start = 16,work_time_End = 5;		//设定起始、结束时间
bit light_flag; 
uchar light;							//定义光照强度
uchar light_limit;						//定义光照阈值
uchar Data;
/******声明函数***************/
void delayms(unsigned int x);			//延时函数
void display();							//显示
void Read_time();
void set_work_time();					//设定工作时间
void set_limit_light();					//设定光照阈值
void work();							//工作状态
void set_time();						//设定时间
void delay10ms(int d);
void InitTimer0(void);					//T0定时器初始化程序
uchar AD(bit Channel);					//ADC0832程序

void main()
{
	Beep = led_1 = 0;					//声光报警，LED关闭
	InitTimer0();						//T0定时器初始化程序
	LcdInit();							//lcd1602初始化
	while(1)
	{
		Ds1302ReadTime();				//读取时间
		set_work_time();				//设定工作时间
		set_limit_light();				//设定光照阈值
		set_time();
		light = AD(0);					//读取ADC0832数值 					
		display();					 	//显示
		work();							//工作状态
	}
}
void Timer0Interrupt(void) interrupt 1
{
	static uchar count;
    TH0 = 0x0D8;
    TL0 = 0x0F0;
	count ++;
	if(count>=100)
	{
		time ++;
		count = 0;
    }
	//add your code here
}
/*******************************************************************************
* 函数名         : InitTimer0()
* 函数功能		 : T0定时器初始化程序
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void InitTimer0(void)
{
    TMOD = 0x01;
    TH0 = 0x0D8;
    TL0 = 0x0F0;
    EA = 0;
    ET0 = 1;
    TR0 = 1;
}
/*******************************************************************************
* 函数名         : work()
* 函数功能		 : 工作状态
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void work()
{
	Read_time();					//转换时间
	if(work_time_End!=0 && work_time_Start!=0)			//工作时间已设置
	{
		if(hour >= work_time_Start)						//当前时间大于起始时间，如起始时间为19：00，当前时间为21：00。
		{
			Data = AD(1);
			led_1 = 1;									//点亮LED
			if(Data < 50)								//未检测到LED点亮
				Beep = 1;								//声光报警
			else										//检测到Led点亮
				Beep = 0;								//停止报警
		}
		else if(hour < work_time_End) 	//当前时间小于结束时间，如结束时间为06：00，当前时间为04：00。
		{
			if(red_sr)					//检测到人或车
			{
				time = 0;
				light_flag = 1;			//LED标志置1，开始T0定时器计时，点亮LED，10s后LED标志清零，LED熄灭
			}
			if(light_flag)					//LED标志置1，检测到人或车
			{
				led_1 = 1;					//点亮LED
				EA = 1;						//开启中断
				if(time > 10)				//10s后
				{
					light_flag = 0;			//LED标志清零
					EA = 0;				   	//关中断
					time = 0;			 	//计时清零
				}
				Data = AD(1);
				if(Data < 50)				//未检测到LED点亮
					Beep = 1;				//声光报警
				else						//检测到Led点亮
					Beep = 0;				//停止报警		
   			}
			else
				led_1 = 0;
		}
		else
		{
			if(light < light_limit)
			{
				led_1 = 1;
				Data = AD(1);
				if(Data < 50)								//未检测到LED点亮
					Beep = 1;								//声光报警
				else										//检测到Led点亮
					Beep = 0;								//停止报警
			}
			else
				led_1 = Beep = 0;
		}
	}		
}
/*******************************************************************************
* 函数名         : set_work_time()
* 函数功能		 : 设定工作时间
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void set_work_time()
{
	if(key_2 == 0)									   	//设置键按下
	{
		delayms(10);									//延时消抖
		if(key_2 == 0)								  	//再次判断
		{
			while(key_2 == 0);
			LcdWriteCom(0x01);						 	//清屏
			delayms(10);
			LcdWriteCom(0x80);						//显示
	  		LcdWriteData('S');
			LcdWriteData('E');
			LcdWriteData('T');
			LcdWriteData('-');
			LcdWriteData('w');
			LcdWriteData('o');
			LcdWriteData('r');
			LcdWriteData('k');
			LcdWriteData(' ');
			LcdWriteData('t');
			LcdWriteData('i');
			LcdWriteData('m');
			LcdWriteData('e');
			for(;;)	   									//设定工作时间段
			{
				LcdWriteCom(0xc0);
				LcdWriteData('s');
				LcdWriteData('t');
				LcdWriteData('a');
				LcdWriteData('r');
				LcdWriteData('t');
				LcdWriteData(':');
				LcdWriteCom(0xca);
				LcdWriteData(0x30+work_time_Start%100/10);
				LcdWriteData(0x30+work_time_Start%10);
				if(key_2 == 0)							//按键2按下，每按一次+5
				{
					delayms(10);
					if(key_2 == 0)
					{
						while(key_2 == 0);
						work_time_Start+=1;
						if(work_time_Start > 24)
							work_time_Start = 24;
					}
				}
				if(key_3 == 0)							//按键3按下，每按一次-5
				{
					delayms(10);
					if(key_3 == 0)
					{
						while(key_3 == 0);
						work_time_Start -=1;
						if(work_time_Start<=work_time_End) 		//禁止起始时间小于结束时间
							work_time_Start = work_time_End;
					}
				}
				if(key_4 == 0) 							//按键4按下，输入下一项
				{
					delayms(10);
					if(key_4 == 0)
					{
						while(key_4 == 0);
						LcdWriteCom(0x01);
						break;
					}
				}
			}
			for(;;)	   									//设定工作时间段
			{
				LcdWriteCom(0xc0);
				LcdWriteData('s');
				LcdWriteData('t');
				LcdWriteData('o');
				LcdWriteData('p');
				LcdWriteData(' ');
				LcdWriteData(':');
				LcdWriteCom(0xca);
				LcdWriteData(0x30+work_time_End%100/10);
				LcdWriteData(0x30+work_time_End%10);
				if(key_2 == 0)
				{
					delayms(10);
					if(key_2 == 0)
					{
						while(key_2 == 0);
						work_time_End += 1;
						if(work_time_End > work_time_Start)		//禁止结束时间大于开始时间
							work_time_End = work_time_Start;
					}
				}
				if(key_3 == 0)
				{
					delayms(10);
					if(key_3 == 0)
					{
						while(key_3 == 0);
						work_time_End -=1;
						if(work_time_End < 0)
							work_time_End = 0;
					}
				}
				if(key_4 == 0)
				{
					delayms(10);
					if(key_4 == 0)
					{
						while(key_4 == 0);
						LcdWriteCom(0x01);
						break;
					}
				}
			}
		}
	}	
}
/*******************************************************************************
* 函数名         : set_limit_light()
* 函数功能		 : 设定光照阈值
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void set_limit_light()
{
	if(key_3 == 0)									   	//键3按下
	{
		delayms(10);									//延时消抖
		if(key_3 == 0)								  	//再次判断
		{
			while(key_3 == 0);
			LcdWriteCom(0x01);						 	//清屏
			delayms(10);
			LcdWriteCom(0x80);						//显示
	  		LcdWriteData('S');
			LcdWriteData('E');
			LcdWriteData('T');
			LcdWriteData('-');
			LcdWriteData('L');
			LcdWriteData('i');
			LcdWriteData('g');
			LcdWriteData('h');
			LcdWriteData('t');
			LcdWriteData('-');
			LcdWriteData('L');
			LcdWriteData('i');
			LcdWriteData('m');
			LcdWriteData('i');
			LcdWriteData('t');
			for(;;)	   									//设定光照阈值
			{
				LcdWriteCom(0xc0);
				LcdWriteData('D');
				LcdWriteData('a');
				LcdWriteData('t');
				LcdWriteData('a');
				LcdWriteData(':');
				LcdWriteCom(0xca);
				LcdWriteData(0x30+light_limit%1000/100);
				LcdWriteData(0x30+light_limit%100/10);
				LcdWriteData(0x30+light_limit%10);
				if(key_2 == 0)							//按键2按下，每按一次+5
				{
					delayms(10);
					if(key_2 == 0)
					{
						while(key_2 == 0);
						light_limit += 5;
						if(light_limit > 250)
							light_limit = 250;
					}
				}
				if(key_3 == 0)							//按键3按下，每按一次-5
				{
					delayms(10);
					if(key_3 == 0)
					{
						while(key_3 == 0);
						light_limit -= 5;
						if(light_limit < 10)
							light_limit = 10;
					}
				}
				if(key_4 == 0) 							//按键4按下，输入下一项
				{
					delayms(10);
					if(key_4 == 0)
					{
						while(key_4 == 0);
						LcdWriteCom(0x01);
						break;
					}
				}
			}
		}
	}	
}
/*******************************************************************************
* 函数名         : read_time()
* 函数功能		 : 转换时间
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void Read_time()
{
	hour = (Time_1[2]/16)*10 + (Time_1[2]&0x0f);
	min = (Time_1[1]/16)*10 + (Time_1[1]&0x0f);
	sec = (Time_1[0]/16)*10 + (Time_1[0]&0x0f);
}
/*******************************************************************************
* 函数名         : display()
* 函数功能		 : 主界面显示函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void display()
{
	LcdWriteCom(0x80);	
	LcdWriteData('2');
	LcdWriteData('0');
	LcdWriteData('0'+Time_1[6]/16);			//年
	LcdWriteData('0'+(Time_1[6]&0x0f));
	LcdWriteData('-');
	LcdWriteData('0'+Time_1[4]/16);			//月
	LcdWriteData('0'+(Time_1[4]&0x0f));
	LcdWriteData('-');
	LcdWriteData('0'+Time_1[3]/16);			//日
	LcdWriteData('0'+(Time_1[3]&0x0f));
	LcdWriteData(' ');

	LcdWriteCom(0x8e);
	LcdWriteData('0'+time%100/10);
	LcdWriteData('0'+time%10);
	
	
	LcdWriteCom(0xc0);	
	LcdWriteData('0'+Time_1[2]/16);			//时
	LcdWriteData('0'+(Time_1[2]&0x0f));
	LcdWriteData(':');
	LcdWriteData('0'+Time_1[1]/16);			//分
	LcdWriteData('0'+(Time_1[1]&0x0f));
	LcdWriteData(':');
	LcdWriteData('0'+Time_1[0]/16);			//秒
	LcdWriteData('0'+(Time_1[0]&0x0f));	

	LcdWriteCom(0xcd); 						//显示当前光照强度
	LcdWriteData('0'+light/100);
	LcdWriteData('0'+light%100/10);
	LcdWriteData('0'+light%10);
}
/*******************************************************************************
* 函数名         : delayms()
* 函数功能		 : 延时函数，ms级别
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void delayms(unsigned int x)   //0.14ms误差 0us
{
	unsigned char i;
	while(x--)
	{
		for (i = 0; i<13; i++)
		{}
	}
}
/*************************************************************
* 函 数 名         : delay10ms
* 函数功能		   : 延时函数，延时时间10ms*d
* 输    入         : 无
* 输    出         : 无
*************************************************************/
void delay10ms(int d)   //误差 0us
{
    unsigned char a,b,c;
    for(c=d;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
/*************************************************************
* 函 数 名         : set_time()
* 函数功能		   : 设置时间
* 输    入         : 无
* 输    出         : 无
*************************************************************/
void set_time()								
{
	char time_flag = 0; 			//定义一个时间变量
	char time_flag_1 = 0;
	   if(key_1==0)				//进入设置时间
	   {
   		while(key_1==0);
		//设置年
		time_flag = Time_1[6]/16;		//取出DS1302 年的十位
		time_flag_1 = (Time_1[6]&0x0f);
		for(;;)
		{				
			LcdWriteCom(0x82);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag--;
			}
			if(time_flag>=9)					   //显示范围
				time_flag = 9;
			else if(time_flag<=0)
				time_flag = 0;

			LcdWriteData(time_flag+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[6] = time_flag*16;			//时间写入数组
				break;
			}
		}
		
		for(;;)
		{
			LcdWriteCom(0x83);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag_1++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag_1--;
			}
			if(time_flag_1>=9)					   //显示范围
				time_flag_1 = 9;
			else if(time_flag_1<=0)
				time_flag_1 = 0;

			LcdWriteData(time_flag_1+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[6] += time_flag_1;	//时间写入数组
				break;
			}
		}

		//设置月
		time_flag = Time_1[4]/16;		//取出DS1302 月的十位
		time_flag_1 = (Time_1[4]&0x0f);
		for(;;)
		{
			LcdWriteCom(0x85);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag--;
			}
			if(time_flag>=9)					   //显示范围
				time_flag = 9;
			else if(time_flag<=0)
				time_flag = 0;

			LcdWriteData(time_flag+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[4] = time_flag*16;			//时间写入数组
				break;
			}
		}

		for(;;)
		{
			LcdWriteCom(0x86);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标		
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag_1++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag_1--;
			}
			if(time_flag_1>=9)					   //显示范围
				time_flag_1 = 9;
			else if(time_flag_1<=0)
				time_flag_1 = 0;

			LcdWriteData(time_flag_1+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[4] += time_flag_1;	//时间写入数组
				break;
			}
		}

		//设置日
		time_flag = Time_1[3]/16;		//取出DS1302 日的十位
		time_flag_1 = (Time_1[3]&0x0f);
		for(;;)
		{
			LcdWriteCom(0x88);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag--;
			}
			if(time_flag>=9)					   //显示范围
				time_flag = 9;
			else if(time_flag<=0)
				time_flag = 0;

			LcdWriteData(time_flag+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[3] = time_flag*16;			//时间写入数组
				break;
			}
		}

		for(;;)
		{
			LcdWriteCom(0x89);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag_1++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag_1--;
			}
			if(time_flag_1>=9)					   //显示范围
				time_flag_1 = 9;
			else if(time_flag_1<=0)
				time_flag_1 = 0;

			LcdWriteData(time_flag_1+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[3] += time_flag_1;	//时间写入数组
				break;
			}
		}

		//设置小时
		time_flag = Time_1[2]/16;		//取出DS1302 小时的十位
		time_flag_1 = (Time_1[2]&0x0f);
		for(;;)
		{
			LcdWriteCom(0xc0);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag--;
			}
			if(time_flag>=9)					   //显示范围
				time_flag = 9;
			else if(time_flag<=0)
				time_flag = 0;

			LcdWriteData(time_flag+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[2] = time_flag*16;			//时间写入数组
				break;
			}
		}

		for(;;)
		{
			LcdWriteCom(0xc1);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag_1++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag_1--;
			}
			if(time_flag_1>=9)					   //显示范围
				time_flag_1 = 9;
			else if(time_flag_1<=0)
				time_flag_1 = 0;

			LcdWriteData(time_flag_1+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[2] += time_flag_1;	//时间写入数组
				break;
			}
		}

		//设置分
		time_flag = Time_1[1]/16;		//取出DS1302 分的十位
			time_flag_1 = (Time_1[1]&0x0f);
		for(;;)
		{
			LcdWriteCom(0xc3);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag--;
			}
			if(time_flag>=9)					   //显示范围
				time_flag = 9;
			else if(time_flag<=0)
				time_flag = 0;

			LcdWriteData(time_flag+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[1] = time_flag*16;			//时间写入数组
				break;
			}
		}

		for(;;)
		{
			LcdWriteCom(0xc4);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag_1++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag_1--;
			}
			if(time_flag_1>=9)					   //显示范围
				time_flag_1 = 9;
			else if(time_flag_1<=0)
				time_flag_1 = 0;

			LcdWriteData(time_flag_1+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[1] += time_flag_1;	//时间写入数组
				break;
			}
		}


		//设置秒
		time_flag = Time_1[0]/16;		//取出DS1302 秒的十位
		time_flag_1 = (Time_1[0]&0x0f);
		for(;;)
		{
			LcdWriteCom(0xc6);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag--;
			}
			if(time_flag>=9)					   //显示范围
				time_flag= 9;
			else if(time_flag<=0)
				time_flag = 0;

			LcdWriteData(time_flag+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[0] = time_flag*16;			//时间写入数组
				break;
			}
		}

		for(;;)
		{
			LcdWriteCom(0xc7);  //显示坐标	 
			LcdWriteCom(0x0e);  //光标
			delay10ms(50);	
			LcdWriteCom(0x0c);  //光标	

			
			if(key_2 ==0)
			{
				while(key_2==0);			 //时间加
				time_flag_1++;
			}
			if(key_3 ==0)					
			{
				while(key_3==0);		  //时间减
				time_flag_1--;
			}
			if(time_flag_1>=9)					   //显示范围
				time_flag_1 = 9;
			else if(time_flag_1<=0)
				time_flag_1 = 0;

			LcdWriteData(time_flag_1+'0');   //在lcd1602进行显示
			
			if(key_4 ==0)					//确认
			{
				while(key_4==0);		  //时间减
				Time_1[0] += time_flag_1;	//时间写入数组
				break;
			}
		}
		Ds1302Init();		 //保存时间
		delay10ms(50);		
	}
}
/*******************************************************************************
* 函数名         : uchar AD(bit Channel)
* 函数功能		 : 采集AD值
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
uchar AD(bit Channel)
{
	unsigned char i;
	unsigned char ad0,ad1;
	ad0 = 0x00;
	ad1 = 0x00;

	AD_cs = 0;
	_nop_();
	AD_clk=0;
	_nop_();
	AD_di=1;
	_nop_();
	AD_clk=1;	   ////输入开始信号（构成一个正脉冲）,时钟上升沿，输入开始信号 
		
	//单通道0~10.  1~11  双通道 0+~00 。0—~01
	AD_clk=0;
	AD_di=1;   //选择单通道
	AD_clk=1;  
	_nop_(); 
	AD_clk=0; 
	AD_di=Channel;	  //选择通道

	//通道选择完毕
	AD_clk=1;
	_nop_();
	AD_clk=0;
	_nop_();
	AD_di=1;		

	for(i=8;i>0;i--)
	{
		AD_clk=1;
		ad0<<=1;
		AD_clk=0;
		if(AD_do)
			ad0 |= 0x01;
		else
			ad0	|= 0x00;	
	}
	for(i=8;i>0;i--)
	{	
		ad1>>=1;		   ////从低位向高位读取八位AD值 
		AD_clk=0;
		if(AD_do)
			ad1 |= 0x80;
		else
			ad1	|= 0x00;
		AD_clk=1;	
	}
	AD_cs = 1;
	AD_do = 1;
	AD_clk = 1;
	if(ad0 == ad1)		  //判断是否采集正确
	{
		return ad1;
	}
	return -1;
}
