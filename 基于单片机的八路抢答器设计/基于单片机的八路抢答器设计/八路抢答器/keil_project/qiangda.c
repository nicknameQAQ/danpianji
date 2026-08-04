#include <reg51.h>
#include <intrins.h>
#include <stdlib.h>

sbit DG1 = P0^0; //二位数码管第一位选择脚
sbit DG2 = P0^1; //二位数码管第二位选择脚
sbit DG3 = P0^3;


//P2端口 数码管段选
//P1端口 八位选手抢答
sbit start_qd_pin = P3^7;//开始抢答按钮连接管脚
sbit end_qd_pin = P3^5;		//结束抢答按钮连接管脚
sbit start_srand_qd_pin = P3^4;//开始随机抽取选手按钮连接管脚
sbit end_srand_qd_pin = P3^6;//结束随机抽取选手按钮连接管脚

sbit beep=P3^3;//蜂鸣器连接管脚

unsigned char weixuan[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//数码管段码：0123456789

unsigned char qd_num=0;	//抢答选手号
unsigned char qd_time=0;	//抢答倒计时间
unsigned char timeout_num=0;	//定时器产生中断次数

unsigned char work_status = 0;	//工作状态，下面几个为工作状态定义的宏
#define START_QD_STATUS 0
#define WAIT_QD_STATUS 1
#define ACK_QD_STATUS 2
#define ACK_SRAND_QD_STATUS 3

/*------延时2ms函数------*/
void delay2ms(void)   //误差 0us
{
    unsigned char a,b;
    for(b=4;b>0;b--)
        for(a=248;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
}
/*------延时50ms函数，参数num为延时多少个50ms------*/
void delay50ms(unsigned int num)   //误差 0us
{
    unsigned char a,b;
	unsigned char i=0;

	for(i=0; i<num; i++)
	{
    for(b=173;b>0;b--)
        for(a=143;a>0;a--);
	}
}

/*---初始化定时器0---*/
void InitTimer0(void)//50ms
{
    TMOD = 0x01;//定时器工作模式1
    TH0 = 0x3C;	//定时器装值高8位
    TL0 = 0x0B0;//定时器装值低8位
    EA = 1;		//使能总中断
    ET0 = 1;	//使能定时器0中断
    TR0 = 1;	//启动定时器
}
/*------关闭定时器0------*/
void end_Timer0(void)
{
	ET0 = 0;//关闭定时器0中断
    TR0 = 0;//停止定时器0
}
/*----初始化函数-----*/
void init(void)
{
	DG1 = 0;
	DG2 = 0;
	DG3 = 0;
	
	beep = 0;//上电时由于该脚为高，而为高时蜂鸣器会叫，故初始化为低
}
/*----------等待开始抢答状态
该状态下一直等待主席台按下开始抢答按钮或随机抢答按钮，再进入相应的工作状态！
这种状态机的编程方式是当前单片机开发者的主流思想。
---------*/
void start_qd_status(void)
{
	if(start_qd_pin == 0)//检测到开始抢答按钮按下
	{
		work_status = WAIT_QD_STATUS;//工作状态设置为等待答题状态
		qd_num = 0;			//回答选手号为0，等待选手按下
		qd_time = 20;		//20s抢答时间
		timeout_num=0;		//中断次数清0
		InitTimer0();		//开启定时器0
	}
	if(start_srand_qd_pin == 0)//检测到随机抽取选手答题按钮按下
	{
		work_status = ACK_SRAND_QD_STATUS;//工作状态设置为随机选手回答状态
		srand(TL0%100);				//产生随机数种子，srand和rand函数有兴趣可以百度一下单片机产生随机数方法！
		qd_num = (rand()%9);//0-8， 产生随机数
		if(qd_num == 0)			//防止产生随机数0
			qd_num = (rand()%9);	//再次产生随机数
		qd_time = 30;				//30s答题时间
		timeout_num=0;				//中断次数清0
		InitTimer0();				//开始定时器0
	}
}
/*-------开始按钮按下后，等待选手抢答状态函数------*/
void wait_qd_status(void)
{
	unsigned char i = 0;
	if(qd_time == 0)//无人抢答，抢答时间已到
	{
		end_Timer0();//停止定时器0

		//蜂鸣器给出无人抢答警报
		beep = 1;	//蜂鸣器响
		delay50ms(40);
		beep = 0;

		work_status = START_QD_STATUS;//无人抢答后恢复原始状态，设置工作状态为开始抢答状态
	}
	if(end_qd_pin == 0)//检测到结束按键
	{
		end_Timer0();//停止定时器0
		qd_num = 0;//回答选手号为0
		qd_time = 0;//抢答时间清0
		work_status = START_QD_STATUS;//工作状态置开始抢答状态
	}
	switch(P1)//检测哪位选手按下抢答按钮
	{
		case 0xfe://1号
			qd_num = 1;//回答选手号为1
			qd_time = 30;
			timeout_num=0;
			InitTimer0();
			work_status = ACK_QD_STATUS;
			break;
	    case 0xfd://2号
			qd_num = 2;//回答选手号为2
			qd_time = 30;
			timeout_num=0;
			InitTimer0();
			work_status = ACK_QD_STATUS;
			break;
		case 0xfb://3号
			qd_num = 3;//回答选手号为3
			qd_time = 30;
			timeout_num=0;
			InitTimer0();
			work_status = ACK_QD_STATUS;
			break;
		case 0xf7://4号
			qd_num = 4;//回答选手号为4
			qd_time = 30;
			timeout_num=0;
			InitTimer0();
			work_status = ACK_QD_STATUS;
			break;
		case 0xef://5号
			qd_num = 5;//回答选手号为5
			qd_time = 30;
			timeout_num=0;
			InitTimer0();
			work_status = ACK_QD_STATUS;
			break;
		case 0xdf://6号
			qd_num = 6;//回答选手号为6
			qd_time = 30;
			timeout_num=0;
			InitTimer0();
			work_status = ACK_QD_STATUS;
			break;
		case 0xbf://7号
			qd_num = 7;//回答选手号为7
			qd_time = 30;
			timeout_num=0;
			InitTimer0();
			work_status = ACK_QD_STATUS;
			break;
		case 0x7f://8号
			qd_num = 8;//回答选手号为8
			qd_time = 30;
			timeout_num=0;
			InitTimer0();
			work_status = ACK_QD_STATUS;
			break;
		default:
			break;
	}
}
/*------选手回答问题状态------*/
void ack_qd_status()
{
	unsigned char i = 0;
	if(qd_time == 0)//答题超时
	{
		end_Timer0();//结束定时器0
		qd_num = 0;
		qd_time = 0;

		for(i=0;i<3;i++)//选手回答问题超时警报
		{
			beep = 1;
			delay50ms(5);
			beep = 0;
			delay50ms(5);
		}

		work_status = START_QD_STATUS;//设置工作状态为开始抢答状态
	}
	if(end_qd_pin == 0)//结束按键按下
	{
		end_Timer0();//结束定时器0
		qd_num = 0;
		qd_time = 0;
		work_status = START_QD_STATUS;//设置工作状态为开始抢答状态
	}
}
/*-------随机选手回答状态-------*/
void ack_srand_status()
{
	unsigned char i = 0;
	if(qd_time == 0)//答题超时
	{
		end_Timer0();//结束定时器0
		qd_num = 0;
		qd_time = 0;
		for(i=0;i<3;i++)//随机选手回答问题超时警报
		{
			beep = 1;
			delay50ms(5);
			beep = 0;
			delay50ms(5);
		}

		work_status = START_QD_STATUS;
	}
	if(end_srand_qd_pin == 0)//结束按键
	{
		end_Timer0();//结束定时器0
		qd_num = 0;	//选手号清0
		qd_time = 0;//时间清0
		work_status = START_QD_STATUS;//设置工作状态为开始抢答状态
	}	
}
/*-----数码管显示函数
二位数码管显示倒计时时间
一位数码管显示抢答选手号------*/
void display(unsigned char qd_time, unsigned char qd_num)
{
	P2 = weixuan[qd_time/10];//显示时间十位
	DG1 = 1;		//打开第一位数码管位选
	delay2ms();
	DG1 = 0;

	P2 = weixuan[qd_time%10];//显示时间个位
	DG2 = 1;		//打开第二位数码管位选
	delay2ms();
	DG2 = 0;

	P2 = weixuan[qd_num%10];//显示选手号
	DG3 = 1;		//打开第三位数码管位选
	delay2ms();
	DG3 = 0;
}

/*-------主函数-----*/
void main()
{
	init();//初始化

	while(1)
	{
		switch(work_status)//工作状态判断
		{
			case START_QD_STATUS://开始抢答状态
				start_qd_status();
				break;
			case WAIT_QD_STATUS://等待选手抢答状态
				wait_qd_status();
				break;
			case ACK_QD_STATUS://选手回答状态
				ack_qd_status();
				break;
			case ACK_SRAND_QD_STATUS://随机选手回答状态
				ack_srand_status();
				break;
			default:
				break;
		}
		display(qd_time, qd_num);//数码管显示
	}
}

/*定时器0中断处理函数*/
void Timer0Interrupt(void) interrupt 1
{
	timeout_num++;
    TH0 = 0x3C;
    TL0 = 0x0B0;

	if(timeout_num==20)
	{
		timeout_num=0;
		qd_time--;
	}
}
