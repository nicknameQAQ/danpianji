#include <reg52.h>	         //调用单片机头文件
#define uchar unsigned char  //无符号字符型 宏定义	变量范围0~255
#define uint  unsigned int	 //无符号整型 宏定义	变量范围0~65535

//数码管段选定义      0     1    2    3    4    5	 6	 7	  8	   9	
uchar code smg_du[]={0x05,0xdd,0x46,0x54,0x9c,0x34,0x24,0x5d,0x04,0x14};	 //断码

sbit smg_we1 = P3^3;	  //数码管位选定义8
sbit smg_we2 = P3^4;
sbit smg_we3 = P3^5;
sbit smg_we4 = P3^6;
sbit beep    = P3^7;      //蜂鸣器IO口定义 


uchar flag_start;         //开始抢答标志位
uchar flag_weigui;        //抢答违规标志位

uchar dis_smg[4] = {0};	 //数码管显示缓冲区
 uchar miao,s_time = 30,s2_time = 25;        //时间
uchar flag_num;           //号码 
uchar menu_1 ;            //菜单设置变量
bit flag_200ms = 1;       //200ms的标志位
uchar flag_qd_en;          //抢答
uchar i,qd2_en;

/***************************数码管位选函数****************************/
void smg_we_switch(uchar i)
{
	switch(i)
	{
		case 0: smg_we1 = 0;  smg_we2 = 1; smg_we3 = 1;  smg_we4 = 1; break;
		case 1: smg_we1 = 1;  smg_we2 = 0; smg_we3 = 1;  smg_we4 = 1; break;
		case 2: smg_we1 = 1;  smg_we2 = 1; smg_we3 = 0;  smg_we4 = 1; break;
		case 3: smg_we1 = 1;  smg_we2 = 1; smg_we3 = 1;  smg_we4 = 0; break;
	}	
}


/********************************************************************
* 名称 : delay_1ms()
* 功能 : 延时1ms函数
* 输入 : q
* 输出 : 无
***********************************************************************/
void delay_1ms(uint q)	 //延时1ms函数
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}


/********************独立按键程序*****************/
uchar key_can;	 //按键值
void key()	 //独立按键程序
{
	static uchar key_new;
	key_can = 0;			    //按键值还原
	if((P2 & 0xff) != 0xff)		//按键按下
	{
		delay_1ms(1);	     	//按键消抖动
		if(((P2 & 0xff) != 0xff) && (key_new == 1))
		{						//确认是按键按下
			key_new = 0;			//key_new = 0   说明按键已按下
			switch(P2 & 0xff)
			{
				case 0xfe:  key_can = 1;  break;	 //得到按键值
				case 0xfd:  key_can = 2;  break;	 //得到按键值
				case 0xfb:  key_can = 3;  break;	 //得到按键值
				case 0xf7:  key_can = 4;  break;	 //得到按键值
				case 0xef:  key_can = 5;  break;	 //得到按键值
				case 0xdf:  key_can = 6;  break;	 //得到按键值
				case 0xbf:  key_can = 7;  break;	 //得到按键值
				case 0x7f:  key_can = 8;  break;	 //得到按键值
			}
		}			
	}
	else 
		key_new = 1;  //key_new = 1   说明按键已经松开了	
}


/********************独立按键程序*****************/
uchar key_can1;	 //按键值
void key_1()  	 //主持人独立按键程序
{
	static uchar key_new;
	key_can1 = 0;
	P3 |= 0x07;
	if((P3 & 0x07) != 0x07)		//按键按下
	{
		delay_1ms(1);	     	//按键消抖动
		if(((P3 & 0x07) != 0x07) && (key_new == 1))
		{						//确认是按键按下
			key_new = 0;
			switch(P3 & 0x07)
			{
				case 0x05:  key_can1 = 9;   break;	 //得到按键值
				case 0x06:  key_can1 = 10;  break;	 //得到按键值
				case 0x03:  key_can1 = 11;  
 				break;	 //得到按键值
			}
 			if(key_can1 == 11)    //复位
			{
				menu_1 = 0;
				beep = 1;
				flag_weigui = 0;   	
				flag_qd_en = 0;  
				miao = s_time;
				qd2_en = 0;
				flag_num = 0; 
				flag_start = 0;
				beep = 0;    //叫一声   
				delay_1ms(100);
				beep = 1;
			}		
		}			
	}
	else 
		key_new = 1;	
}

/******************按键处理函数*****************/
void key_with()	  //按键处理函数
{
	if(key_can1 == 10)    //设置按键
	{
		menu_1 ++;
		if(menu_1 == 1)
		{
			dis_smg[0] = smg_du[s_time % 10];	    //显示设置值
			dis_smg[1] = smg_du[s_time / 10 % 10];	//显示设置值
			dis_smg[2] = 0xfe;
			dis_smg[3] = 0x0c;	            //显示A
		}		
		if(menu_1 == 2)
		{
			dis_smg[0] = smg_du[s2_time % 10];	    //显示设置值
			dis_smg[1] = smg_du[s2_time / 10 % 10];	//显示设置值
			dis_smg[2] = 0xfe;
			dis_smg[3] = 0xa4;	            //显示B
		}
		if(menu_1 > 2)
			menu_1 = 0;
	}
	if(key_can1 == 9)        //开始抢答键
	{
		flag_start = 1;
		for(i=0;i<2;i++)
		{
			beep = ~beep;
			delay_1ms(20);	
		}
	}

	if((key_can < 9) && (key_can > 0))
	if(menu_1 == 0)
	{
		if(flag_qd_en == 0)	     //没有抢答		 
		{
			if(key_can == 1)
				flag_num = 1;    //1号选手
			if(key_can == 2)
				flag_num = 2;    //2号选手
			if(key_can == 3)
				flag_num = 3;    //3号选手
			if(key_can == 4)
				flag_num = 4;    //4号选手
			if(key_can == 5)
				flag_num = 5;    //5号选手
			if(key_can == 6)
				flag_num = 6;    //6号选手
			if(key_can == 7)
				flag_num = 7;    //7号选手
			if(key_can == 8)
				flag_num = 8;    //8号选手
			for(i=0;i<4;i++)
			{
				beep = ~beep;
				delay_1ms(100);   //蜂鸣器提示抢答
			}
		}
		if(flag_start == 1)		  //开始抢答
		{
			flag_weigui = 0;   	
			flag_qd_en = 1;       //已经抢答	 
			if(qd2_en == 0)
			{
				qd2_en = 1;
				miao = s2_time;				
			}
		}
		else 
		{
			flag_weigui = 1;   //违规了  
		}		
	}

	if(menu_1 == 1)
	{
		if(key_can == 8)   //加键
		{
			s_time ++;     //设置倒计时加1	
			miao = s_time;
			if(s_time >= 99)
				s_time = 99;
			dis_smg[0] = smg_du[s_time % 10];	//显示设置值
			dis_smg[1] = smg_du[s_time / 10 % 10];	//显示设置值
 		}
		if(key_can == 7) 
		{
			if(s_time > 3)
				s_time -- ;	//设置倒计时减1	
			miao = s_time;
			dis_smg[0] = smg_du[s_time % 10];	    //显示设置值
			dis_smg[1] = smg_du[s_time / 10 % 10];	//显示设置值
 		}
	}	

	if(menu_1 == 2)
	{
		if(key_can == 8)  //加键
		{
			s2_time ++;     //设置倒计时加1	
			if(s2_time >= 99)
				s2_time = 99;
			dis_smg[0] = smg_du[s2_time % 10];	//显示设置值
			dis_smg[1] = smg_du[s2_time / 10 % 10];	//显示设置值
 		}
		if(key_can == 7)
		{
			if(s2_time > 3)
				s2_time -- ;	//设置倒计时减1	
			dis_smg[0] = smg_du[s2_time % 10];	    //显示设置值
			dis_smg[1] = smg_du[s2_time / 10 % 10];	//显示设置值
 		}
	}	
}

/******************倒计时处理*****************/
void djs_dis()	   //倒计时处理
{
	static uchar value;
	if((flag_start == 1) && (flag_weigui == 0))
	{
		value ++;
		if(value >= 5)     //1s
		{
			value = 0;
			if(miao != 0)
				miao --;	 //倒计时减一 
		}			
		if(miao <= 5)
		{
			beep = ~beep; //蜂鸣器提示报警
		}
		if(miao == 0)
		{
			beep = 1;     //关闭蜂鸣器
			flag_start = 0;
		}
	}		
}

/********************************************************************
* 名称 : display()
* 功能 : 数码管显示
* 输入 : 无
* 输出 : 无
***********************************************************************/
void display()	   //数码管显示
{
	static uchar i;
	i++;
	if(i >= 4)
		i = 0;	
	P1 = 0xff;	        //消隐 
	smg_we_switch(i);	//位选
	P1 = dis_smg[i];	//	
}


/*************定时器0初始化程序***************/
void time_init()	//  定时器0初始化程序
{
	EA   = 1;	 	  //开总中断
	TMOD = 0X01;	  //定时器0、工作方式1
	ET0  = 1;		  //开定时器0中断 
	TR0  = 1;		  //允许定时器0定时
}

/******************主程序**********************/	   
void main()
{
	static uchar value;
	beep = 0;    //开机叫一声   
	delay_1ms(100);
	P0 = P1 = P2 = P3 = 0XFF;    //IO口全部输出为1
 	time_init();  	   //  定时器0初始化程序
	miao = s_time;
	while(1)
	{
		key();	       //独立按键程序
		key_1();  	   //主持人独立按键程序
		if(flag_weigui == 0)
		{
			if((key_can != 0) || (key_can1 != 0))
				key_with();	   //按键处理函数
		}
		if(flag_200ms == 1)
		{
			flag_200ms = 0;
			if(menu_1 == 0)
			{				
				dis_smg[3] = smg_du[miao / 10 % 10];	//显示设置值
				dis_smg[2] = smg_du[miao % 10];	//显示设置值
				dis_smg[1] = 0xfe;
				dis_smg[0] = smg_du[flag_num];	//显示选手号码	
				if(flag_weigui == 1)  //违规了显示FF
				{
					dis_smg[3] = 0x2e;	//显示设置值
					dis_smg[2] = 0x2e;	//显示设置值	
					if(value == 0)		 //违规提示报警
					{
						value = 1;
						for(i=0;i<6;i++)
						{
							beep = ~beep;
							delay_1ms(100);
						}
						beep = 1;					
					}			
				}
				djs_dis();	   //倒计时处理				
			}
		}
	}
}

/*************定时器0中断服务程序***************/
void time0_int() interrupt 1
{	
	static uchar value;
	TH0 = 0xf8;			
	TL0 = 0x30;     // 2ms
	display();	   //数码管显示
	value ++;
	if(value >= 100)
	{
		value = 0;
		flag_200ms = 1;
	}
}


