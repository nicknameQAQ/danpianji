#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int		//宏定义

sbit gewei0 = P1^2; //P1.2南北个位显示
sbit shiwei0= P1^3; //P1.3南北十位显示
sbit gewei1 = P1^4; //P1.4东西个位显示
sbit shiwei1= P1^5; //P1.5东西十位显示

sbit Red0=P2^6;		//南北红灯标志 引脚定义
sbit Yellow0=P2^5;	//南北黄灯标志引脚定义
sbit Green_0=P2^4;     //南北绿灯标志引脚定义

sbit Red1=P2^3;		//东西红灯标志
sbit Yellow1=P2^2;	//东西黄灯标志
sbit Green1=P2^1;		//东西绿灯标志


bit biaozhi=0;			//东西南北控制位
bit huan=0;			//信号灯闪标志位

sbit key0=P1^0; //定义紧急按键的管脚


uint form=0,form1=0,form2=0;  //切换变量
uchar data shu[4];				//秒显示的变量
uchar data time0=60;  			//东西数默认
uchar data time1=63;			//南北默认值

/*东西通行*/
uchar data set_time0=60;		//设置东西方向的时间
uchar data set_time1=63;		//设置南北方向的时间

/*南北通行*/
uchar data time2=33;		//设置东西方向的时间
uchar data time3=30;		//设置南北方向的时间

uchar data num;//定时器0中断次数


uchar code table[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x71};//段码




void delay(uint ms)		//延时程序
{
		uint j,k;
		for(j=0;j<ms;j++)			//延时ms
		for(k=0;k<110;k++);		//大约1毫秒的延时
}



void main()        //主程序
{
	  key0=1;    //键初始
		TMOD=0x01;					//定时器设置
		TH0=0x3C;							//重赋初值
	  TL0=0xB0;							//12m晶振50ms//重赋初值
		EA=1;						//开总中断
		ET0=1;						//定时器0中断开启
		TR0=1;						//启动定时0
	 	Green1=1;    //初始化红绿灯的状态
		Green_0=0;
		Red0=1;
		Red1=0;
		Yellow1=0;
		Yellow0=0;
		while(1)					  //主循环
		{
			if(form2==0)   //显示判断 输入form2
	 {
		shu[0]=time1/10; 		//第1位东西秒十位
		shu[1]=time1%10; 		//第2位东西秒个位
		shu[2]=time0/10; 		//第3位南北秒十位
		shu[3]=time0%10; 		//第4位南北秒个位

		gewei0=0;
		shiwei0=1;
		gewei1=1;
		shiwei1=1;             
		P0=table[shu[0]];		//送东西时间十位的数码管编码
		delay(1);				//延时
		gewei0=1;
		shiwei0=0;
		gewei1=1;
		shiwei1=1;   		
		P0=table[shu[1]];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		gewei0=1;
		shiwei0=1;
		gewei1=0;
		shiwei1=1;    			
		P0=table[shu[2]];		//送南北时间十位的数码管编码
		delay(1);				//延时
		gewei0=1;
		shiwei0=1;
		gewei1=1;
		shiwei1=0;    				 
		P0=table[shu[3]];		//送南北时间个位的数码管编码
		delay(1);				//延时
	 }
	if(form2==1)    //当按键按下的时候显示状态
	 {
		gewei0=0;
		shiwei0=1;
		gewei1=1;
		shiwei1=1;         
		P0=table[11];		//送东西时间十位的数码管编码
		delay(1);				//延时
		gewei0=1;
		shiwei0=0;
		gewei1=1;
		shiwei1=1;   	
		P0=table[11];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		gewei0=1;
		shiwei0=1;
		gewei1=0;
		shiwei1=1;    			
		P0=table[11];		//送南北时间十位的数码管编码
		delay(1);				//延时
		gewei0=1;
		shiwei0=1;
		gewei1=1;
		shiwei1=0;    			
		P0=table[11];		//送南北时间个位的数码管编码
		delay(1);				//延时
		}
}
}

void times(void) interrupt 1   	//定时中断子程序
{
	TH0=0x3C;							//重赋初值
	TL0=0xB0;							//12m晶振50ms//重赋初值
	TR0=1;								//重新启动定时器
	num++;							//软件计数加1
	
/*判断开关是否闭合*/	
	

if(key0==1)    //当开关未闭合时
{	
	form2=0;
	if(form1==1)   //再次初始状态
	{
		Green1=1;
		Green_0=0;
		Red0=1;
		Red1=0;
		Yellow1=0;
		Yellow0=0;
		form1--;
		num=0;
	}
	
  if(form==0)    //当主干道通行时
{
	if(num==10)						//加到10也就是半秒
	{
		if((time1<=3)&&(biaozhi==0)&&(huan==1))  		//东西黄灯闪
		{
			Green1=0;
			Yellow1=0;
		}
		if((time0<=3)&&(biaozhi==1)&&(huan==1))  		//南北黄灯闪
		{
			Green_0=0;
			Yellow0=0;
		}
	}
if(num==20)		// 定时器中断次数=20时（即1秒时）
{	  num=0;						//清零计数器
		time0--;						//东西时间减1
		time1--;						//南北时间减1

	if((time1<=3)&&(biaozhi==0)&&(huan==1))  		//东西黄灯闪
	{
		Green1=0;
		Yellow1=1;
	}
	if((time0<=3)&&(biaozhi==1)&&(huan==1))  		//南北黄灯闪
	{
		Green_0=0;
		Yellow0=1;
	}
	
	if(time0==0&&time1==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		time0=3;
		huan=1;
	}
		if(time0==3&&time1==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		time1=3;
		huan=1;
	}
		if(time1==0&&time0==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			Green_0=1;
			Red1=1;
		  Red0=0;
		  Green1=0;
			biaozhi=!biaozhi;
			huan=0;
			time0=time2;			//重赋南北方向的起始值
			time1=time3;		//重赋东西方向的起始值
		  form++;
	}
 }
}
if(form==1)   //当辅干道通行时
{
  if(num==10)						//加到10也就是半秒
	{
		if((time1<=3)&&(biaozhi==0)&&(huan==1))  		//东西黄灯闪
		{
			Green1=0;
			Yellow1=0;
		}
		if((time0<=3)&&(biaozhi==1)&&(huan==1))  		//南北黄灯闪
		{
			Green_0=0;
			Yellow0=0;
		}
	}
if(num==20)		// 定时器中断次数=20时（即1秒时）
{	  num=0;						//清零计数器
		time0--;						//东西时间减1
		time1--;						//南北时间减1

	if((time1<=3)&&(biaozhi==0)&&(huan==1))  		//东西黄灯闪
	{
		Green1=0;
		Yellow1=1;
	}
	if((time0<=3)&&(biaozhi==1)&&(huan==1))  		//南北黄灯闪
	{
		Green_0=0;
		Yellow0=1;
	}
	
	if(time0==0&&time1==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		time0=3;
		huan=1;
	}
		if(time0==3&&time1==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		time1=3;
		huan=1;
	}
		if(time1==0&&time0==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			Green_0=0;
			Red1=0;
		  Red0=1;
		  Green1=1;
			biaozhi=!biaozhi;
			huan=0;
			time0=set_time0;			//重赋南北方向的起始值
			time1=set_time1;		//重赋东西方向的起始值
		  form--;
	 }	
  }
 }
}

if(key0==0)           //当开关闭合时
	{
	  form2=1;
		form=0;
		Green1=0;   
		Green_0=0;
		Red0=0;
		Red1=0;
		Yellow1=1;
		Yellow0=1;
    	time0=set_time0;			//重赋南北方向的起始值
		time1=set_time1;		//重赋东西方向的起始值	
		num=0;   //停止计数
    form1=1;		
}
	
}

