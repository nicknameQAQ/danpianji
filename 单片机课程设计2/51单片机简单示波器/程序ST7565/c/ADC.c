#include <stdio.h>
#include <intrins.h>
#include "ADC.h"
#include "LCD12864.h"
#include "STC12C5A60S2.h"
//时间/每格 1-5ms 2-2ms 3-1ms 4-500us 5-200us 6-100us 7-50us 8-20us 9-10us
extern unsigned char timpd;
extern unsigned char xdata wave0[];
extern unsigned int  samptime;		//采样时间，即定时器0的初值
extern unsigned int  T0count;		//定时器采样时间间隔计数
extern unsigned int  index;			//每次采样的wave数组下标

void ADCinit(void)
{
	//unsigned int i;
	AUXR1=0x00;						//8位结果
	P1ASF=0x02;						//P1ASF=0x02;设置P1.1作为模拟口，AD输入
	ADC_RES=0;						//AD结果寄存器清0
	ADC_CONTR=0x80;					//开启AD电源
	//for(i=2000;i>0;i--);			//开启AD电源延时等待电压稳定
}
unsigned char getADC(unsigned char ch)
{
	ADC_CONTR=0xe8|ch;				//开启AD中断，最高速度 ADC_CONTR=0xe8|ch;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	while(!(ADC_CONTR&0x10));
	ADC_CONTR&=~0x10;
	return ADC_RES;
}

void timerinit(void)
{
	AUXR=0xd0;						//1T模式，不分频，开启时钟输出（12分频）；0xc0不开时钟输出
	BRT=6;							//SYSclk/24/(256-BRT)=5KHz时钟输出
	WAKE_CLKO=0x04;					//允许P1.0输出编程时钟
	//AUXR=0xc0;						
	TMOD=0x51;						//T0、T1都工作在方式1;T0定时，T1计数（测频率用）
	TH0=samptime>>8;
	TL0=samptime&0xff;
	TH1=0x3c;						//T1测频率，计数50000进一次中断
	TL1=0xb0;
	TR0=0;
	ET0=0;
	TR1=1;
	ET1=1;
	EA =1;
}
void T0_interrupt(void) interrupt 1
{
	TH0=samptime>>8;
	TL0=samptime&0xff;
	T0count=T0count+1;
	switch(timpd)		
	{
		case  3:
		{
		    if(T0count>=100)
			{
				wave0[index]=getADC(1);
				index++;
				T0count=0;
			}
			break;
		}
		case  4:				//5ms
		{
		    if(T0count>=50)
			{
				wave0[index]=getADC(1);
				index++;
				T0count=0;
			}
			break;
		}
		case  5:				//2ms
		{
			if(T0count>=20)
			{
				wave0[index]=getADC(1);
				index++;
				T0count=0;
			}
			break;
		}
		case  6:				//1ms
		{
		    if(T0count>=10)
			{
				wave0[index]=getADC(1);
				index++;
				T0count=0;
			}
			break;
		}
		case  7:				//500us
		{
		    if(T0count>=5)
			{
				wave0[index]=getADC(1);
				index++;
				T0count=0;
			}
			break;
		}
		case  8:				//200us
		{
			if(T0count>=2)
			{
				wave0[index]=getADC(1);
				index++;
				T0count=0;
			}
			break;
		}
		case  9:				//100us
		{
			wave0[index]=getADC(1);
			index++;
			T0count=0;
			break;
		}
	}
	if(index>=511)
	{
		index=511;
	}
}

void fastADC(void)		//30M晶振  精确40us/div
{
	wave0[0]=getADC(1);_nop_();_nop_();
	wave0[1]=getADC(1);_nop_();_nop_();
	wave0[2]=getADC(1);_nop_();_nop_();
	wave0[3]=getADC(1);_nop_();_nop_();
	wave0[4]=getADC(1);_nop_();_nop_();
	wave0[5]=getADC(1);_nop_();_nop_();
	wave0[6]=getADC(1);_nop_();_nop_();
	wave0[7]=getADC(1);_nop_();_nop_();
	wave0[8]=getADC(1);_nop_();_nop_();
	wave0[9]=getADC(1);_nop_();_nop_();
	wave0[10]=getADC(1);_nop_();_nop_();
	wave0[11]=getADC(1);_nop_();_nop_();
	wave0[12]=getADC(1);_nop_();_nop_();
	wave0[13]=getADC(1);_nop_();_nop_();
	wave0[14]=getADC(1);_nop_();_nop_();
	wave0[15]=getADC(1);_nop_();_nop_();
	wave0[16]=getADC(1);_nop_();_nop_();
	wave0[17]=getADC(1);_nop_();_nop_();
	wave0[18]=getADC(1);_nop_();_nop_();
	wave0[19]=getADC(1);_nop_();_nop_();
	wave0[20]=getADC(1);_nop_();_nop_();
	wave0[21]=getADC(1);_nop_();_nop_();
	wave0[22]=getADC(1);_nop_();_nop_();
	wave0[23]=getADC(1);_nop_();_nop_();
	wave0[24]=getADC(1);_nop_();_nop_();
	wave0[25]=getADC(1);_nop_();_nop_();
	wave0[26]=getADC(1);_nop_();_nop_();
	wave0[27]=getADC(1);_nop_();_nop_();
	wave0[28]=getADC(1);_nop_();_nop_();
	wave0[29]=getADC(1);_nop_();_nop_();
	wave0[30]=getADC(1);_nop_();_nop_();
	wave0[31]=getADC(1);_nop_();_nop_();
	wave0[32]=getADC(1);_nop_();_nop_();
	wave0[33]=getADC(1);_nop_();_nop_();
	wave0[34]=getADC(1);_nop_();_nop_();
	wave0[35]=getADC(1);_nop_();_nop_();
	wave0[36]=getADC(1);_nop_();_nop_();
	wave0[37]=getADC(1);_nop_();_nop_();
	wave0[38]=getADC(1);_nop_();_nop_();
	wave0[39]=getADC(1);_nop_();_nop_();
	wave0[40]=getADC(1);_nop_();_nop_();
	wave0[41]=getADC(1);_nop_();_nop_();
	wave0[42]=getADC(1);_nop_();_nop_();
	wave0[43]=getADC(1);_nop_();_nop_();
	wave0[44]=getADC(1);_nop_();_nop_();
	wave0[45]=getADC(1);_nop_();_nop_();
	wave0[46]=getADC(1);_nop_();_nop_();
	wave0[47]=getADC(1);_nop_();_nop_();
	wave0[48]=getADC(1);_nop_();_nop_();
	wave0[49]=getADC(1);_nop_();_nop_();
	wave0[50]=getADC(1);_nop_();_nop_();
	wave0[51]=getADC(1);_nop_();_nop_();
	wave0[52]=getADC(1);_nop_();_nop_();
	wave0[53]=getADC(1);_nop_();_nop_();
	wave0[54]=getADC(1);_nop_();_nop_();
	wave0[55]=getADC(1);_nop_();_nop_();
	wave0[56]=getADC(1);_nop_();_nop_();
	wave0[57]=getADC(1);_nop_();_nop_();
	wave0[58]=getADC(1);_nop_();_nop_();
	wave0[59]=getADC(1);_nop_();_nop_();
	wave0[60]=getADC(1);_nop_();_nop_();
	wave0[61]=getADC(1);_nop_();_nop_();
	wave0[62]=getADC(1);_nop_();_nop_();
	wave0[63]=getADC(1);_nop_();_nop_();
	wave0[64]=getADC(1);_nop_();_nop_();
	wave0[65]=getADC(1);_nop_();_nop_();
	wave0[66]=getADC(1);_nop_();_nop_();
	wave0[67]=getADC(1);_nop_();_nop_();
	wave0[68]=getADC(1);_nop_();_nop_();
	wave0[69]=getADC(1);_nop_();_nop_();
	wave0[70]=getADC(1);_nop_();_nop_();
	wave0[71]=getADC(1);_nop_();_nop_();
	wave0[72]=getADC(1);_nop_();_nop_();
	wave0[73]=getADC(1);_nop_();_nop_();
	wave0[74]=getADC(1);_nop_();_nop_();
	wave0[75]=getADC(1);_nop_();_nop_();
	wave0[76]=getADC(1);_nop_();_nop_();
	wave0[77]=getADC(1);_nop_();_nop_();
	wave0[78]=getADC(1);_nop_();_nop_();
	wave0[79]=getADC(1);_nop_();_nop_();
	wave0[80]=getADC(1);_nop_();_nop_();
	wave0[81]=getADC(1);_nop_();_nop_();
	wave0[82]=getADC(1);_nop_();_nop_();
	wave0[83]=getADC(1);_nop_();_nop_();
	wave0[84]=getADC(1);_nop_();_nop_();
	wave0[85]=getADC(1);_nop_();_nop_();
	wave0[86]=getADC(1);_nop_();_nop_();
	wave0[87]=getADC(1);_nop_();_nop_();
	wave0[88]=getADC(1);_nop_();_nop_();
	wave0[89]=getADC(1);_nop_();_nop_();
	wave0[90]=getADC(1);_nop_();_nop_();
	wave0[91]=getADC(1);_nop_();_nop_();
	wave0[92]=getADC(1);_nop_();_nop_();
	wave0[93]=getADC(1);_nop_();_nop_();
	wave0[94]=getADC(1);_nop_();_nop_();
	wave0[95]=getADC(1);_nop_();_nop_();
	wave0[96]=getADC(1);_nop_();_nop_();
	wave0[97]=getADC(1);_nop_();_nop_();
	wave0[98]=getADC(1);_nop_();_nop_();
	wave0[99]=getADC(1);_nop_();_nop_();
	wave0[100]=getADC(1);_nop_();_nop_();
	wave0[101]=getADC(1);_nop_();_nop_();
	wave0[102]=getADC(1);_nop_();_nop_();
	wave0[103]=getADC(1);_nop_();_nop_();
	wave0[104]=getADC(1);_nop_();_nop_();
	wave0[105]=getADC(1);_nop_();_nop_();
	wave0[106]=getADC(1);_nop_();_nop_();
	wave0[107]=getADC(1);_nop_();_nop_();
	wave0[108]=getADC(1);_nop_();_nop_();
	wave0[109]=getADC(1);_nop_();_nop_();
	wave0[110]=getADC(1);_nop_();_nop_();
	wave0[111]=getADC(1);_nop_();_nop_();
	wave0[112]=getADC(1);_nop_();_nop_();
	wave0[113]=getADC(1);_nop_();_nop_();
	wave0[114]=getADC(1);_nop_();_nop_();
	wave0[115]=getADC(1);_nop_();_nop_();
	wave0[116]=getADC(1);_nop_();_nop_();
	wave0[117]=getADC(1);_nop_();_nop_();
	wave0[118]=getADC(1);_nop_();_nop_();
	wave0[119]=getADC(1);_nop_();_nop_();
	wave0[120]=getADC(1);_nop_();_nop_();
	wave0[121]=getADC(1);_nop_();_nop_();
	wave0[122]=getADC(1);_nop_();_nop_();
	wave0[123]=getADC(1);_nop_();_nop_();
	wave0[124]=getADC(1);_nop_();_nop_();
	wave0[125]=getADC(1);_nop_();_nop_();
	wave0[126]=getADC(1);_nop_();_nop_();
	wave0[127]=getADC(1);_nop_();_nop_();
	wave0[128]=getADC(1);_nop_();_nop_();
	wave0[129]=getADC(1);_nop_();_nop_();
	wave0[130]=getADC(1);_nop_();_nop_();
	wave0[131]=getADC(1);_nop_();_nop_();
	wave0[132]=getADC(1);_nop_();_nop_();
	wave0[133]=getADC(1);_nop_();_nop_();
	wave0[134]=getADC(1);_nop_();_nop_();
	wave0[135]=getADC(1);_nop_();_nop_();
	wave0[136]=getADC(1);_nop_();_nop_();
	wave0[137]=getADC(1);_nop_();_nop_();
	wave0[138]=getADC(1);_nop_();_nop_();
	wave0[139]=getADC(1);_nop_();_nop_();
	wave0[140]=getADC(1);_nop_();_nop_();
	wave0[141]=getADC(1);_nop_();_nop_();
	wave0[142]=getADC(1);_nop_();_nop_();
	wave0[143]=getADC(1);_nop_();_nop_();
	wave0[144]=getADC(1);_nop_();_nop_();
	wave0[145]=getADC(1);_nop_();_nop_();
	wave0[146]=getADC(1);_nop_();_nop_();
	wave0[147]=getADC(1);_nop_();_nop_();
	wave0[148]=getADC(1);_nop_();_nop_();
	wave0[149]=getADC(1);_nop_();_nop_();
	wave0[150]=getADC(1);_nop_();_nop_();
	wave0[151]=getADC(1);_nop_();_nop_();
	wave0[152]=getADC(1);_nop_();_nop_();
	wave0[153]=getADC(1);_nop_();_nop_();
	wave0[154]=getADC(1);_nop_();_nop_();
	wave0[155]=getADC(1);_nop_();_nop_();
	wave0[156]=getADC(1);_nop_();_nop_();
	wave0[157]=getADC(1);_nop_();_nop_();
	wave0[158]=getADC(1);_nop_();_nop_();
	wave0[159]=getADC(1);_nop_();_nop_();
	wave0[160]=getADC(1);_nop_();_nop_();
	wave0[161]=getADC(1);_nop_();_nop_();
	wave0[162]=getADC(1);_nop_();_nop_();
	wave0[163]=getADC(1);_nop_();_nop_();
	wave0[164]=getADC(1);_nop_();_nop_();
	wave0[165]=getADC(1);_nop_();_nop_();
	wave0[166]=getADC(1);_nop_();_nop_();
	wave0[167]=getADC(1);_nop_();_nop_();
	wave0[168]=getADC(1);_nop_();_nop_();
	wave0[169]=getADC(1);_nop_();_nop_();
	wave0[170]=getADC(1);_nop_();_nop_();
	wave0[171]=getADC(1);_nop_();_nop_();
	wave0[172]=getADC(1);_nop_();_nop_();
	wave0[173]=getADC(1);_nop_();_nop_();
	wave0[174]=getADC(1);_nop_();_nop_();
	wave0[175]=getADC(1);_nop_();_nop_();
	wave0[176]=getADC(1);_nop_();_nop_();
	wave0[177]=getADC(1);_nop_();_nop_();
	wave0[178]=getADC(1);_nop_();_nop_();
	wave0[179]=getADC(1);_nop_();_nop_();
	wave0[180]=getADC(1);_nop_();_nop_();
	wave0[181]=getADC(1);_nop_();_nop_();
	wave0[182]=getADC(1);_nop_();_nop_();
	wave0[183]=getADC(1);_nop_();_nop_();
	wave0[184]=getADC(1);_nop_();_nop_();
	wave0[185]=getADC(1);_nop_();_nop_();
	wave0[186]=getADC(1);_nop_();_nop_();
	wave0[187]=getADC(1);_nop_();_nop_();
	wave0[188]=getADC(1);_nop_();_nop_();
	wave0[189]=getADC(1);_nop_();_nop_();
	wave0[190]=getADC(1);_nop_();_nop_();
	wave0[191]=getADC(1);_nop_();_nop_();
	wave0[192]=getADC(1);_nop_();_nop_();
	wave0[193]=getADC(1);_nop_();_nop_();
	wave0[194]=getADC(1);_nop_();_nop_();
	wave0[195]=getADC(1);_nop_();_nop_();
	wave0[196]=getADC(1);_nop_();_nop_();
	wave0[197]=getADC(1);_nop_();_nop_();
	wave0[198]=getADC(1);_nop_();_nop_();
	wave0[199]=getADC(1);_nop_();_nop_();
	wave0[200]=getADC(1);_nop_();_nop_();
	wave0[201]=getADC(1);_nop_();_nop_();
	wave0[202]=getADC(1);_nop_();_nop_();
	wave0[203]=getADC(1);_nop_();_nop_();
	wave0[204]=getADC(1);_nop_();_nop_();
	wave0[205]=getADC(1);_nop_();_nop_();
	wave0[206]=getADC(1);_nop_();_nop_();
	wave0[207]=getADC(1);_nop_();_nop_();
	wave0[208]=getADC(1);_nop_();_nop_();
	wave0[209]=getADC(1);_nop_();_nop_();
	wave0[210]=getADC(1);_nop_();_nop_();
	wave0[211]=getADC(1);_nop_();_nop_();
	wave0[212]=getADC(1);_nop_();_nop_();
	wave0[213]=getADC(1);_nop_();_nop_();
	wave0[214]=getADC(1);_nop_();_nop_();
	wave0[215]=getADC(1);_nop_();_nop_();
	wave0[216]=getADC(1);_nop_();_nop_();
	wave0[217]=getADC(1);_nop_();_nop_();
	wave0[218]=getADC(1);_nop_();_nop_();
	wave0[219]=getADC(1);_nop_();_nop_();
	wave0[220]=getADC(1);_nop_();_nop_();
	wave0[221]=getADC(1);_nop_();_nop_();
	wave0[222]=getADC(1);_nop_();_nop_();
	wave0[223]=getADC(1);_nop_();_nop_();
	wave0[224]=getADC(1);_nop_();_nop_();
	wave0[225]=getADC(1);_nop_();_nop_();
	wave0[226]=getADC(1);_nop_();_nop_();
	wave0[227]=getADC(1);_nop_();_nop_();
	wave0[228]=getADC(1);_nop_();_nop_();
	wave0[229]=getADC(1);_nop_();_nop_();
	wave0[230]=getADC(1);_nop_();_nop_();
	wave0[231]=getADC(1);_nop_();_nop_();
	wave0[232]=getADC(1);_nop_();_nop_();
	wave0[233]=getADC(1);_nop_();_nop_();
	wave0[234]=getADC(1);_nop_();_nop_();
	wave0[235]=getADC(1);_nop_();_nop_();
	wave0[236]=getADC(1);_nop_();_nop_();
	wave0[237]=getADC(1);_nop_();_nop_();
	wave0[238]=getADC(1);_nop_();_nop_();
	wave0[239]=getADC(1);_nop_();_nop_();
	wave0[240]=getADC(1);_nop_();_nop_();
	wave0[241]=getADC(1);_nop_();_nop_();
	wave0[242]=getADC(1);_nop_();_nop_();
	wave0[243]=getADC(1);_nop_();_nop_();
	wave0[244]=getADC(1);_nop_();_nop_();
	wave0[245]=getADC(1);_nop_();_nop_();
	wave0[246]=getADC(1);_nop_();_nop_();
	wave0[247]=getADC(1);_nop_();_nop_();
	wave0[248]=getADC(1);_nop_();_nop_();
	wave0[249]=getADC(1);_nop_();_nop_();
	wave0[250]=getADC(1);_nop_();_nop_();
	wave0[251]=getADC(1);_nop_();_nop_();
	wave0[252]=getADC(1);_nop_();_nop_();
	wave0[253]=getADC(1);_nop_();_nop_();
	wave0[254]=getADC(1);_nop_();_nop_();
	wave0[255]=getADC(1);_nop_();_nop_();
	wave0[256]=getADC(1);_nop_();_nop_();
	wave0[257]=getADC(1);_nop_();_nop_();
}

