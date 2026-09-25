#include <reg51.h>
#include <intrins.h>
#include <stdlib.h>

sbit DG1 = P0^0; //二位数码管第一位选择脚
sbit DG2 = P0^1; //二位数码管第二位选择脚
sbit DG3 = P0^3;

//P2端口 数码管段选
//P1端口 八位选手抢答
sbit start_qd_pin = P3^7;//开始抢答按钮连接管脚
sbit qd_error_pin = P3^6;
sbit qd_true_pin = P3^5;		


sbit beep=P3^3;//蜂鸣器连接管脚

unsigned char weixuan[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//数码管段码：0123456789

unsigned char qd_num=0;	//抢答选手号
unsigned char qd_time=0;	//抢答倒计时间
unsigned char timeout_num=0;	//定时器产生中断次数

unsigned char work_status = 0;	//工作状态，下面几个为工作状态定义的宏
#define INIT_STATUS 0
#define QD_STATUS 1
#define ACK_QD_STATUS 2

void InitTimer0(void);
void end_Timer0(void);
void Start_Timer0(void);
void init(void);
void init_status();
void qd_status();
void ack_qd_status();
void delay1ms(unsigned int num);
void display(unsigned char qd_time,unsigned char qd_num);

void main()
{
	init();
	work_status =  INIT_STATUS;
	while(1)
	{
		switch(work_status)//工作状态判断
		{
			case INIT_STATUS://开始抢答状态
				init_status();
				break;
			case QD_STATUS://等待选手抢答状态
				qd_status();
				break;
			case ACK_QD_STATUS://选手回答状态
				ack_qd_status();
				break;
			default:
				break;
		}
		display(qd_time, qd_num);//数码管显示	
	}	
}
void init_status()
{
	qd_time = 0;
	qd_num = 0;
	if(start_qd_pin == 0)
	{
		work_status =  QD_STATUS;
		qd_time = 60;
		InitTimer0();	
	}
}
void qd_status()
{
	switch(P1)//检测哪位选手按下抢答按钮
	{
		case 0xfe://1号
			qd_num = 1;//回答选手号为1
			end_Timer0();
			work_status =  ACK_QD_STATUS;
			break;
	    case 0xfd://2号
			qd_num = 2;//回答选手号为2
			end_Timer0();
			work_status =  ACK_QD_STATUS;
			break;
		case 0xfb://3号
			qd_num = 3;//回答选手号为3
			end_Timer0();
			work_status =  ACK_QD_STATUS;
			break;
		case 0xf7://4号
			qd_num = 4;//回答选手号为4
			end_Timer0();
			work_status =  ACK_QD_STATUS;
			break;
		case 0xef://5号
			qd_num = 5;//回答选手号为5
			end_Timer0();
			work_status =  ACK_QD_STATUS;
			break;
		case 0xdf://6号
			qd_num = 6;//回答选手号为6
			end_Timer0();
			work_status =  ACK_QD_STATUS;
			break;
		case 0xbf://7号
			qd_num = 7;//回答选手号为7
			end_Timer0();
			work_status =  ACK_QD_STATUS;
			break;
		case 0x7f://8号
			qd_num = 8;//回答选手号为8
			end_Timer0();
			work_status =  ACK_QD_STATUS;
			break;
		default:
			break;
	}

	if(qd_time <= 0)
	{
		end_Timer0();
		qd_num = 0;
		beep = 1;
		delay1ms(3000);
		beep = 0;
		work_status =  INIT_STATUS;		
	}
	if(qd_true_pin == 0)
	{
		end_Timer0();
		qd_num = 0;
		qd_time = 0;
		work_status = INIT_STATUS;
	}	
}
void ack_qd_status()
{
	if(qd_true_pin == 0)
	{
		end_Timer0();
		qd_num = 0;
		qd_time = 0;
		work_status = INIT_STATUS;
	}
	if(qd_error_pin == 0)
	{
		Start_Timer0();
		qd_num = 0;
		work_status = QD_STATUS;
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
void Start_Timer0(void)
{
	ET0 = 1;//启动定时器0中断
    TR0 = 1;//启动定时器0
}
/*----初始化函数-----*/
void init(void)
{
	DG1 = 0;
	DG2 = 0;
	DG3 = 0;
	
	beep = 0;//上电时由于该脚为高，而为高时蜂鸣器会叫，故初始化为低
}


void delay1ms(unsigned int num)   //误差 0us
{
    unsigned char a,b,c;
	unsigned int i;
	for(i=0;i<num;i++)
	    for(c=1;c>0;c--)
	        for(b=142;b>0;b--)
	            for(a=2;a>0;a--);
}

void display(unsigned char qd_time,unsigned char qd_num)
{
	unsigned int j;

	DG1 = 1;
	P2 = weixuan[qd_time/10];//显示时间十位
	j = 10;
	while(j--);
	P2 = 0xff;
	DG1 = 0;

	DG2 = 1;
	P2 = weixuan[qd_time%10];//显示时间个位
	j = 10;
	while(j--);
	P2 = 0xff;
	DG2 = 0;

	DG3 = 1;
	P2 = weixuan[qd_num%10];//显示选手号
	j = 10;
	while(j--);
	P2 = 0xff;
	DG3 = 0;

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