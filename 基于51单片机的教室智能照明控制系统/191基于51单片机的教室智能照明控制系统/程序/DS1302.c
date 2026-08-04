#include <reg51.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int

uchar time_data[7];
uchar code write_add[7]={0x8c,0x8a,0x88,0x86,0x84,0x82,0x80};       //数据的地址
uchar code read_add[7]={0x8d,0x8b,0x89,0x87,0x85,0x83,0x81};

uchar code table1[]="C: 00|2000/00/00";
uchar code table2[]="T: 00|00:00:00 0";
uchar code table3[]="                ";			 //清屏内容
uchar code table4[]="  Set Real Time ";
uchar code table5[]="  Set Open Time ";
uchar code table6[]="   Start Time:  ";
uchar code table7[]="    00:00:00    ";
uchar code table8[]="    End Time:   ";
uchar code table9[]="Date: 2000/00/00";
uchar code table0[]="Time: 00:00:00 0";

bit Adjust;			  //调节标志位，=1表示进入调节模式，=0是正常模式
bit Auto_flg;		  //自动模式标志位，=1表示手动模式，=0是自动模式
sbit rs=P1^0;		  //LCD1602
sbit rw=P1^1; 		  //LCD1602
sbit e=P1^2;		  //LCD1602
sbit sck=P2^5;		  //时钟端口
sbit io=P2^6;		  //时钟端口
sbit rst=P2^7;		  //时钟端口
sbit SELT =P1^6;	  //选择键
sbit ENTER=P1^7;	  //确认键
sbit UP   =P3^0;	  //加键
sbit DOWN =P3^1;	  //减键
sbit MODE =P3^2;	  //切换按键接口
sbit SWITCH=P3^3;	  //手动开关
sbit AUTO_LED=P1^3;	  //自动模式灯接口
sbit HAND_LED=P1^4;	  //手动灯接口
sbit LAMP1=P3^4;	  //灯接口
sbit LAMP2=P3^5;	  //灯接口
sbit LAMP3=P3^6;	  //灯接口
sbit LAMP4=P3^7;	  //灯接口
sbit LIGHT1=P2^0;	  //光线检测端口
sbit IN1=P2^2;
sbit OUT1=P2^1;
sbit DQ=P1^5;		  //温度数据口
uchar Select_num;	  //选择按键按下次数
uchar Enter_num;	  //确认按键按下次数
uchar Switch_num;	  //手动开关按键按下次数
float Year,Month,Day,Hour,Minute,Second;			//时间设置值
uchar Week;		
float Hour_H,Minute_H,Second_H;  					//设置开始时间
float Hour_L,Minute_L,Second_L;  					//设置结束时间
int Value1;											//人数
uchar IN1_flag=0;
uchar OUT1_flag=0;
uint Count;           //T0定时器中断计数次数
uchar Miao;

int temperature;									//全局变量 温度

void read_rtc();

/**************************************************LCD1602显示*******************************************************************/
void delay1(uint z)			  //延时函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=10;y>0;y--);
}

void write_com(uchar com)			 //写指令函数
{
	rw=0;
	delay1(5);
	rs=0;
	delay1(5);
	e=1;
	delay1(5);
	P0=com;
	delay1(5);
	e=0;
	delay1(5);	
}

void write_date(uchar date)			//写数据函数
{									
	rw=0;
	delay1(5);
	rs=1;
	delay1(5);
	e=1;
	delay1(5);
	P0=date;
	delay1(5);
	e=0;
	delay1(5);	
}

void init()						    //初始化函数
{	
	uchar num;
	Year=17;
	Month=1;
	Day=1;
	Hour=15;
	Minute=30;
	Second=50;
	Week=1;
	Hour_H=22;
	Minute_H=0;
	Second_H=0;
	Hour_L=18;
	Minute_L=0;
	Second_L=0;
	e=0;						    // 时序表e初始为0
	write_com(0x38);		        //设置16*2显示，5*7点阵，8位数据接口
	write_com(0x0c);				//设置光标
	write_com(0x06);				//光标自动加1,光标输入方式
	write_com(0x01);				//清屏
	write_com(0x80);				//设置初始显示位置
	for(num=0;num<16;num++)
	{
		write_date(table1[num]);
		delay1(5);
	}
	write_com(0x80+0x40);
	for(num=0;num<16;num++)
	{
		write_date(table2[num]);
		delay1(5);
	}					  
}

void SetTime_dispaly(uchar add,uchar dat)	 //第一个：参数的地址，第二个：参数的内容
{
	uchar shi,ge;
	shi=dat/10;							//把十位提取出来
	ge=dat%10;							//把个位提取出来
	write_com(add);			   	 		//要写的地址
	write_date(0x30+shi);				//十位的内容   1602字符库
	write_date(0x30+ge);				//个位的内容   1602字符库
}

void Week_dispaly(uchar add,uchar dat)	//第一个：参数的地址，第二个：参数的内容
{
	write_com(add);						//要写的地址
	write_date(0x30+dat);				//十位的内容   1602字符库
}
void Date_dispaly(uchar add,uchar dat)	 //第一个：参数的地址，第二个：参数的内容
{
	uchar shi,ge;
	shi=dat/16;							//把十位提取出来
	ge=dat%16;							//把个位提取出来
	write_com(add+3);			   	 	//要写的地址
	write_date(0x30+shi);				//十位的内容   1602字符库
	write_date(0x30+ge);				//个位的内容   1602字符库				   
}

void LCD_Clean(uchar i)					//液晶屏清除显示
{
	uchar num;
	if(i==1)
	{
		write_com(0x80);				//设置初始显示位置
		for(num=0;num<16;num++)			//显示第一行内容
		{
			write_date(table3[num]);
			delay1(1);
		}
	}
	if(i==2)
	{
		write_com(0x80+0x40);
		for(num=0;num<16;num++)			//显示第二行内容
		{
			write_date(table3[num]);
			delay1(1);
		}
	}
}

void LCD_Display_String(unsigned char line,unsigned char *string)
{ 										//液晶屏显示内容，把要显示的内容写到对应的位置
	unsigned char i;
	unsigned char address=0;
	if(line==1)
	{
		address=0x80;	 				//0X80是第1行的第1个位置  ,0x81第2位
	}
	else if(line==2)
	{
		address=0x80+0x40;  			//0X80+0x40是第2行的第1个位置  ,0X80+0x40+1是第2行第2位
	}
	
	for(i=0;i<16;i++)
	{
		write_com(address);
		write_date(string[i]);
		address++;
	} 
}
/**********************************DS1302时钟***************************************/
void Time_Display(void)
{
	read_rtc();
	Date_dispaly(0x80+0x40+9,time_data[6]);  //显示秒
	Date_dispaly(0x80+0x40+6,time_data[5]);	 //显示分
	Date_dispaly(0x80+0x40+3,time_data[4]);	 //显示时
	Date_dispaly(0x80+11,time_data[3]);		 //显示日
	Date_dispaly(0x80+8,time_data[2]);		 //显示月
	Week_dispaly(0x80+0x40+15,time_data[1]); //显示周
	Date_dispaly(0x80+5,time_data[0]);		 //显示年
					   //	Year/10*16+Year%10
	Hour=time_data[4]/16*10+time_data[4]%16;;
	Minute=time_data[5]/16*10+time_data[5]%16;;
	Second=time_data[6]/16*10+time_data[6]%16;;
}


void write_ds1302_byte(uchar dat)
{
	uchar i;
	for(i=0;i<8;i++)
	{
		sck=0;
		io=dat&0x01;					   //准备数据,从最低位开始
		dat=dat>>1;
		sck=1;
	}
}

void write_ds1302(uchar add,uchar dat)
{
	rst=0;
	_nop_();  						//CPU原地踏步
	sck=0;
	_nop_();
	rst=1;
	_nop_();
	write_ds1302_byte(add);		    //传地址
	write_ds1302_byte(dat);		    //传数据
	rst=0;     						//不受其他影响
	_nop_();
	io=1;						   //释放
	sck=1;
}

uchar read_ds1302(uchar add)
{
	uchar i,value;
	rst=0;
	_nop_();  //CPU原地踏步
	sck=0;
	_nop_();
	rst=1;
	_nop_();
	write_ds1302_byte(add);
	for(i=0;i<8;i++)
	{
		value=value>>1;
		sck=0;
		if(io)
		value=value|0x80;
		sck=1;
	}
	rst=0;
	_nop_();
	sck=0;
	_nop_();
	sck=1;
	io=1;
	return value;
}

void set_rtc()			          //设置时间
{
	uchar i,j;
	for(i=0;i<7;i++)			 //转换BCD码
	{
		j=time_data[i]/10;
		time_data[i]=time_data[i]%10;
		time_data[i]=time_data[i]+j*16;
	}
	write_ds1302(0x8e,0x00);	  //去除写保护
	for(i=0;i<7;i++)
	{
		write_ds1302(write_add[i],time_data[i]);
	}
	write_ds1302(0x8e,0x80);      //加写保护
}

void read_rtc()
{
	uchar i;
	for(i=0;i<7;i++)
	{
		time_data[i]=read_ds1302(read_add[i]);	//最终读出来的数 16进制
	}
}

/*************************************************按键****************************************************************/
void Keyscan(void)
{
	if(SELT==0)
	{
		delay1(2);
		if(SELT==0)
		{
			while(!SELT);
			Select_num++;					//选择键按下一次
			Adjust=1;						//进入调节模式			
		}
		if(Select_num==1)
		{
			LCD_Clean(1);
			LCD_Clean(2);
			LCD_Display_String(1,table4);
			LCD_Display_String(2,table5);
			write_com(0x80+0);				//写 >>
			write_date(0x3e);
			write_com(0x80+1);				//写 >>
			write_date(0x3e);
			Enter_num=0;
		}
		if(Select_num==2)
		{
			LCD_Clean(1);
			LCD_Clean(2);
			LCD_Display_String(1,table4);
			LCD_Display_String(2,table5);
			write_com(0x80+0x40+0);				//写 >>
			write_date(0x3e);
			write_com(0x80+0x40+1);				//写 >>
			write_date(0x3e);
			Enter_num=0;
		}
		if(Select_num==3)
		{
			LCD_Clean(1);
			LCD_Clean(2);
			LCD_Display_String(1,table1);
			LCD_Display_String(2,table2);
			Select_num=0;
			Enter_num=0;					
			Adjust=0;
		}
		write_com(0x0c);									//光标不再闪烁
		Enter_num=0;
	}
	
	if(ENTER==0)
	{
		delay1(2);
		if(ENTER==0)
		{
			while(!ENTER);
			Enter_num++;
		}
		if(Select_num==1)					//设置实时时间
		{
			if(Enter_num==1)
			{
				LCD_Clean(1);
				LCD_Clean(2);
				LCD_Display_String(1,table9);
				LCD_Display_String(2,table0);
				SetTime_dispaly(0x80+8,Year);
				SetTime_dispaly(0x80+11,Month);
				SetTime_dispaly(0x80+14,Day);
				SetTime_dispaly(0x80+0x40+6,Hour);
				SetTime_dispaly(0x80+0x40+9,Minute);
				SetTime_dispaly(0x80+0x40+12,Second);
				Week_dispaly(0x80+0x40+15,Week);
				write_com(0x80+8);			//光标闪烁地址，停留在年的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==2)
			{
				write_com(0x80+11);			//光标闪烁地址，停留在月的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==3)
			{
				write_com(0x80+14);			//光标闪烁地址，停留在日的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==4)
			{
				write_com(0x80+0x40+6);		//光标闪烁地址，停留在时的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==5)
			{
				write_com(0x80+0x40+9);		//光标闪烁地址，停留在分的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==6)
			{
				write_com(0x80+0x40+12);	//光标闪烁地址，停留在秒的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==7)
			{
				write_com(0x80+0x40+15);	//光标闪烁地址，停留在星期的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==8)
			{
				Enter_num=0;
				write_com(0x0c);			//光标不再闪烁
				LCD_Clean(1);
				LCD_Clean(2);
				LCD_Display_String(1,table1);
				LCD_Display_String(2,table2);
				time_data[0]=Year;///10*16+Year%10;
				time_data[1]=Week;///10*16+Week%10;
				time_data[2]=Month;///10*16+Month%10;
				time_data[3]=Day;///10*16+Day%10;
				time_data[4]=Hour;///10*16+Hour%10;
				time_data[5]=Minute;///10*16+Minute%10;
				time_data[6]=Second;///10*16+Second%10;
				set_rtc();					 //设置时间
				Select_num=0;
				Adjust=0;
			}
		}

		if(Select_num==2)					//设置开关锁时间
		{
			if(Enter_num==1)
			{
				LCD_Clean(1);
				LCD_Clean(2);
				LCD_Display_String(1,table6);
				LCD_Display_String(2,table7);
				SetTime_dispaly(0x80+0x40+4,Hour_L);
				SetTime_dispaly(0x80+0x40+7,Minute_L);
				SetTime_dispaly(0x80+0x40+10,Second_L);
				write_com(0x80+0x40+4);		//光标闪烁地址，停留在时的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==2)
			{
				write_com(0x80+0x40+7);		//光标闪烁地址，停留在分的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==3)
			{
				write_com(0x80+0x40+10);	//光标闪烁地址，停留在秒的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==4)
			{
				write_com(0x0c);			//光标不再闪烁
				LCD_Clean(1);
				LCD_Clean(2);
				LCD_Display_String(1,table8);
				LCD_Display_String(2,table7);	
				SetTime_dispaly(0x80+0x40+4,Hour_H);
				SetTime_dispaly(0x80+0x40+7,Minute_H);
				SetTime_dispaly(0x80+0x40+10,Second_H);
				write_com(0x80+0x40+4);		//光标闪烁地址，停留在时的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==5)
			{
				write_com(0x80+0x40+7);		//光标闪烁地址，停留在分的位置上					
				write_com(0x0f);			//光标闪烁
			}
			if(Enter_num==6)
			{
				write_com(0x80+0x40+10);	//光标闪烁地址，停留在秒的位置上					
				write_com(0x0f);			//光标闪烁
			}		
			if(Enter_num==7)
			{
				Enter_num=0;
				write_com(0x0c);			//光标不再闪烁
				LCD_Clean(1);
				LCD_Clean(2);
				LCD_Display_String(1,table1);
				LCD_Display_String(2,table2);
				Select_num=0;
				Adjust=0;
			}
		}

	}
	if(UP==0)
	{
		delay1(2);
		if(UP==0)
		{
			while(!UP);
			if(Select_num==1)
			{
				if(Enter_num==1)
				{
					Year++;
					if(Year>99)
					Year=0;
					SetTime_dispaly(0x80+8,Year);
					write_com(0x80+8);								
					write_com(0x0f);			
				}
				if(Enter_num==2)
				{
					Month++;
					if(Month>12)
					Month=1;
					SetTime_dispaly(0x80+11,Month);
					write_com(0x80+11);								
					write_com(0x0f);							  
				}
				if(Enter_num==3)
				{
					Day++;
					if(Day>31)
					Day=1;
					SetTime_dispaly(0x80+14,Day);
					write_com(0x80+14);								
					write_com(0x0f);
				}
				if(Enter_num==4)
				{
					Hour++;
					if(Hour>23)
					Hour=0;
					SetTime_dispaly(0x80+0x40+6,Hour);
					write_com(0x80+0x40+6);								
					write_com(0x0f);
				}
				if(Enter_num==5)
				{
					Minute++;
					if(Minute>59)
					Minute=0;
					SetTime_dispaly(0x80+0x40+9,Minute);
					write_com(0x80+0x40+9);								
					write_com(0x0f);
				}
				if(Enter_num==6)
				{
					Second++;
					if(Second>59)
					Second=0;
					SetTime_dispaly(0x80+0x40+12,Second);
					write_com(0x80+0x40+12);								
					write_com(0x0f);
				}
				if(Enter_num==7)
				{
					Week++;
					if(Week>7)
					Week=1;
					Week_dispaly(0x80+0x40+15,Week);
					write_com(0x80+0x40+15);								
					write_com(0x0f);
				}
			}
			if(Select_num==2)
			{				
				if(Enter_num==1)
				{
					Hour_L++;
					if(Hour_L>23)
					Hour_L=0;
					SetTime_dispaly(0x80+0x40+4,Hour_L);
					write_com(0x80+0x40+4);								
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					Minute_L++;
					if(Minute_L>59)
					Minute_L=0;
					SetTime_dispaly(0x80+0x40+7,Minute_L);
					write_com(0x80+0x40+7);								
					write_com(0x0f);
				}
				 
				if(Enter_num==3)
				{
					Second_L++;
					if(Second_L>59)
					Second_L=0;
					SetTime_dispaly(0x80+0x40+10,Second_L);
					write_com(0x80+0x40+10);								
					write_com(0x0f);
				}
				if(Enter_num==4)
				{
					Hour_H++;
					if(Hour_H>23)
					Hour_H=0;
					SetTime_dispaly(0x80+0x40+4,Hour_H);
					write_com(0x80+0x40+4);								
					write_com(0x0f);
				}
				if(Enter_num==5)
				{
					Minute_H++;
					if(Minute_H>59)
					Minute_H=0;
					SetTime_dispaly(0x80+0x40+7,Minute_H);
					write_com(0x80+0x40+7);								
					write_com(0x0f);
				}
				 
				if(Enter_num==6)
				{
					Second_H++;
					if(Second_H>59)
					Second_H=0;
					SetTime_dispaly(0x80+0x40+10,Second_H);
					write_com(0x80+0x40+10);								
					write_com(0x0f);
				}				
			}
		}
	}
	if(DOWN==0)
	{
		delay1(2);
		if(DOWN==0)
		{
			while(!DOWN);
			if(Select_num==1)
			{
				if(Enter_num==1)
				{
					Year--;
					if(Year<0)
					Year=99;
					SetTime_dispaly(0x80+8,Year);
					write_com(0x80+8);								
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					Month--;
					if(Month<1)
					Month=12;
					SetTime_dispaly(0x80+11,Month);
					write_com(0x80+11);								
					write_com(0x0f);
				}
				if(Enter_num==3)
				{
					Day--;
					if(Day<1)
					Day=31;
					SetTime_dispaly(0x80+14,Day);
					write_com(0x80+14);								
					write_com(0x0f);
				}
				if(Enter_num==4)
				{
					Hour--;
					if(Hour<0)
					Hour=23;
					SetTime_dispaly(0x80+0x40+6,Hour);
					write_com(0x80+0x40+6);								
					write_com(0x0f);
				}
				if(Enter_num==5)
				{
					Minute--;
					if(Minute<0)
					Minute=59;
					SetTime_dispaly(0x80+0x40+9,Minute);
					write_com(0x80+0x40+9);								
					write_com(0x0f);
				}
				if(Enter_num==6)
				{
					Second--;
					if(Second<0)
					Second=59;
					SetTime_dispaly(0x80+0x40+12,Second);
					write_com(0x80+0x40+12);								
					write_com(0x0f);
				}
				if(Enter_num==7)
				{
					Week--;
					if(Week<1)
					Week=7;
					Week_dispaly(0x80+0x40+15,Week);
					write_com(0x80+0x40+15);								
					write_com(0x0f);
				}		
			}

			if(Select_num==2)
			{
				if(Enter_num==1)
				{
					Hour_L--;
					if(Hour_L<0)
					Hour_L=23;
					SetTime_dispaly(0x80+0x40+4,Hour_L);
					write_com(0x80+0x40+4);								
					write_com(0x0f);
				}
				if(Enter_num==2)
				{
					Minute_L--;
					if(Minute_L<0)
					Minute_L=59;
					SetTime_dispaly(0x80+0x40+7,Minute_L);
					write_com(0x80+0x40+7);								
					write_com(0x0f);
				}
				if(Enter_num==3)
				{
					Second_L--;
					if(Second_L<0)
					Second_L=59;
					SetTime_dispaly(0x80+0x40+10,Second_L);
					write_com(0x80+0x40+10);								
					write_com(0x0f);
				}
				 
				if(Enter_num==4)
				{
					Hour_H--;
					if(Hour_H<0)
					Hour_H=23;
					SetTime_dispaly(0x80+0x40+4,Hour_H);
					write_com(0x80+0x40+4);								
					write_com(0x0f);
				}
				if(Enter_num==5)
				{
					Minute_H--;
					if(Minute_H<0)
					Minute_H=59;
					SetTime_dispaly(0x80+0x40+7,Minute_H);
					write_com(0x80+0x40+7);								
					write_com(0x0f);
				}
				if(Enter_num==6)
				{
					Second_H--;
					if(Second_H<0)
					Second_H=59;
					SetTime_dispaly(0x80+0x40+10,Second_H);
					write_com(0x80+0x40+10);								
					write_com(0x0f);
				}
			}
		}
	}

	if(MODE==0)							//切换按键按下
	{
		delay1(2);							//消抖
		if(MODE==0)
		{
			while(!MODE);					//按键释放
			Auto_flg=~Auto_flg;				//模式切换
			if(Auto_flg==0)					//自动模式
			{
				AUTO_LED=0;					//自动灯亮
				HAND_LED=1;
			}
			if(Auto_flg==1)					//手动模式
			{
				AUTO_LED=1;					
				HAND_LED=0;					//手动灯亮
				LAMP1=1;					//关灯
				LAMP2=1;
				LAMP3=1;
				LAMP4=1;
			}
		}
	}
	if(Auto_flg==1)							//手动模式
	{
		if(SWITCH==0)						//灯开关按键按下
		{
			delay1(2);						//消抖
			if(SWITCH==0)
			{
				while(!SWITCH);				//按键释放
				Switch_num++;				//按下次数增加
				if(Switch_num==1)			//按下一次，开一盏灯
				{
					LAMP1=0;
					LAMP2=1;
					LAMP3=1;
					LAMP4=1;
				}
				if(Switch_num==2)			//按下二次，开二盏灯
				{
					LAMP1=0;
					LAMP2=0;
					LAMP3=1;
					LAMP4=1;
				}
				if(Switch_num==3)			//按下3次，开3盏灯
				{
					LAMP1=0;
					LAMP2=0;
					LAMP3=0;
					LAMP4=1;
				}
				if(Switch_num==4)			//按下4次，开4盏灯
				{
					LAMP1=0;
					LAMP2=0;
					LAMP3=0;
					LAMP4=0;
				}
				if(Switch_num==5)			//按下5次，开0盏灯
				{
					LAMP1=1;
					LAMP2=1;
					LAMP3=1;
					LAMP4=1;
					Switch_num=0;			//次数清除
				}
			}
		}
	}
}

void Open(void)
{
	float Now;
	float Set_L,Set_H;
	Now=Hour+Minute/60+Second/3600;				  		//把实时时间化成小时为单位
	Set_L=Hour_L+Minute_L/60+Second_L/3600;		  		//把设置下限时间化成小时为单位
	Set_H=Hour_H+Minute_H/60+Second_H/3600;		  		//把设置上限时间化成小时为单位
	if(Auto_flg==0)										//自动模式
	{
		if((Now>=Set_L)&(Now<=Set_H))				  	//实时时间在上下限时间范围内
		{
			if(LIGHT1==0)							    //光线不足
			{
				if(Value1==0)
				{
					LAMP1=1;							//全灭
					LAMP2=1;
					LAMP3=1;
					LAMP4=1;
				}
				if((Value1>0)&(Value1<10))
				{
					LAMP1=0;							//亮一个灯
					LAMP2=1;
					LAMP3=1;
					LAMP4=1;
				}
				if((Value1>=10)&(Value1<20))
				{
					LAMP1=0;							//亮二个灯
					LAMP2=0;
					LAMP3=1;
					LAMP4=1;
				}
				if((Value1>=20)&(Value1<30))
				{
					LAMP1=0;							//亮三个灯
					LAMP2=0;
					LAMP3=0;
					LAMP4=1;
				}
				if(Value1>=30)
				{
					LAMP1=0;							//亮四个灯
					LAMP2=0;
					LAMP3=0;
					LAMP4=0;
				}
			}
			if(LIGHT1==1)							    //光线强烈
			{
				LAMP1=1;								//全灭
				LAMP2=1;
				LAMP3=1;
				LAMP4=1;
			}
		}
	
		if((Now<Set_L)|(Now>Set_H))					  	//实时时间不在范围内
		{
			LAMP1=1;									//全灭
			LAMP2=1;
			LAMP3=1;
			LAMP4=1;
		}
	}
} 

/*******************************************************************************************************/
void Count_Value(void)
{
	SetTime_dispaly(0x80+3,Value1);
	if(IN1==0)
	{
		delay1(1);
		if(IN1==0)
		{
			while(!IN1);
			if(OUT1_flag==0)
			{
				IN1_flag=1;
				TR0=1;					 //开启定时器 防止误触发
			}
			if(OUT1_flag==1)
			{
				
				Value1++;				 //人数增加
				if(Value1>=99)			 //如果人数计数到99，变为最大99
				Value1=99;
				OUT1_flag=0;			 //清零
				IN1_flag=0;
				TR0=0;					 //关闭定时器
				Count=0;
				Miao=0;
			}
		}
	}
	if(OUT1==0)
	{
		delay1(1);
		if(OUT1==0)
		{
			while(!OUT1);
			if(IN1_flag==0)
			{
				OUT1_flag=1;
				TR0=1;					 //开启定时器 防止误触发
			}
			if(IN1_flag==1)
			{
				Value1--;				//人数减少
				if(Value1<0)			//如果人数计数小于0，变为最小0
				Value1=0;
				IN1_flag=0;				//清零
				OUT1_flag=0;
				TR0=0;					 //关闭定时器
				Count=0;
				Miao=0;
			}
		}
	}
}
/****************************************温度************************************************************/
/*void delay(uint y)		   							//DS18B20延时函数（仿真用）
{
	while(y--);
} 
*/
void delay(uint z)			  							//DS18B20延时函数（实物用）
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=1;y>0;y--);
}  

void write_byte(uint dat)			//写一个字节
{
	uchar i;
	for(i=0;i<8;i++)		        //循环八次	  共有八位
	{
		DQ=0;						//写零
		DQ=dat&0x01;                //向总线写位数据，从最低位写起
		delay(4);
		DQ=1;						//写一
		dat>>=1;				    //下次写作准备，移位数据
	}
	delay(4);
}				     

uchar read_byte(void)				//读一个字节，返回值
{
	uchar i;
	uint value;
	for(i=0;i<8;i++)		        //循环八次	  共有八位
	{
		DQ=0;
		value>>=1;
		DQ=1;						//释放总线
		if(DQ)
		value|=0x80;			    //DQ=1，value取1
		delay(4);
	}
	return value;
}

void ds18b20_init()				    //初始化函数
{
	uint n;
	DQ=1;
	delay(8);					   
	DQ=0;
	delay(80);					    //低电平480——960us
	DQ=1;							//总线释放
	delay(8);					    //等待50——100us
	n=DQ;						    //读取复位状态
	delay(4);
}

int readtemperature()				//读整数部分
{	
    int a,b;						//高八位，低八位
	ds18b20_init();
	write_byte(0xcc);			    //跳过rom匹配，跳过读序列号的操作，可节省操作时间
	write_byte(0x44);			    //启动温度检测
	delay(300);					    //测温度
	ds18b20_init();				    //开始操作前需要复位
	write_byte(0xcc);				//跳过rom匹配
	write_byte(0xbe);			    //写读寄存器中温度值的命令
	a=read_byte();				    //low	  低位开始读取
	b=read_byte();				    //high	  高位开始读取
	b<<=8;							//把高位左移八位
	b=b|a;							//高八位与第八位进行或运算，得到总和
	if(b>=0)						//如果温度大于等于0
	{
		b=b*0.0625;					//直接乘以0.0625
		write_com(0x80+0x40+2);		//在LCD1602对应的位置不写内容
		write_date(0x20);
	}
    else							//温度小于0
	{	
		b=~b+1;						//取反码再加一
		b=b*0.0625;					//再运算
		write_com(0x80+0x40+2);		//在LCD1602对应的位置写一个负号
		write_date(0x2d);
	}
	return b;						//返回b，此时以表示温度
}
void temperature_dispaly(char add,char dat)	 		//温度显示函数：第一个：参数的地址，第二个：参数的内容
{
	uchar shi,ge;
	shi=dat/10;								 		//把温度的十位提取出来
	ge=dat%10;								 		//把温度的个位提取出来
	write_com(0x80+0x40+add);			   	 		//要写的地址
	write_date(0x30+shi);				   	 		//十位的内容   1602字符库
	write_date(0x30+ge);				     		//个位的内容	1602字符库
}
/*****************************************定时器初始化和主函数***********************************************************/
void T0_init(void)
{
	TMOD=0x11;				   	   //设置定时器0为工作方式1
	TH0=(65536-50000)/256;   	   //50MS定时
	TL0=(65536-50000)%256;
	EA=1;					   	   //开定时器T0的中断	总中断
	ET0=1;						   //允许T0中断
//	TR0=1;						   //开启定时器
}
void main()
{
	AUTO_LED=0;					 //默认自动模式
	temperature=readtemperature();
	init();						 //液晶初始化
	T0_init();					 //定时器初始化
	delay1(8000);
	while(1)
	{
		if(Adjust==0)			 //非调节模式下显示时间人数和温度
		{	
			temperature=readtemperature();			//读取温度
			temperature_dispaly(3,temperature);		//显示温度
			Time_Display();		 //显示时间
			Open();		 		 //动作判断,根据光线和声响开关
			Count_Value();		 //计算人数
		}
		Keyscan();				 //扫描键盘
	}
}
void Timer0() interrupt 1					//定时器T0中断函数
{
	TH0=(65536-50000)/256;					//赋初值=50ms
	TL0=(65536-50000)%256;					//TH0=3C,TL0=B0
	Count++;								//每中断一次，Count加一，计数20次，表示1s的时间到
	if(Count>=20)							//1s到
	{
		Count=0;
		Miao++;
		if(Miao>=3)							//3秒内无触发另一个，全部清零
		{
			Miao=0;
			OUT1_flag=0;			 		//清零
			IN1_flag=0;
			TR0=0;					 		//关闭定时器
		}
	}

}