#include<reg51.h>
#include<intrins.h>
typedef unsigned char uchar;
typedef unsigned int  uint;
sbit k1=P1^6;
sbit k2=P1^7;
sbit k3=P2^7;
sbit k4=P3^0;
sbit yellowled_nb=P1^4;	//南北黄灯
sbit yellowled_dx=P1^1;	//东西黄灯
uchar code table[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00};
uchar data dig;	//位选
uchar data led;	//偏移量
uchar data buf[4];
uchar data sec_dx=39;//东西数码指示值
uchar data sec_nb=39;//南北数码指示值
uchar data set_timedx=39;
uchar data set_timenb=39;//倒计时设置的键值保存
uchar data b;//定时器中断次数
bit time;//灯状态循环标志
bit int0_time;//中断强行标志
bit set;//调时方向切换键标志
void delay(int ms)
{
	uint j,k;
	for(j=0;j<ms;j++)
		for(k=0;k<124;k++);
}
void key_to1()
{
	TR0=0;	 //关定时器
	if(set==0)
		sec_nb++; //南北加1S
	else
		sec_dx++;//东西加1S
	if(sec_nb==100)
		sec_nb=1;
	if(sec_dx==100)
		sec_dx=1;//加到100置1	
}
void key_to2()
{
	TR0=0;//关定时器
	if(set==0)
		sec_nb--;  //南北减1S
	else
		sec_dx--;  //东西减1S
	if(sec_nb==0)
		sec_nb=99;	
	if(	sec_dx==0 )
		sec_dx=99;//减到1重置99	
}
void key()	//按键扫描子程序
{
	if(k1!=1)
	{
		delay(10);
		if(k1!=1)
		{
			while(k1!=1);
			key_to1();
		}
	}
	if(k2!=1)
	{
		delay(10);
		if(k2!=1)
		{
			while(k2!=1);
			key_to2();
		}
	}
	if(k4!=1)
	{
		delay(10);
		if(k4!=1)
		{
			while(k4!=1);
			set=!set;
		}
	}
	if(k3!=1&&int0_time==1)
	{	
		TR0=1;   //启动定时器
		sec_nb=59;
		sec_dx=59;
		int0_time=0;//清标志
	} 	
	else if(k3!=1&&int0_time==0)
	{
		TR0=1;
		set_timenb=sec_nb;
		set_timedx=sec_dx; //设置的键值返回保存
	}
}
void display()
{
	buf[1]=sec_dx/10; //第1位 东西秒十位
	buf[2]=sec_dx%10; //第2位 东西秒个位
	buf[3]=sec_nb/10; //第3位 南北秒十位
	buf[0]=sec_nb%10; //第4位 南北秒个位
	P0=table[buf[led]];
	delay(2);	//先延时，提前显示一位了
	P2=dig;
	dig=_crol_(dig,1);
	led++;
	if(led==4)
	{
		led=0;
		dig=0xfe;
	}
}
void main()
{
	TMOD=0x01;	
	TH0=0x3C;
	TL0=0xB0;
	EA=1;
	ET0=1;
	TR0=1;
	EX0=1;
	EX1=1;
	P1=0Xf3;// 东西通行
	while(1)
	{
		key(); //调用按键扫描程序
		display(); //调用显示程序
	}	
}
void t0() interrupt 1 using 1  //定时中断子程序
{
	b++;
	if(b==10)	  // 定时器中断次数。
	{
		b=0;
		sec_dx--;
		sec_nb--;
		//南北黄灯闪烁判断		
		if(sec_nb==3&&time==0)
		{
			yellowled_nb=1;//南北黄灯亮
			delay(300);
			yellowled_nb=0;
		}
		if(sec_nb==2&&time==0)
		{
			yellowled_nb=1;//南北黄灯亮
			delay(300);
			yellowled_nb=0;
		}
		if(sec_nb==1&&time==0)
		{
			yellowled_nb=1;
			delay(300);
			yellowled_nb=0;
		}		 
		//东西黄灯闪判断	    
		if(sec_dx==3&&time==1)
		{
			yellowled_dx=1;//南北黄灯亮
			delay(300);
			yellowled_dx=0;
		}	 	
		if(sec_dx==2&&time==1)
		{
			yellowled_dx=1;//南北黄灯亮
			delay(300);
			yellowled_dx=0;
		}
		if(sec_dx==1&&time==1)
		{
			yellowled_dx=1;
			delay(300);
			yellowled_dx=0;
		}			
		if(sec_dx==0||sec_nb==0) //东西或南北先到达1S时即开始重新计时
		{ 
			sec_dx=set_timedx;
			sec_nb=set_timenb; //第一次循环结束重置
			if(time==1)
			{
				P1=0xF3;  //东西通行
			}
			else
			{
				P1=0xde; //南北通行
			}
			time=!time;   //取反
		}
	}
}
void wb0() interrupt 0 using 1	 //东西强行
{
	TR0=0;
	P1=0xF3;
	sec_dx=88;
	sec_nb=88;
	int0_time=1;
}
void wb1() interrupt 2 using 1	  //南北强行
{
	TR0=0;
	P1=0xDE;
	sec_nb=88;
	sec_dx=88;
	int0_time=1;
}

