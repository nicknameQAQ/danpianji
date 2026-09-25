#include"lcd.h"

//--定义使用的IO口--//
#define KEY P3

sbit BEEP=P2^6;	   //接蜂鸣器	
unsigned char mima[6]={1,2,3,4,5,6};	
unsigned char mima3[6]={6,5,4,3,2,1};	
unsigned char mima4[6]={7,7,7,7,7,7};	
unsigned char mima2[6];

unsigned char state=0;
unsigned char cuowu=0;
unsigned char KeyValue;		 
unsigned char KeyState;	//记录按键的状态，0没有，1有

void TimerConfiguration();
unsigned char Time=0;
unsigned char sec=60;

void KeyDown();		 //检测按键函数

void Delay10ms(unsigned int c)   //误差 0us
{
    unsigned char a,b;
    for(;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}

void BZ()
{
	BEEP= 0;
	Delay10ms(100);
	BEEP= 1;
}

void BZ2()
{
	BEEP= 0;
	Delay10ms(30);
	BEEP= 1;
	Delay10ms(30);

	BEEP= 0;
	Delay10ms(30);
	BEEP= 1;
	Delay10ms(30);

	BEEP= 0;
	Delay10ms(30);
	BEEP= 1;
}




void main(void)
{
	unsigned char i=0;
	unsigned char j=0;

	KeyState=0;
	TimerConfiguration();
	
	LcdInit();
	while(1)
	{
		if(state==0)
		{
		LcdWriteCom(0x01);
		LCDWrite_String(0, 0, 16, "Shu ru mi ma:");
		LCDWrite_String(4, 1, 6, "      ");
		while(state==0)
		{
		KeyDown();
		if((KeyState==1))
		{
			if((KeyValue<10)&&(i<6))
			{
				LcdWriteCom(0xc4+i);
				LcdWriteData('*');
				mima2[i]=KeyValue;
				i++;
			}

			if((i==6)&&(KeyValue==15))
			{
				if(
					 ((mima2[0]==mima[0])
					&&(mima2[1]==mima[1])
					&&(mima2[2]==mima[2])
					&&(mima2[3]==mima[3])
					&&(mima2[4]==mima[4])
					&&(mima2[5]==mima[5]))
					||((mima2[0]==mima3[0])
					&&(mima2[1]==mima3[1])
					&&(mima2[2]==mima3[2])
					&&(mima2[3]==mima3[3])
					&&(mima2[4]==mima3[4])
					&&(mima2[5]==mima3[5]))
					||((mima2[0]==mima4[0])
					&&(mima2[1]==mima4[1])
					&&(mima2[2]==mima4[2])
					&&(mima2[3]==mima4[3])
					&&(mima2[4]==mima4[4])
					&&(mima2[5]==mima4[5]))
					)
					{
						LCDWrite_String(4, 1, 6, " Kai!");
						BZ();
						LCDWrite_String(4, 1, 6, "      ");
					}
				else
				{
					
					LCDWrite_String(4, 1, 6, "Cuowu!");
					BZ2();
					cuowu++;
					if(cuowu>=3)
					{
						state=1;
						cuowu=0;
						ET0 = 1;TR0 = 1;		 //打开定时器0中断//启动定时器0  
					}
					LCDWrite_String(4, 1, 6, "      ");
				}

				for(i=0;i<6;i++)
				{
					mima2[i]=0;	
				}
				i=0;	
			}
			
				if(KeyValue==14)
			{
				LCDWrite_String(4, 1, 6, "      ");
				for(i=0;i<6;i++)
				{
					mima2[i]=0;	
				}
				i=0;
			}	

			if(KeyValue==13)		//qingling
			{
				LCDWrite_String(4, 1, 6, "      ");
				for(i=0;i<6;i++)
				{
					mima2[i]=0;	
				}
				i=0;
			}

			if(KeyValue==12)		//shemoima
			{
				state=2;
			}

			KeyState=0;
		}
		}
		}

		if(state==1)
		{
		LcdWriteCom(0x01);
		LCDWrite_String(0, 0, 16, "Please wait!... ");
		while(state==1)
		{
			BZ2();
			LcdWriteCom(0xc4);
			LcdWriteData('0'+sec/10%10);
			LcdWriteData('0'+sec%10);
		}
		}

		if(state==3)
		{
			LcdWriteCom(0x01);
			LCDWrite_String(3, 0, 16, "WARRING... ");
	
			while(state==3);
		}



		if(state==2)
		{
		LcdWriteCom(0x01);
		LCDWrite_String(0, 0, 16, "Jiu mima :");
		while(state==2)
		{
		KeyDown();
		if((KeyState==1))
		{
			if((KeyValue<10)&&(i<6))
			{
				LcdWriteCom(0xc4+i);
				LcdWriteData('*');
				mima2[i]=KeyValue;
				i++;
			}
		

			if(KeyValue==14)	
			{
				if(i>0)
				{
					i--;
					LcdWriteCom(0xc4+i);
					LcdWriteData(' ');
				}				
			}	

			if(KeyValue==13)//qingling	
			{
				LCDWrite_String(4, 1, 6, "      ");
				for(i=0;i<6;i++)
				{
					mima2[i]=0;	
				}
				i=0;
			}

			if((i==6)&&(KeyValue==15)) //queding 
			{
				 if(
					 ((mima2[0]==mima[0])
					&&(mima2[1]==mima[1])
					&&(mima2[2]==mima[2])
					&&(mima2[3]==mima[3])
					&&(mima2[4]==mima[4])
					&&(mima2[5]==mima[5]))
					||((mima2[0]==mima3[0])
					&&(mima2[1]==mima3[1])
					&&(mima2[2]==mima3[2])
					&&(mima2[3]==mima3[3])
					&&(mima2[4]==mima3[4])
					&&(mima2[5]==mima3[5]))
					||((mima2[0]==mima4[0])
					&&(mima2[1]==mima4[1])
					&&(mima2[2]==mima4[2])
					&&(mima2[3]==mima4[3])
					&&(mima2[4]==mima4[4])
					&&(mima2[5]==mima4[5]))
					)
					{
						state=3;
					}
				else
				{
					
					LCDWrite_String(4, 1, 6, "Cuowu!");
					BZ2();
					state=0;
					LCDWrite_String(4, 1, 6, "      ");
				}

				for(i=0;i<6;i++)
				{
					mima2[i]=0;	
				}
				i=0;	
			}

			KeyState=0;
		}
		}
		}



		if(state==3)
		{
		LcdWriteCom(0x01);
		LCDWrite_String(0, 0, 16, "Xin mi ma:");
		while(state==3)
		{
		KeyDown();
		if((KeyState==1))
		{
			if((KeyValue<10)&&(i<6))
			{
				LcdWriteCom(0xc4+i);
		
				LcdWriteData('*');
				mima2[i]=KeyValue;
				i++;
			}

				if(KeyValue==14)	
			{
				if(i>0)
				{
					i--;
					LcdWriteCom(0xc4+i);
					LcdWriteData(' ');
				}				
			}	

			if(KeyValue==13)		//清零
			{
				LCDWrite_String(4, 1, 6, "      ");
				for(i=0;i<6;i++)
				{
					mima2[i]=0;	
				}
				i=0;
			}

			if((i==6)&&(KeyValue==15))
			{
				for(i=0;i<6;i++)
				{
					mima[i]=mima2[i];	
				}
				i=0;
				state=0;

				for(i=0;i<6;i++)
				{
					mima2[i]=0;	
				}
				i=0;	
			}

			KeyState=0;
		}
		}
		}


	}				
}


void KeyDown(void)
{
	unsigned int a=0;
	KEY=0x0f;
	if(KEY!=0x0f)
  {
		Delay10ms(1);
		a++;	
		a=0;
		if(KEY!=0x0f)
		{
			KeyState=1;//有按键按下
			//测试列
			KEY=0X0F;
			switch(KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;

			}
			//测试行
			KEY=0XF0;
			Delay10ms(1);
			switch(KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;

			}
			while((a<500)&&(KEY!=0xf0))	 //按键松手检测
			{
				Delay10ms(1);
				a++;
			}
			a=0;
		}
	}
}
void TimerConfiguration()
{
    TMOD = 0x01; //选择工作方式1
    TH0 = 0x3C;	 //设置初始值,定时50MS
    TL0 = 0xB0; 
    EA = 1;			 //打开总中断 		 
}

void Timer0() interrupt 1
{
	TH0 = 0x3C;	 //设置初始值
    TL0 = 0xB0;
	Time++;
	if(Time>=20)
	{
		Time=0;
		sec--;
		if(sec==0)
		{
			ET0 = 0;TR0 = 0;
			sec=0;
			state=0;
		}
	}
}