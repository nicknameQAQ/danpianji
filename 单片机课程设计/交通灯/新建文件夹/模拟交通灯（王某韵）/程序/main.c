#include<reg51.h>  //包含51单片机的库

#define uchar unsigned char
#define uint unsigned int		

uint ttd=0,ttd1=0,ttd2=0;  //切换变量
uchar data xian[4];				//秒显示的变量
uchar data sec0=20;  			//东西数默认
uchar data sec1=23;			//南北默认值

/*东西通行*/
uchar data dongsec=20;		//设置东西方向的时间
uchar data nansec=23;		//设置南北方向的时间

/*南北通行*/
uchar data dongsec1=13;		//设置东西方向的时间
uchar data nansec1=10;		//设置南北方向的时间

uchar data jishi;//定时器0中断次数


uchar code table[12]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x71};//段码

sbit ggd0 = P0^1; //南北方向个位显示
sbit ggd1= P0^2; //南北方向十位显示
sbit ggd2 = P0^3; //东西方向个位显示
sbit ggd3= P0^4; //东西方向十位显示

sbit lang0=P2^6;		//南北方向红灯引脚定义
sbit lang1=P2^5;	//南北方向黄灯引脚定义
sbit lang2=P2^4;     //南北方向绿灯引脚定义

sbit dengred1=P2^3;		//东西红灯
sbit dengyellow1=P2^2;	//东西黄灯
sbit denggreen1=P2^1;		//东西绿灯


bit quan=0;			//东西南北控制位
bit shuo=0;			//信号灯闪标志位

sbit kaigaun=P0^7; //定义紧急按键的管脚



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

void shuma()   //数码管显示
{
	switch(ttd2)
{
		case 0:
		{
		xian[0]=sec1/10; 		//第1位东西秒十位
		xian[1]=sec1%10; 		//第2位东西秒个位
		xian[2]=sec0/10; 		//第3位南北秒十位
		xian[3]=sec0%10; 		//第4位南北秒个位

		ggd0=0;
		ggd1=1;
		ggd2=1;
		ggd3=1;             
		P3=table[xian[0]];		//送东西时间十位的数码管编码
		delay(1);				//延时
		ggd0=1;
		ggd1=0;
		ggd2=1;
		ggd3=1;   		
		P3=table[xian[1]];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		ggd0=1;
		ggd1=1;
		ggd2=0;
		ggd3=1;    			
		P3=table[xian[2]];		//送南北时间十位的数码管编码
		delay(1);				//延时
		ggd0=1;
		ggd1=1;
		ggd2=1;
		ggd3=0;    				 
		P3=table[xian[3]];		//送南北时间个位的数码管编码
		delay(1);				//延时
		};break;
		
		case 1:
		{
		ggd0=0;
		ggd1=1;
		ggd2=1;
		ggd3=1;         
		P3=table[11];		//送东西时间十位的数码管编码
		delay(1);				//延时
		ggd0=1;
		ggd1=0;
		ggd2=1;
		ggd3=1;   	
		P3=table[11];		 //送东西时间个位的数码管编码
		delay(1);				 //延时
		ggd0=1;
		ggd1=1;
		ggd2=0;
		ggd3=1;    			
		P3=table[11];		//送南北时间十位的数码管编码
		delay(1);				//延时
		ggd0=1;
		ggd1=1;
		ggd2=1;
		ggd3=0;    			
		P3=table[11];		//送南北时间个位的数码管编码
		delay(1);				//延时
		};break;
		
		default :  break;
}
	
}




void main()        //主程序
{
	  chushihua();
	  kaigaun=1;    //键初始
	 	denggreen1=1;    //初始化红绿灯的状态
		lang2=0;
		lang0=1;
		dengred1=0;
		dengyellow1=0;
		lang1=0;
		while(1)					  //主循环
		{
			shuma();
		}
}

void time0(void) interrupt 1   	//定时中断子程序
{
	TH0=0x3C;							//重赋初值
	TL0=0xB0;							//12m晶振50ms//重赋初值
	TR0=1;								//重新启动定时器
	jishi++;							//软件计数加1
	
/*判断开关是否闭合*/	
if(kaigaun==0)           //当开关闭合时
	{
	  ttd2=1;
		ttd=0;
		denggreen1=0;   
		lang2=0;
		lang0=1;
		dengred1=1;
		dengyellow1=0;
		lang1=0;
		sec0=dongsec;			//重赋南北方向的起始值
		sec1=nansec;		//重赋东西方向的起始值	
		jishi=0;   //停止计数
    ttd1=1;		
}	

if(kaigaun==1)    //当开关未闭合时
{	
	ttd2=0;
	if(ttd1==1)   //再次初始状态
	{
		denggreen1=1;
		lang2=0;
		lang0=1;
		dengred1=0;
		dengyellow1=0;
		lang1=0;
		ttd1--;
		jishi=0;
	}
	
  if(ttd==0)    //当主干道通行时
{
	if(jishi==10)						//加到10也就是半秒
	{
		if((sec1<=3)&&(quan==0)&&(shuo==1))  		//东西黄灯闪
		{
			denggreen1=0;
			dengyellow1=0;
		}
		if((sec0<=3)&&(quan==1)&&(shuo==1))  		//南北黄灯闪
		{
			lang2=0;
			lang1=0;
		}
	}
if(jishi==20)		// 定时器中断次数=20时（即1秒时）
{	  jishi=0;						//清零计数器
		sec0--;						//东西时间减1
		sec1--;						//南北时间减1

	if((sec1<=3)&&(quan==0)&&(shuo==1))  		//东西黄灯闪
	{
		denggreen1=0;
		dengyellow1=1;
	}
	if((sec0<=3)&&(quan==1)&&(shuo==1))  		//南北黄灯闪
	{
		lang2=0;
		lang1=1;
	}
	
	if(sec0==0&&sec1==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec0=3;
		shuo=1;
	}
		if(sec0==3&&sec1==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec1=3;
		shuo=1;
	}
		if(sec1==0&&sec0==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			lang2=1;
			dengred1=1;
			quan=!quan;
			shuo=0;
			sec0=dongsec1;			//重赋南北方向的起始值
			sec1=nansec1;		//重赋东西方向的起始值
		  ttd++;
	}
 }
}
if(ttd==1)   //当辅干道通行时
{
  if(jishi==10)						//加到10也就是半秒
	{
		if((sec1<=3)&&(quan==0)&&(shuo==1))  		//东西黄灯闪
		{
			denggreen1=0;
			dengyellow1=0;
		}
		if((sec0<=3)&&(quan==1)&&(shuo==1))  		//南北黄灯闪
		{
			lang2=0;
			lang1=0;
		}
	}
if(jishi==20)		// 定时器中断次数=20时（即1秒时）
{	  jishi=0;						//清零计数器
		sec0--;						//东西时间减1
		sec1--;						//南北时间减1

	if((sec1<=3)&&(quan==0)&&(shuo==1))  		//东西黄灯闪
	{
		denggreen1=0;
		dengyellow1=1;
	}
	if((sec0<=3)&&(quan==1)&&(shuo==1))  		//南北黄灯闪
	{
		lang2=0;
		lang1=1;
	}
	
	if(sec0==0&&sec1==3) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec0=3;
		shuo=1;
	}
		if(sec0==3&&sec1==0) 		//当东西倒计时到0时，重置3秒，用于黄灯闪烁时间
	{
		sec1=3;
		shuo=1;
	}
		if(sec1==0&&sec0==0)			//当黄灯闪烁时间倒计时到0时，
	{
			P2=0x00;					//重置东西南背方向的红绿灯
			lang2=1;
			dengred1=1;
			quan=!quan;
			shuo=0;
			sec0=dongsec;			//重赋南北方向的起始值
			sec1=nansec;		//重赋东西方向的起始值
		  ttd--;
	 }	
  }
 }
}
	
}

