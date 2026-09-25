//51单片机简人体红外报警器设计，手动按键控制实现紧急报警、布防、撤防、关闭报警）//
#include <reg52.h>
#define uchar unsigned char 
#define uint  unsigned int 

uchar Mode=1,Mode1=0;
uchar t;

sbit s1=P1^0;
sbit s2=P1^2;
sbit s3=P1^1;

bit alarm=1,kai=1;

sbit LR=P2^0;//红灯报警灯
sbit LY=P2^2;//布防灯
sbit LG=P2^1;//红外信号检测灯

sbit Feng=P1^3;//蜂鸣器报警
sbit HR=P0^3;//红外模块接入端口

void delay(uint x) //毫秒级延时函数
{
  uint i,j;
  for(i=x;i>0;i--)
     for(j=110;j>0;j--);
}

void di()
{
	LR=0;//蜂鸣器报警，红灯跟随闪烁
	Feng=0;
	delay(80);
	LR=1;
	Feng=1;
	delay(200);	
}
void main()
{
	LR=1;//初始化三个灯都是灭
	LG=1;
	LY=1;
	delay(5);
	while(1)
	{
		if(s1==0)				//紧急手动报警
		{
				delay(5);	   //延时消抖
				if(s1==0)	
				{
					delay(5);
					while(!s1);
					kai=0;
					LY=1;	    
					LG=1;
					alarm=0; //进入报警状态
					Mode1=1; //Mode1置1：紧急报警过程中以防按了布防键取消紧急报警
					delay(5);
				}			 

		}
		if(s3==0)			//取消报警
		{
								
				delay(5);
				if(s3==0)				
				{
					delay(5);
					while(!s3);
					LR=1;
					LY=1;
					Mode=1;
					Mode1=0;
					alarm=1;
					delay(5);
				}
		}
		if(s2==0)			//布防
		{
			if(Mode1==1) //判断Mode1的状态，如果是紧急报警状态就保持，否则进入布防设置
			{
				alarm=0;
			}
			else
			{
				for(t=0;t<200;t++)
				{
				  LY=~LY;
				  delay(200);
				}
				LR=1;
				LY=0;
				Mode=0;	//布防状态标识
				kai=1;
				delay(5);
			}
				
		} 		
		if(alarm==0)
		{
			di();
			delay(100);
			di();
			delay(100);
		}
		if(HR==1&&Mode==0)
		{
			alarm=0;//开始进入报警
		}
		else
		{
			if(kai)
			{
				alarm=1;
				LR=1;
			}
		}
		if(HR==1)  //检测红外模块有信号，绿灯亮,否则灭
		{	   
			LG=0;
			delay(200);
		}
		else
		{
			LG=1;
		}			 
	}
}
