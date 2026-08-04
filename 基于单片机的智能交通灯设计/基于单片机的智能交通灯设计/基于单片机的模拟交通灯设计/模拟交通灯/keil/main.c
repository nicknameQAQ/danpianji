#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int		//宏定义

void delay(uint ms)		//延时子程序
{
		uint j,k;
		for(j=0;j<ms;j++)			//延时ms
		for(k=0;k<110;k++);		//大约1毫秒的延时
}
uchar data buf[4];				//秒显示的变量
uchar data sec_dx=20;  			//东西数默认
uchar data sec_nb=65;			//南北默认值
uchar data set_timedx=20;		//设置东西方向的时间
uchar data set_timenb=65;		//设置南北方向的时间
uchar data countt0;//定时器0中断次数

sbit nb_ge = P3^1; //P1.0南北个位显示
sbit nb_shi= P2^7; //P1.1南北十位显示
sbit dx_ge = P3^6; //P1.2东西个位显示
sbit dx_shi= P3^7; //P1.3东西十位显示

sbit Red_nb=P2^6;		//南北红灯标志
sbit Yellow_nb=P2^5;	//南北黄灯标志
sbit Green_nb=P2^4;     //南北绿灯标志

sbit Red_dx=P2^3;		//东西红灯标志
sbit Yellow_dx=P2^2;	//东西黄灯标志
sbit Green_dx=P2^1;		//东西绿灯标志


bit dx_nb=0;			//东西南北控制位
bit shanruo=0;			//闪烁标志位
uchar code table[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00};//共阴极字型码

void display();				//显示子程序

void main()
{
		TMOD=0x01;					//定时器设置
		TH0=0x3C;							//重赋初值
	  TL0=0xB0;							//12m晶振50ms//重赋初值
		EA=1;						//开总中断
		ET0=1;						//定时器0中断开启
		TR0=1;						//启动定时0
	
		Green_dx=1;
		Green_nb=0;
		Red_nb=1;
		Red_dx=0;
		Yellow_dx=0;
		Yellow_nb=0;
		sec_nb=sec_dx+5; 			//默认南北通行时间比东西多5秒
		while(1)					  //主循环
		{
			display(); 				//调用显示程序

		}
}
void display(void) //显示子程序
{

		buf[0]=sec_nb/10; 		//第1位东西秒十位
		buf[1]=sec_nb%10; 		//第2位东西秒个位
		buf[2]=sec_dx/10; 		//第3位南北秒十位
		buf[3]=sec_dx%10; 		//第4位南北秒个位

		nb_ge=0;
		nb_shi=1;
		dx_ge=1;
		dx_shi=1;             		//片选LED1
		P0=table[buf[0]];		//送东西时间十位的数码管编码
		delay(1);				//延时
		nb_ge=1;
		nb_shi=0;
		dx_ge=1;
		dx_shi=1;   		//片选LED2
		P0=table[buf[1]];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		nb_ge=1;
		nb_shi=1;
		dx_ge=0;
		dx_shi=1;    				//片选LED3
		P0=table[buf[2]];		//送南北时间十位的数码管编码
		delay(1);				//延时
		nb_ge=1;
		nb_shi=1;
		dx_ge=1;
		dx_shi=0;    				 //片选LED4
		P0=table[buf[3]];		//送南北时间个位的数码管编码
		delay(1);				//延时
}

void time0(void) interrupt 1   	//定时中断子程序
{
	TH0=0x3C;							//重赋初值
	TL0=0xB0;							//12m晶振50ms//重赋初值
	TR0=1;								//重新启动定时器
	countt0++;							//软件计数加1
	
if(countt0==10)						//加到10也就是半秒
	{
		if((sec_nb<=5)&&(dx_nb==0)&&(shanruo==1))  		//东西黄灯闪
		{
			Green_dx=0;
			Yellow_dx=0;
		}
		if((sec_dx<=5)&&(dx_nb==1)&&(shanruo==1))  		//南北黄灯闪
		{
			Green_nb=0;
			Yellow_nb=0;
		}
	}
if(countt0==20)		// 定时器中断次数=20时（即1秒时）
{	  countt0=0;						//清零计数器
		sec_dx--;						//东西时间减1
		sec_nb--;						//南北时间减1

	if((sec_nb<=5)&&(dx_nb==0)&&(shanruo==1))  		//东西黄灯闪
	{
		Green_dx=0;
		Yellow_dx=1;
	}
	if((sec_dx<=5)&&(dx_nb==1)&&(shanruo==1))  		//南北黄灯闪
	{
		Green_nb=0;
		Yellow_nb=1;
	}
	if(sec_dx==0&&sec_nb==5) 		//当东西倒计时到0时，重置5秒，用于黄灯闪烁时间
	{
		sec_dx=5;
		shanruo=1;
	}
	if(sec_nb==0&&sec_dx==5)		//当南北倒计时到0时，重置5秒，用于黄灯闪烁时间
	{
		sec_nb=5;
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
			sec_dx=set_timenb+5;		//重赋东西方向的起始值
	}
	if(dx_nb==1&&sec_dx==0)			//当黄灯闪烁时间到
	{
			P2=0x00;					//重置东西南北的红绿灯状态
			Green_dx=1;					 //东西绿灯亮
			Red_nb=1;					 //南北红灯亮
			dx_nb=!dx_nb;				 //取反
			shanruo=0;					//闪烁
			sec_dx=set_timedx;			//重赋东西方向的起始值
			sec_nb=set_timedx+5;		//重赋南北方向的起始值
	}
}


}
