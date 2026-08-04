
#include<reg51.h>    //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义

#include"lcd1602.h"
#include"i2c.h"

sbit LED=P3^3;  //继电器控制引脚
sbit RELAY=P3^4;  //继电器控制引脚
sbit BUZZER=P3^5; //蜂鸣器控制引脚
sbit SET=P0^7;	  //设置
sbit LCK=P0^6;	  //关锁

#define LOCK_TIME_second 10
#define SU 1/256
#define GPIO_KEY P1	//4X4矩阵键盘输入
unsigned char KeyValue; //用来存放读取到的键值
unsigned char saved_Value[4]={6,7,6,7};	  //初始密码
unsigned char input_Value[4];
unsigned char Right_flg=0,Right_flg0=0; 
unsigned char set_flg=0,LOCK_flg=0;
unsigned char temp00;
unsigned int  time_cnt,time_remember,delay_ddll;
long int DECvalue=0;

//存储芯片存储的内容
/*===============================================
地址： 0    1    2     3    4    5      6     
数据：无   密码 密码  密码 密码	 无  错误次数
=================================================*/

void TimerConfiguration(void);  //定时中断
void KeyDown();		            //检测按键函数
void At24c02Write(unsigned char addr,unsigned char dat);
unsigned char At24c02Read(unsigned char addr);

//====主函数入口======
void main()
{
   char i=0,j=0;
   //unsigned char aa;
   char k=0;

   LcdInit();    //液晶初始化 
   clear_window();
   TimerConfiguration();   //初始化定时器

   Lcd1602_Delay1ms(5);
   At24c02Write(1,6);   //初始化存储芯片，实际电路中就不需要
   At24c02Write(2,7);
   At24c02Write(3,6);
   At24c02Write(4,7);
   At24c02Write(6,0);	////清除错误输入次数

   RELAY=1;
   BUZZER=1;

        while(1)
		{
		   KeyDown();	 ////按键扫描

		  ///===========正常输入密码操作======================
		   if (KeyValue<=9 && !set_flg && !LOCK_flg) //有按键按下
		   {
		     input_Value[i]=KeyValue;			 			 
			 if (Right_flg0)
			 {  clear_window();
			 	Right_flg0=0;
			 }

			 LcdWriteCom(0x80+i);  //显示当前按键的值
			 LcdWriteData('*');

			 i++;
		   	 if(i>=4)  //判断
			 {
			 	Right_flg0=0;
			    for (j=0;j<4;j++)
				{
				    if (input_Value[j]!=saved_Value[j])
					   Right_flg0=1;				
				}
				clear_window();
				if(!Right_flg0) 		 ////密码输入正确
				{ 
				  BUZZER=0;	 //声音报警
				  delay_ddll = 1;
				  LcdWriteCom(0x80+i+2);
				  LcdWriteData('O');
				  LcdWriteData('K');
				  At24c02Write(6,0);	////清除错误输入次数 
				}
				else
				{ 
				  BUZZER=1;
				  LcdWriteCom(0x80+i+2);
				  LcdWriteData('E');
				  LcdWriteData('R');

				  temp00=At24c02Read(6);  ////存储输入错误次数
				  if (temp00>=2)	      //连续输入错误超过3次
				  {	  
				    LOCK_flg=1;
					RELAY=0;  /////指示灯起来
					LcdWriteCom(0x80+i+2);
				    LcdWriteData('L');
				    LcdWriteData('K');				  	  
				  }
				  At24c02Write(6,temp00+1);		 				  	 
				}

				i=0;		    
			 }
		   }
		///===========正常输入密码操作 end======================


		 ///===========修改密码操作======================
		   if (KeyValue<=9 && set_flg && !LOCK_flg) //有按键按下 KeyValue!=66
		   {
			 input_Value[k]=KeyValue;
			 
			 LcdWriteCom(0x80+k);  //显示当前按键的值
			 LcdWriteData('*');

			  k++;
			  if(k>=4)  //判断
			  {
				  if (set_flg==2)
				  {					
				        for (j=0;j<4;j++)
						{
						  saved_Value[j]=input_Value[j];
						  At24c02Write(j+1,saved_Value[j]);						  						  						 						   				    
						}
						clear_window();

				  		set_flg=0;
						LED = 0;  //设置状态指示灯亮起来
						delay_ddll =1;
//				  		LcdWriteCom(0x80+k+2);
//						LcdWriteData('O');
//						LcdWriteData('K');
				  }

			 	  if (set_flg==1)
				  {///============================================					  							
						Right_flg=0;
					    for (j=0;j<4;j++)
						{
						  if(input_Value[j]!=saved_Value[j])
						    Right_flg=1;				    
						}
						clear_window();
						if(!Right_flg) 		 ///旧密码输入正确
						{ 
						  set_flg=2;         ///可以进行下一步操作						  
						  LcdWriteCom(0x80+0x40);  //
						  LcdWriteData('N');
						  LcdWriteData('E');
						  LcdWriteData('W');	 
						}
						else
						{ 
						  LcdWriteCom(0x80+k+2);
						  LcdWriteData('E');
						  LcdWriteData('R');
						  
						  temp00=At24c02Read(6);  ////存储输入错误次数
						  if (temp00>=2)	
						  {	  
						    LOCK_flg=1;
							RELAY=0;  /////指示灯起来
							LcdWriteCom(0x80+i+2);
						    LcdWriteData('L');
						    LcdWriteData('K');				  	  
						  }
						  At24c02Write(6,temp00+1);			 
						}
				  }	  ///============================================
					  				

				k=0;		    
			 }
		   }
	  ///===========修改密码操作 end======================

		   if (KeyValue==0x0A && !LOCK_flg)  //有按键按下	 
		   {
			   clear_window();
			   set_flg=1;
			   LcdWriteCom(0x80+0x40);  //
			   LcdWriteData('O');
			   LcdWriteData('L');
			   LcdWriteData('D');

		 	   k=0;  //=================		    			 
		   }
	 ////=============############================================
		   else if (KeyValue==0x0B && !LOCK_flg)  //有按键按下，关上锁，清除窗口!LCK
		   {	
			  BUZZER=1;
			  clear_window();		    			 
		   }
		  
		  if (delay_ddll)
		  {
		  	   delay_ddll++;
			   if (delay_ddll>2000)
			   {
			   	   LED = 1;
				   BUZZER = 1;
				   delay_ddll = 0;
			   }
		  }
	   ////=============================================
		 Lcd1602_Delay1ms(1);

		}
}


/*******************************************************************************
* 函 数 名         : KeyDown
* 函数功能		   : 检测有按键按下并读取键值
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void KeyDown(void)
{ 
	unsigned char a=0;
		 
	KeyValue=66;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)//读取按键是否按下
	{
		Lcd1602_Delay1ms(10); //延时10ms进行消抖
		if(GPIO_KEY!=0x0f)    //再次检测键盘是否按下
		{		
			//测试列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//测试行
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}

			while((a<250)&&(GPIO_KEY!=0xf0))	 //检测按键松手检测
			{
				Lcd1602_Delay1ms(2);
				a++;
			}
		}
	}
}

/*******************************************************************************
* 函数名         : void At24c02Write(unsigned char addr,unsigned char dat)
* 函数功能		   : 往24c02的一个地址写入一个数据
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
void At24c02Write(unsigned char addr,unsigned char dat)
{
	I2cStart();
	I2cSendByte(0xa0);//发送写器件地址
	I2cSendByte(addr);//发送要写入内存地址
	I2cSendByte(dat);	//发送数据
	I2cStop();
}
/*******************************************************************************
* 函数名         : unsigned char At24c02Read(unsigned char addr)
* 函数功能		   : 读取24c02的一个地址的一个数据
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/
unsigned char At24c02Read(unsigned char addr)
{
	unsigned char num;
	I2cStart();
	I2cSendByte(0xa0); //发送写器件地址
	I2cSendByte(addr); //发送要读取的地址
	I2cStart();
	I2cSendByte(0xa1); //发送读器件地址
	num=I2cReadByte(); //读取数据
	I2cStop();
	return num;	
}


/*******************************************************************************
* 函数名         : TimerConfiguration()
* 函数功能		   : 配置定时器值
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/		   

void TimerConfiguration()
{
    TMOD = 0x01; //选择工作方式1
    TH0 = 0x3C;	 //设置初始值,定时50MS
    TL0 = 0xB0; 
    EA = 1;			 //打开总中断
    ET0 = 1;		 //打开定时器0中断
    TR0 = 1;		 //启动定时器0
}

/*******************************************************************************
* 函数名         : Timer0()
* 函数功能		 : 定时器0中断函数
* 输入           : 无
* 输出         	 : 无
*******************************************************************************/		   
void Timer0() interrupt 1
{
	  TH0 = 0x3C;	 //设置初始值
      TL0 = 0xB0;
	  time_cnt++;
}