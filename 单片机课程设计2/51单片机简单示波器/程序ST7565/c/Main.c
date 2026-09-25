#include <stdio.h>
#include <intrins.h>
#include "ADC.h"
#include "PCA.h"
#include "Chars.h"
#include "EEPROM.h"
#include "LCD12864.h"
#include "STC12C5A60S2.h"

//时间/每格3-10ms 4-5ms 5-2ms 6-1ms 7-500us 8-200us 9-100us 10-50us 11-20us 12-10us
unsigned char timpd;			
unsigned char volpd;					//示波器 电压/每格 1--1V/d，2--0.5V/d，3--0.2V/d
unsigned long freqin;					//示波器输入的信号频率,单位Hz
unsigned int  T1count;					//T1定时器，频率计数
unsigned int  PCA0loop;					//PCA0循环初值
unsigned int  PCA1loop;					//PCA1循环初值
unsigned int  PCA0count;				//PCA0计时累计
unsigned int  PCA1count;				//PCA1循环递增值，即输出方波周期/2.5
unsigned int  samptime;					//采样时间，即定时器0的初值
unsigned int  T0count;					//定时器采样时间间隔计数
unsigned int  index;					//每次采样的wave0数组下标
unsigned int  shiftindex;				//移位时数组下标位置
unsigned char hold;						//hold键标志位
unsigned char autotim;					//是否根据频率自动选择时间每格，是=1，否=其它
unsigned char shift;					//hold状态移位标志位。1、左移；2、右移
//unsigned char grid;					//点格样式
unsigned char shap;				    	//波形失真修正，上下轮流显示
unsigned char ROMEN;					//需要写EEPROM时=0x55;不需要=其它
unsigned char EEPROM[5];				//四字节掉电保存数据，点格样式，时间/每格,电压/每格,波形样式,自动选时间档
unsigned char idata wave[127];			//当前显示数组
unsigned char xdata wave0[512];			//采样数组，处于不断更新状态
unsigned char xdata wavep[512];			//显示及HOLD缓存数组

//******************************************************************//
//小于100us/div(50/20/10)采用直接采样，软件延时
//大于等于100us/div采用定时器采样
//******************************************************************//

void main(void)
{
	unsigned char disp;					//disp显示刷新标志位
	unsigned char grid;					//点格样式
	unsigned char wavemode;				//波形样式
//	unsigned char volmax;	
	unsigned int i,j;
	unsigned int delaybtn;				//按键消抖延时
	unsigned long int delay,delaymin,freqauto;	

	disp=0;
    freqin=0;
	autotim=0;							//默认设置,手动
	timpd=8;							//默认设置200us/div							
	volpd=1;							//默认设置1V/div					
	index=0;					  	  	//赋值0非常重要
	shiftindex=0;
	T0count=0;
	T1count=0;
	PCA0loop=50000;						//PCA0定时20ms  //不分频时PCA0loop=62500; 480个周期1s
	PCA0count=0;
	//PCA1loop=15000;						//PCA1(P1.4)输出500Hz方波(产生LCD负压),本程序没开启						
	//PCA1count=15000;
	hold=0;
	shift=0;
	grid=1;								//默认设置，坐标线
	wavemode=1;							//默认设置，粗线；=2细线
	samptime=0xfee3;					//精确延时100us,此值严禁任何改动samptime=0xfee2;	
	EEPROM[0]=grid;
  	EEPROM[1]=timpd;  
  	EEPROM[2]=volpd;  
	EEPROM[3]=wavemode;
	EEPROM[4]=autotim;
	ROMEN=0x55;							//临时测试，正式必须=0；

	//M1/M0：00弱上拉，01强推挽，10输入（高阻），11开漏
	P0M1=0x00;P0M0=0x00;				//
	P1M1=0xee;P1M0=0x11;				//P1.0/P1.4推挽输出，其余设为输入
	P2M1=0x00;P2M0=0x00;				//
	P3M1=0x00;P3M0=0x00;				//

	init_st7565();						//12864初始化

	dispchar16(1,8,16,7,HZ1);			//开机LOGO
	write_string8(3,16,"www.crystalradio.cn");
	dispchar16(5,10,12,9,HZ2);

	ADCinit();
	timerinit();
	PCAinit();

	//for(i=500;i>0;i--)for(j=5000;j>0;j--);	//延时1

	IAPReadData(0,5,EEPROM);			//读取上一次关机前最后的用户设置
	if((EEPROM[0]>=0)&&(EEPROM[0]<=2)&&(EEPROM[1]>=3)&&(EEPROM[1]<=16)
	&&(EEPROM[2]>=1)&&(EEPROM[2]<=4)&&(EEPROM[3]>=1)&&(EEPROM[3]<=2)&&(EEPROM[4]==0)||(EEPROM[4]==1))
	{									//读取到合法设置则赋值，否则显示默认设置
		grid=EEPROM[0];
	  	timpd=EEPROM[1];  
	  	volpd=EEPROM[2]; 
		wavemode=EEPROM[3];
 	    autotim=EEPROM[4];
	}
	
	//for(i=500;i>0;i--)for(j=5000;j>0;j--);		//延时2

	disp_grid(grid);					//显示网格
    disp_osc_settings();				//显示示波器设置

	while(1)
	{
		//---------------------1、采样-----------------------//
		//按了HOLD键，采样继续，但不允许对wavep缓存数组更新
		if(hold==0)
		{
			switch(timpd)	
			{
				case  3:
				case  4:					//5ms/div
				case  5:					//2ms/div
				case  6:					//1ms/div
				case  7:					//500us/div
				case  8:					//200us/div
				case  9:					//100us/div
				{
					TR0=1;ET0=1;
					if(index>=511)			
					{
						for(i=0;i<512;i++) 
						{
							wavep[i]=wave0[i];
						}
						index=0;			//采样下一组数据
						disp=1;				//刷新标志置位
					}
					break;
				}
				case  10:					//50us/div
				{	
					TR0=0;ET0=0;
					for(i=0;i<512;i++)		//50us/div
					{
						wavep[i]=getADC(1);
						_nop_();_nop_();
						_nop_();_nop_();
					}
					disp=1;					//刷新标志置位
					break;
				}
				case  11:					//20us(40us插值得来)
				{
					TR0=0;ET0=0;
					fastADC();
					for(i=0;i<512;i++)  	//赋值（包含插值运算）
					{
						if(!(i%2))
						{wavep[i]=wave0[i/2];}
						else
						{wavep[i]=(unsigned int)(wave0[i/2]+wave0[i/2+1])/2;}
					}
					disp=1;					//刷新标志置位
					break;
				}
				case  12:					//10us(40us两次插值得来)
				{
					TR0=0;ET0=0;
					fastADC();
					for(i=0;i<512;i++)  	//赋值（包含插值运算）
					{
						if(!(i%2))
						{wavep[i]=wave0[i/2];}
						else
						{wavep[i]=(unsigned int)(wave0[i/2]+wave0[i/2+1])/2;}
					}
					for(i=0;i<512;i++)  	//赋值（包含插值运算）
					{
						wave0[i]=wavep[i];
					}
					for(i=0;i<512;i++)  	//赋值（包含插值运算）
					{
						if(!(i%2))
						{wavep[i]=wave0[i/2];}
						else
						{wavep[i]=(unsigned int)(wave0[i/2]+wave0[i/2+1])/2;}
					}
					disp=1;					//刷新标志置位
					break;	
				}
			}//switch(timpd)采样结束
		}

		//-------------------2-1：波形触发处理-----------------//
		if((disp==1)&&(hold==0))			//接收到新数据且正常显示时才处理
		{
			for(i=2;i<127;i++)		
			{
				if( (((signed int)wavep[i-2]-127)<=0)&&(((signed int)wavep[i-1]-127)<=0)//&&(((signed int)wavep[i]-127)<=0)
				&&(((signed int)wavep[i+1]-127)>=0)&&(((signed int)wavep[i+2]-127)>=0)&&(((signed int)wavep[i+3]-127)>0) )
				{	
					shiftindex=i;
					break;
				}		
			}
		}//if((disp==1)&&(hold==0))波形触发处理结束

		//-------------------2-2：HOLD状态移位处理-----------------//
		if(hold==1)
		{
			if(shift==2)					//左移
			{
				shiftindex=shiftindex+9;
				if((shiftindex+126)>=512)
				{
					shiftindex=shiftindex-9;
				}
			}
			if(shift==1)					//右移
			{
				shiftindex=shiftindex-9;	//注意无符号数
				if(shiftindex>512)			//相当于判断小于0
				{
					shiftindex=shiftindex+9;//防止无限递减
				}
			}
		}

		//-------------------3、电压衰减处理-----------------//
		switch(volpd)					//此部分余数计算可能有问题，需改进
		{
			case  1:					//1v/div
			{
				for(i=0;i<127;i++)  		
				{
					if(((signed int)wavep[i+shiftindex]>=127))
					{
						if(((wavep[i+shiftindex]-127)*35/256)<128)
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/256+28;
						else
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/256+29;
					}
					else
					{
						if(((127-wavep[i+shiftindex])*35/256)<128)
							wave[i]=27-(unsigned int)(127-wavep[i+shiftindex])*35/256;
						else
							wave[i]=26-(unsigned int)(127-wavep[i+shiftindex])*35/256;
					}
				}
				break;
			}
			case  2:					//0.5v/div
			{
				for(i=0;i<127;i++)  		
				{
					if(((signed int)wavep[i+shiftindex]>=127))
					{
						if(((wavep[i+shiftindex]-127)*35/128)<64)
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/128+28;
						else
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/128+29;
					}
					else
					{
						if(((127-wavep[i+shiftindex])*35/128)<64)
							wave[i]=27-(unsigned int)(127-wavep[i+shiftindex])*35/128;
						else
							wave[i]=26-(unsigned int)(127-wavep[i+shiftindex])*35/128;
						//注意 要防止显示越界@@@@@@@@@@@@@@@@3（见LCD12864.c）
						if(wave[i]>27)wave[i]=0;		//防止超出LCD12864显示边界
					}
				}
				break;
			}
			case  3:					//0.2v/div
			{
				for(i=0;i<127;i++)  		
				{
					if(((signed int)wavep[i+shiftindex]>=127))
					{
						if(((wavep[i+shiftindex]-127)*35/51)<26)							
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/51+28;						
						else									  
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/51+29;
						//if(wave[i]>56)wave[i]=56;
					}
					else
					{
						if(((127-wavep[i+shiftindex])*35/51)<26)
							wave[i]=27-(unsigned int)(127-wavep[i+shiftindex])*35/51;
						else
							wave[i]=26-(unsigned int)(127-wavep[i+shiftindex])*35/51;
						if(wave[i]>27)wave[i]=0;		//防止超出LCD12864显示边界
					}
				}
				break;
			}
			case  4:					//0.1v/div
			{
				for(i=0;i<127;i++)  		
				{
					if(((signed int)wavep[i+shiftindex]>=127))
					{
						if(((wavep[i+shiftindex]-127)*70/51)<26)							
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*70/51+28;						
						else									  
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*70/51+29;
						//if(wave[i]>56)wave[i]=56;
					}
					else
					{
						if(((127-wavep[i+shiftindex])*70/51)<26)
							wave[i]=27-(unsigned int)(127-wavep[i+shiftindex])*70/51;
						else
							wave[i]=26-(unsigned int)(127-wavep[i+shiftindex])*70/51;
						if(wave[i]>27)wave[i]=0;		//防止超出LCD12864显示边界
					}
				}
				break;
			}
		}//switch(volpd)电压衰减处理结束
		
		//---------------------4、波形显示-------------------//

		if((disp==1)&&(hold==0))		//正常显示
		{
			TR0=0;ET0=0;				//刷新过程不允许中断
			if(ROMEN==0x55)
			{
				EEPROM[0]=grid;
	  			EEPROM[1]=timpd;  
	  			EEPROM[2]=volpd; 
 			    EEPROM[3]=wavemode; 
				EEPROM[4]=autotim;
				IAPWriteData(0,5,EEPROM);
				ROMEN=0;
			}
			if(wavemode==1){dispwave2(grid);}		//矢量图
			if(wavemode==2){dispwave(grid);}		//散点图
			if(autotim==1)write_string8(3,106,"Auto");
			disp=0;						//刷新标志清零
			index=0;					//刷新后重新采样
			TR0=1;ET0=1;
		}
		if(hold==1)						//HOLD状态的显示
		{
			if(wavemode==1){dispwave2(grid);}		//矢量图
			if(wavemode==2){dispwave(grid);}		//散点图
			if(autotim==1)write_string8(3,106,"Auto");
			for(i=40;i>0;i--)			//显示延时
			{
				for(j=1200;j>0;j--);
				//if(P1&0x38)break;		//有按键按下提前退出延时循环
			}		
		}
		
		//---------------5、按键处理及显示延时---------------//

		shift=0;					//按键处理前清移位标志位
		if(wavemode==1)				//矢量显示延时小
		{
			if(timpd==3){i=1;}
			else if(timpd==4){i=2;}
			else {i=30;}
		}			
		if(wavemode==2)				//散点显示延时大
		{
			if(timpd==3){i=1;}
			else if(timpd==4){i=2;}
			else {i=120;}
		}			
		for(;i>0;i--)				//散点显示刷新i=120
		{
			//不同采样率采用不同延时，以得到相同刷新率
			switch(timpd)					
			{
				case  3:{j=1;delaymin=180000;delaybtn=60000;break;}		//10ms/div
				case  4:{j=1;delaymin=140000;delaybtn=60000;break;}		//5ms/div
				case  5:{j=100;delaymin=120000;delaybtn=40000;break;}		//2ms/div
				case  6:{j=200;delaymin=120000;delaybtn=40000;break;}		//1ms/div
				case  7:{j=400;delaymin=90000;delaybtn=30000;break;}		//500us/div
				case  8:{j=200;delaymin=60000;delaybtn=20000;break;}		//200us/div
				case  9:{j=120;delaymin=30000;delaybtn=10000;break;}		//100us/div
				case  10:{j=1000;delaymin=250000;delaybtn=80000;break;} 	//50us/div
				case  11:{j=1000;delaymin=250000;delaybtn=80000;break;} 	//20us(40us插值得来)
				case  12:{j=1000;delaymin=250000;delaybtn=80000;break;} 	//10us(40us两次插值得来)
			}
			for(;j>0;j--);					//内层循环延时
			//--------按键处理--------//
			//--1、HOLD键--//
			if((P3&0x04)==0)				
			{
				for(;delaybtn>0;delaybtn--);
				if((P3&0x04)==0)		
				{
					delay=0;
					if(hold==1){delaymin*=3;}//hold模式定时器全关，按键检测加长延时
					while((P3&0x04)==0)
					{
						delay++;
					}
					if(delay<delaymin)
					{
						if(hold==0){hold=1;TR0=0;ET0=0;}
						else{hold=0;TR0=1;ET0=1;}
					}
					else
					{
						if(hold==0)			//hold模式下不切换自动手动
						{
							if(autotim==1){autotim=0;}
							else {autotim=1;}
							ROMEN=0x55;
						}
					}
				}
			}
			//--2、时间++/左移键--//
			if((P3&0x08)==0)				
			{
				if(hold==0)					//正常显示，时间++
				{
					for(;delaybtn>0;delaybtn--);
					if((P3&0x08)==0)		
					{
						while((P3&0x08)==0);
						index=0;			//调整时间后必须重新采样
						timpd--;
						ROMEN=0x55;
						if(timpd<3){timpd=12;}
					}
				}
				if(hold==1)					//HOLD状态，波形左移
				{
					for(j=50000;j>0;j--);
					if((P3&0x08)==0)		
					{
						shift=1;break;
					}
				}
			}
			//--3、时间--/右移键--//
			if((P3&0x80)==0)				//时间--
			{
				if(hold==0)					//正常显示，时间++
				{
					for(;delaybtn>0;delaybtn--);
					if((P3&0x80)==0)		
					{
						while((P3&0x80)==0);
						index=0;
						timpd++;
						ROMEN=0x55;
						if(timpd>12)timpd=3;
					}
				}
				if(hold==1)					//HOLD状态，波形左移
				{
					for(j=50000;j>0;j--);
					if((P3&0x80)==0)		
					{
						shift=2;break;
					}
				}
			}
			//--4、//电压++--//
			if((P3&0x40)==0)				//电压++
			{
				for(;delaybtn>0;delaybtn--);
				if((P3&0x40)==0)		
				{
					while((P3&0x40)==0);
					volpd++;
					if(volpd>4)volpd=1;
					if(hold==0){ROMEN=0x55;}
				}
			}
			//--5、//电压----//
			if((P3&0x10)==0)				//电压--
			{
				for(;delaybtn>0;delaybtn--);
				if((P3&0x10)==0)		
				{
					while((P3&0x10)==0);
					volpd--;
					if(volpd<1)volpd=4;
					if(hold==0){ROMEN=0x55;}
				}
			}
			//--6、点格样式切换/线形切换--//
			if((P1&0x80)==0)				//点格样式切换
			{
				for(;delaybtn>0;delaybtn--);
				if((P1&0x80)==0)		
				{
					delay=0;
					if(hold==1){delaymin*=3;}//hold模式定时器全关，按键检测加长延时
					while((P1&0x80)==0)
					{
						delay++;
					}
					if(delay<delaymin)
					{
						grid++;
						if(grid>3)grid=0;
						if(hold==0){ROMEN=0x55;}
					}
					else
					{
						wavemode++;
						if(wavemode>2)wavemode=1;
						if(hold==0){ROMEN=0x55;}
					}
				}
			}
		}//for()按键处理end	

		//---------------6、根据频率自动选择采样时间---------------//
		if(autotim==1)
		{
			if(freqauto==freqin)
			{
				if((freqin>5)&&(freqin<20)){timpd=3;}
				else if((freqin>=20)&&(freqin<45)){timpd=4;}
				else if((freqin>=45)&&(freqin<120)){timpd=5;}
				else if((freqin>=120)&&(freqin<220)){timpd=6;}
				else if((freqin>=220)&&(freqin<500)){timpd=7;}
				else if((freqin>=500)&&(freqin<1200)){timpd=8;}
				else if((freqin>=1200)&&(freqin<2200)){timpd=9;}
				else if((freqin>=2200)&&(freqin<5000)){timpd=10;}
				else if((freqin>=5000)&&(freqin<12000)){timpd=11;}
				else if(freqin>=12000){timpd=12;}					
			}
			else
			{
				freqauto=freqin;
			}
			//volmax=0;
			//for(j=0;j<512;j++)
			//{
			//	if(wavep[j]>volmax){volmax=wavep[i];}
			//}
			//if(volmax>187){volpd=1;}
			//else if(volmax>151){volpd=2;}
			//else if(volmax>135){volpd=3;}
			//else if(volmax>127){volpd=4;}
		}

	}//while(1)-end
//*/
}
/*
		//-------------------3、电压衰减处理-----------------//
		switch(volpd)					//此部分余数计算可能有问题，需改进
		{
			case  1:					//1v/div
			{
				for(i=0;i<127;i++)  		
				{
					if(((signed int)wavep[i+shiftindex]>=127))
					{
						if(((wavep[i+shiftindex]-127)*35/256)<128)
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/256+28;
						else
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/256+29;
					}
					else
					{
						if(((127-wavep[i+shiftindex])*35/256)<128)
							wave[i]=27-(unsigned int)(127-wavep[i+shiftindex])*35/256;
						else
							wave[i]=26-(unsigned int)(127-wavep[i+shiftindex])*35/256;
					}
				}
				break;
			}
			case  2:					//0.5v/div
			{
				for(i=0;i<127;i++)  		
				{
					if(((signed int)wavep[i+shiftindex]>=127))
					{
						if(((wavep[i+shiftindex]-127)*35/128)<64)
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/128+28;
						else
							wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/128+29;
					}
					else
					{
						if(((127-wavep[i+shiftindex])*35/128)<64)
							wave[i]=27-(unsigned int)(127-wavep[i+shiftindex])*35/128;
						else
							wave[i]=26-(unsigned int)(127-wavep[i+shiftindex])*35/128;
					}
				}
				break;
			}
			case  3:					//0.2v/div
			{
				for(i=0;i<127;i++)  		
				{
					if(((signed int)wavep[i+shiftindex]>=127))
					{
						if(((wavep[i+shiftindex]-127)*35/51)<26)
						{
							if(((unsigned int)(wavep[i+shiftindex]-127)*35/51+28)>55){wave[i]=56;}
							else{wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/51+28;}
						}
						else
						{
							if(((unsigned int)(wavep[i+shiftindex]-127)*35/51+29)>55){wave[i]=56;}
							else{wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/51+29;}
						}
						//	wave[i]=(unsigned int)(wavep[i+shiftindex]-127)*35/51+29;
					}
					else
					{
						if(((127-wavep[i+shiftindex])*35/51)<26)
						{
							if(((signed int)27-(127-wavep[i+shiftindex])*35/51)<0){wave[i]=0;}
							else{wave[i]=(signed int)27-(127-wavep[i+shiftindex])*35/51;}
						}
						else
						{
							if(((signed int)26-(127-wavep[i+shiftindex])*35/51)<0){wave[i]=0;}
							else{wave[i]=(signed int)26-(127-wavep[i+shiftindex])*35/51;}
						}
					}
				}
				break;
			}
		}//switch(volpd)电压衰减处理结束

*/		
