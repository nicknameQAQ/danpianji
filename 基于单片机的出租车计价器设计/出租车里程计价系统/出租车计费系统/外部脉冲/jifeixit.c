#include<reg52.h>
#define uchar unsigned char 
#define	uint unsigned int 
#define T 100  	
sbit RES=P3^7;	
int S=0,M=30;
uchar count=0,ct=0;

uchar tab_d[10]={0X3F,0X06,0X5B,0X4F,0X66,	 //段码
0X6D,0X7D,0X07,0X7F,0X6F};
uchar tab_wei[8]={0xfe,0xfd,0xfb,0xf7,		 //位码
0xef,0xdf,0xbf,0x7f};

//延时函数
void delay(uint t)  // tms
{
	uchar i=0;
	while(t--)
	for(i;i<120;i++);
}

//数码管显示函数
void disp(uchar d,uchar x,uchar w)	//d为段选，x为小数点，w为位选
{
	P2=0xff; 	//位码选择	 只能先设置位码再设置段码
	P2=tab_wei[w];		
	P0=0;
	P0=tab_d[d]|x;		 // 加上小数点	
	delay(T);			
}

//计数函数
void timer()
{	
	disp(M%10,0,0);				//   角					
	disp(M/10%10,0x80,1);	 	//	 元		  0x80表示加上小数点
	if(M>=100)
	disp(M/100%10,0,2);	   		//	十元
	if(M>=1000)
	disp(M/1000%10,0,3);	 	//	百元
	disp(S%10,0,4);	     		//	  公里		
	disp(S/10%10,0x80,5);
	if(S>=100)
	disp(S/100%10,0,6);
	if(S>=1000)
	disp(S/1000%10,0,7);	 	  
}

//定时器0中断
void time_50ms() interrupt 1	
{
	ct++;
 	if(ct==5)
	{
		ct=0;
		S+=5;
		if(S>20)
		M+=8;			
	}
}	

//主函数
void main()
{		
	P0=0;
	P2=0XFF;
	TMOD=0X0E;	  //设置计数器模式
	IT0=1;		 //电平触发
	TH0=255;	   //1个脉冲延时
	TL0=255;
	TR0=1;
	IE=0X82;
	while(1)
	{
		if(RES==0)
		{
			S=0;
			M=30;
		}	
		timer();
	}
}