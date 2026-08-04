#include <reg52.h>	         //调用单片机头文件
#define uchar unsigned char  //无符号字符型 宏定义	变量范围0~255
#define uint  unsigned int	 //无符号整型 宏定义	变量范围0~65535
#include "eeprom52.h"
#include "nongli.h"

bit flag_200ms ;
bit flag_100ms ;
sbit beep = P3^7;	  //蜂鸣器定义
bit flag_beep_en;
uint clock_value;     //用作闹钟用的

sbit dq   = P3^1;	  //18b20 IO口的定义


uint temperature ;    //温度变量
uchar flag_nl;        //农历 阳历显示标志位


uchar menu_1,menu_2;

uchar key_time,flag_value;      //用做连加的中间变量
bit key_500ms  ;
uchar n_nian,n_yue,n_ri;		//农历显示的函数



#include "ds1302.h"
#include "lcd1602.h"



/******************把数据保存到单片机内部eeprom中******************/
void write_eeprom()
{
	SectorErase(0x2000);
	byte_write(0x2000, fen1);
	byte_write(0x2001, shi1);
	byte_write(0x2002, open1);
	byte_write(0x2058, a_a);	
}

/******************把数据从单片机内部eeprom中读出来*****************/
void read_eeprom()
{
	fen1  = byte_read(0x2000);
	shi1  = byte_read(0x2001);
	open1  = byte_read(0x2002);
	a_a      = byte_read(0x2058);
}

/**************开机自检eeprom初始化*****************/
void init_eeprom()
{
	read_eeprom();		//先读
	if(a_a != 1)		//新的单片机初始单片机内问eeprom
	{
		fen1  = 3;
		shi1  = 8;
		open1  = 1;
		a_a = 1;
		write_eeprom();	   //保存数据
	}	
}

/***********************18b20初始化函数*****************************/
void init_18b20()
{
	bit q;
	dq = 1;				//把总线拿高
	delay_uint(1);	    //15us
	dq = 0;				//给复位脉冲
	delay_uint(80);		//750us
	dq = 1;				//把总线拿高 等待
	delay_uint(10);		//110us
	q = dq;				//读取18b20初始化信号
	delay_uint(20);		//200us
	dq = 1;				//把总线拿高 释放总线
}

/*************写18b20内的数据***************/
void write_18b20(uchar dat)
{
	uchar i;
	for(i=0;i<8;i++)
	{					 //写数据是低位开始
		dq = 0;			 //把总线拿低写时间隙开始 
		dq = dat & 0x01; //向18b20总线写数据了
		delay_uint(5);	 // 60us
		dq = 1;			 //释放总线
		dat >>= 1;
	}	
}

/*************读取18b20内的数据***************/
uchar read_18b20()
{
	uchar i,value;
	for(i=0;i<8;i++)
	{
		dq = 0;			 //把总线拿低读时间隙开始 
		value >>= 1;	 //读数据是低位开始
		dq = 1;			 //释放总线
		if(dq == 1)		 //开始读写数据 
			value |= 0x80;
		delay_uint(5);	 //60us	读一个时间隙最少要保持60us的时间
	}
	return value;		 //返回数据
}

/*************读取温度的值 读出来的是小数***************/
uint read_temp()
{
	uint value;
	uchar low;			   //在读取温度的时候如果中断的太频繁了，就应该把中断给关了，否则会影响到18b20的时序
	init_18b20();		   //初始化18b20
	write_18b20(0xcc);	   //跳过64位ROM
	write_18b20(0x44);	   //启动一次温度转换命令
	delay_uint(50);		   //500us

	init_18b20();		   //初始化18b20
	
	write_18b20(0xcc);	   //跳过64位ROM
	write_18b20(0xbe);	   //发出读取暂存器命令
	
	EA = 0;
	low = read_18b20();	   //读温度低字节
	value = read_18b20();  //读温度高字节
	EA = 1;
	value <<= 8;		   //把温度的高位左移8位
	value |= low;		   //把读出的温度低位放到value的低八位中
	value *= 0.625;	       //转换到温度值 小数
	return value;		   //返回读出的温度 带小数
}



/******************1ms 延时函数*******************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}

/******************写星期函数*******************/
void write_week(uchar hang,uchar add,uchar week)//写星期函数
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);	  
	switch(week)
	{
		case 1:write_data('M');//星期数为1时，显示
			   write_data('O');
			   write_data('N');
			   break;
	   
		case 2:write_data('T');//星期数据为2时显示
			   write_data('U');
			   write_data('E');
			   break;
		
		case 3:write_data('W');//星期数据为3时显示
			   write_data('E');
			   write_data('D');
			   break;
		
		case 4:write_data('T');//星期数据为4是显示
			   write_data('H');
			   write_data('U');
			   break;
		
		case 5:write_data('F');//星期数据为5时显示
			   write_data('R');
			   write_data('I');
			   break;
		
		case 6:write_data('S');//星期数据为6时显示
			   write_data('T');
			   write_data('A');
			   break;
		
		case 0:write_data('S');//星期数据为7时显示
			   write_data('U');
			   write_data('N');
			   break;
	}
}

/*************时钟显示***************/
void init_1602_ds1302()
{
	write_sfm2_ds1302(1,1,shi);		   //显示时
	write_sfm2_ds1302(1,4,fen);		   //显示分
	write_sfm2_ds1302(1,7,miao);	   //显示秒
	write_week(2,12,week);
//	write_sfm1(1,14,week);			   //显示星期
	write_sfm3_18B20(1,11,temperature);	   //显示温度
	if(flag_nl == 0)  //显示阳历
	{
		write_sfm2_ds1302(2,2,nian);   //显示年
		write_sfm2_ds1302(2,5,yue);	   //显示月	
		write_sfm2_ds1302(2,8,ri);	   //显示日 	
	}
	else 			 //显示农历
	{
		write_sfm2_ds1302(2,2,n_nian);	//显示年
		write_sfm2_ds1302(2,5,n_yue);	//显示月	
		write_sfm2_ds1302(2,8,n_ri);	//显示日 
		
	}
	

	
		
}									   

/*************定时器0初始化程序***************/
void init_time0()	  
{
	EA   = 1;	 	  //开总中断
	TMOD = 0X01;	  //定时器0、工作方式1
	ET0  = 1;		  //开定时器0中断 
	TR0  = 1;		  //允许定时器0定时
}

/*************闹钟报警函数***************/
void menu_dis()
{
	static uchar mm,value;
	if(flag_100ms == 1)		  //100ms执行一次
	{
		flag_100ms = 0;
		if(open1 == 1)	//如果闹钟打开
		{
			if((miao == 0) && (fen == fen1) && (shi == shi1)) 
			{		
				flag_beep_en = 1;	//有报警 打开蜂鸣器响的标志位				
			}		
			if(flag_beep_en == 1)	//闹钟以被打开
			{
				clock_value++;
				if(clock_value <= 30)	 
					beep = ~beep;	   //蜂鸣器叫3秒
				else if(clock_value > 30)
				{
					beep = 1;		    //蜂鸣器停1秒
					if(clock_value > 40)
					{
						clock_value = 0;

					}
				}
				//  1 分钟后自动关闭闹钟
				value ++;
				if(value >= 10)
				{
					value = 0;
					mm++;
					if(mm >= 60)
					{
						mm = 0;
						flag_beep_en = 0;
						beep = 1;	
					}
				}					
			}
		}	
	}
}


/********************独立按键程序*****************/
uchar key_can;	 //按键值

void key()	 //独立按键程序
{
	static uchar key_new;
	key_can = 20;                   //按键值还原
	P3 |= 0x78;                     //对应的按键IO口输出为1
	if((P3 & 0x78) != 0x78)		//按键按下
	{
		delay_1ms(1);	     	//按键消抖动
		if(((P3 & 0x78) != 0x78) && (key_new == 1))
		{						//确认是按键按下
			key_new = 0;
			switch(P3 & 0x78)
			{
				case 0x70:  key_can = 4;  break;	 //得到按键值
				case 0x68:  key_can = 3;  break;	 //得到按键值
				case 0x58:  key_can = 2;  break;	 //得到按键值
				case 0x38:  key_can = 1;  break;	 //得到按键值
			}
//			write_sfm2(1,0,key_can);				 //显示按键值
		}			
	}
	else 
		key_new = 1;	
}


/**********************设置函数************************/
void key_with()
{
	if(key_can == 1)	//设置键
	{
		menu_1++;
		if(menu_1 == 1)	  //设置时间
		{
			menu_2 = 1;
			write_string(1,0,"    :  :    W:  ");			
			write_string(2,0," 20  -  -       ");	
		}
		if(menu_1 == 2)	  //设置闹钟
		{
			menu_2 = 1;
			write_string(1,0,"   set clock    ");			
			write_string(2,0,"    Y  00:00      ");	
		}
		if(menu_1 > 2)    //回到正常显示
		{
			menu_1 = 0;
			write_guanbiao(1,2,0);	 //关闭光标
			init_1602_dis_csf();      //初始化液晶显示		
		}
	}
	if(key_can == 2)	//选择键
	{
		flag_200ms = 1;
		if(menu_1 == 1)		  //设置时间
		{
			menu_2 ++;
			if(menu_2 > 7)
				menu_2 = 1;
		}
		if(menu_1 == 2)		 //设置闹钟
		{
			menu_2 ++;
			if(menu_2 > 3)
				menu_2 = 1;				
		}
	}
	if(menu_1 == 1)
	{
		if(menu_2 == 1)		  //设置时
		{
			if(key_can == 3)	//加
			{
				shi+=0x01;
				if((shi & 0x0f) >= 0x0a)
					shi = (shi & 0xf0) + 0x10;
				if(shi >= 0x24)
					shi = 0;
			}		
			if(key_can == 4)	//减
			{
				if(shi == 0x00)
					shi = 0x24;
				if((shi & 0x0f) == 0x00)
					shi = (shi | 0x0a) - 0x10;
				shi -- ; 
			}	  				
		}
		if(menu_2 == 2)		  //设置分
		{
			if(key_can == 3)	//加
			{
				fen+=0x01;
				if((fen & 0x0f) >= 0x0a)
					fen = (fen & 0xf0) + 0x10;
				if(fen >= 0x60)
					fen = 0;
			}		
			if(key_can == 4)	//减	  
			{
				if(fen == 0x00)
					fen = 0x5a;
				if((fen & 0x0f) == 0x00)
					fen = (fen | 0x0a) - 0x10;
				fen -- ;
			}	
		}
		if(menu_2 == 3)		  //设置秒
		{
			if(key_can == 3)	//加
			{
				miao+=0x01;
				if((miao & 0x0f) >= 0x0a)
					miao = (miao & 0xf0) + 0x10;
				if(miao >= 0x60)
					miao = 0;
			}	
			if(key_can == 4)	//减	  
			{
				if(miao == 0x00)
					miao = 0x5a;
				if((miao & 0x0f) == 0x00)
					miao = (miao | 0x0a) - 0x10;
				miao -- ;			
			}
		}
		if(menu_2 == 4)		  //设置星期
		{
			if(key_can == 3)	//加
			{
	    		week+=0x01;
				if((week & 0x0f) >= 0x0a)
					week = (week & 0xf0) + 0x10;
				if(week >= 0x08)
					week = 1;
			}		
			if(key_can == 4)	//减	  
			{
				if(week == 0x01)
					week = 0x08;
				if((week & 0x0f) == 0x00)
					week = (week | 0x0a) - 0x10;
				week -- ;
			}	
		}
		if(menu_2 == 5)		  //设置年
		{
			if(key_can == 3)	//加
			{
		    	nian+=0x01;
				if((nian & 0x0f) >= 0x0a)
					nian = (nian & 0xf0) + 0x10;
				if(nian >= 0x9a)
					nian = 1;
			}		
			if(key_can == 4)	//减	  
			{
				if(nian == 0x01)
					nian = 0x9a;
				if((nian & 0x0f) == 0x00)
					nian = (nian | 0x0a) - 0x10;
				nian -- ;		
			}	
		}
		if(menu_2 == 6)		  //设置月
		{
			if(key_can == 3)	//加
			{
		    	yue+=0x01;
				if((yue & 0x0f) >= 0x0a)
					yue = (yue & 0xf0) + 0x10;
				if(yue >= 0x13)
					yue = 1;
			}		
			if(key_can == 4)	//减	  
			{
				if(yue == 0x01)
					yue = 0x13;
				if((yue & 0x0f) == 0x00)
					yue = (yue | 0x0a) - 0x10;
				yue -- ;					
			}	
		}
		if(menu_2 == 7)		  //设置日
		{
			if(key_can == 3)	//加
			{
	    	ri+=0x01;
			if((ri & 0x0f) >= 0x0a)
				ri = (ri & 0xf0) + 0x10;
			if(ri >= 0x32)
				ri = 0;			
			}		
			if(key_can == 4)	//减	  
			{
				if(ri == 0x01)
					ri = 0x32;
				if((ri & 0x0f) == 0x00)
					ri = (ri | 0x0a) - 0x10;
				ri -- ;			
			}	
		}
		write_sfm2_ds1302(1,2,shi);	   //显示时
		write_sfm2_ds1302(1,5,fen);	   //显示分
		write_sfm2_ds1302(1,8,miao);	   //显示秒
		write_sfm1(1,14,week);	   //显示星期					
		write_sfm2_ds1302(2,3,nian);	   //显示年
		write_sfm2_ds1302(2,6,yue);	   //显示月
		write_sfm2_ds1302(2,9,ri);	   //显示日
		switch(menu_2)	   // 光标显示
		{
			case 1:  write_guanbiao(1,2,1);  break;
			case 2:  write_guanbiao(1,5,1);  break;
			case 3:  write_guanbiao(1,8,1);  break;
			case 4:  write_guanbiao(1,14,1);  break;
			case 5:  write_guanbiao(2,3,1);  break;
			case 6:  write_guanbiao(2,6,1);  break;
			case 7:  write_guanbiao(2,9,1);  break;
		}
		write_time();	   //把时间写进去
	}	
/***************设置闹钟*********************/
	if(menu_1 == 2)
	{
		if(menu_2 == 1)		  //设置闹钟开关
		{
			if(key_can == 3)	
			{
				open1 = 1;	  //闹钟开
			}		
			if(key_can == 4)	
			{
				open1 = 0;	  //闹钟关
			}	  				
		}
		if(menu_2 == 2)		  //设置闹钟时
		{
			if(key_can == 3)	//加
			{
		    	shi1+=0x01;
				if((shi1 & 0x0f) >= 0x0a)
					shi1 = (shi1 & 0xf0) + 0x10;
				if(shi1 >= 0x24)
					shi1 = 0;
			}		
			if(key_can == 4)	//减	  
			{
				if(shi1 == 0x00)
					shi1 = 0x5a;
				if((shi1 & 0x0f) == 0x00)
					shi1 = (shi1 | 0x0a) - 0x10;
				shi1 -- ;
			}	
		}
		if(menu_2 == 3)		  //设置秒
		{
			if(key_can == 3)	//加
			{
	    		fen1+=0x01;
				if((fen1 & 0x0f) >= 0x0a)
					fen1 = (fen1 & 0xf0) + 0x10;
				if(fen1 >= 0x60)
					fen1 = 0;
			}	
			if(key_can == 4)	//减	  
			{
				if(fen1 == 0x00)
					fen1 = 0x5a;
				if((fen1 & 0x0f) == 0x00)
					fen1 = (fen1 | 0x0a) - 0x10;
				fen1 -- ;			
			}
		}
		if(open1 == 1)
			write_string(2,4,"Y");	
		else 
			write_string(2,4,"N");	
		write_sfm2_ds1302(2,7,shi1);	   //显示闹钟时
		write_sfm2_ds1302(2,10,fen1);	   //显示闹钟分
		switch(menu_2)	   // 光标显示
		{
			case 1:  write_guanbiao(2,4,1);  break;
			case 2:  write_guanbiao(2,7,1);  break;
			case 3:  write_guanbiao(2,10,1);  break;
		}	
		write_eeprom();     //保存闹钟时间
	}			
}

/*****************主函数********************/
void main()
{	
	beep = 0;		                //开机叫一声   
	delay_1ms(150);
	P0 = P1 = P2 = P3 = 0xff;		//单片机IO口初始化为1
	init_time0();		 //初始化定时器
	init_ds1302();		 //ds1302初始化
	init_1602();		 //lcd1602初始化
	init_1602_dis_csf(); //lcd1602初始化显示
	init_eeprom();       //开始初始化保存的数据
	temperature = read_temp();	//先读出温度的值
	delay_1ms(650);
	temperature = read_temp();	//先读出温度的值
	while(1)
	{
		key();			 //按键程序
		if(key_can < 10)
		{
			if(flag_beep_en == 0)  //只有闹钟关了的时候才能进入设置
				key_with();	
			else 
			{
				flag_beep_en = 0;	 //按下任意键可关闭闹钟
				beep = 1;
				clock_value = 0;
				read_time();
			}
			if(menu_1 == 0)
			{
				if(key_can == 4)  //减键
				{
					flag_nl = ~flag_nl;		  //切换农历和阳历的显示标志位	
					if(flag_nl == 0)
						write_string(2,0,"2013");
					else 	
						write_string(2,0,"N ");
				}
			}
		}
		
		if(flag_200ms == 1)
		{	
			flag_200ms = 0;
			if(menu_1 == 0)
			{
				read_time();		  //读时间
				temperature = read_temp();	//先读出温度的值
				init_1602_ds1302();   //显示时钟
			}
		}
		menu_dis();	   //闹钟报警函数
		delay_1ms(1);
	}
}

/**************定时器0中断程序*****************/
void time0() interrupt 1
{			 
	static uchar value;
	TH0 = 0X3C;
	TL0 = 0XB0;	   //50ms
	value ++;	   
	if((value % 2) == 0)	 //100ms  
		flag_100ms = 1;	 
	if(value >= 6)			 //200ms
	{
		value = 0;
		flag_200ms = 1;
	}
}

