#include<reg51.h>
#define LEDData P0

unsigned char code LEDCode[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

int minit,second,count,count1;        	//分，秒，计数器
char min=15,sec=0;

sbit add1=P1^0;						//甲对加分，每按一次加1分   /在未开始比赛时为加时间分
sbit dec1=P1^1;						//甲对减分，每按一次减1分	/在未开始比赛时为减时间分
sbit exchange=P1^2;					//交换场地
sbit add2=P1^3;						//乙对加分，每按一次加1分	/在未开始比赛时为加时间秒
sbit dec2=P1^4;						//乙对减分，每按一次减1分	/在未开始比赛时为减时间秒
sbit p24_sec=P1^5;
sbit secondpoint=P0^7;				//秒闪动点
//----依次点亮数码管的位------
sbit led1=P2^7;
sbit led2=P2^6;
sbit led3=P2^5;
sbit led4=P2^4;
sbit led5=P2^3;
sbit led6=P2^2;
sbit led7=P2^1;
sbit led8=P2^0;
sbit led9=P3^7;
sbit led10=P3^6;
sbit led11=P3^5;
sbit led12=P3^4;

sbit alam=P1^7;						//报警

bit  playon=0;						//比赛进行标志位，为1时表示比赛开始，计时开启
bit  timeover=0;					//比赛结束标志位，为1时表示时间已经完
bit  AorB=0;						//甲乙对交换位置标志位
bit  halfsecond=0;					//半秒标志位

unsigned int scoreA;				//甲队得分
unsigned int scoreB;				//乙队得分
char sec24=24;		

void Delay5ms(void)
{
	unsigned int i;
	for(i=100;i>0;i--);
}


void display(void)
{
//-----------显示时间分--------------
	LEDData=LEDCode[minit/10];
	led1=0;
	Delay5ms();
	led1=1;
	LEDData=LEDCode[minit%10];
	led2=0;
	Delay5ms();
	led2=1;
//-------------秒点闪动------------
	if(halfsecond==1)
		LEDData=0x7f;
	else
		LEDData=0xff;
	led2=0;
	Delay5ms();
	led2=1;
	secondpoint=0;
//-----------显示时间秒------------
	LEDData=LEDCode[second/10];
	led3=0;
	Delay5ms();
	led3=1;
	LEDData=LEDCode[second%10];
	led4=0;
	Delay5ms();
	led4=1;
//-----------显示1组的分数百位-------
	if(AorB==0)
		LEDData=LEDCode[scoreA/100];
	else
		LEDData=LEDCode[scoreB/100];
	led5=0;
	Delay5ms();
	led5=1;
//---------------显示1组分数的十位-----------
	if(AorB==0)
		LEDData=LEDCode[(scoreA%100)/10];
	else
		LEDData=LEDCode[(scoreB%100)/10];
	led6=0;
	Delay5ms();
	led6=1;

//---------------显示1组分数的个位-----------
	if(AorB==0)
		LEDData=LEDCode[scoreA%10];
	else
		LEDData=LEDCode[scoreB%10];
	led7=0;
	Delay5ms();
	led7=1;

//-----------显示2组分数的百位-------
	if(AorB==1)
		LEDData=LEDCode[scoreA/100];
	else
		LEDData=LEDCode[scoreB/100];
	led8=0;
	Delay5ms();
	led8=1;
//-----------显示2组分数的十位-----------
	if(AorB==1)
		LEDData=LEDCode[(scoreA%100)/10];
	else
		LEDData=LEDCode[(scoreB%100)/10];
	led9=0;
	Delay5ms();
	led9=1;

//-----------显示2组分数的个位-----------
	if(AorB==1)
		LEDData=LEDCode[scoreA%10];
	else
		LEDData=LEDCode[scoreB%10];
	led10=0;
	Delay5ms();
	led10=1;

//-----------显示时间分--------------
	LEDData=LEDCode[sec24/10];
	led11=0;
	Delay5ms();
	led11=1;
	LEDData=LEDCode[sec24%10];
	led12=0;
	Delay5ms();
	led12=1;
}


//=========================按键检测程序================================================
void keyscan(void)
{
	if(playon==0)
	{
		if(add1==0)
		{
			display();
			if(add1==0);
			{
				if(minit<99)
					minit++;
				else
					minit=99; 
					min=minit;
			}
			do
				display();
			while(add1==0);
			
		}

		if(dec1==0)
		{
			display();
			if(dec1==0);
			{
				if(minit>0)
					minit--;
				else
					minit=0;
					min=minit;
			}
			do
				display();
			while(dec1==0);
			
		}

		if(add2==0)
		{
			display();
			if(add2==0);
			{
				if(second<59)
					second++;
				else
					second=59; 
					sec=second;
			}
			do
				display();
			while(add2==0);
			
		}

		if(dec2==0)
		{
			display();
			if(dec2==0);
			{
				if(second>0)
					second--;
				else
					second=0;  
					sec=second;
			}
			do
				display();
			while(dec2==0);
			
		}

		if(exchange==0)
		{
			display();
			if(exchange==0);
			{
				TR1=0;					//关闭T1计数器
				alam=1;					//关报警
   				AorB=~AorB;				//开启交换	
				minit=min;				//并将时间预设为15：00
				second=sec;
				sec24=24;
			}
			do
				display();
			while(exchange==0);
		}	
	}
	else
	{
		if(add1==0)
		{
			display();
			if(add1==0);
			{
				if(AorB==0)
				{
					if(scoreA<999)
						scoreA++;
					else
						scoreA=999;
				}
				else
				{
					if(scoreB<999)
						scoreB++;
					else
						scoreB=999;
				}
			}
			do
				display();
			while(add1==0);
		}

		if(dec1==0)
		{
			display();
			if(dec1==0);
			{
				if(AorB==0)
				{
					if(scoreA>0)
						scoreA--;
					else
						scoreA=0;
				}
				else
				{
					if(scoreB>0)
						scoreB--;
					else
						scoreB=0;
				}
			}
			do
				display();
			while(dec1==0);
		}

		if(add2==0)
		{
			display();
			if(add2==0);
			{
				if(AorB==1)
				{
					if(scoreA<999)
						scoreA++;
					else
						scoreA=999;
				}
				else
				{
					if(scoreB<999)
						scoreB++;
					else
						scoreB=999;
				}
			}
			do
				display();
			while(add2==0);
		}

		if(dec2==0)
		{
			display();
			if(dec2==0);
			{
				if(AorB==1)
				{
					if(scoreA>0)
						scoreA--;
					else
						scoreA=0;
				}
				else
				{
					if(scoreB>0)
						scoreB--;
					else
						scoreB=0;
				}
			}
			do
				display();
			while(dec2==0);
		}
		if(p24_sec==0)
		{
			display();
			if(p24_sec==0)
			{
				sec24=24;
				TR0=1;
				alam=1;
				while(p24_sec==0) display();
			}
		}
	}
}

//******************************主函数*************************************************
void main(void)
{
	TMOD=0x11;
 	TL0=0xb0;
 	TH0=0x3c;
 	TL1=0xb0;
 	TH1=0x3c;
	minit=min;				//初始值为15：00
	second=sec;
 	EA=1;
	ET0=1;
	ET1=1;
 	TR0=0;
	TR1=0;
	EX0=1;
	IT0=1;
	IT1=1;
//	EX1=1;
	PX0=1;
//	PX1=1;
	PT0=0;
	P1=0xFF;
	P3=0xFF;
	alam=1;
 	while(1)
 	{
		keyscan();
		display();			
    }
}


void PxInt0(void) interrupt 0
{
	Delay5ms();
	EX0=0;
	alam=1;
	TR1=0;
	if(timeover==1)
	{
		timeover=0;
	}

	if(playon==0)
	{
		playon=1;				//开始标志位
		TR0=1;					//开启计时
		
		if((minit+second)==0)
		{
			sec24=24;
			minit=min;			
			second=sec;
		}
	}
	else
	{
		playon=0;				//开始标志位清零，表示暂停
		TR0=0;					//暂时计时
	}	
	EX0=1;						//开中断	
}

/*
void PxInt1(void) interrupt 2
{
	Delay5ms();
	EX1=0;						//关中断
	if(timeover==1)				//比赛结束标志，必须一节结束后才可以交换，中途不能交换场地
	{
		TR1=0;					//关闭T1计数器
		alam=1;					//关报警
   		AorB=~AorB;				//开启交换	
		minit=15;				//并将时间预设为15：00
		second=0;
	}
	EX1=1;						//开中断
}
*/


//*********************************中断服务函数**************************************
void  time0_int(void) interrupt 1 
{
 	TL0=0xb0;
 	TH0=0x3c;
 	TR0=1;
  	count++;
	if(count==10)
	{
		halfsecond=0;
	}

   	if(count==20)
   	{
		count=0;
		halfsecond=1;
//		if(second>0)
		{
			second--;
			if(second<0)
			{
				if(minit>0)
				{
					second=59;
					minit--;
				}
				else
				{
					second=0;
					timeover=1;
					playon=0;
					TR0=0;
					TR1=1;
				}
			}
			sec24--;
			if(sec24<=0)
			{
				if((minit+second)!=0)
				{
					alam=0;
					TR0=0;
				}
				sec24=0;
			}
		}
   	}
}


//*********************************中断服务函数**************************************
void  time1_int(void) interrupt 3
{
	TL1=0xb0;
 	TH1=0x3c;
 	TR1=1;
  	count1++;
	if(count1==10)
	{
		alam=0;
	}

   	if(count1==20)
   	{
		count1=0;
		alam=1;
	}
} 
