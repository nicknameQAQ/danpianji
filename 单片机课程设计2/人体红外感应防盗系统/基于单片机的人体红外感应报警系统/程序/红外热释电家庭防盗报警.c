#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int
sbit huang=P2^0;//黄灯
sbit lv=P2^2;//绿灯
sbit hong=P2^4;//红灯

sbit hongwai=P1^0;//热释电
sbit butter=P2^6;//蜂鸣器低电平开蜂鸣器

sbit  baojing=P1^1//直接报警按键;
sbit  bufang=P1^7;//布防按键
sbit  quxiao=P3^4;//取消按键
uchar tt=0;//定时器变量
uchar num=0;//定时的计时变量
uchar flag=0;//标志位
void timer1init(void)//定时器1，16位方式2
{	EA=0;
       TMOD=0x11;//设置定时器0为工作方式1
	TH0=(65536-10000)/256;
	TL0=(65536-10000)%256;
	ET0=1;//开定时器0中断
	TR0=1;//启动定时器0
	tt=0;//
	EA=0;//关总中断
}
 void delay(uint z)//延时函数
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
void main()//主函数
{       
                 huang=0;//初始黄灯亮
		 lv=0;//初始绿灯亮
		 hong=0;//初始红灯亮
		 timer1init();//初始化定时器
		 while(1)
		 {
		   
            if(!baojing)//如果直接报警键被按下
				{
					delay(5);//延时消抖
					if(!baojing)//判断报警键是不是被按下
					{ 
                                          hong=1;//红灯亮
					  lv=0;	//绿灯灭
					  flag=7;//标志位等于7
					  butter=0;//蜂鸣器响
					  EA=0;//关中断
					}
					while(!baojing);//判断按键是不是弹开
					delay(5);//如果弹开延时消抖
					while(!baojing);//弹开后跳出程序
				}            
             if((!bufang)&&(flag<4))//在初始状态和按下取消键后可以进去这个函数
				{
					delay(5);//延时消抖
					if(!bufang)//如果布防被按下
					{ 
					   EA=1;   //开定时器
					   flag=2;//另=2
					}
					while(!bufang);//布防键是不是弹开
					delay(5);//延时
					while(!bufang);//布防键弹开
				}
			if(!quxiao)//如果取消键按下
				{
					delay(5);//延时消抖
					if(!quxiao)//取消键是不是按下
					{   
					  flag=3;//FLAG等于3
					  butter=1;//蜂鸣器不响
					  tt=0;//
					  num=0;//定时初始0
					  EA=0;//关中断
					  lv=0;//绿灯灭
					  hong=0;//红灯灭
					}
					while(!quxiao);//取消按键抬开
					delay(5);//延时消抖
					while(!quxiao);//如果弹开跳出

				}
		    if(flag==4)//如果布防时间到
			  {
			   tt=0;//清计数为0
			   EA=0;//关中断
			   num=0;//记的秒数清零
			   lv=1;//关绿灯
			   flag=5;//flag等于5

			  }	
			if(hongwai)//如果感应到人黄灯亮
			   huang=1;
			else  huang=0;//感应不到人黄灯灭

			if(flag==5)//如果flag等于5
			   {
			     if(hongwai)//如果感应到人
				   {
				    butter=0;//蜂鸣器响
				    EA=1;//开中断
				    }
			   }
				  
		 }
}

void time0() interrupt 1
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	tt++;
	if(tt==10)
	  {tt=0; num++;}
	if((flag==5)&&(num%2==0))//布防15秒后会让flag=5 ，num为偶数灯亮 
	    hong=1;
	if((flag==5)&&(num%2==1))//布防15秒后会让flag=5 ，num为奇数灯灭 
	    hong=0;
	if((flag==2)&&(num%2==0))//按键布防键后flag等于2，num为偶数灯亮
	      lv=1;
	if((flag==2)&&(num%2==1)) // 按键布防键后flag等于2，num为奇数灯灭
	      lv=0;
	if((flag==2)&&(num==30))//布防时间，num=30即15秒，可以更改此实现布防延迟时间
	   flag=4;

	   
}
  