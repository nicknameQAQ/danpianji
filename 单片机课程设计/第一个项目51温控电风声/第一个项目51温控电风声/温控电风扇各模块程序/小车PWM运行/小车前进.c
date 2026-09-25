#include<AT89x51.H>
//p10;p11接in1,in2      p12,p13接in3,in4


#define Left_moto_pwm	  P1_1	 
#define Left_moto_pwm1	  P1_3
#define Left_moto_go      {P1_0=1,P1_1=0,P1_2=1,P1_3=0;}    
#define Left_moto_Stop    {P1_0=0,P1_1=0,P1_2=0,P1_3=0;}                       

sbit K1=P3^0;//低速
sbit K2=P3^1;//中速
sbit K3=P3^2;
typedef unsigned char uint8;
	unsigned char pwm_val_left=0 ;//变量定义
	unsigned char push_val_left=0;// 电机占空比N/10
	bit Left_moto_stop =1;
	unsigned  int  time=0;
  uint8 temp,i;

void delay(unsigned int k)
{    
     unsigned int x,y;
	 for(x=k;x>0;x--) 
	   for(y=110;y>0;y--);
}

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

	pwm_val_left++;
	 pwm_out_left_moto();

 }	

void main(void)
{

	TMOD=0X01;
	TH0= 0XFc;		  //1ms定时
 	TL0= 0X18;
	TR0= 1;
	ET0= 1;
	EA = 1;
  //run();	 //PWM调速前进
 Left_moto_go ;   //电机开始转动。
	while(1)							/*无限循环*/
	{ 

			run();
					 
	 }
}
	