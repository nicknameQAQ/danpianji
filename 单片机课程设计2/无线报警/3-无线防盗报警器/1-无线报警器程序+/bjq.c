//包含头文件
#include <reg51.h>
//宏定义
#define uc unsigned char
#define ui unsigned int

sbit SOS	=	P1^0;//报警按键 连接热释电模块
sbit bufang	=	P1^1;//布防
sbit chefang=	P1^2;//撤防
sbit baojing=	P1^3;//蜂鸣器

bit bdata flag,flag1;//flag布防标志，flag1布防倒计时标志 
uc t,t1;			//定时器专用变量 

void delay(ui x)	//延时函数 大约延时x ms （不精确） 
{
	ui i,j;
	for(i=0;i<x;i++)
	for(j=0;j<110;j++);
}
	
void kongzhi()		//控制函数 
{
	if((bufang==1)&&(flag==0))	//在待机时按下布防键 
	{
		delay(10);				//延时去抖
		if((bufang==1)&&(flag==0))//再次判断按键是否按下
		{
			t=0;			//清零计时器
			t1=0;			//清零计时器 
			TR0=1;			//打开计时器 
			P2=0x85;		//显示字母b 
		}
	}
	if((flag==1)&&(SOS==1))	//进入布防状态后报警按键按下（或者热释电模块有信号发出）
	{
		flag1=1;			//蜂鸣器鸣响 
		P2=0xbf;			//显示"-"
		TR0=1;				//开启定时器 
	}
	if(chefang==1)			//撤防键按下 
	{
		delay(10);
		if(chefang==1)
		{
			{
				flag1=0;	//关闭布防计时
				baojing=1;	//关闭蜂鸣器 
				flag=0;		//停止计时器计时
				TR0=0;		//关闭计时器 
				P2=0xa7;	//显示字母c 
				delay(1000);//延时1s 
				P2=0xff;	//关闭显示，进入等待布防状态  
			}
		}
	}
}

void init()	//初始化函数 
{
	TH0=0x3C;
	TL0=0xB0;  //赋初值50ms（12MHz晶振）
	TMOD=0x01; //定义工作方式
	EA=1;	   //打开中断总开关
	ET0=1;	   //打开中断允许开关
	TR0=0;	   //关闭定时器定时开关 
	bufang=0;
	SOS=0;
	chefang=0;	//按键初始化 
}

void main()	//主函数
{
	init();	//调用初始化 
	while(1)//循环
	{
		kongzhi();//控制函数 
	}
}

void time() interrupt 1//定时器函数 
{
	TH0=0x3C;
	TL0=0xB0;		//重新赋初值 
	t++;
	if(t==20)		//t加一次时50ms，加20次时1s
	{
		t=0;		//t清零
		t1++;		//t1加一次时1s
		if(t1==30)//计时到30s时开启布防数码管停止显示  关闭定时器   
		{
			t1=0;	//t1清零
			flag=1;	//布防标志位，=1时进入布防状态
			P2=0xff;//数码管关闭显示
			TR0=0;	//停止计时
		}
	}
	if((t>=10)&&(flag1==1))//报警 t每加到10（500ms）蜂鸣器引脚取反一次
	{
		baojing=~baojing;  //闪烁鸣响
		t=0;			   //t清零
	}
}

