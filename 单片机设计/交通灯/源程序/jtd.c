#include <reg51.h>				//头文件

#define uchar unsigned char		//宏定义
#define uint  unsigned int		

uchar data buf[4];				//秒显示的变量
uchar data sec_dx=80;  			//东西数默认值
uchar data sec_nb=60;			//南北数默认值
uchar data set_timedx=80;		//设置东西方向的时间
uchar data set_timenb=60;		//设置南北方向的时间
int n;
uchar data countt0,countt1;//定时器0中断次数

//定义6组开关
sbit  k4=P3^7;      //切换方向
sbit  k1=P3^5;		//时间加
sbit  k2=P3^6;		//时间减
sbit  k3=P3^4;		//确认
sbit  k5=P3^1;		//禁止
sbit  k6=P1^5;		//夜间模式
sbit  k7=P1^6;		//东西模式
sbit  k8=P1^7;		//南北模式


sbit Red_nb=P2^6;		//南北红灯标志
sbit Yellow_nb=P2^5;	//南北黄灯标志
sbit Green_nb=P2^4;     //南北绿灯标志

sbit Red_dx=P2^3;		//东西红灯标志
sbit Yellow_dx=P2^2;	//东西黄灯标志
sbit Green_dx=P2^1;		//东西绿灯标志
		
bit set=0;				//调时方向切换键标志 =1时，南北，=0时，东西
bit dx_nb=0;			//东西南北控制位
bit shanruo=0;			//闪烁标志位
bit yejian=0;			//夜间黄灯闪烁标志位

uchar code table[11]={	//共阴极字型码
	0x3f,  //--0
	0x06,  //--1
	0x5b,  //--2
	0x4f,  //--3
	0x66,  //--4
	0x6d,  //--5
	0x7d,  //--6
	0x07,  //--7
	0x7f,  //--8
	0x6f,  //--9
	0x00   //--NULL
};

//函数的声明部分
void delay(int ms);			//延时子程序
void key();					//按键扫描子程序
void delay1(int i);
void key_to1();				//键处理子程序
void key_to2();
void key_to3();
void display();				//显示子程序
void Buzzer();
//主程序

void main()
{
	TMOD=0X11;	   				//定时器设置 ，定时器1用于
	TH1=0X3C;		
	TL1=0XB0;
	TH0=0X3C;					//定时器0置初值 0.05S
	TL0=0XB0;
	EA=1;						//开总中断
	ET0=1;						//定时器0中断开启
	ET1=1;					    //定时器1中断开启
	TR0=1;						//启动定时0
	TR1=0;						//关闭定时1
	P2=0Xc3;					// 开始默认状态，东西绿灯，南北黄灯   
    sec_nb=sec_dx+3; 			//默认南北通行时间比东西多3秒
	while(1)					//主循环
	{	 
		key(); 					//调用按键扫描程序
		display(); 				//调用显示程序
	}	
}

//函数的定义部分
void key(void)	//按键扫描子程序
{	 
	if(k1!=1)							//当K1(时间加)按下时
	{
		delay1(800);       				//调用显示，用于延时消抖
		if(k1!=1)							//如果确定按下
		{
		  	TR0=0;	       				//关定时器	
     		shanruo=0;					//闪烁标志位关
			P2=0x00;					//灭显示
			TR1=0;						//启动定时1
			if(set==0)					//设置键按下
				set_timedx++;   		//南北加1S
			else
				set_timenb++;    		//东西加1S
			if(set_timenb==100)
				set_timenb=1;
			if(	set_timedx==100)
				set_timedx=1;   		//加到100置1
			sec_nb=set_timenb ; 		//设置的数值赋给东西南北
			sec_dx=set_timedx;			
		  	do
		 	{ 
				display();				 //调用显示，用于延时
			}
			while(k1!=1);			 //等待按键释放
		}
	}

	if(k2!=1)							//当K2(时间减)按键按下时
	{		
		delay1(800);      				//调用显示，用于延时消抖  
		if(k2!=1)						//如果确定按下
		{	
		 	TR0=0;         				//关定时器0	
			shanruo=0;					//闪烁标志位关
			P2=0x00;					//灭显示
			TR1=0;						//关定时器1	
			if(set==0)
				set_timedx--;  			//南北减1S
			else
				set_timenb--;  			//东西减1S
			if(set_timenb==0)
				set_timenb=99;	
			if(	set_timedx==0 )
				set_timedx=99;   		//减到1重置99
			sec_nb=set_timenb ;	 		//设置的数值赋给东西南北
			sec_dx=set_timedx;
		  	do	
		    { 
				display();       		//调用显示，用于延时
			}
			while(k2!=1);			   //等待按键释放
		}
	}


	if(k3!=1)							//当K3（确认）键按下时
	{	
		delay1(1000);        				//调用显示，用于延时消抖
		if(k3!=1)						//如果确定按下
		{
			TR0=1;  						//启动定时器0
			sec_nb=set_timenb;				//从中断回复，仍显示设置过的数值
			sec_dx=set_timedx;				//显示设置过的时间
			TR1=0;							//关定时器1	
			do{
			if(set==0)						//时间倒时到0时
			{ 
				P2=0X00;					 //灭显示
				Green_dx=1;						//东西绿灯亮
				Red_nb=1;					//南北红灯亮
				sec_nb=sec_dx+3; 			//回到初值
			}
			else 
			{ 
				P2=0x00;					//南北绿灯，东西红灯
				Green_nb=1;
				Red_dx=1;
				sec_dx=sec_nb+3; 
			}	  
			 }while(k3!=1);
		}
	}
	 


	if(k4!=1)							//当K4（切换）键按下
    {
		display();       				//调用显示，用于延时消抖
		if(k4!=1)						//如果确定按下
		{  
		 	TR0=0;						//关定时器0	
			set=!set;					//取反set标志位，以切换调节方向
			TR1=0;						//关定时器1
			dx_nb=set;
			do
			{
				display(); 			   //调用显示，用于延时
			}				
			while(k4!=1);			  //等待按键释放
		}
	}	

	if(k5!=1)							//当K5（禁止）键按下时
	{
	   	display();       				//调用显示，用于延时消抖
	   	if(k5!=1)						//如果确定按下
	   	{ 	
	    	TR0=0;						//关定时器
			P2=0x00;					//灭显示
			Red_dx=1;
			Red_nb=1;					//全部置红灯
			TR1=0;
			sec_dx=00;					//四个方向的时间都为00
			sec_nb=00;                       
			do
			{
				display(); 				//调用显示，用于延时
			}	
			while(k5!=1);			//等待按键释放	
	   	}
	}
	if(k6!=1)							//当K6（夜间模式）按下 
	{
	   	display();       				//调用显示，用于延时消抖 
	   	if(k6!=1)						//如果确定按下
	   	{ 	
	    	TR0=0;						//关定时器
			P2=0x00;
			TR1=1;
			sec_dx=00;					//四个方向的时间都为00
			sec_nb=00;
			do
			{
				display(); 			  //调用显示，用于延时
			}	
			while(k6!=1);			//等待按键释放	
	   	}
	}

  	if(k7!=1)							//当K5（东西）键按下时
	{
	   	display();       				//调用显示，用于延时消抖
	   	if(k7!=1)						//如果确定按下
	   	{ 	
	    	TR0=0;						//关定时器
			P2=0x00;					//灭显示
			Green_dx=1;
			Red_nb=1;					
			TR1=0;
			sec_dx=00;					//四个方向的时间都为00
			sec_nb=00;                       
			do
			{
				display(); 				//调用显示，用于延时
			}	
			while(k7!=1);			//等待按键释放	
	   	}
	}

		if(k8!=1)							//当K5（南北）键按下时
	{
	   	display();       				//调用显示，用于延时消抖
	   	if(k8!=1)						//如果确定按下
	   	{ 	
	    	TR0=0;						//关定时器
			P2=0x00;					//灭显示
			Red_dx=1;
			Green_nb=1;					
			TR1=0;
			sec_dx=00;					//四个方向的时间都为00
			sec_nb=00;                       
			do
			{
				display(); 				//调用显示，用于延时
			}	
			while(k8!=1);			//等待按键释放	
	   	}
	}

}
void display(void) //显示子程序
{		
	buf[1]=sec_nb/10; 		//第1位 东西秒十位
	buf[2]=sec_nb%10; 		//第2位 东西秒个位
	buf[3]=sec_dx/10; 		//第3位 南北秒十位
	buf[0]=sec_dx%10; 		//第4位 南北秒个位		
	P1=0xff;          		// 初始灯为灭的
	P0=0x00;				 ////灭显示
	P1=0xfe;           		//片选LED1
	P0=table[buf[1]];		//送东西时间十位的数码管编码			
	delay(1);				//延时
	P1=0xff;				//关显示
	P0=0x00;				//灭显示
						   
	P1=0xfd;             	//片选LED2
	P0=table[buf[2]];		 //送东西时间个位的数码管编码
	delay(1);				 //延时
	P1=0xff;				//关显示
	P0=0x00;				//关显示

	P1=0Xfb;		  		//片选LED3
	P0=table[buf[3]];		//送南北时间十位的数码管编码
	delay(1);				//延时
	P1=0xff;				 //关显示
	P0=0x00;				 //关显示

	P1=0Xf7;				 //片选LED4	
	P0=table[buf[0]];		//送南北时间个位的数码管编码
	delay(1);				//延时

	P1=0xff;				 //关显示
	P0=0x00;				 //关显示
}

void time0(void) interrupt 1 using 1  	//定时中断子程序 （发生在东西南北红绿灯交替时）
{
	TH0=0X3C;							//重赋初值
	TL0=0XB0;							//12m晶振50ms//重赋初值
	TR0=1;								//重新启动定时器
	countt0++;							//软件计数加1
	if(countt0==20)						//加到10也就是半秒	 半秒钟的黄灯闪烁
	{
		if((sec_nb<=3)&&(dx_nb==0)&&(shanruo==1))  		//东西黄灯闪		
        {
			Green_dx=0;
			Yellow_dx=0;
		}		   				
	    if((sec_dx<=3)&&(dx_nb==1)&&(shanruo==1))  		//南北黄灯闪		
        {  
			Green_nb=0;
			Yellow_nb=0;
		}	
	}
		
	if(countt0==40)	                  	// 定时器中断次数=20时（即1秒时）
	{	
		countt0=0;						//清零计数器
		sec_dx--;						//东西时间减1
		sec_nb--;						//南北时间减1

		if((sec_nb<=3)&&(dx_nb==0)&&(shanruo==1))  		//东西黄灯闪		
        {
			Green_dx=0;
			Yellow_dx=1;
		}		   				
	    if((sec_dx<=3)&&(dx_nb==1)&&(shanruo==1))  		//南北黄灯闪		
        {  
			Green_nb=0;
			Yellow_nb=1;
		}		 						
		if(sec_dx==0&&sec_nb==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间   
		{
			sec_dx=3;
			shanruo=1;
		}
		if(sec_nb==0&&sec_dx==3)		//当南北倒计时到0时，重置3秒，用于黄灯闪烁时间   
		{
			sec_nb=3;
			shanruo=1;
		}
		if(dx_nb==0&&sec_nb==0)			//当黄灯闪烁时间倒计时到0时，
		{
			P2=0x00;					//重置东西南背方向的红绿灯
			Green_nb=1;
			Red_dx=1;
			dx_nb=!dx_nb;
			shanruo=0;
			sec_nb=set_timenb;			//重赋南北方向的起始值
			sec_dx=set_timenb+3;		//重赋东西方向的起始值
		}		
		if(dx_nb==1&&sec_dx==0)			//当黄灯闪烁时间到
		{
			P2=0X00;					//重置东西南北的红绿灯状态
			Green_dx=1;					 //东西绿灯亮
			Red_nb=1;					 //南北红灯亮
			dx_nb=!dx_nb;				 //取反
			shanruo=0;					//闪烁
			sec_dx=set_timedx;			//重赋东西方向的起始值
			sec_nb=set_timedx+3;		//重赋南北方向的起始值
		}
	}	
}
void time1(void) interrupt 3 	//定时中断子程序 （用于黄灯闪烁）
{
	TH1=0X3C;							//重赋初值
	TL1=0XB0;							//12m晶振50ms//重赋初值
	countt1++;							//软件计数加1
	if(countt1==20)					   // 定时器中断次数=10时（即0.5秒）
	{
			Yellow_nb=0;				//南北黄灯灭
			Yellow_dx=0;				//东西黄灯灭
	}
	if(countt1==40)	                  	// 定时器中断次数=20时（即1秒时）
	{	countt1=0;						//清零计数器
			Yellow_nb=1;					//南北黄灯亮
			Yellow_dx=1;			  	//东西黄灯亮
	}	
}

void delay(int ms)		//延时子程序
{
	uint j,k;
	for(j=0;j<ms;j++)			//延时ms
	   for(k=0;k<124;k++);		//大约1毫秒的延时
}

void delay1(int i)
{
	while(i--);	
}