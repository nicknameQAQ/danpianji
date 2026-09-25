#include<reg52.h>  //包含52单片机的库
#define uchar unsigned char
#define uint unsigned int		//宏定义


bit biao=0;			//两干道控制引脚定义
bit shan=0;			//闪烁标志位
sbit kaiguan=P3^2; //定义按键的管脚


//初始时定义南北为主干道，东西方向为次干道//

sbit shu0 = P3^1; //次干道个位
sbit shu1= P2^7; //次干道十位
sbit shu2 = P3^6; //主干道个位
sbit shu3= P3^7; //主干道十位

sbit LED0=P2^6;		//南北方向红灯定义
sbit LED1=P2^5;	//南北方向黄灯定义
sbit LED2=P2^4;     //南北方向绿灯定义

sbit LED3=P2^3;		//东西方向红灯定义
sbit LED4=P2^2;	//东西方向黄灯定义
sbit LED5=P2^1;		//东西方向绿灯定义



uint xing=0,xing1=0,xing2=0,xing3;  //类型切换变量

uchar data xianshi[4];				//秒显示数组
uchar data sec_zhu=23;  			//东西数默认
uchar data sec_ci=20;			//南北默认值

uchar data chuzhi0=23;		//设置东西方向的时间
uchar data chuzhi1=20;		//设置南北方向的时间

uchar data chuzhi01=13;		//设置东西方向的时间
uchar data chuzhi11=10;		//设置南北方向的时间

uchar data jishu;//定时器0中断次数计数

uchar code zu[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x71};//段码


void delay(uint ms)		//延时函数
{
		uint j,k;
		for(j=0;j<ms;j++)			//延时ms
		for(k=0;k<110;k++);		//大约1毫秒的延时
}

void panduan()  //判断函数
{
		if(kaiguan==0)
		{
		xing3=0;
		}
    if(kaiguan==1)
		{
		xing3=1;
		}
}
	


//主程序
void main()
{
	  TMOD=0x01;					//定时器设置
		TH0=0x3C;							//重赋初值
	  TL0=0xB0;							//重赋初值50ms
		EA=1;						//开总中断
		ET0=1;						//定时器0中断开启
		TR0=1;
	  kaiguan=1;   //初始开关
	 	LED5=0;    //初始化让南北绿灯亮
		LED2=1;    //东西红灯亮
		LED0=0;
		LED3=1;
		LED4=0;
		LED1=0;
	
		while(1)					  //主循环
		{
     panduan();//判断函数
			
 if(xing2==0)   //显示判断 输入xing2
	{
		xianshi[0]=sec_ci/10; 		//东西秒十位
		xianshi[1]=sec_ci%10; 		//东西秒个位
		xianshi[2]=sec_zhu/10; 		//南北秒十位
		xianshi[3]=sec_zhu%10; 		//南北秒个位

		shu0=0;
		shu1=1;
		shu2=1;
		shu3=1;  

		P0=zu[xianshi[0]];		//送东西时间十位的数码管编码
		delay(1);				//延时
		shu0=1;
		shu1=0;
		shu2=1;
		shu3=1;   		
		P0=zu[xianshi[1]];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		shu0=1;
		shu1=1;
		shu2=0;
		shu3=1;    			
		P0=zu[xianshi[2]];		//送南北时间十位的数码管编码
		delay(1);				//延时
		shu0=1;
		shu1=1;
		shu2=1;
		shu3=0;    				 
		P0=zu[xianshi[3]];		//送南北时间个位的数码管编码
		delay(1);				//延时
	}
	
	if(xing2==1)    //当按键按下xing=1
	{
		shu0=0;
		shu1=1;
		shu2=1;
		shu3=1;             
		P0=zu[11];		//显示F值
		delay(1);				//延时
		shu0=1;
		shu1=0;
		shu2=1;
		shu3=1;   		
		P0=zu[11];		 //显示F值
		delay(1);				 //延时
		shu0=1;
		shu1=1;
		shu2=0;
		shu3=1;    			
		P0=zu[11];		//显示F值
		delay(1);				//延时
		shu0=1;
		shu1=1;
		shu2=1;
		shu3=0;    			
		P0=zu[11];		//显示F值
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

switch(xing3)
{
	case 0:
	{
	  xing2=1;       //定义为类型一为1
		xing=0;
		LED5=0;   //两通道都亮红灯
		LED2=0;
		LED0=1;
		LED3=1;
		LED4=0;
		LED1=0;
		sec_zhu=chuzhi0;			//重赋南北方向的起始值
		sec_ci=chuzhi1;		//重赋东西方向的起始值	
		
		jishu=0;   //暂停计数
    xing1=1;		//数码管显示标志位
   }; break;
	
	 
	case 1:
	{	
   xing2=0;
	if(xing1==1)   //再次初始状态
	{
		LED5=0;
		LED2=1;
		LED0=0;
		LED3=1;
		LED4=0;
		LED1=0;
		xing1--;
		jishu=0;
	}
	
  if(xing==0)    //当主干道通行时
{
	if(jishu==10)						//加到10也就是半秒
	{
		if((sec_ci<=3)&&(biao==0)&&(shan==1))  		//东西黄灯闪
		{
			LED5=0;
			LED4=0;
		}
		if((sec_zhu<=3)&&(biao==1)&&(shan==1))  		//南北黄灯闪
		{
			LED2=0;
			LED1=0;
		}
	}
if(jishu==20)		// 定时器中断次数=20时（即1秒时）
{	  jishu=0;						//清零计数器
		sec_zhu--;						//东西时间减1
		sec_ci--;						//南北时间减1

	if((sec_ci<=3)&&(biao==0)&&(shan==1))  		//东西黄灯闪
	{
				LED2=0;
		LED1=1;
		

	}
	if((sec_zhu<=3)&&(biao==1)&&(shan==1))  		//南北黄灯闪
	{
		LED5=0;
		LED4=1;
	}
	
	if(sec_zhu==0&&sec_ci==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_zhu=3;
		shan=1;
	}
		if(sec_zhu==3&&sec_ci==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_ci=3;
		shan=1;
	}
		if(sec_ci==0&&sec_zhu==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			LED0=1;
			LED5=1;
			biao=!biao;
			shan=0;
			sec_zhu=chuzhi01;			//重赋南北方向的起始值
			sec_ci=chuzhi11;		//重赋东西方向的起始值
		  xing++;
	}
 }
}
if(xing==1)   //当次干道通行时
{
  if(jishu==10)						//加到10也就是半秒
	{
		if((sec_ci<=3)&&(biao==0)&&(shan==1))  		//东西黄灯闪
		{
			LED2=0;
			LED1=0;
		}
		if((sec_zhu<=3)&&(biao==1)&&(shan==1))  		//南北黄灯闪
		{

			LED5=0;
			LED4=0;
		}
	}
if(jishu==20)		// 定时器中断次数=20时（即1秒时）
{	 
  	jishu=0;						//清零计数器
		sec_zhu--;						//东西时间减1
		sec_ci--;						//南北时间减1

	if((sec_ci<=3)&&(biao==0)&&(shan==1))  		//东西黄灯闪
	{
				LED2=0;
		LED1=1;
		

	}
	if((sec_zhu<=3)&&(biao==1)&&(shan==1))  		//南北黄灯闪
	{
		LED5=0;
		LED4=1;
	}
	
	if(sec_zhu==0&&sec_ci==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_zhu=3;
		shan=1;
	}
		if(sec_zhu==3&&sec_ci==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_ci=3;
		shan=1;
	}
		if(sec_ci==0&&sec_zhu==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			LED2=1;
			LED3=1;
			biao=!biao;
			shan=0;
			sec_zhu=chuzhi0;			//重赋南北方向的起始值
			sec_ci=chuzhi1;		//重赋东西方向的起始值
		  xing--;
	}
  }
  }
 };break;
	
	default : break;

}

}

