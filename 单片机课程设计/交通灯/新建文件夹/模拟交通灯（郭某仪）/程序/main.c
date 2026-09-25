#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int		//宏定义

sbit ci0 = P3^1; //P3.1南北个位显示
sbit ci1= P2^7; //P2.7南北十位显示
sbit zhu0 = P3^6; //P3.6东西个位显示
sbit zhu1= P3^7; //P3.7东西十位显示

sbit nan0=P2^6;		//南北红灯标志
sbit nan1=P2^5;	//南北黄灯标志
sbit nan2=P2^4;     //南北绿灯标志

sbit dong0=P2^3;		//东西红灯标志
sbit dong1=P2^2;	//东西黄灯标志
sbit dong2=P2^1;		//东西绿灯标志


bit kongzhi=0;			//东西南北控制位
bit shanruo=0;			//闪烁标志位

sbit key=P3^0; //定义按键的管脚

uint lei=0,lei1=0,lei2=0;  //类型切换变量

uchar data xianshi[4];				//秒显示的变量
uchar data sec_zhu=20;  			//东西数默认
uchar data sec_ci=23;			//南北默认值

uchar data num1=20;		//设置东西方向的时间
uchar data num2=23;		//设置南北方向的时间

uchar data num11=13;		//设置东西方向的时间
uchar data num21=10;		//设置南北方向的时间

uchar data jishu;//定时器0中断次数

uchar code duan[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x71};//共阴极字型码

void Display();				//显示程序

void delay(uint ms)		//延时子程序
{
		uint j,k;
		for(j=0;j<ms;j++)			//延时ms
		for(k=0;k<110;k++);		//大约1毫秒的延时
}


void main()
{
	  key=1;
		TMOD=0x01;					//定时器设置
		TH0=0x3C;							//重赋初值
	  TL0=0xB0;							//12m晶振50ms//重赋初值
		EA=1;						//开总中断
		ET0=1;						//定时器0中断开启
		TR0=1;						//启动定时0
	 	dong2=1;    //初始化红绿灯的状态
		nan2=0;
		nan0=1;
		dong0=0;
		dong1=0;
		nan1=0;
		while(1)					  //主循环
		{
			Display(); 				//调用显示程序

		}
}
void Display(void) //显示子程序
{
  if(lei2==0)   //显示判断 输入lei2
	{
		xianshi[0]=sec_ci/10; 		//第1位东西秒十位
		xianshi[1]=sec_ci%10; 		//第2位东西秒个位
		xianshi[2]=sec_zhu/10; 		//第3位南北秒十位
		xianshi[3]=sec_zhu%10; 		//第4位南北秒个位

		ci0=0;
		ci1=1;
		zhu0=1;
		zhu1=1;             		//片选LED1
		P0=duan[xianshi[0]];		//送东西时间十位的数码管编码
		delay(1);				//延时
		ci0=1;
		ci1=0;
		zhu0=1;
		zhu1=1;   		
		P0=duan[xianshi[1]];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		ci0=1;
		ci1=1;
		zhu0=0;
		zhu1=1;    			
		P0=duan[xianshi[2]];		//送南北时间十位的数码管编码
		delay(1);				//延时
		ci0=1;
		ci1=1;
		zhu0=1;
		zhu1=0;    				 
		P0=duan[xianshi[3]];		//送南北时间个位的数码管编码
		delay(1);				//延时
	}
	if(lei2==1)    //当按键按下的时候显示状态
	{
		ci0=0;
		ci1=1;
		zhu0=1;
		zhu1=1;             
		P0=duan[11];		//送东西时间十位的数码管编码
		delay(1);				//延时
		ci0=1;
		ci1=0;
		zhu0=1;
		zhu1=1;   		
		P0=duan[11];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		ci0=1;
		ci1=1;
		zhu0=0;
		zhu1=1;    			
		P0=duan[11];		//送南北时间十位的数码管编码
		delay(1);				//延时
		ci0=1;
		ci1=1;
		zhu0=1;
		zhu1=0;    			
		P0=duan[11];		//送南北时间个位的数码管编码
		delay(1);				//延时
	}
	
	
	
}

void time0(void) interrupt 1   	//定时中断子程序
{
	TH0=0x3C;							//重赋初值
	TL0=0xB0;							//12m晶振50ms//重赋初值
	TR0=1;								//重新启动定时器
	jishu++;							//软件计数加1
	
if(key==0)           //当开关闭合时
	{
	  lei2=1;       //定义为类型一为1
		lei=0;
		dong2=0;   //两通道都亮红灯
		nan2=0;
		nan0=1;
		dong0=1;
		dong1=0;
		nan1=0;
		sec_zhu=num1;			//重赋南北方向的起始值
		sec_ci=num2;		//重赋东西方向的起始值	
		jishu=0;   //停止计数
    lei1=1;		
}
	
if(key==1)    //当开关未闭合时
{	
	lei2=0;
	if(lei1==1)   //再次初始状态
	{
		dong2=1;
		nan2=0;
		nan0=1;
		dong0=0;
		dong1=0;
		nan1=0;
		lei1--;
		jishu=0;
	}
	
  if(lei==0)    //当主干道通行时
{
	if(jishu==10)						//加到10也就是半秒
	{
		if((sec_ci<=3)&&(kongzhi==0)&&(shanruo==1))  		//东西黄灯闪
		{
			dong2=0;
			dong1=0;
		}
		if((sec_zhu<=3)&&(kongzhi==1)&&(shanruo==1))  		//南北黄灯闪
		{
			nan2=0;
			nan1=0;
		}
	}
if(jishu==20)		// 定时器中断次数=20时（即1秒时）
{	  jishu=0;						//清零计数器
		sec_zhu--;						//东西时间减1
		sec_ci--;						//南北时间减1

	if((sec_ci<=3)&&(kongzhi==0)&&(shanruo==1))  		//东西黄灯闪
	{
		dong2=0;
		dong1=1;
	}
	if((sec_zhu<=3)&&(kongzhi==1)&&(shanruo==1))  		//南北黄灯闪
	{
		nan2=0;
		nan1=1;
	}
	
	if(sec_zhu==0&&sec_ci==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_zhu=3;
		shanruo=1;
	}
		if(sec_zhu==3&&sec_ci==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_ci=3;
		shanruo=1;
	}
		if(sec_ci==0&&sec_zhu==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			nan2=1;
			dong0=1;
		  nan0=0;
		  dong2=0;
		
			kongzhi=!kongzhi;
			shanruo=0;
			sec_zhu=num11;			//重赋南北方向的起始值
			sec_ci=num21;		//重赋东西方向的起始值
		  lei++;
	}
 }
}
if(lei==1)   //当次干道通行时
{
  if(jishu==10)						//加到10也就是半秒
	{
		if((sec_ci<=3)&&(kongzhi==0)&&(shanruo==1))  		//东西黄灯闪
		{
			dong2=0;
			dong1=0;
		}
		if((sec_zhu<=3)&&(kongzhi==1)&&(shanruo==1))  		//南北黄灯闪
		{
			nan2=0;
			nan1=0;
		}
	}
if(jishu==20)		// 定时器中断次数=20时（即1秒时）
{	  jishu=0;						//清零计数器
		sec_zhu--;						//东西时间减1
		sec_ci--;						//南北时间减1

	if((sec_ci<=3)&&(kongzhi==0)&&(shanruo==1))  		//东西黄灯闪
	{
		dong2=0;
		dong1=1;
	}
	if((sec_zhu<=3)&&(kongzhi==1)&&(shanruo==1))  		//南北黄灯闪
	{
		nan2=0;
		nan1=1;
	}
	
	if(sec_zhu==0&&sec_ci==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_zhu=3;
		shanruo=1;
	}
		if(sec_zhu==3&&sec_ci==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_ci=3;
		shanruo=1;
	}
		if(sec_ci==0&&sec_zhu==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			nan2=0;
		  nan0=1;
			dong0=0;
	  	dong2=1;
		
			kongzhi=!kongzhi;
			shanruo=0;
			sec_zhu=num1;			//重赋南北方向的起始值
			sec_ci=num2;		//重赋东西方向的起始值
		  lei--;
	}
	
}
}
}
}

