#include<reg51.h>  //包含51单片机的库
#define uchar unsigned char
#define uint unsigned int		//宏定义


//初始时定义东西为主干道，南北方向为次干道//

sbit wei0 = P3^1; //次干道个位
sbit wei1= P2^7; //次干道十位
sbit zhuwei0 = P3^6; //主干道个位
sbit zhuwei1= P3^7; //主干道十位

sbit nbdeng0=P2^6;		//南北方向红灯定义
sbit nbdeng1=P2^5;	//南北方向黄灯定义
sbit nbdeng2=P2^4;     //南北方向绿灯定义

sbit dxled0=P2^3;		//东西方向红灯定义
sbit dxled1=P2^2;	//东西方向黄灯定义
sbit dxled2=P2^1;		//东西方向绿灯定义


bit biaozhi=0;			//两干道控制引脚定义

bit flag=0;			//闪烁标志位

sbit key1=P3^0; //定义按键的管脚

uint bian=0,bian1=0,bian2=0;  //类型切换变量

uchar data xianshi[4];				//秒显示的变量
uchar data sec_zhu=20;  			//东西数默认
uchar data sec_ci=23;			//南北默认值

uchar data fuzhi0=20;		//设置东西方向的时间
uchar data fuzhi1=23;		//设置南北方向的时间

uchar data fuzhi01=13;		//设置东西方向的时间
uchar data fuzhi11=10;		//设置南北方向的时间

uchar data jishu;//定时器0中断次数

uchar code table[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x71};//段码


void delay(uint ms)		//延时函数
{
		uint j,k;
		for(j=0;j<ms;j++)			//延时ms
		for(k=0;k<110;k++);		//大约1毫秒的延时
}

void InitTimer0(void)
{
   	TMOD=0x01;					//定时器设置
		TH0=0x3C;							//重赋初值
	  TL0=0xB0;							//重赋初值50ms
		EA=1;						//开总中断
		ET0=1;						//定时器0中断开启
		TR0=1;						//启动定时0
}



void main()
{
	  InitTimer0(); //初始化定时器0
	  key1=1;
	 	dxled2=1;    //初始化让东西绿灯亮
		nbdeng2=0;    //南北红灯亮
		nbdeng0=1;
		dxled0=0;
		dxled1=0;
		nbdeng1=0;
	
		while(1)					  //主循环
		{

 if(bian2==0)   //显示判断 输入bian2
	{
		xianshi[0]=sec_ci/10; 		//东西秒十位
		xianshi[1]=sec_ci%10; 		//东西秒个位
		xianshi[2]=sec_zhu/10; 		//南北秒十位
		xianshi[3]=sec_zhu%10; 		//南北秒个位

		wei0=0;
		wei1=1;
		zhuwei0=1;
		zhuwei1=1;  

		P1=table[xianshi[0]];		//送东西时间十位的数码管编码
		delay(1);				//延时
		wei0=1;
		wei1=0;
		zhuwei0=1;
		zhuwei1=1;   		
		P1=table[xianshi[1]];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		wei0=1;
		wei1=1;
		zhuwei0=0;
		zhuwei1=1;    			
		P1=table[xianshi[2]];		//送南北时间十位的数码管编码
		delay(1);				//延时
		wei0=1;
		wei1=1;
		zhuwei0=1;
		zhuwei1=0;    				 
		P1=table[xianshi[3]];		//送南北时间个位的数码管编码
		delay(1);				//延时
	}
	
	if(bian2==1)    //当按键按下bian=1
	{
		wei0=0;
		wei1=1;
		zhuwei0=1;
		zhuwei1=1;             
		P1=table[11];		//显示F值
		delay(1);				//延时
		wei0=1;
		wei1=0;
		zhuwei0=1;
		zhuwei1=1;   		
		P1=table[11];		 //显示F值
		delay(1);				 //延时
		wei0=1;
		wei1=1;
		zhuwei0=0;
		zhuwei1=1;    			
		P1=table[11];		//显示F值
		delay(1);				//延时
		wei0=1;
		wei1=1;
		zhuwei0=1;
		zhuwei1=0;    			
		P1=table[11];		//显示F值
		delay(1);				//延时
	  }
	}
}



void time0(void) interrupt 1   	//定时中断子程序
{
	TH0=0x3C;							//重赋初值
	TL0=0xB0;							//12m晶振50ms//重赋初值
	TR0=1;								//重新启动定时器
	jishu++;							//软件计数加1 //20个计数周期为1秒
	
if(key1==0)           //当开关闭合时
	{
	  bian2=1;       //定义为类型一为1
		bian=0;
		dxled2=0;   //两通道都亮红灯
		nbdeng2=0;
		nbdeng0=1;
		dxled0=1;
		dxled1=0;
		nbdeng1=0;
		sec_zhu=fuzhi0;			//重赋南北方向的起始值
		sec_ci=fuzhi1;		//重赋东西方向的起始值	
		
		jishu=0;   //暂停计数
    bian1=1;		//数码管显示标志位
}
	
if(key1==1)    //当开关未闭合时
{	
	bian2=0;
	if(bian1==1)   //再次初始状态
	{
		dxled2=1;
		nbdeng2=0;
		nbdeng0=1;
		dxled0=0;
		dxled1=0;
		nbdeng1=0;
		bian1--;
		jishu=0;
	}
	
  if(bian==0)    //当主干道通行时
{
	if(jishu==10)						//加到10也就是半秒
	{
		if((sec_ci<=3)&&(biaozhi==0)&&(flag==1))  		//东西黄灯闪
		{
			dxled2=0;
			dxled1=0;
		}
		if((sec_zhu<=3)&&(biaozhi==1)&&(flag==1))  		//南北黄灯闪
		{
			nbdeng2=0;
			nbdeng1=0;
		}
	}
if(jishu==20)		// 定时器中断次数=20时（即1秒时）
{	  jishu=0;						//清零计数器
		sec_zhu--;						//东西时间减1
		sec_ci--;						//南北时间减1

	if((sec_ci<=3)&&(biaozhi==0)&&(flag==1))  		//东西黄灯闪
	{
		dxled2=0;
		dxled1=1;
	}
	if((sec_zhu<=3)&&(biaozhi==1)&&(flag==1))  		//南北黄灯闪
	{
		nbdeng2=0;
		nbdeng1=1;
	}
	
	if(sec_zhu==0&&sec_ci==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_zhu=3;
		flag=1;
	}
		if(sec_zhu==3&&sec_ci==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_ci=3;
		flag=1;
	}
		if(sec_ci==0&&sec_zhu==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
		  dxled2=0;
		  nbdeng2=1;
		  nbdeng0=0;
		  dxled0=1;
		  dxled1=0;
		  nbdeng1=0;
			biaozhi=!biaozhi;
			flag=0;
			sec_zhu=fuzhi01;			//重赋南北方向的起始值
			sec_ci=fuzhi11;		//重赋东西方向的起始值
		  bian++;
	}
 }
}
if(bian==1)   //当次干道通行时
{
  if(jishu==10)						//加到10也就是半秒
	{
		if((sec_ci<=3)&&(biaozhi==0)&&(flag==1))  		//东西黄灯闪
		{
			dxled2=0;
			dxled1=0;
		}
		if((sec_zhu<=3)&&(biaozhi==1)&&(flag==1))  		//南北黄灯闪
		{
			nbdeng2=0;
			nbdeng1=0;
		}
	}
if(jishu==20)		// 定时器中断次数=20时（即1秒时）
{	  jishu=0;						//清零计数器
		sec_zhu--;						//东西时间减1
		sec_ci--;						//南北时间减1

	if((sec_ci<=3)&&(biaozhi==0)&&(flag==1))  		//东西黄灯闪
	{
		dxled2=0;
		dxled1=1;
	}
	if((sec_zhu<=3)&&(biaozhi==1)&&(flag==1))  		//南北黄灯闪
	{
		nbdeng2=0;
		nbdeng1=1;
	}
	
	if(sec_zhu==0&&sec_ci==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_zhu=3;
		flag=1;
	}
		if(sec_zhu==3&&sec_ci==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_ci=3;
		flag=1;
	}
		if(sec_ci==0&&sec_zhu==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
		  dxled2=1;
		  nbdeng2=0;
		  nbdeng0=1;
		  dxled0=0;
		  dxled1=0;
		  nbdeng1=0;
			biaozhi=!biaozhi;
			flag=0;
			sec_zhu=fuzhi0;			//重赋南北方向的起始值
			sec_ci=fuzhi1;		//重赋东西方向的起始值
		  bian--;
	}
	
}
}
}
}

