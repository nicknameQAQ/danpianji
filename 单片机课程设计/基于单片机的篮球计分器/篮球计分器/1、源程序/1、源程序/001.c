#include<reg51.h>//51单片机头文件
typedef unsigned char uchar;
typedef unsigned int  uint;
sbit K1=P3^2;//比赛倒计时开始/暂停
sbit K2=P3^3;//24s倒计时重新开始
sbit K3=P1^3;//乙队+1按键
sbit K4=P1^2;//乙队+2按键
sbit K5=P1^1;//甲队+1按键
sbit K6=P1^0;//甲队+2按键
sbit K7=P1^4;//甲队+3按键
sbit K8=P1^5;//乙队+3按键
sbit K9=P1^6;//预留按键
sbit beep=P2^0;//蜂鸣器接口  
sbit RS=P2^7;
sbit E=P2^5;
char miao,fen,num,time,aa;
uchar bb,cc;
uchar code table1[]={"H.T 000:000 R.T "};
uchar code table2[]={"12:00  SEC-1  24"};
//延时子函数
void delay(uint z)  
{
	uint x;
	uchar y;
	for(x=z;x>0;x--)
  		for(y=110;y>0;y--);
}
//LCD1602液晶写指令子函数
void write_com(uchar com)
{
	RS=0;
	P0=com;
	delay(5);
	E=1;
	delay(5);
	E=0;
}
//LCD1602液晶写数据子函数
void write_date(uchar date)
{
	RS=1;
	P0=date;
	delay(5);
	E=1;
	delay(5);
	E=0;
}
//LCD1602液晶初始化子函数
void LCD1602_init()
{
	uchar i;
	bb=0;    //H.T分数初始化
	cc=0;    //R.T分数初始化
	time=0;
	TMOD=0x10;    //定时器1初始化
	TL1=0x00;
	TH1=0x4c;
	EA=1;     //开总中断
	ET1=1;     //开定时器1
	TR1=0;     //定时器1不工作
	EX0=1;     //开中断0
	EX1=1;     //开中断1
	IT0=1;     //中断0为边沿触发
	IT1=1;     //中断1为边沿触发
	E=0; 
	beep=0;
	miao=0;
	fen=12;
	num=1;
	aa=24;
	write_com(0x38);  //LCD设置初始化
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
	write_com(0x80);  //LCD显示初始化
	for(i=0;i<16;i++)
	{
	  	write_date(table1[i]);
	}
	write_com(0x80+0x40);
	for(i=0;i<16;i++)
	{
	  	write_date(table2[i]);
	}
}
//分数更新子函数
void point_lcd(uchar add,uchar dat)
{
	write_com(0x80+add);
	write_date(0x30+dat/100);
	write_date(0x30+(dat%100)/10);
	write_date(0x30+dat%10);
	write_com(0x80+add);
}
//按键扫描子函数
void keyscan()
{
	if(K3==0)     //K3按下H.T分数加一
	{
  		while(K3==0);
		bb++;
  		point_lcd(0x04,bb);  //分数显示更新
  		if(K3==0)    //松手检测
  		{
   			while(K3==0);
   			delay(20);
  		}
	}
	else if(K4==0)    //S4按下H.T分数加二
	{
  		while(K4==0);
		bb=bb+2;
  		point_lcd(0x04,bb);
  		if(K4==0)
  		{
   			while(K4==0);
   			delay(20);
  		}
	}
	else if(K7==0)    //S4按下H.T分数加二
	{
  		while(K7==0);
		bb=bb+3;
  		point_lcd(0x04,bb);
  		if(K7==0)
  		{
   			while(K7==0);
   			delay(20);
  		}
	}
	else if(K5==0)    //S5按下R.T分数加一
	{
  		while(K5==0);
		cc++;
  		point_lcd(0x08,cc);
  		if(K5==0)
  		{
   			while(K5==0);
   			delay(20);
  		}
	}
	else if(K6==0)    //S6按下R.T分数加2
	{
  		while(K6==0);
		cc=cc+2;
  		point_lcd(0x08,cc);
  		if(K6==0)
  		{
   			while(K6==0);
   			delay(20);
  		}
	}
	else if(K8==0)    //S6按下R.T分数加2
	{
  		while(K8==0);
		cc=cc+3;
  		point_lcd(0x08,cc);
  		if(K8==0)
  		{
   			while(K8==0);
   			delay(20);
  		}
	}
}
//比赛倒计时/24s倒计时
void counter_down()
{
	uchar i;
	if(time>=20)     //每1s倒计时做减一操作
	{
  		miao--;
  		aa--;
  		write_com(0x80+0x4e);  //24s倒计时显示
  		write_date(0x30+aa/10);
  		write_date(0x30+aa%10);
		write_com(0x80+0x4e);
  		if(aa==0)    //24s结束发出3s连续报警
  		{
   			beep=1;
  		 	delay(3000);
   			beep=0;
   			aa=24;
  		}
  		if((miao==0)&&(fen==0))  //检测一节比赛是否结束
  		{
   			TR1=0;     //定时器1暂停
   			write_com(0x80+0x44);
   			write_date(0x30);
   			num++;
   			aa=24;    //24s计时复位
   			write_com(0x80+0x4e);  //24s倒计时显示
   			write_date(0x30+aa/10);
   			write_date(0x30+aa%10);
			write_com(0x80+0x4e);
   			if(num<5)    //每节结束蜂鸣器发出8s的间断报警
   			{
    			for(i=80;i>0;i--)
    			{
     				beep=1;
     				delay(500);
     				beep=0;
     				delay(500);
    			}
   			}
   			if(num==5)    //终场结束，蜂鸣器发出10s的连续警报声
   			{
     			beep=1;
     			delay(10000);
     			num=0;
   			}	
   			beep=0;     //蜂鸣器关闭
			write_com(0x80+0x4b); //更新"SEC-?"
			write_date(0x30+num);
			write_com(0x80+0x4b);
			miao=0;     //倒计时复位
			fen=12;
  		}
  		if(miao==-1)
  		{
   			miao=59;
   			fen--;
  		}
  		write_com(0x80+0x40);  //更新倒计时显示
  		write_date(0x30+fen/10);
  		write_date(0x30+fen%10);
  		write_com(0x80+0x43);
  		write_date(0x30+miao/10);
  		write_date(0x30+miao%10);
		write_com(0x80+0x43);
  		time=0;
	}
}
//主函数
void main()
{
	LCD1602_init();
	while(1)
	{
  		keyscan();  //分数按键检测
	}
}
//外部0中断子函数
void wb0() interrupt 0  //比赛时间开始/暂停
{
	TR1=~TR1;     //定时器1工作/暂停
	if(TR1==1)  //当倒计时工作时，S1按下定时器立即停止工作
	{
  		PT1=0;
	}
	else   //倒计时不工作时，S1按下倒计时立即工作
	{
  		PT1=1;
	}
	if(K1==0)     //松手检测
	{
  		while(K1==0);
   		counter_down();
  	//	delay(20);
	}
}
//外部1中断子函数
void wb1() interrupt 2   //24s倒计时重新开始
{
	aa=24;
	write_com(0x80+0x4e);   //24s倒计时显示
	write_date(0x30+aa/10);
	write_date(0x30+aa%10);
	write_com(0x80+0x4e);
	if(K2==0)      //松手检测
	{
  		while(K2==0)
  		{
   			counter_down();
  		}
  		delay(20);
	}
}
//定时器1中断子函数
void t1() interrupt 3   //定时器1中断20次为1s
{
	time++;
	TL1=0x00;
	TH1=0x4c;
	counter_down();     //倒计时 
}



