//程序头函数
#include <reg52.h>
//显示函数
#include <display.h>

//宏定义
#define uint unsigned int 
#define uchar unsigned char
#define Data_ADC0809 P1
 
//管脚声明
sbit LED_R= P2^2;	//COled
sbit LED_Y= P2^0;
sbit FENG = P2^5;	//蜂鸣器
sbit san=P3^4;		//风扇
//ADC0809
sbit ST=P3^3;
sbit EOC=P3^6;
sbit OE=P3^2;
sbit A0=P3^1;		//0809A0地址脚
//按键
sbit Key1=P2^6;		//设置键
sbit Key2=P2^7;		//加键
sbit Key3=P3^7;		//减键

bit bdata flag2,flag7;
uchar set;


//函数声明
extern uchar ADC0809();
extern void Key();
/*
void delay(uint z)
{
	uint i,j;
	for(i=0;i<z;i++)
	for(j=0;j<121;j++);
}
*/

//
uchar MQ2=0,MQ7=0;		//读取传感器数据变量

//指示灯变量

uchar WARNING2=30,WARNING7=20;		//初始上限值

void init()				 //定时器初始化函数
{
	TMOD=0x01;			  //工作方式
 	TL0=0xb0;
 	TH0=0x3c;			 //赋初值
 	EA=1;				 //打开中断总开关
	ET0=1;				//打开中断允许开关
 	TR0=1;				//打开中断定时开关
}

void main()			   //主函数
{
	Init1602();		   //初始化显示函数
	init();			   //初始化定时器
	while(1)		   //进入循环
	{
		A0=0;		   //地址脚=0时，读取MQ2数据				                   
		MQ2=ADC0809();
		A0=1;		   //地址脚=1时，读取MQ7数据
		MQ7=ADC0809();
		if(set==0)	   //不在设置状态时
		Display_1602(MQ2,MQ7,WARNING2,WARNING7); //正常显示数据
		if(set==0)
		{
			if(MQ2<WARNING2)	   //判断数据是否小于报警值
			flag2=0;
			else if(MQ2>WARNING2)
			flag2=1;
			if(MQ7<WARNING7)
			flag7=0;
			else if(MQ7>WARNING7)
			flag7=1;			
		}
		else
		{
			flag2=0;flag7=0;
		}
		Key();						 //扫描按键函数
	}
}
//ADC0809读取信息
uchar ADC0809()
{
	uchar temp_=0x00;
	OE=0;	//初始化高阻太
	ST=0;	//转化初始化
	ST=1;	//开始转换
	ST=0;
	while(EOC==0)	//外部中断等待AD转换结束
	OE=1;	   //读取转换的AD值
	temp_=Data_ADC0809;
	OE=0;
	return temp_;
}

void Key()			  //按键函数
{
	if(Key1==0)		   //设置按键
	{
		while(Key1==0);//按键释放
		FENG=0;		   //打开蜂鸣器
		set++;		   //设置变量加
		flag2=0;
		flag7=0;	   //报警变量清零
		LED_Y=1;
		LED_R=1;	   //关闭LED
		san=1;		   //关闭风扇
		TR0=0;		   //关闭定时器
	}
	if(set==1)		   //设置报警值
	{
		write_com(0x38);//屏幕初始化
		write_com(0x80+0x40+6);//位置
   		write_com(0x0f);//打开显示 无光标 光标闪烁
   		write_com(0x06);//当读或写一个字符是指针后一一位
		FENG=1;

	}
	if(set==2)
	{
		write_com(0x38);//屏幕初始化
		write_com(0x80+0x40+15);//位置
   		write_com(0x0f);//打开显示 无光标 光标闪烁
   		write_com(0x06);//当读或写一个字符是指针后一一位
		FENG=1;

	}
	else if(set>=3)		 //设置完
	{
		set=0;			 //清零
		write_com(0x38);//屏幕初始化
		write_com(0x0c);//打开显示 无光标 无光标闪烁
		FENG=1;			//关闭蜂鸣器
		TR0=1;			//打开定时器
	}
	if(Key2==0&&set==1)	//加按键按下
	{
		while(Key2==0);		//按键释放
		FENG=0;				//打开蜂鸣器，做按键音
		WARNING2++;						//报警值加
		if(WARNING2>=255)				//报警值加到最大
		WARNING2=0;						//报警值清零
		write_com(0x80+0x40+4);			//选中位置显示报警值
		write_data('0'+WARNING2/100);	//将报警值拆开成单独的一位，显示。例如报警值是123，那么123除以100的商是1，这里显示的就是1
		write_data('0'+WARNING2/10%10);	//123除以10得到的商是12，,12除以10的余数就是2，这里显示的就是2
		write_data('0'+WARNING2%10);	//123除以10的余数是3，这里显示的就是3，数据+“0”是为了得到液晶中对应数字的显示码
		write_com(0x80+0x40+6);			//闪烁的位置
		FENG=1;							//关闭蜂鸣器
	}
	if(Key2==0&&set==2)					//注释同上
	{
		while(Key2==0);
		FENG=0;
		WARNING7++;
		if(WARNING7>=255)
		WARNING7=0;
		write_com(0x80+0x40+13);
		write_data('0'+WARNING7/100);
		write_data('0'+WARNING7/10%10);
		write_data('0'+WARNING7%10);
		write_com(0x80+0x40+15);//位置
		FENG=1;
	}
	if(Key3==0&&set==1)
	{
		while(Key3==0);
		FENG=0;
		WARNING2--;
		if(WARNING2<=0)
		WARNING2=255;
		write_com(0x80+0x40+4);
		write_data('0'+WARNING2/100);
		write_data('0'+WARNING2/10%10);
		write_data('0'+WARNING2%10);
		write_com(0x80+0x40+6);//位置
		FENG=1;
	}
	if(Key3==0&&set==2)
	{
		while(Key3==0);
		FENG=0;
		WARNING7--;
		if(WARNING7<=0)
		WARNING7=255;
		write_com(0x80+0x40+13);
		write_data('0'+WARNING7/100);
		write_data('0'+WARNING7/10%10);
		write_data('0'+WARNING7%10);
		write_com(0x80+0x40+15);//位置
		FENG=1;
	}
}

void  time1_int(void) interrupt 1//定时器工作函数
{
	uchar count;				 //定义变量用于定时	 
	TL0=0xb0;
 	TH0=0x3c;					 //定时器重新赋初值50ms
  	count++;					 //变量50ms加一次
	if(count==10)				 //加10次就是500ms
	{
		count=0;				 //变量清零
		if(flag2==0)			 //报警关
			LED_Y=1;			 //报警指示灯熄灭
		else					 //报警开
			LED_Y=0;			 //对应的报警指示灯点亮
		if(flag7==0)
			LED_R=1;
		else 
			LED_R=0;
		if(flag2==1||flag7==1)	 //有报警
		{
			FENG=!FENG;			 //蜂鸣器闪烁报警
			san=0;				 //打开继电器启动风扇
		}
		if(flag2==0&&flag7==0)	 //都没有报警
		{
			FENG=1;				 //关闭蜂鸣器
			san=1;				 //关闭风扇
		}
	}
} 