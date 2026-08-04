#include<AT89x51.H>
//p10;p11接in1,in2      p12,p13接in3,in4
#include<intrins.h>
#define Left_moto_pwm	  P1_1	 
#define Left_moto_pwm1	  P1_3
#define Left_moto_go      {P1_0=1,P1_1=0,P1_2=1,P1_3=0;}    
#define Left_moto_Stop    {P1_0=0,P1_1=0,P1_2=0,P1_3=0;}                       
typedef unsigned char uint8;
typedef unsigned int uint16;
typedef unsigned long uint32;
unsigned char i;  
sbit rs=P2^6;
sbit rw=P2^5;
sbit e=P2^7;
sbit dq=P3^7;
sbit K1=P3^0;//低速
sbit K2=P3^1;//中速
sbit K3=P3^2;//高速
sbit q=P3^3;//按下为自动
unsigned char pwm_val_left  =0;//变量定义
unsigned char push_val_left =0;// 电机占空比N/10
bit Left_moto_stop =1;
unsigned  int  time=0;
uint8 d[16]="   tempertury   ";
uint8 num[10]="0123456789";
uint8 a,c,temp;
	uint8 b;

void delay(uint16 i)
{
	while(i--);
}

//void delay(unsigned int k)
//{    
//     unsigned int x,y;
//	 for(x=0;x<k;x++) 
//	   for(y=0;y<2000;y++);
//}
void wrc(uint8 c)
{
	delay(1000);
	rs=0;
	rw=0;
	e=0;
	P0=c;
	e=1;
	delay(10);
	e=0;
}
void wrd(uint8 dat)
{
	delay(1000);
	rs=1;
	rw=0;
	e=0;
	P0=dat;
	e=1;
	delay(10);
	e=0;
	rs=0;
}
void lcdinit()
{
	delay(1000);
	wrc(0x38);
	wrc(0x38);
	wrc(0x38);
	wrc(0x06);
	wrc(0x0c);
	wrc(0x01);
}
void ds18b20init()		//18b20的初始化
{
	dq=1;
	delay(1);
	dq=0;
	delay(80);
	dq=1;
	delay(5);
	dq=0;
	delay(20);
	dq=1;
	delay(35);
}
void ds18b20wr(uint8 dat)	  //18b20写数据
{
	uint8 i;
	for(i=0;i<8;i++)
	{
		dq=0;
		dq=dat&0x01;
		dat>>=1;
		delay(8);//在时序上只有这一块对时序要求最准确，他的时间必须大于15us
		dq=1;
		delay(1);
	}	
}
uint8 ds18b20rd()		  //18b20读数据
{
	uint8 value,i;
	for(i=0;i<8;i++)	
	{
		dq=0;
		value>>=1;
		dq=1;
		if(dq==1)value|=0x80;
		delay(8);//在这一块也对时间要求特别准确，整段程序必须大于60us		
	}
	return value;
}
uint8 readtemp()			  //读取温度内需要复位的
{

	ds18b20init();		//初始化
	ds18b20wr(0xcc);   //发送忽略ROM指令
	ds18b20wr(0x44);   //发送温度转换指令
	delay(100);
	ds18b20init();	   //初始化
	ds18b20wr(0xcc);   //发送忽略ROM指令
	ds18b20wr(0xbe);   //发读暂存器指令
	a=ds18b20rd();	 //温度的低八位
	b=ds18b20rd();	 //温度的高八位
	b<<=4;			 //ssss s***；s为标志位s=0表示温度值为正数，s=1温度值为负数
	c=b&0x80;		//温度正负标志位确认
	b+=(a&0xf0)>>4;
	a=a&0x0f;	  //温度的小数部分
	return b;
}

void display()
{
	uint16 i,k;
	float dio;
	dio=a*0.0625;
	k=dio*10000;//取小数点后两位有效数字
	wrc(0x80+0x00);
	for(i=0;i<16;i++)
	{
		wrd(d[i]);
	}
	wrc(0x80+0x43);
	if(c==0x80)	//读取到负温度即为补码，要将其转换成源码
	{
		wrd('-');
		temp=temp-1;
		temp=(~temp)|0x80;//负数的补码即为反码+1；而负数的反码为其源码取反，除了符号位；正数的补码等于正数的反码等于正数的源码
	}	
	else
	{
		wrd('+');
		temp=(temp);
	}			
	wrd(num[temp/100]);
	wrd(num[temp%100/10]);
	wrd(num[temp%100%10]);
	wrd('.');
	wrd(num[k/1000]);
	wrd(num[k%1000/100]);
	wrd(0xdf);
	wrd('C');
}
//void buzidong()
//{
//if(K1==0)
//{delay(200);
//if(K1==0)
//i=7;}
//if(K2==0)
//{delay(200);
//if(K2==0)
//i=5;}
//if(K3==0)
//{delay(200);
//if(K3==0)
//i=0;}

//}
//void zidong()
//{	if(q==0&&20<temp<26) //速度1
//	{ 
//		delay(10);
//	if(q==0&&20<temp<26) 
//		     	push_val_left=8; 
//	    }
//		else if(q==0&&26<temp<28) //速度2
//		{ 
//			delay(10);	
//		  if(q==0&&26<temp<28)   
//			    push_val_left=5; 
//		}
//  else	if(q==0&&28<temp<35) //速度3
//	{
//		delay(10);   
//	  if(q==0&&28<temp<35) 
//			  push_val_left=0;
//   }
//  }
void  run(void)
{
   if(K1==0)
	{
		delay(10);
		if(K1==0)
			push_val_left=8;
	}
	else if(K2==0)
	{
				delay(10);
				if(K2==0)
				push_val_left=5;
	}  
	else if(K3==0)
	{
		delay(10);
		if(K3==0)
		push_val_left=0;
	}
	if((q==0)&&(temp<28)) //速度1
	{ 
		delay(10);
//	if((q==0)&&(22<temp<30)) 
		     	push_val_left=4; 
	  }
		else if((q==0)&&(28<temp&&temp<30)) //速度2
		{ 
			delay(10);	
//		  if((q==0)&&(30<temp<35))   
			    push_val_left=2; 
		}
  else	if(q==0&&30<temp&&temp<40) //速度3
	{
//		delay(10);   
//	  if(q==0&&35<temp<40) 
			  push_val_left=0;
   }
     }

void pwm_out_left_moto(void)
{  
     if(pwm_val_left<=push_val_left)
	  {
		     Left_moto_pwm=1; 
		     Left_moto_pwm1=1; 
		 }
		else 
	    {
	         Left_moto_pwm=0;
		     Left_moto_pwm1=0; 
		   }
		if(pwm_val_left>=10)
	       pwm_val_left=0;
   

}


 void timer0()interrupt 1   using 2
{
     TH0=0XFc;	  
	 TL0=0X18;
	 time++;
	 pwm_val_left++;

	 pwm_out_left_moto();

 }	

void main(void)
{
  lcdinit();
	 
	TMOD=0X01;
	TH0= 0XFc;		  //1ms定时
 	TL0= 0X18;
	TR0= 1;
	ET0= 1;
	EA = 1;
	
//       run( );	 //PWM调速前进
	Left_moto_go ;   
	while(1)							/*无限循环*/
	{ 
   	
//    display( );
   temp=readtemp();
		 display( );
		 run( );	
//		buzidong( );
//	   zidong( );
//	
		
	
	 }
}
	