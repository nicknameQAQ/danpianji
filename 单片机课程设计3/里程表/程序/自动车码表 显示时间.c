#include <reg52.h>	         //调用单片机头文件
#define uchar unsigned char  //无符号字符型 宏定义	变量范围0~255
#define uint  unsigned int	 //无符号整型 宏定义	变量范围0~65535
#include "eeprom52.h"

/******************
	ds1302 内部RAM   RAM0  1100 000R/W   1读 0写
					 RAM1  1100 001R/W  
					       ....... 
					 RAM30 1111 110R/W   
********************/

sbit clk = P1^3;	  //ds1302时钟线定义
sbit io =  P1^4;	  //数据线
sbit rst = P1^5;	  //复位线
						//秒  分   时   日   月  年   星期 	
uchar code write_add[]={0x80,0x82,0x84,0x86,0x88,0x8c,0x8a};   //写地址
uchar code read_add[] ={0x81,0x83,0x85,0x87,0x89,0x8d,0x8b};   //读地址
uchar code init_ds[]  ={0x55,0x17,0x15,0x01,0x01,0x13,0x13};   
uchar miao,fen,shi,ri,yue,week,nian;
uchar i;

uchar t1_num,t2_num;	//计时间中断的次数
unsigned long speed1,juli,time2;
float f_hz	,speed_km,speed_m;
//dlaout time1	,speed_km,speed_m;
uchar TH11,TL11;
uchar flag_en;   //开始计算速度使能
uchar flag_stop_en;   //要确定车子是否停下了

uint juli_s;	     //每秒走的距离
uint juli_z;         //总路程
float zhijing = 0.55;  //直径 0.55M
bit flag_1s = 1;	     //1s
uchar menu_1;        //菜单设置变量
uchar menu_2;        //菜单设置变量

long zong_lc;        //总量程

uchar flag_200ms;
uint shudu;   //定义速度的变量
uint bj_shudu = 80;	 //报警速度


//这三个引脚参考资料
sbit rs=P1^0;	 //寄存器选择信号 H:数据寄存器  	L:指令寄存器
sbit rw=P1^1;	 //寄存器选择信号 H:数据寄存器  	L:指令寄存器
sbit e =P1^2;	 //片选信号   下降沿触发

uchar code table_num[]="0123456789abcdefg";
uchar i;

sbit beep = P3^7;   //蜂鸣器IO口定义



/******************1ms 延时函数*******************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}

/******************把数据保存到单片机内部eepom中******************/
void write_eeprom()
{
	
	SectorErase(0x2000);
	byte_write(0x2000, bj_shudu % 256);
	byte_write(0x2001, bj_shudu / 256);
	byte_write(0x2002, zong_lc % 256);
	byte_write(0x2003, zong_lc / 256 % 256);
	byte_write(0x2004, zong_lc / 256 / 256 % 256);

	byte_write(0x2055, a_a);	
}

/******************把数据从单片机内部eepom中读出来*****************/
void read_eeprom()
{
	uint value;
	bj_shudu  = byte_read(0x2001);
	bj_shudu <<= 8;
	bj_shudu  |= byte_read(0x2000);
	zong_lc  = byte_read(0x2004);
	zong_lc <<= 16;
	value  = byte_read(0x2003);
	zong_lc |= (value << 8);
	zong_lc  |= byte_read(0x2002);
	a_a     = byte_read(0x2055);
}

/**************开机初始化保存的数据*****************/
void init_eeprom()	 //开机初始化保存的数据*
{
	read_eeprom();		//先读
	if(a_a != 12)		//新的单片机初始单片机内问eeprom
	{
		bj_shudu = 50;
		a_a = 12;
		write_eeprom();	   //保存数据
	}	
}

/********************************************************************
* 名称 : delay_uint()
* 功能 : 小延时。
* 输入 : 无
* 输出 : 无
***********************************************************************/
void delay_uint(uint q)
{
	while(q--);
}

/********************************************************************
* 名称 : write_com(uchar com)
* 功能 : 1602命令函数
* 输入 : 输入的命令值
* 输出 : 无
***********************************************************************/
void write_com(uchar com)
{
	i =0;
	e=0;
	rs=0;
	rw=0;
	P0=com;
	delay_uint(25);
	e=1;
	delay_uint(50);
	e=0;
}

/********************************************************************
* 名称 : write_data(uchar dat)
* 功能 : 1602写数据函数
* 输入 : 需要写入1602的数据
* 输出 : 无
***********************************************************************/
void write_data(uchar dat)
{
	i =0;
	e=0;
	rs=1;
	rw=0;
	P0=dat;
	delay_uint(25);
	e=1;
	delay_uint(50);
	e=0;	
}

/********************************************************************
* 名称 : write_sfm2(uchar hang,uchar add,uchar date)
* 功能 : 显示2位十进制数，如果要让第一行，第五个字符开始显示"23" ，调用该函数如下
	 	 write_sfm1(1,5,23)
* 输入 : 行，列，需要输入1602的数据
* 输出 : 无
***********************************************************************/
void write_sfm2(uchar hang,uchar add,uint date)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
	write_data(0x30+date/10%10);
	write_data(0x30+date%10);	
}

/********************************************************************
* 名称 : write_sfm4(uchar hang,uchar add,uchar date)
* 功能 : 显示2位十进制数，如果要让第一行，第五个字符开始显示"23" ，调用该函数如下
	 	 write_sfm1(1,5,23)
* 输入 : 行，列，需要输入1602的数据
* 输出 : 无
***********************************************************************/
void write_sfm4(uchar hang,uchar add,uint date)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
	write_data(0x30+date/10000%10);
	write_data(0x30+date/1000%10);
	write_data('.');	
	write_data(0x30+date/100%10);	
	write_data(0x30+date/10%10);
	write_data(0x30+date%10);	
	write_data('k');	
	write_data('m');	
}

void write_sfm7(uchar hang,uchar add,uint date)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
	write_data(0x30+date/100000%10);
	write_data(0x30+date/100000%10);
	write_data(0x30+date/10000%10);
	write_data(0x30+date/1000%10);
	write_data('.');	
	write_data(0x30+date/100%10);	
	write_data(0x30+date/10%10);
	write_data('k');	
	write_data('m');	
}

/***********************lcd1602上显示两位十进制数************************/
void write_sfm1(uchar hang,uchar add,uchar date)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
	write_data(0x30+date % 10);	
}


/********************************************************************
* 名称 : write_string(uchar hang,uchar add,uchar *p)
* 功能 : 改变液晶中某位的值，如果要让第一行，第五个字符开始显示"ab cd ef" ，调用该函数如下
	 	 write_string(1,5,"ab cd ef;")
* 输入 : 行，列，需要输入1602的数据
* 输出 : 无
***********************************************************************/
void write_string(uchar hang,uchar add,uchar *p)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
		while(1)
		{
			if(*p == '\0')  break;
			write_data(*p);
			p++;
		}	
}


/***********************lcd1602上显示两位十进制数************************/
void write_sfm2_ds1302(uchar hang,uchar add,uchar date)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
	write_data(table_num[date / 16]);
	write_data(table_num[date % 16]);	
}

/*****************控制光标函数********************/
void write_guanbiao(uchar hang,uchar add,uchar date)
{		
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
	if(date == 1)
		write_com(0x0f);     //显示光标并且闪烁	
	else 
		write_com(0x0c);   //关闭光标
}	 



/********************************************************************
* 名称 : init_1602()
* 功能 : 初始化1602液晶 
* 输入 : 无
* 输出 : 无
***********************************************************************/
void init_1602()      //1602初始化
{
	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	delay_uint(1000);
	write_string(1,0,"sd:00km/h  00:00");		
	write_string(2,0,"lc:00.00km      ");	
}


 //	 E309R

/*************写一个数据到对应的地址里***************/
void write_ds1302(uchar add,uchar dat)
{		
	rst = 1;			 //把复位线拿高
	for(i=0;i<8;i++)
	{				     //低位在前
		clk = 0;		 //时钟线拿低开始写数据
		io = add & 0x01;    	
		add >>= 1;		 //把地址右移一位
		clk = 1;		 //时钟线拿高
	}	
	for(i=0;i<8;i++)
	{
		clk = 0;		 //时钟线拿低开始写数据
		io = dat & 0x01;
		dat >>= 1;		 //把数据右移一位
		clk = 1;		 //时钟线拿高
	}
	rst = 0;			 //复位线合低
	clk = 0;
	io = 0;
}

/*************从对应的地址读一个数据出来***************/
uchar read_ds1302(uchar add)
{
	uchar value,i;
	rst = 1;			 //把复位线拿高
	for(i=0;i<8;i++)
	{				     //低位在前
		clk = 0;		 //时钟线拿低开始写数据
		io = add & 0x01;    	
		add >>= 1;		 //把地址右移一位
		clk = 1;		 //时钟线拿高
	}		
	for(i=0;i<8;i++)
	{
		clk = 0;		 //时钟线拿低开始读数据
		value >>= 1;
		if(io == 1)
			value |= 0x80;
		clk = 1;		 //时钟线拿高
	}
	rst = 0;			 //复位线合低
	clk = 0;
	io = 0;
	return value;		 //返回读出来的数据
}



/*************把要的时间 年月日 都读出来***************/
void read_time()
{
	miao = read_ds1302(read_add[0]);	//读秒
	fen  = read_ds1302(read_add[1]);	//读分
	shi  = read_ds1302(read_add[2]);	//读时
	ri   = read_ds1302(read_add[3]);	//读日
	yue  = read_ds1302(read_add[4]);	//读月
	nian = read_ds1302(read_add[5]);	//读年
	week = read_ds1302(read_add[6]);	//读星期
}

/*************把要写的时间 年月日 都写入ds1302里***************/
void write_time()
{
	write_ds1302(0x8e,0x00);			//打开写保护
	write_ds1302(write_add[0],miao);	//写秒
	write_ds1302(write_add[1],fen);		//写分
	write_ds1302(write_add[2],shi);		//写时
	write_ds1302(write_add[3],ri);		//写日
	write_ds1302(write_add[4],yue);		//写月
	write_ds1302(write_add[5],nian);	//写星期
	write_ds1302(write_add[6],week);	//写年
	write_ds1302(0x8e,0x80);			//关闭写保护
}

/*************把数据保存到ds1302 RAM中**0-31*************/
void write_ds1302ram(uchar add,uchar dat)
{
	add <<= 1;     //地址是从第二位开始的
	add &= 0xfe;   //把最低位清零  是写的命令
	add |= 0xc0;   //地址最高两位为 1  
	write_ds1302(0x8e,0x00);
	write_ds1302(add,dat);	
	write_ds1302(0x8e,0x80);
}

/*************把数据从ds1302 RAM读出来**0-31*************/
uchar read_ds1302ram(uchar add)
{
	add <<= 1;     //地址是从第二位开始的
	add |= 0x01;   //把最高位置1  是读命令
	add |= 0xc0;   //地址最高两位为 1  
	return(read_ds1302(add));	
}




/*************初始化ds1302时间***************/
void init_ds1302()
{
	uchar i;
	rst = 0;	//第一次读写数据时要把IO品拿低
	clk = 0;
	io = 0;
	i = read_ds1302ram(30);   
	if(i != 3)
	{	
		i = 3;
		write_ds1302ram(30,i);
		write_ds1302(0x8e,0x00);	            //打开写保护
		for(i=0;i<7;i++)
			write_ds1302(write_add[i],init_ds[i]);	//把最高位值0 允许ds1302工作
		write_ds1302(0x8e,0x80);	//关写保护
	}
	write_sfm2(1,0,i);
}

void init_ds1302_io()
{
	rst = 0;	//第一次读写数据时要把IO品拿低
	clk = 0;
	io = 0;	
}

/*************定时器0初始化程序***************/
void init_1602_ds1302()
{
	if((miao <= 0x60) || (shi <= 0x24))
	{
	//	write_sfm2_ds1302(2,3,nian);	
	//	write_sfm2_ds1302(2,6,yue);	
	//	write_sfm2_ds1302(2,9,ri);	
		write_sfm2_ds1302(1,11,shi);	
		write_sfm2_ds1302(1,14,fen);	
	//	write_sfm2_ds1302(1,14,miao);
	//	write_sfm1(1,14,week);		
	}
}

/***********外部中断0初始化程序****************/
void init_int0()
{
	EX0=1;			  //允许外部中断0中断
	EA=1;	 		  //开总中断
	IT0 = 1; 		  //外部中断0负跳变中断
}


/*************定时器0初始化程序***************/
void time_init()	  //定时器0初始化程序 
{
	EA   = 1;	 	  //开总中断
	TMOD = 0X11;	  //定时器0、工作方式1
	ET0  = 1;		  //开定时器0中断 
	TR0  = 1;		  //允许定时器0定时
	ET1  = 1;		  //开定时器1中断 
	TR1  = 1;		  //允许定时器1定时
}

/***********计算速度函数**************/
void menu_dis()	  //计算速度函数
{
	static uchar value,value1;
	if(menu_1 == 0)
	{	
		if(flag_1s == 1)   
		{	
			flag_1s = 0; 		
			if(flag_en == 0)
			{
				value ++;
				if(value >= 3)	//2秒
				{	
					speed_km = 0;  //速度为0
					shudu = (uint)speed_km;	
					value = 0;				
				}
			}
			if((flag_en == 1))
			{	
				value = 0;		
				flag_en = 0;	 
				 //1s = 1 / 1000000us;	  // 1m/s=0.001km除以1/3600h=3.6km/h			
				f_hz = 1000000 / (t2_num * 65536.0 + TH11 * 256 + TL11);  //算出来就是秒
				t2_num = 0;	//把变量清零
				TH11 = 0;
				TL11 = 0;					   
				speed_m = f_hz * zhijing * 3.14  ;	//算出来的是m/s
				juli_z = (juli_z + (uint)speed_m) ;        //总路程m	
				speed_km = speed_m * 3.6 ;	//(带个小数点) km/s		
			    shudu = (uint)speed_km;
				if(shudu >= 99)
					shudu = 99;
				zong_lc += speed_m;
				value1++;
				if(value1 >= 20)
				{
					value1 = 0;
					write_eeprom();
				}
			}		

			write_sfm2(1,3,shudu);		
			write_sfm4(2,3,juli_z);	

		}
	}
}

/********************独立按键程序*****************/
uchar key_can;	 //按键值

void key()	 //独立按键程序
{
	static uchar key_new;
	key_can = 20;                   //按键值还原
	P2 |= 0x78;                     //对应的按键IO口输出为1
	if((P2 & 0x78) != 0x78)		//按键按下
	{
		delay_1ms(1);	     	//按键消抖动
		if(((P2 & 0x78) != 0x78) && (key_new == 1))
		{						//确认是按键按下
			key_new = 0;
			switch(P2 & 0x78)
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
		if(menu_1 == 2)	  //设置报警速度
		{
			menu_2 = 1;
			write_string(1,0,"set-sd:00km/h   ");			
			write_string(2,0,"zlc:             ");	
		}
		menu_2 = 1;

		if(menu_1 > 2)    //回到正常显示
		{
			menu_1 = 0;
			write_guanbiao(1,2,0);	  //关闭光标
			init_1602();      //1602初始化     //初始化液晶显示		
		}
	}
	if(key_can == 2)	//选择键
	{
		if(menu_1 == 1)		  //设置时间
		{
			menu_2 ++;
			if(menu_2 > 7)
				menu_2 = 1;
		}
		if(menu_1 == 2)		 //设置
		{
			menu_2 ++;
			if(menu_2 > 2)
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
			case 4:  write_guanbiao(1,14,1); break;
			case 5:  write_guanbiao(2,3,1);  break;
			case 6:  write_guanbiao(2,6,1);  break;
			case 7:  write_guanbiao(2,9,1);  break;
		}
		write_time();	   //把时间写进去
	}	

	if(menu_1 == 2)
	{		
		if(menu_2 == 1)		  //设置速度
		{
			if(key_can == 3)	//加
			{
				bj_shudu++;
				if(bj_shudu >= 99)
					bj_shudu = 99;
			}	
			if(key_can == 4)	//减	  
			{
				if(bj_shudu != 0)
					bj_shudu -- ;			
			}
		}	
		if(menu_2 == 2)		  //把总量程清零
		{
			if(key_can == 3)	
			{
				zong_lc= 0;
			}	
			if(key_can == 4)		  
			{
				zong_lc= 0;			
			}
		}	


		write_sfm2(1,7,bj_shudu);   //显示报警速度
		write_sfm7(2,4,zong_lc);   //显示
		
		switch(menu_2)	   // 光标显示
		{
			case 1:  write_guanbiao(1,6,1);  break;
			case 2:  write_guanbiao(2,3,1);  break;
		}
		write_eeprom();
	}
}

/****************报警函数***************/
void clock_h_l()
{
	static uchar value;	   
	if((shudu >= bj_shudu))
	{
		value ++;  //消除实际距离在设定距离左右变化时的干扰
		if(value > 5)
		{
			beep = ~beep; 	  //蜂鸣器报警			
		}	
	}
	else
	{
		beep = 1;	
	}			
}


/******************主程序**********************/	   
void main()
{
	beep = 0;
	delay_1ms(200) ;
	P0 = P1 = P2 = P3 = 0xff;  //IO口初始为电平
	init_ds1302_io();
	init_ds1302();
	init_1602();      //1602初始化
	init_int0();
	time_init();	  //定时器0初始化程序 
	init_eeprom();	 //开机初始化保存的数据*
	while(1)
	{	 //  shudu = 11;
		key();			 //按键程序
		if(key_can < 10)
		{
			key_with();	

		}
		if(flag_200ms == 1)
		{
			flag_200ms = 0;
			read_time();		  //读时间
			if(menu_1 == 0)
				init_1602_ds1302();   //显示时钟
			menu_dis();	  //计算速度函数
			clock_h_l();        //报警函数
		}
	}
}


/*********************外部中断0中断服务程序************************/
void int0() interrupt 0
{
	static uchar value;
	switch(value)
	{
		case 0: 
			t1_num = 0;		 //第一次就把变量清零
			TH1 = 0;
			TL1 = 0;
			break;
		case 1: 
				t2_num = t1_num;  //保存
				TH11 = TH1;
				TL11 = TL1;
				flag_en = 1;
			break;
	}		
	value ++;
	if(value >= 2)
		value =0;
	/************
		2.1   16.6
		2	  15.8
		1.9   15.0
	************/
}

/*************定时器0中断服务程序***************/
void time0_int() interrupt 1
{	
	static uchar value;
	TH0 = 0x3c;
	TL0 = 0xb0;     // 50ms
	value++;
	if(value % 4 == 0)
		flag_200ms = 1;

	if(value >= 20)  //1秒   才是一秒钟的速度
	{
		value = 0;
		flag_1s = 1;
	}
}

/*************定时器1中断服务程序***************/
void time1_int() interrupt 3
{
	t1_num++;
}
