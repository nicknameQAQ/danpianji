#include<reg51.h>  //包含51单片机的库

#define uchar unsigned char
#define uint unsigned int		

uint sdd=0,sdd1=0,sdd2=0;  //切换变量
uchar data bianliang[4];				//秒显示的变量
uchar data secdong=23;  			//东西数默认
uchar data secnan=20;			//南北默认值

/*东西通行*/
uchar data chuzhi_dx0=23;		//设置东西方向的时间
uchar data chuzhi_nb0=20;		//设置南北方向的时间

/*南北通行*/
uchar data chuzhi_dx1=13;		//设置东西方向的时间
uchar data chuzhi_nb1=10;		//设置南北方向的时间

uchar data jishi;//定时器0中断次数


uchar code bianliangle[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x71};//段码

sbit zhi0 = P0^1; //南北方向个位显示
sbit zhi1= P0^2; //南北方向十位显示
sbit zhi2 = P0^3; //东西方向个位显示
sbit zhi3= P0^4; //东西方向十位显示

sbit dongdeng0=P2^6;		//南北方向红灯引脚定义
sbit dongdeng1=P2^5;	//南北方向黄灯引脚定义
sbit dongdeng2=P2^4;     //南北方向绿灯引脚定义

sbit nandeng0=P2^3;		//东西红灯
sbit nandeng1=P2^2;	//东西黄灯
sbit nandeng2=P2^1;		//东西绿灯


bit kongzhi=0;			//东西南北控制位
bit shuo=0;			//信号灯闪标志位

sbit guan=P0^7; //定义紧急按键的管脚



void delay(uint ms)		//延时程序
{
		uint j,k;
		for(j=0;j<ms;j++)			//延时ms
		for(k=0;k<110;k++);		
}

void chushihua()  //时钟初始化
{
		TMOD=0x01;					//定时器设置
		TH0=0x3C;							//重赋初值
	  TL0=0xB0;							//12m晶振50ms//重赋初值
		EA=1;						//开总中断
		ET0=1;						//定时器0中断开启
		TR0=1;						//启动定时0

}

void display()   //数码管显示
{
	switch(sdd2)
{
		case 0:
		{
		bianliang[0]=secnan/10; 		//第1位东西秒十位
		bianliang[1]=secnan%10; 		//第2位东西秒个位
		bianliang[2]=secdong/10; 		//第3位南北秒十位
		bianliang[3]=secdong%10; 		//第4位南北秒个位

		zhi0=0;
		zhi1=1;
		zhi2=1;
		zhi3=1;             
		P3=bianliangle[bianliang[0]];		//送东西时间十位的数码管编码
		delay(1);				//延时
		zhi0=1;
		zhi1=0;
		zhi2=1;
		zhi3=1;   		
		P3=bianliangle[bianliang[1]];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		zhi0=1;
		zhi1=1;
		zhi2=0;
		zhi3=1;    			
		P3=bianliangle[bianliang[2]];		//送南北时间十位的数码管编码
		delay(1);				//延时
		zhi0=1;
		zhi1=1;
		zhi2=1;
		zhi3=0;    				 
		P3=bianliangle[bianliang[3]];		//送南北时间个位的数码管编码
		delay(1);				//延时
		};break;
		
		case 1:
		{
		zhi0=0;
		zhi1=1;
		zhi2=1;
		zhi3=1;         
		P3=bianliangle[11];		//送东西时间十位的数码管编码
		delay(1);				//延时
		zhi0=1;
		zhi1=0;
		zhi2=1;
		zhi3=1;   	
		P3=bianliangle[11];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		zhi0=1;
		zhi1=1;
		zhi2=0;
		zhi3=1;    			
		P3=bianliangle[11];		//送南北时间十位的数码管编码
		delay(1);				//延时
		zhi0=1;
		zhi1=1;
		zhi2=1;
		zhi3=0;    			
		P3=bianliangle[11];		//送南北时间个位的数码管编码
		delay(1);				//延时
		};break;
		
		default :  break;
}
	
}




void main()        //主程序
{
	  chushihua();  //初始化定时器
	  guan=1;    //键初始
	 	nandeng2=0;    //初始化红绿灯的状态
		dongdeng2=1;
		dongdeng0=0;
		nandeng0=1;
		nandeng1=0;
		dongdeng1=0;
		while(1)					  //主循环
		{
			display();
		}
}

void time0(void) interrupt 1   	//定时中断子程序
{
	TH0=0x3C;							//重赋初值
	TL0=0xB0;							//12m晶振  50ms
	TR0=1;								//重新启动定时器
	jishi++;							//软件计数加1
	
/*判断开关是否闭合*/	
if(guan==0)           //当开关闭合时
	{
	  sdd2=1;
		sdd=0;
		nandeng2=0;   //定义小灯泡的状态
		dongdeng2=0;
		dongdeng0=1;
		nandeng0=1;
		nandeng1=0;
		dongdeng1=0;
		secdong=chuzhi_dx0;			//重赋南北方向的起始值
		secnan=chuzhi_nb0;		//重赋东西方向的起始值	
		jishi=0;   //停止计数
    sdd1=1;		
}	

if(guan==1)    //当开关未闭合时
{	
	sdd2=0;
	if(sdd1==1)   //再次初始状态
	{
		nandeng2=0;   //重新定义小灯泡状态
		dongdeng2=1;
		dongdeng0=0;
		nandeng0=1;
		nandeng1=0;
		dongdeng1=0;
		sdd1--;    //执行一次后跳出
		jishi=0;
	}
	
  if(sdd==0)    //当主干道通行时
{
	if(jishi==10)						//加到10也就是半秒
	{
		if((secnan<=3)&&(kongzhi==0)&&(shuo==1))  		//东西黄灯闪
		{
			dongdeng2=0;
			dongdeng1=0;
		}
		if((secdong<=3)&&(kongzhi==1)&&(shuo==1))  		//南北黄灯闪
		{

			nandeng2=0;
			nandeng1=0;
		}
	}
if(jishi==20)		// 定时器中断次数=20时（即1秒时）
{	  jishi=0;						//清零计数器
		secdong--;						//东西时间减1
		secnan--;						//南北时间减1

	if((secnan<=3)&&(kongzhi==0)&&(shuo==1))  		//东西黄灯闪
	{
		dongdeng2=0;
		dongdeng1=1;
	}
	if((secdong<=3)&&(kongzhi==1)&&(shuo==1))  		//南北黄灯闪
	{

		
				nandeng2=0;
		nandeng1=1;
	}
	
	if(secdong==0&&secnan==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		secdong=3;
		shuo=1;
	}
		if(secdong==3&&secnan==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		secnan=3;
		shuo=1;
	}
		if(secnan==0&&secdong==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			dongdeng2=0;
			nandeng0=0;
		
			dongdeng0=1;
			nandeng2=1;
		
			kongzhi=!kongzhi;
			shuo=0;
			secdong=chuzhi_dx1;			//重赋南北方向的起始值
			secnan=chuzhi_nb1;		//重赋东西方向的起始值
		  sdd++;
	}
 }
}
if(sdd==1)   //当辅干道通行时
{
  if(jishi==10)						//加到10也就是半秒
	{
		if((secnan<=3)&&(kongzhi==0)&&(shuo==1))  		//东西黄灯闪
		{
			dongdeng2=0;
			dongdeng1=0;
		}
		if((secdong<=3)&&(kongzhi==1)&&(shuo==1))  		//南北黄灯闪
		{
						nandeng2=0;
			nandeng1=0;
			

		}
	}
if(jishi==20)		// 定时器中断次数=20时（即1秒时）
{	  jishi=0;						//清零计数器
		secdong--;						//东西时间减1
		secnan--;						//南北时间减1

	if((secnan<=3)&&(kongzhi==0)&&(shuo==1))  		//东西黄灯闪
	{
				dongdeng2=0;
		dongdeng1=1;
		

	}
	if((secdong<=3)&&(kongzhi==1)&&(shuo==1))  		//南北黄灯闪
	{
		nandeng2=0;
		nandeng1=1;
	}
	
	if(secdong==0&&secnan==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		secdong=3;
		shuo=1;
	}
		if(secdong==3&&secnan==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		secnan=3;
		shuo=1;
	}
		if(secnan==0&&secdong==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			dongdeng2=1;
			nandeng0=1;
			dongdeng0=0;
			nandeng2=0;
			kongzhi=!kongzhi;
			shuo=0;
			secdong=chuzhi_dx0;			//重赋南北方向的起始值
			secnan=chuzhi_nb0;		//重赋东西方向的起始值
		  sdd--;  //类型切换
	 }	
  }
 }
}
	
}

