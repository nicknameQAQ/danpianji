#include <reg52.h>	         //调用单片机头文件
#define uchar unsigned char  //无符号字符型 宏定义	变量范围0~255
#define uint  unsigned int	 //无符号整型 宏定义	变量范围0~65535

uchar code table_num[]="0123456789abcdefg";

sbit rs=P2^5;	 //寄存器选择信号 H:数据寄存器  	L:指令寄存器
sbit rw=P2^6;	 //寄存器选择信号 H:数据寄存器  	L:指令寄存器
sbit e =P2^7;	 //片选信号   下降沿触发

sbit beep = P2^4;

sbit key_quan = P2^0;	 

sbit key_quxiao = P2^1;	  //取消键


uchar flag_en;
uchar i;

bit flag_300ms ;    //300ms的标志位

sbit led1 = P1^0;   //第1号病房指示灯
sbit led2 = P1^1;   //第2号病房指示灯
sbit led3 = P1^2;   //第3号病房指示灯
sbit led4 = P1^3;   //第4号病房指示灯
sbit led5 = P1^4;   //第5号病房指示灯
sbit led6 = P1^5;   //第6号病房指示灯
sbit led7 = P1^6;   //第7号病房指示灯
sbit led8 = P1^7;   //第8号病房指示灯


sbit k1=P2^2;
sbit k2=P2^3;
sbit k3=P3^2;
sbit k4=P3^3;
sbit k5=P3^4;
sbit k6=P3^5;
sbit k7=P3^6;
sbit k8=P3^7;



uchar dis_lcd[8];   //8个病房数据显示的缓冲区
uchar br_geshu;     //报警病人数
 uchar key_new;
 uchar key_new1;

/********************************************************************
* 名称 : delay_1ms()
* 功能 : 延时1ms函数
* 输入 : q
* 输出 : 无
***********************************************************************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<120;j++);
}

/***********************延时函数************************/
void delay_uint(uint q)
{
	while(q--);
}

/***********************lcd1602写命令函数************************/
void write_com(uchar com)
{
	e=0;
	rs=0;
	rw=0;
	P0=com;
	delay_uint(3);
	e=1;
	delay_uint(25);
	e=0;
}

/***********************lcd1602写数据函数************************/
void write_data(uchar dat)
{
	e=0;
	rs=1;
	rw=0;
	P0=dat;
	delay_uint(3);
	e=1;
	delay_uint(25);
	e=0;	
}


/***********************lcd1602上显示这字符函数************************/
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
/***********************lcd1602上显示这字符函数************************/
void write_string_ge(uchar hang,uchar add,uchar *p,uchar ge)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
	for(i=0;i<ge;i++)
		write_data(*p++);	
}

/***********************lcd1602初始化设置************************/
void init_1602()	 //lcd1602初始化设置
{
	write_com(0x38);	//
	write_com(0x0c);
	write_com(0x06);
	delay_uint(1000);
	write_string(1,0," bingfang hujiao ");		
	write_string(2,0,"  wu bingren    ");	
}



/*********************定时器0初始化******************/
void time0_init()	  
{					 
	EA  = 1;	 	  //开总中断
	TMOD = 0X01;	  //定时器0、工作方式1
	ET0 = 1;		  //开定时器0中断 
	TR0 = 1;		  //允许定时器0定时
}

/********************独立按键程序*****************/
uchar key_can;	 //按键值

void key()	 //独立按键程序
{

	
	if(k1== 0)   key_can = 1;  //得到按键值
	if(k2== 0)   key_can = 2; 	 //得到按键值
	if(k3== 0)   key_can = 3;   //得到按键值
	if(k4== 0)   key_can = 4;   //得到按键值
	if(k5== 0)   key_can = 5;   //得到按键值
	if(k6== 0)   key_can = 6;  	 //得到按键值
	if(k7== 0)   key_can = 7;  	 //得到按键值
	if(k8== 0)   key_can = 8;  	 //得到按键值	
	
}
	
void key_qx()	 //取消按键程序
{

	key_quxiao = 1;                     //对应的按键IO口输出为1
	if(key_quxiao == 0)		//按键按下
	{
		delay_1ms(1);	     	//按键消抖动
		if(key_quxiao == 0) 
		{						//确认是按键按下
			key_can = 9;
				while(key_quxiao == 0);
		}			
	}

	
	
	if(key_quan == 0)		//按键按下
	{
		delay_1ms(1);	     	//按键消抖动
		if((key_quan == 0))
		{						//确认是按键按下
			key_can = 10;
			while(key_quan == 0);
		}			
	}

		
	

}	


/**********************按键处理函数************************/
void key_with()
{
	uchar i;
	if(key_can <= 8)
	{
		if(key_can == 1)
			led1 = 1;	  //1号病房灯亮
		if(key_can == 2)
			led2 = 1;	  //2号病房灯亮
		if(key_can == 3)
			led3 = 1;	  //3号病房灯亮
		if(key_can == 4)
			led4 = 1;	  //4号病房灯亮
		if(key_can == 5)
			led5 = 1;	  //5号病房灯亮
		if(key_can == 6)
			led6 = 1;	  //6号病房灯亮
		if(key_can == 7)
			led7 = 1;	  //7号病房灯亮
		if(key_can == 8)
			led8 = 1;	  //8号病房灯亮
		flag_en = 1;
		for(i=0;i<8;i++)
		{
			if(dis_lcd[i] == table_num[key_can])
			{
				flag_en = 0;      //说明这个病人已经按下过了
			}				
		}
		if(br_geshu < 8)
		{
		if(flag_en == 1)	//能进入到这里说明是第一次按下呼叫按键
		{
			if(br_geshu == 0)	 //第一次清除显示屏
				write_string(2,0,"               ");		
			 br_geshu ++;   			//呼叫病人的人数加1 
	
			//if(key_can!=1)
			{
			 for(i=7;i>0;i--)
				dis_lcd[i] = dis_lcd[i-1] ;     //把病人的数据向后移一位
				dis_lcd[0] = table_num[key_can] ;	//把病人的号码保存起来
				}

			//if(key_can==1)
			{
				//dis_lcd[br_geshu-1] =table_num[key_can];

			}

		  write_string_ge(2,0,dis_lcd,br_geshu);	 //显示出来
		 }
		}
	}
	if(key_can == 9)   //取消键的处理
	{
		if(br_geshu > 0)
		{
			if(dis_lcd[br_geshu-1] == '1')
				led1 = 0;	  //1号病房灯灭
			if(dis_lcd[br_geshu-1] == '2')
				led2 = 0;	  //2号病房灯灭
			if(dis_lcd[br_geshu-1] == '3')
				led3 = 0;	  //3号病房灯灭
			if(dis_lcd[br_geshu-1] == '4')
				led4 = 0;	  //4号病房灯灭
			if(dis_lcd[br_geshu-1] == '5')
				led5 = 0;	  //5号病房灯灭
			if(dis_lcd[br_geshu-1] == '6')
				led6 = 0;	  //6号病房灯灭
			if(dis_lcd[br_geshu-1] == '7')
				led7 = 0;	  //7号病房灯灭
			if(dis_lcd[br_geshu-1] == '8')
				led8 = 0;	  //8号病房灯灭
			dis_lcd[br_geshu-1] = ' ';					
			br_geshu --;   			//呼叫病人的人数减1 
		    write_string(2,0,"          ");		  //清显示 
		    write_string_ge(2,0,dis_lcd,br_geshu);
			if(br_geshu == 0)	 //取消到最后一次清显示屏
			{
				write_string(2,0,"  wu bingren    ");	
				P1 = 0x00;  
			}			
		} 		
	}
	

	if(key_can == 10)   //取消键的处理
	{
		   for(i=0;i<8;i++)
			 dis_lcd[i] = ' ';		

			br_geshu=0;   			//呼叫病人的人数减1 
		
			if(br_geshu == 0)	 //取消到最后一次清显示屏
			{
				write_string(2,0,"  wu bingren    ");	
				P1 = 0x00;  
			}			
		} 		

	
}


/*****************主函数********************/
void main()
{	
	time0_init();		 //初始化定时器
	init_1602();		 //lcd1602初始化
	P1=0;
	while(1)
	{
		key_can = 20;   	
		key();	 //独立按键程序	   
		key_qx();	 //取消按键程序
		if(key_can <= 10)
			key_with();
		if(flag_300ms == 1)
		{
			flag_300ms = 0;
			if(br_geshu > 0)
				beep = ~beep;   //蜂鸣器报警
			else 
				beep = 1;       //取消报警
		}
	}
}


/**************定时器0中断程序*****************/
void time0() interrupt 1
{			 
	static uchar value;
	TH0 = 0X3C;
	TL0 = 0XB0;	   //50ms
	value ++;	   
	if(value >= 6)			 //300ms
	{
		value = 0;
		flag_300ms = 1;
	}
}