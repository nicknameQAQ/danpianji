#include<reg52.h>  //包含52单片机的库
#define uchar unsigned char
#define uint unsigned int		
	

bit quanju=0;			//两干道控制引脚定义
bit ruo=0;			//闪烁标志位
sbit guan=P3^2; //定义按键的管脚


//初始时定义南北为主干道，东西方向为次干道//

sbit shumaguan0 = P3^1; //次干道个位
sbit shumaguan1= P1^7; //次干道十位
sbit shumaguan2 = P3^6; //主干道个位
sbit shumaguan3= P3^7; //主干道十位

sbit LE0=P1^6;		//南北方向红灯定义
sbit LE1=P1^5;	//南北方向黄灯定义
sbit LE2=P1^4;     //南北方向绿灯定义

sbit LE3=P1^3;		//东西方向红灯定义
sbit LE4=P1^2;	//东西方向黄灯定义
sbit LE5=P1^1;		//东西方向绿灯定义



uint SFD=0,SFD1=0,SFD2=0,SFD3;  //类型切换变量

uchar data xianshi[4];				//秒显示数组
uchar data sec_zhu=63;  			//东西数默认
uchar data sec_ci=60;			//南北默认值

uchar data set0=63;		//设置东西方向的时间
uchar data set1=60;		//设置南北方向的时间

uchar data set01=33;		//设置东西方向的时间
uchar data set11=30;		//设置南北方向的时间

uchar data jishu;//定时器0中断次数计数

uchar code tab[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x71};//段码


void delay(uint ms)		//延时函数
{
		uint j,k;
		for(j=0;j<ms;j++)			//延时ms
		for(k=0;k<110;k++);		//大约1毫秒的延时
}

void Inc()
{
	 TMOD=0x01;					//定时器设置
		TH0=0x3C;							//重赋初值
	  TL0=0xB0;							//重赋初值50ms
		EA=1;						//开总中断
		ET0=1;						//定时器0中断开启
		TR0=1;
}

void panduan()  //判断函数
{
		if(guan==0)
		{
		SFD3=0;
		}
    if(guan==1)
		{
		SFD3=1;
		}
}
	
void shumaguan()  //数码管显示
{
	switch(SFD2)
	{
	  case 0:      //开关打开时显示的内容
		{
		xianshi[0]=sec_ci/10; 		//东西秒十位
		xianshi[1]=sec_ci%10; 		//东西秒个位
		xianshi[2]=sec_zhu/10; 		//南北秒十位
		xianshi[3]=sec_zhu%10; 		//南北秒个位

		shumaguan0=0;
		shumaguan1=1;
		shumaguan2=1;
		shumaguan3=1;  

		P2=tab[xianshi[0]];		//送东西时间十位的数码管编码
		delay(1);				//延时
		shumaguan0=1;
		shumaguan1=0;
		shumaguan2=1;
		shumaguan3=1;   		
		P2=tab[xianshi[1]];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		shumaguan0=1;
		shumaguan1=1;
		shumaguan2=0;
		shumaguan3=1;    			
		P2=tab[xianshi[2]];		//送南北时间十位的数码管编码
		delay(1);				//延时
		shumaguan0=1;
		shumaguan1=1;
		shumaguan2=1;
		shumaguan3=0;    				 
		P2=tab[xianshi[3]];		//送南北时间个位的数码管编码
		delay(1);				//延时
		};break;
		
		case 1:    //开关闭合时显示的内容
		{
		shumaguan0=0;
		shumaguan1=1;
		shumaguan2=1;
		shumaguan3=1;             
		P2=tab[11];		//显示F值
		delay(1);				//延时
		shumaguan0=1;
		shumaguan1=0;
		shumaguan2=1;
		shumaguan3=1;   		
		P2=tab[11];		 //显示F值
		delay(1);				 //延时
		shumaguan0=1;
		shumaguan1=1;
		shumaguan2=0;
		shumaguan3=1;    			
		P2=tab[11];		//显示F值
		delay(1);				//延时
		shumaguan0=1;
		shumaguan1=1;
		shumaguan2=1;
		shumaguan3=0;    			
		P2=tab[11];		//显示F值
		delay(1);				//延时
		};break;
		
		default : break;
		
	}
}



//主程序
void main()
{
	  Inc();   //初始化定时器
	  guan=1;   //初始开关
	 	LE5=0;    //初始化让南北绿灯亮
		LE2=1;    //东西红灯亮
		LE0=0;
		LE3=1;
		LE4=0;
		LE1=0;
	
		while(1)					  //主循环
		{
     panduan();//判断函数
		 shumaguan();  //数码管显示
		}
}



void time0(void) interrupt 1   	//定时中断子程序
{
	TH0=0x3C;							//重赋初值
	TL0=0xB0;							//12m晶振50ms//重赋初值
	TR0=1;								//重新启动定时器
	jishu++;							//软件计数加1 //20个计数周期为1秒

switch(SFD3)
{
	case 0:
	{
	  SFD2=1;       //定义为类型一为1
		SFD=0;
		LE5=0;   //两通道都亮红灯
		LE2=0;
		LE0=1;
		LE3=1;
		LE4=0;
		LE1=0;
		sec_zhu=set0;			//重赋南北方向的起始值
		sec_ci=set1;		//重赋东西方向的起始值	
		
		jishu=0;   //暂停计数
    SFD1=1;		//数码管显示标志位
   }; break;
	
	 //东西通行时
	case 1:
	{	
   SFD2=0;
	if(SFD1==1)   //再次初始状态
	{
		LE5=0;
		LE2=1;
		LE0=0;
		LE3=1;
		LE4=0;
		LE1=0;
		SFD1--;
		jishu=0;
	}
	
  if(SFD==0)    //当主干道通行时
{
	if(jishu==5)						//加到10也就是半秒
	{
		if((sec_ci<=3)&&(quanju==0)&&(ruo==1))  		//东西黄灯闪
		{
			LE5=0;
			LE4=0;
		}
		if((sec_zhu<=3)&&(quanju==1)&&(ruo==1))  		//南北黄灯闪
		{
			LE2=0;
			LE1=0;
		}
	}
if(jishu==20)		// 定时器中断次数=20时（即1秒时）
{	  jishu=0;						//清零计数器
		sec_zhu--;						//东西时间减1
		sec_ci--;						//南北时间减1

	if((sec_ci<=3)&&(quanju==0)&&(ruo==1))  		//东西黄灯闪
	{
				LE2=0;
		LE1=1;
		

	}
	if((sec_zhu<=3)&&(quanju==1)&&(ruo==1))  		//南北黄灯闪
	{
		LE5=0;
		LE4=1;
	}
	
	if(sec_zhu==0&&sec_ci==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_zhu=3;
		ruo=1;
	}
		if(sec_zhu==3&&sec_ci==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_ci=3;
		ruo=1;
	}
		if(sec_ci==0&&sec_zhu==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
		
			LE0=1;
			LE5=1;
		  LE2=0;
		  LE3=0;
		  LE4=0;
		  LE1=0;
			quanju=!quanju;
			ruo=0;
			sec_zhu=set01;			//重赋南北方向的起始值
			sec_ci=set11;		//重赋东西方向的起始值
		  SFD++;
	}
 }
}
if(SFD==1)   //当次干道通行时
{
  if(jishu==10)						//加到10也就是半秒
	{
		if((sec_ci<=3)&&(quanju==0)&&(ruo==1))  		//东西黄灯闪
		{
			LE2=0;
			LE1=0;
		}
		if((sec_zhu<=3)&&(quanju==1)&&(ruo==1))  		//南北黄灯闪
		{

			LE5=0;
			LE4=0;
		}
	}
if(jishu==20)		// 定时器中断次数=20时（即1秒时）
{	 
  	jishu=0;						//清零计数器
		sec_zhu--;						//东西时间减1
		sec_ci--;						//南北时间减1

	if((sec_ci<=3)&&(quanju==0)&&(ruo==1))  		//东西黄灯闪
	{
				LE2=0;
		LE1=1;
		

	}
	if((sec_zhu<=3)&&(quanju==1)&&(ruo==1))  		//南北黄灯闪
	{
		LE5=0;
		LE4=1;
	}
	
	if(sec_zhu==0&&sec_ci==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_zhu=3;
		ruo=1;
	}
		if(sec_zhu==3&&sec_ci==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec_ci=3;
		ruo=1;
	}
		if(sec_ci==0&&sec_zhu==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			LE2=1;
			LE3=1;
		
			LE0=0;
			LE5=0;
		  LE4=0;
		  LE1=0;
		
			quanju=!quanju;
			ruo=0;
			sec_zhu=set0;			//重赋南北方向的起始值
			sec_ci=set1;		//重赋东西方向的起始值
		  SFD--;
	}
  }
  }
 };break;
	
	default : break;

}

}

