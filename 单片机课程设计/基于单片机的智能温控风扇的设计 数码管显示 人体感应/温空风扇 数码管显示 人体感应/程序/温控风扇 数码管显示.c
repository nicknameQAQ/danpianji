#include <reg52.h>	         //调用单片机头文件
#define uchar unsigned char  //无符号字符型 宏定义	变量范围0~255
#define uint  unsigned int	 //无符号整型 宏定义	变量范围0~65535

//数码管段选定义      0     1    2    3    4    5	6	 7	  8	   9	
uchar code smg_du[]={0x28,0xe4,0x42,0x72,0xe5,0xa8,0x41,0x77,0x20,0xa0,
				  	 0x60,0x25,0x39,0x26,0x31,0x71,0xff};	 //断码
//数码管位选定义
uchar code smg_we[]={0xef,0xdf,0xbf,0x7f};
uchar dis_smg[8] = {0};	
uchar smg_i = 3;    //显示数码管的个位数
sbit dq   = P2^4;	//18b20 IO口的定义
sbit hw = P2^5;
uchar miao = 30;
uchar flag_en;

bit flag_lj_en;		 //按键连加使能
bit flag_lj_3_en;	 //按键连3次连加后使能  加的数就越大了 
uchar key_time,key_value;      //用做连加的中间变量
bit key_500ms  ;
sbit pwm = P2^3;  
uchar f_pwm_l ;	  //越小越慢

uint temperature ;  //
bit flag_200ms ;
uchar menu_1;       //菜单设计的变量
uint t_high = 200,t_low = 100;	   //温度上下限报警值 

/***********************1ms延时函数*****************************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}

/***********************小延时函数*****************************/
void delay_uint(uint q)
{
	while(q--);
}


/***********************数码显示函数*****************************/
void display()
{
	static uchar i;   
	i++;
	if(i >= smg_i)
		i = 0;	
	P1 = 0xff;			 //消隐 
	P3 = smg_we[i];	 		 //位选
	P1 = dis_smg[i];		 //段选	        

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
	
	low = read_18b20();	   //读温度低字节
	value = read_18b20();  //读温度高字节
	value <<= 8;		   //把温度的高位左移8位
	value |= low;		   //把读出的温度低位放到value的低八位中
	value *= 0.625;	       //转换到温度值 小数
	return value;		   //返回读出的温度 带小数
}

/*************定时器0初始化程序***************/
void time_init()	  
{
	EA   = 1;	 	  //开总中断
	TMOD = 0X21;	  //定时器0、定时器1工作方式1
	ET0  = 1;		  //开定时器0中断 
	TR0  = 1;		  //允许定时器0定时

	ET1  = 1;		  //开定时器0中断 
	TR1  = 1;		  //允许定时器0定时
}

/********************独立按键程序*****************/
uchar key_can;	 //按键值

void key()	 //独立按键程序
{
	static uchar key_new;
	key_can = 20;                   //按键值还原
	P2 |= 0x07;
	if((P2 & 0x07) != 0x07)		//按键按下
	{
		if(key_500ms == 1)	//连加
		{
			key_500ms = 0;
			key_new = 1;
		}
		delay_1ms(1);	     	//按键消抖动
		if(((P2 & 0x07) != 0x07) && (key_new == 1))
		{						//确认是按键按下
			key_new = 0;
			switch(P2 & 0x07)
			{
				case 0x06: key_can = 1; break;	   //得到k2键值
				case 0x04: key_can = 2; break;	   //得到k3键值
				case 0x02: key_can = 3; break;	   //得到k4键值
			}
			flag_lj_en = 1;	 //连加使能
		}			
	}
	else 
	{
		if(key_new == 0)
		{
			key_new = 1;
			flag_lj_en = 0;		//关闭连加使能
			flag_lj_3_en = 0;	//关闭3秒后使能
			key_value = 0;		//清零
			key_time = 0;
			key_500ms = 0;
		}
	}	
}

/****************按键处理数码管显示函数***************/
void key_with()
{
	if(key_can == 1)	  //设置键
	{
		f_pwm_l = 30; 
		menu_1 ++;
		if(menu_1 >= 3)
		{
			menu_1 = 0;
			smg_i = 3;		  //数码管显示3位
		}
	}
	if(menu_1 == 1)			//设置高温报警
	{
		smg_i = 4;		  //数码管显示4位
		if(key_can == 2)
		{
			if(flag_lj_3_en == 0)
				t_high ++ ;		//按键按下未松开自动加三次	
			else 
				t_high += 10;	//按键按下未松开自动加三次之后每次自动加10
			if(t_high > 990)
				t_high = 990;
		}
		if(key_can == 3)
		{
			if(flag_lj_3_en == 0)
				t_high -- ;		//按键按下未松开自动减三次	
			else 
				t_high -= 10;	//按键按下未松开自动减三次之后每次自动减10
			if(t_high <= t_low)
				t_high = t_low + 1;
		}
		dis_smg[0] = smg_du[t_high % 10];	           //取小数显示
		dis_smg[1] = smg_du[t_high / 10 % 10] & 0xdf;  //取个位显示
		dis_smg[2] = smg_du[t_high / 100 % 10] ;	   //取十位显示
		dis_smg[3] = 0x64;	 //H
	}	
	if(menu_1 == 2)			//设置低温报警
	{
		smg_i = 4;		  //数码管显示4位
		if(key_can == 2)
		{
			if(flag_lj_3_en == 0)
				t_low ++ ;			//按键按下未松开自动加三次	
			else 
				t_low += 10;		//按键按下未松开自动加三次之后每次自动加10
			if(t_low >= t_high)
				t_low = t_high - 1;
		}
		if(key_can == 3)
		{
			if(flag_lj_3_en == 0)
				t_low -- ;			//按键按下未松开自动减三次	
			else 
				t_low -= 10;		//按键按下未松开自动加三次之后每次自动加10
			if(t_low <= 10)
				t_low = 10;
		}
		dis_smg[0] = smg_du[t_low % 10];	           //取小数显示
		dis_smg[1] = smg_du[t_low / 10 % 10] & 0xdf;   //取个位显示
		dis_smg[2] = smg_du[t_low / 100 % 10] ;	       //取十位显示
		dis_smg[3] = 0x3D;	  //L
	}	
}  

/****************风扇控制函数***************/
void fengshan_kz()
{
	if(flag_en == 1)
	{
		if(temperature >= t_high)	  //风扇全开
		{	
			TR1 = 1;
			pwm = 0;
		}
		else if((temperature < t_high)	&& (temperature >= t_low))		   //风扇缓慢
		{
			f_pwm_l = 60; 	
			TR1 = 1;
		}
		else if(temperature < t_low)	 //关闭风扇
		{
			TR1 = 0;
			pwm = 1;
		}	
	}		
}
		

/****************主函数***************/
void main()
{
	static uchar value;
	time_init();                    //初始化定时器 
	temperature = read_temp();		//先读出温度的值	
	delay_1ms(650);				
	temperature = read_temp();		         //先读出温度的值
	dis_smg[0] = smg_du[temperature % 10];	 //取温度的小数显示
	dis_smg[1] = smg_du[temperature / 10 % 10] & 0xdf; //取温度的个位显示
	dis_smg[2] = smg_du[temperature / 100 % 10] ;	   //取温度的十位显示
	while(1)
	{		
		key();					//按键程序
		if(key_can < 10)
		{
			key_with();			//设置报警温度	
		}
		if(flag_200ms == 1)	    //200ms 处理一次温度程序
		{	   
			flag_200ms = 0;	
			temperature = read_temp();	//先读出温度的值
			if(menu_1 == 0)
			{	
				smg_i = 3;
				dis_smg[0] = smg_du[temperature % 10];	 //取温度的小数显示
				dis_smg[1] = smg_du[temperature / 10 % 10] & 0xdf; //取温度的个位显示
				dis_smg[2] = smg_du[temperature / 100 % 10] ;	   //取温度的十位显示
			}
			fengshan_kz();        //风扇控制函数
			value ++;
			if(value >= 4)
			{
				value = 0;
				if(miao != 0)
				{
					miao --;    //时间减1
				}
				if(miao == 0)
					flag_en = 0;
			}
			if(hw == 1)  //感应到人
			{
				miao = 30;
				flag_en = 1;
			}
		}

		display();		//数码管显示函数
	}
}

/*************定时器0中断服务程序***************/
void time0_int() interrupt 1
{	
	static uchar value;			 //定时2ms中断一次
	TH0 = 0xf8;
	TL0 = 0x30;     //2ms
	value++;	  
	if(value >= 150)
	{
		value = 0;	  
		flag_200ms = 1;
	}
	if(flag_lj_en == 1)	   //按下按键使能
	{
		key_time ++;
		if(key_time >= 250) //500ms
		{
			key_time = 0;
			key_500ms = 1; //500ms
			key_value ++;
			if(key_value > 3)
			{
				key_value = 10;
				flag_lj_3_en = 1; //3次后1.5秒连加大些
			}						
		}
	}
}


/*******************定时器1用做单片机模拟PWM 调节***********************/
void Timer1() interrupt 3  //调用定时器1
{
	static uchar value_l;
	TH1=0x0f;    //    定时中断一次
	TL1=0xec;	 //
	if(pwm==1)
	{
		value_l+=3;
		if(value_l > f_pwm_l)   //高电平
		{
			value_l=0;
				pwm=0;	 
		}
	}
	else				
	{
		value_l+=3;
		if(value_l  > 100 - f_pwm_l)		  //低电平
		{
			value_l=0;
			pwm=1;
		}
    }
}
