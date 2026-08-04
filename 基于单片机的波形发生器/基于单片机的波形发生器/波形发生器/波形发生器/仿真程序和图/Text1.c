#include <reg51.h>
#include<string.h>
#define uchar unsigned char
#define uint unsigned int
sbit RS=P3^0;
sbit RW=P3^1;
sbit EN=P3^3;
sbit K1=P2^0;
sbit K2=P2^1;
sbit K3=P2^2;
sbit K4=P2^3;
sbit K5=P2^4;
uchar WaveChoice=1;
uchar ys=30;
uchar i,a=0;
uchar sqar_num=128;
uint freq;
uchar code Sin[]={"Sine      "};
uchar code Squ[]={"Square    "} ;
uchar code Tri[]={"Triangle  "};
uchar code Saw[]={"Sawtooth  "} ;
uchar code No[]={"No Signal out "};
uchar code Wave[]={"Wave :"}; 
uchar code Fre[]={"Freq :"};
uchar code tosin[256]=
{0x80,0x83,0x86,0x89,0x8d,0x90,0x93,0x96,0x99,0x9c,0x9f,0xa2,0xa5,0xa8,0xab,
0xae,0xb1,0xb4,0xb7,0xba,0xbc,0xbf,0xc2,0xc5,0xc7,0xca,0xcc,0xcf,0xd1,0xd4,0xd6,0xd8,
0xda,0xdd,0xdf,0xe1,0xe3,0xe5,0xe7,0xe9,0xea,0xec,0xee,0xef,0xf1,0xf2,0xf4,
0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfd,0xfe,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xfe,0xfd,0xfd,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,0xf6,0xf5,
0xf4,0xf2,0xf1,0xef,0xee,0xec,0xea,0xe9,0xe7,0xe5,0xe3,0xe1,0xde,0xdd,0xda,0xd8,
0xd6,0xd4,0xd1,0xcf,0xcc,0xca,0xc7,0xc5,0xc2,0xbf,0xbc,0xba,0xb7,0xb4,0xb1,0xae,0xab,
0xa8,0xa5,0xa2,0x9f,0x9c,0x99,0x96,0x93,0x90,0x8d,0x89,0x86,0x83,0x80,
0x80,0x7c,0x79,0x76,0x72,0x6f,0x6c,0x69,0x66,0x63,0x60,0x5d,0x5a,0x57,0x55,
0x51,0x4e,0x4c,0x48,0x45,0x43,0x40,0x3d,0x3a,0x38,0x35,0x33,0x30,0x2e,0x2b,
0x29,0x27,0x25,0x22,0x20,0x1e,0x1c,0x1a,0x18,0x16,0x15,0x13,0x11,0x10,0x0e,
0x0d,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x02,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02 ,0x02,0x03,0x04,
0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0d,0x0e,0x10,0x11,0x13,0x15,0x16,0x18,
0x1a,0x1c,0x1e,0x20,0x22,0x25,0x27,0x29,0x2b,0x2e,0x30,0x33,0x35,0x38,0x3a,
0x3d,0x40,0x43,0x45,0x48,0x4c,0x4e ,0x51,0x55,0x57,0x5a,0x5d,0x60,0x63,0x66 ,
0x69,0x6c,0x6f,0x72,0x76,0x79,0x7c,0x80 }; 
void DelayMS(uchar ms)
{
	uchar i;
	while(ms--) for(i=0;i<120;i++);
}
 void Delay1(uint y)
 {
 	uint i;
	for(i=y;i>0;i--);
 }
uchar Busy_Check()
{
	uchar LCD_Status;
	RS=0;  //	寄存器选择
	RW=1; //读状态寄存器
	EN=1;  // 开始读
	DelayMS(1);
	LCD_Status=P1;
	EN=0;
	return  LCD_Status;

}
void Write_LCD_Command(uchar cmd)
{
	   while((Busy_Check()&0x80)==0x80);   //忙等待
	   RS=0;  //选择命令寄存器
	   RW=0;  //写
	   EN=0;
	   P1=cmd;EN=1;DelayMS(1);EN=0;
}
void Write_LCD_Data(uchar dat)
{
	 while((Busy_Check()&0x80)==0x80);   //忙等待	
	  RS=1;  RW=0; EN=0; P1=dat;EN=1;DelayMS(1);EN=0;	
}
void Init_LCD()
{		
	Write_LCD_Command(0x38);
	DelayMS(1);
	Write_LCD_Command(0x01); //清屏
	DelayMS(1);
	Write_LCD_Command(0x06); //字符进入模式：屏幕不动，字符后移
	DelayMS(1);
	Write_LCD_Command(0x0C); //显示开、关光标
	DelayMS(1);
}
void Write_freq(uint k)
{
	uchar qian,bai,shi,ge;
	qian=k/1000;
	bai=k/100%10;
	shi=k/10%10;
	ge=k%10;
	Write_LCD_Command(0x86+0x40);
	Write_LCD_Data(0x30+qian);
	Write_LCD_Data(0x30+bai);
	Write_LCD_Data(0x30+shi);
	Write_LCD_Data(0x30+ge);
	Write_LCD_Data(0x48);
	Write_LCD_Data(0x5a);
}
  void Xianshi_f()		   
{
  if(WaveChoice==1)
  {
     freq=(10000000/(50000+2860*ys));
	 Write_freq(freq);
  }
  if(WaveChoice==2)
  {
     freq=(10000000/(50000+2300*ys));
	 Write_freq(freq);
  }
   if(WaveChoice==3)
  {
     freq=(10000000/(14000+2300*ys));
	 Write_freq(freq);
  }
    if(WaveChoice==4)
  {
     freq=(10000000/(15000+2300*ys));
	 Write_freq(freq);
  }
}
void Write_wave(uchar  t )
{
	switch(t)
	{
		case 0:
				 Write_LCD_Command(0x86);
  				 DelayMS(5);
   				 for (i=0;i<sizeof(No)-1;i++)
					{
						Write_LCD_Data(No[i]);
						DelayMS(5);
					}
					break;
		case 1:
				ys=25;
				Write_LCD_Command(0x86);
  				 DelayMS(5);
   				 for (i=0;i<sizeof(Sin)-1;i++)
					{
						Write_LCD_Data(Sin[i]);
						DelayMS(5);
					}
				break;
		case 2:
				ys=30;
				Write_LCD_Command(0x86);
  				 DelayMS(5);
   				 for (i=0;i<sizeof(Squ)-1;i++)
					{
						Write_LCD_Data(Squ[i]);
						DelayMS(5);
					 }
			
				break;
		case 3:
				ys=30;
				Write_LCD_Command(0x86);
  				 DelayMS(5);
   				 for (i=0;i<sizeof(Tri)-1;i++)
					{
						Write_LCD_Data(Tri[i]);
						DelayMS(5);
					}
				break;
		case 4:
				ys=30;
			  	 Write_LCD_Command(0x86);//液晶显示位置
  				 DelayMS(5);
   				 for (i=0;i<sizeof(Saw)-1;i++)
					{
						Write_LCD_Data(Saw[i]);
						DelayMS(5);
					 }
				break;
	}
}
void Out_Wave(uchar i)
{	 uchar j;	
	switch(i)
	{
		case 0:	P0=0x00;break;
		case 1:
				for (j=0;j<255;j++)
				{
					P0=tosin[j];
					Delay1(ys);
				} 
				 break;
		case 2:
						{
					if(a<sqar_num)
						{
							P0=0xff;
							Delay1(ys);
						}
					else  
						{
							P0=0x00;
							Delay1(ys);
						}
						a++;
				}  break;
		case 3:
				{
					if(a<128)
						{
							P0=a;
							Delay1(ys);
						}
					else  
						{
							P0=255-a;
							Delay1(ys);
						}
						a++;
				}   break;
		case 4:
				{
		 			if(a<255)
					{
						P0=a;
						Delay1(ys);
					}
				
				a++;
			if(a==255)
				{
					a=0;
				}   break;
		}
	}
}
void keyscanf()
{	
	if(K2==0)
	{
		DelayMS(5);
		if(K2==0)
		{
			while(!K2);
			ys--;
			if(ys==0)
			ys=20;
		}
	}
	if(K3==0)
	{
		DelayMS(5);
		if(K3==0)
		{
			while(!K3);
			ys++;
			if(ys>22)
			ys=20;
		}
	}
	if(K4==0)
	{
		DelayMS(5);
		if(K4==0)
		{
			while(!K4);
			if(WaveChoice==2)
			sqar_num=sqar_num+2;
			if(sqar_num==238)
				sqar_num=128;
		}
	}
	if(K5==0)
	{
		DelayMS(5);
		if(K5==0)
		{
			while(!K5);
			if(WaveChoice==2)
			sqar_num=sqar_num-2;
			if(sqar_num==18)
				sqar_num=128;			
		}
	}
} 
void main()
{
	Init_LCD();
	IE=0X81;
	IT0=1;
	Write_LCD_Command(0x80);//--显示wave:
   	DelayMS(5);	 	
   for (i=0;i<sizeof(Wave)-1;i++)
		{
			Write_LCD_Data(Wave[i]);
			DelayMS(5);
		}
   	 Write_LCD_Command(0x80+0X40);//----显示freq:
     DelayMS(5);
    for (i=0;i<sizeof(Fre)-1;i++)
		{
			Write_LCD_Data(Fre[i]);
			DelayMS(5);
		}
	   Write_wave(WaveChoice);
	while (1)
	{
			 keyscanf();
		Out_Wave(WaveChoice);
		if(!(K1&K2&K3))
				Xianshi_f();
	}	  
}
void EX_INT0() interrupt 0
{
		WaveChoice++;
		if(WaveChoice==5) WaveChoice=1;
		Write_wave(WaveChoice);
}
