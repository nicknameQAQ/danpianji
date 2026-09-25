#include<reg52.h>
#define uchar unsigned char 
#define uint unsigned int
uchar temp,signal,tt1;


void delay_1ms(uint d)
{
	uint i;
	while(d--)
		for(i=0;i<75;i++);
}

void motor_run()		   //电机起动
{
  	P1=0x35;
	delay_1ms(150);
	P1=0x00;
	delay_1ms(20);
/*	ENA=1;

	OUT1=0;

	OUT2=1;
	delay_1ms(800);
	OUT2=0;
	delay_1ms(200);
	
	ENB=1;

	OUT4=0;

	OUT3=1;
	delay_1ms(800);
	OUT3=0;
	delay_1ms(200);
*/
}
void motor_left()			 //左进
{
	P1=0x30;
	delay_1ms(150);
	P1=0x00;
	delay_1ms(20);
}

void motor_right()		   //右进
{
	P1=0x05;
	delay_1ms(150);
	P1=0x00;
	delay_1ms(20);
}

void motor_big_right()			 //粗右进
{
	P1=0x55;
	delay_1ms(150);
	P1=0x00;
	delay_1ms(20);
}
void motor_big_left()
{
	P1=0x33;
	delay_1ms(150);
	P1=0x00;
	delay_1ms(20);
}
 void motor_stop()					//电机停止
{
	 P1=0x00;
}
void motor_back()
{
	P1=0x53;
}
void main()
{
	tt1=0;
	EA=1;
	ET1=1;
	TR1=1;
	TMOD=0x01;
	TH1=-(1000/256);
	TL1=-(1000%256);
	while(1)
	{

	 	temp=P2;
		signal=temp&0xff;		   //得到红外反向信号
		switch(signal)
		{
			case 0xff:	   //无偏差
				motor_run();
				break;
			case 0xfd:	   //1轮右偏			

				motor_left();
				break;
			case 0xef:	    //4轮左偏			   

				motor_right(); 
				break;
			case 0xfb:     //2轮右偏出轨
			case 0xf9:	   //1、2轮右偏						
 				motor_big_left();
//				delay_1ms(10);
				break;

			case 0xdf:	   //5左偏出轨		   	
			case 0xcf:      //4、5轮左偏
 				motor_big_right();
//				delay_1ms(10);
				break;

			case 0xfe:	   //0最右偏出轨	   
			case 0xfa:
 				motor_big_left();
//				delay_1ms(10);
				break;

			case 0xbf:     //6最左偏出轨	  
 			case 0x9f:	   
				motor_big_right();
//				delay_1ms(10);
				break;	
			
			case 0xeb:	 //前两传感器压在黑线上
			case 0xdb:	 //后两传感器压在黑线上
			case 0xbe:	 //中间两传感器压在黑线上
			case 0xac:	 	//前四传感器压在黑线上
			case 0x9a:		//后四传感器压在黑线上
				motor_back();
				delay_1ms(200);
				motor_stop();
				
//				delay_1ms(1000);																  	

			default:
				break;
		}  
	} 
			
}

void time1() interrupt 3
{

	TH1=-(1000/256);
	TL1=-(1000%256);
	tt1++;
	if(tt1==300)
	{
		motor_back();
	//	delay_1ms(1);
		tt1=0;
	}
}