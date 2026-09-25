#include <REGX52.H>
#define	uint	unsigned int
#define	uchar	unsigned char
uchar code table0[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//LED从低位往高位移
uchar code table1[]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};//LED从高位往低位移
uchar code table2[]={0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80,0x00};//LED从1个亮到8个都点亮(从低位往高位)
uchar code table3[]={0x7f,0x3f,0x1f,0x0f,0x07,0x03,0x01,0x00};//LED从1个亮到8个都点亮(从高位往低位)
uchar code table4[]={0x00,0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};//LED从8个全亮到一个都不亮(从低位往高位)
uchar code table5[]={0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff};//LED从8个全亮到一个都不亮(从高位往低位)
uchar code table6[]={0xfe,0xfc,0xfa,0xf6,0xee,0xde,0xbe,0x7e};//LED从低位往高位移(最低位始终为0)
uchar code table7[]={0x7f,0x3f,0x5f,0x6f,0x77,0x7b,0x7d,0x7e};//LED从高位往低位移(最高位始终为0)
uchar i,j;		//定义循环变量
uint tt=70;		//定义时间指数
void delay(uint time)	//延时函数
{
	uint x,y;
	for(x=time;x>0;x--)
	for(y=110;y>0;y--);
}

void disp0()			//状态0 所有LED闪烁3次
{
	for(i=0;i<3;i++)
	{
		P0=0x00; P2=0x00; P3=0x00; P1=0x00;
		delay(300);
		P0=0xff; P2=0xff; P3=0xff; P1=0xff;
		delay(300);
	}
}

void disp1()			//状态1 LED顺时针转一圈
{
	for(i=0;i<8;i++)
	{
		P2=table1[i];
		delay(100);
	}
	P2=0xff;
	for(i=0;i<8;i++)
	{
		P3=table1[i];
		delay(100);
	}
	P3=0xff;
	for(i=0;i<8;i++)
	{
		P1=table1[i];        
		delay(100);
	}
	P1=0xff;
	for(i=0;i<8;i++)
	{
		P0=table0[i];
		delay(100);
	}
	P0=0xff;
}

void disp2()			//状态2 LED逆时针转一圈
{
	for(i=0;i<8;i++)
	{
		P0=table1[i];
		delay(100);
	}
	P0=0xff;
	for(i=0;i<8;i++)
	{
		P1=table0[i];
		delay(100);
	}
	P1=0xff;
	for(i=0;i<8;i++)
	{
		P3=table0[i];
		delay(100);
	}
	P3=0xff;
	for(i=0;i<8;i++)
	{
		P2=table0[i];
		delay(100);
	}
	P2=0xff;
}

void disp3()			//状态3 4个LED同时顺时、逆时针移动1/4圈
{
	for(i=0;i<8;i++)
	{
		P0=table1[i];
		P1=table1[i];
		P2=table1[i];
		P3=table0[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P0=table0[i];
		P1=table0[i];
		P2=table0[i];
		P3=table1[i];
		delay(100);
	}
	P3=0xff; P0=0xff; 
}

void disp4()			//状态4  2个LED同时顺时针移动半圈
{
	for(i=0;i<8;i++)
	{
		P2=table1[i];
		P1=table1[i];
		delay(100);
	}
	P2=0xff; P1=0xff;
	for(i=0;i<8;i++)
	{
		P3=table1[i];
		P0=table0[i];
		delay(100);
	}
	P3=0xff; P0=0xff;
}

void disp5()			//状态5  2个LED同时逆时针移动半圈
{
	for(i=0;i<8;i++)
	{
		P0=table1[i];
		P3=table0[i];
		delay(100);
	}
	P0=0xff; P3=0xff;
	for(i=0;i<8;i++)
	{
		P1=table0[i];
		P2=table0[i];
		delay(100);
	}
	P1=0xff; P2=0xff;
}

void disp6()			//状态6 LED自上而下逐渐点亮(一半点亮一半不亮)
{
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		P2=table3[i];
		delay(100);
	}
	P0=0xff; P2=0xff;
	for(i=0;i<8;i++)
	{
		P1=table2[i];
		P3=table3[i];
		delay(100);
	}
	P1=0xff; P3=0xff;
}

void disp7()			//状态7 LED自下而上逐渐点亮(直到全部点亮)
{
	for(i=0;i<8;i++)
	{
		P1=table3[i];
		P3=table2[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P0=table2[i];
		P2=table2[i];
		delay(100);
	}
}

void disp8()			//状态8 间断8格的4个LED亮并逆时针旋转
{
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			P0=table1[i];
			P2=table0[i];
			P1=table0[i];
			P3=table0[i];
			delay(100);
		}
		P0=0xff; P2=0xff; P1=0xff; P3=0xff;
		for(i=0;i<8;i++)
		{
			P0=table1[i];
			P2=table0[i];
			P1=table0[i];
			P3=table0[i];
			delay(100);
		}
		P0=0xff; P2=0xff; P1=0xff; P3=0xff;
	}
}

void disp9()			//状态9 间断8格的4个LED亮，然后逆时针逐渐点亮(直到全部点亮)
{
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		P2=table2[i];
		P1=table2[i];
		P3=table2[i];
		delay(100);
	}
	delay(500);	
}

void disp10()			//状态10 从LED全部亮到全不亮(间断8格的4个LED开始逆时针熄灭)
{
	for(i=0;i<9;i++)
	{
		P0=table5[i];
		P2=table4[i];
		P1=table4[i];
		P3=table4[i];
		delay(100);
	}
	delay(300);
}

void disp11()			//状态11 间断8格的4个LED亮并顺时针旋转
{
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			P0=table0[i];
			P2=table1[i];
			P1=table1[i];
			P3=table1[i];
			delay(100);
		}
		P0=0xff; P2=0xff; P1=0xff; P3=0xff;
		for(i=0;i<8;i++)
		{
			P0=table0[i];
			P2=table1[i];
			P1=table1[i];
			P3=table1[i];
			delay(100);
		}
		P0=0xff; P2=0xff; P1=0xff; P3=0xff;
	}
}

void disp12()			//间断8格的4个LED亮，然后顺时针逐渐点亮(直到全部点亮)
{
	for(i=0;i<8;i++)
	{
		P0=table2[i];
		P2=table3[i];
		P1=table3[i];
		P3=table3[i];
		delay(100);
	}
	delay(1000);
}

void disp13()			//状态13 从LED全部亮到全不亮(间断8格的4个LED开始顺时针熄灭)
{
	for(i=0;i<9;i++)
	{
		P0=table4[i];
		P2=table5[i];
		P1=table5[i];
		P3=table5[i];
		delay(100);
	}
	delay(300);
}

void disp14()			//状态14 从LED不亮到全亮(从P0.0、P1.0、P2.0、P3.7开始逐步点亮)
{
	for(i=0;i<8;i++)
	{
		P0=table2[i];
		P1=table2[i];
		P2=table2[i];
		P3=table3[i];
		delay(100);
	}
}

void disp15()			//状态15 从LED全亮到全不亮(从P0.7、P1.7、P2.7、P3.0开始逐步熄灭)
{
	for(i=0;i<9;i++)
	{
		P0=table5[i];
		P1=table5[i];
		P2=table5[i];
		P3=table4[i];
		delay(100);
	}
	delay(300);
}

void disp16()			//状态16 LED灯交替闪烁(频率有慢变快)
{
	for(i=0;i<5;i++)
	{
		P0=0xaa; P1=0xaa; P2=0xaa; P3=0xaa;
		delay(100);
		P0=~P0; P1=~P1; P2=~P2; P3=~P3;
		delay(100);
	}
	for(i=0;i<5;i++)
	{
		P0=0xaa; P1=0xaa; P2=0xaa; P3=0xaa;
		delay(200);
		P0=~P0; P1=~P1; P2=~P2; P3=~P3;
		delay(200);
	}
	for(i=0;i<5;i++)
	{
		P0=0xaa; P1=0xaa; P2=0xaa; P3=0xaa;
		delay(300);
		P0=~P0; P1=~P1; P2=~P2; P3=~P3;
		delay(300);
	}
	P0=0xff; P2=0xff; P1=0xff; P3=0xff;
	delay(300);
}

void disp17()			//状态17 LED从P0.7开始逆时针逐步点亮,(循环2次)并且速度会逐步提高
{

	for(i=0;i<8;i++)
	{
		P0=table3[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P1=table2[i];
		delay(100);
	}	
	for(i=0;i<8;i++)
	{
		P3=table2[i];
		delay(100);
	}	
	for(i=0;i<8;i++)
	{
		P2=table2[i];
		delay(100);
	}
	P0=0xff; P1=0xff; P3=0xff; P2=0xff;
/*******************************/
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		delay(50);
	}
	for(i=0;i<8;i++)
	{
		P1=table2[i];
		delay(50);
	}	
	for(i=0;i<8;i++)
	{
		P3=table2[i];
		delay(50);
	}	
	for(i=0;i<8;i++)
	{
		P2=table2[i];
		delay(50);
	}
	P0=0xff; P1=0xff; P3=0xff; P2=0xff;	
}

void disp18()			//状态18 LED从P0.7开始逆时针逐步点亮(每亮8位后就熄灭)
{
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		delay(100);
	}
	P0=0xff;
	for(i=0;i<8;i++)
	{
		P1=table2[i];
		delay(100);
	}	
	P1=0xff;
	for(i=0;i<8;i++)
	{
		P3=table2[i];
		delay(100);
	}
	P3=0xff;	
	for(i=0;i<8;i++)
	{
		P2=table2[i];
		delay(100);
	}
	P2=0xff;	
}

void disp19()			//状态19 LED从P2.7开始顺时针逐步点亮,(循环2次)并且速度会逐步提高
{
	for(i=0;i<8;i++)
	{
		P2=table3[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P3=table3[i];
		delay(100);
	}	
	for(i=0;i<8;i++)
	{
		P1=table3[i];
		delay(100);
	}	
	for(i=0;i<8;i++)
	{
		P0=table2[i];
		delay(100);
	}
	P0=0xff; P1=0xff; P3=0xff; P2=0xff;
/*******************************/
	for(i=0;i<8;i++)
	{
		P2=table3[i];
		delay(50);
	}
	for(i=0;i<8;i++)
	{
		P3=table3[i];
		delay(50);
	}	
	for(i=0;i<8;i++)
	{
		P1=table3[i];
		delay(50);
	}	
	for(i=0;i<8;i++)
	{
		P0=table2[i];
		delay(50);
	}
	P0=0xff; P1=0xff; P3=0xff; P2=0xff;	
}

void disp20()			//状态20 LED从P2.7开始顺时针逐步点亮(每亮8位后就熄灭)
{
	for(i=0;i<8;i++)
	{
		P2=table3[i];
		delay(100);
	}
	P2=0xff;
	for(i=0;i<8;i++)
	{
		P3=table3[i];
		delay(100);
	}	
	P3=0xff;
	for(i=0;i<8;i++)
	{
		P1=table3[i];
		delay(100);
	}
	P1=0xff;	
	for(i=0;i<8;i++)
	{
		P0=table2[i];
		delay(100);
	}
	P0=0xff;		
}

void disp21()			//状态21 2个LED开始上下移动半圈
{
	for(i=0;i<8;i++)
	{
		P0=table1[i];
		P2=table1[i];
		delay(100);
	}
	P0=0xff; P2=0xff;
	for(i=0;i<8;i++)
	{
		P1=table0[i];
		P3=table1[i];
		delay(100);
	}
	
	for(i=0;i<8;i++)
	{
		P1=table1[i];
		P3=table0[i];
		delay(100);
	}
	P1=0xff; P3=0xff;
	for(i=0;i<8;i++)
	{
		P0=table0[i];
		P2=table0[i];
		delay(100);
	}
}

void disp22()			//装态22 2个LED自上而下移动(循环5次,且频率渐快,到最快时持续循环16次,然后循环5次频率再逐渐降低)
{	
	for(j=0;j<5;j++)
	{
		for(i=0;i<8;i++)
		{
			P0=table1[i];
			P2=table1[i];
			delay(tt);
		}
		P0=0xff; P2=0xff;
		for(i=0;i<8;i++)
		{
			P1=table0[i];
			P3=table1[i];
			delay(tt);
		}
		P1=0xff; P3=0xff;
		tt=tt-10;
	}

	for(j=0;j<16;j++)
	{
		for(i=0;i<8;i++)
		{
			P0=table1[i];
			P2=table1[i];
			delay(20);
		}
		P0=0xff; P2=0xff;
		for(i=0;i<8;i++)
		{
			P1=table0[i];
			P3=table1[i];
			delay(20);
		}
		P1=0xff; P3=0xff;
	}
	tt=20;
	for(j=0;j<5;j++)
	{
		for(i=0;i<8;i++)
		{
			P0=table1[i];
			P2=table1[i];
			delay(tt);
		}
		P0=0xff; P2=0xff;
		for(i=0;i<8;i++)
		{
			P1=table0[i];
			P3=table1[i];
			delay(tt);
		}
		P1=0xff; P3=0xff;
		tt=tt+10;
	}
	tt=70;	
		
}

void disp23()			//状态23 LED上下交替闪烁
{

	for(i=0;i<2;i++)
	{
		P0=0x00; P2=0x00;
		delay(1000);
		P0=0xff; P2=0xff;
		P1=0x00; P3=0x00;
		delay(1000);
		P1=0xff; P3=0xff;
	}
}

void disp24()			//状态24 LED左右交替闪烁
{
	P1=0xff; P3=0xff; P0=0xff; P2=0xff;
	for(i=0;i<2;i++)
	{
		P0=0x00; P1=0x00;
		delay(1000);
		P0=0xff; P1=0xff;
		P2=0x00; P3=0x00; 
		delay(1000);
		P2=0xff; P3=0xff;
	}
}

void disp25()			//状态25 LED交替闪烁
{
	for(i=0;i<5;i++)
	{
		P0=0xaa; P1=0xaa; P2=0xaa; P3=0xaa;
		delay(1000);
		P0=~P0; P1=~P1; P2=~P2; P3=~P3;
		delay(1000);		
	}
}

void disp26()			//状态26 2个LED开始上下移动半圈(LED不亮流水)
{
	P0=0x00;P1=0x00;P2=0x00;P3=0x00;
	for(i=0;i<8;i++)
	{
		P0=~table1[i];
		P2=~table1[i];
		delay(200);
	}
	P0=0x00; P2=0x00;
	for(i=0;i<8;i++)
	{
		P1=~table0[i];
		P3=~table1[i];
		delay(200);
	}
	
	for(i=0;i<8;i++)
	{
		P1=~table1[i];
		P3=~table0[i];
		delay(200);
	}
	P1=0x00; P3=0x00;
	for(i=0;i<8;i++)
	{
		P0=~table0[i];
		P2=~table0[i];
		delay(200);
	}
}

void disp27()			//状态27 逐步顺时针点亮P2、P1的LED,熄灭后，再顺时针点亮P0、P3的LED
{
	P0=0xff; P1=0xff; P2=0xff; P3=0xff;
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			P2=table3[i];
			P1=table3[i];
			delay(100);
		}
		P2=0xff; P1=0xff;
		for(i=0;i<8;i++)
		{
			P0=table2[i];
			P3=table3[i];
			delay(100);
		}
		P0=0xff; P3=0xff;
	}
}
			
void disp28()			//状态28 P1、P2和P0、P3交替闪烁
{	
	for(i=0;i<2;i++)
	{
		P1=0x00; P2=0x00; P0=0xff; P3=0xff; 
		delay(1000);
		P1=0xff; P2=0xff; P0=0x00; P3=0x00; 
		delay(1000);
	}
	P1=0xff; P2=0xff; P0=0xff; P3=0xff; 
}

void disp29()			//状态29 逐步顺时针点亮P2、P1的LED,再顺时熄灭后，再顺时针点亮P0、P3的LED,再顺时熄灭
{	

	P0=0xff; P1=0xff; P2=0xff; P3=0xff;
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			P2=table3[i];
			P1=table3[i];
			delay(100);
		}
		for(i=0;i<9;i++)
		{
			P2=table5[i];
			P1=table5[i];
			delay(100);
		}
		for(i=0;i<8;i++)
		{
			P0=table2[i];
			P3=table3[i];
			delay(100);
		}
		for(i=0;i<9;i++)
		{
			P0=table4[i];
			P3=table5[i];
			delay(100);
		}
	}
	delay(200);	
}

void disp30()			//状态30 逐步逆时针点亮P2、P1的LED,熄灭后，再逆时针点亮P0、P3的LED
{
	P0=0xff; P1=0xff; P2=0xff; P3=0xff;
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			P2=table2[i];
			P1=table2[i];
			delay(100);
		}
		P2=0xff; P1=0xff;
		for(i=0;i<8;i++)
		{
			P0=table3[i];
			P3=table2[i];
			delay(100);
		}
		P0=0xff; P3=0xff;
	}
}

void disp31()			//状态31 逐步逆时针点亮P2、P1的LED,再逆时熄灭后，再逆时针点亮P0、P3的LED,再逆时熄灭
{
	P0=0xff; P1=0xff; P2=0xff; P3=0xff;
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			P2=table2[i];
			P1=table2[i];
			delay(100);
		}
		for(i=0;i<9;i++)
		{
			P2=table4[i];
			P1=table4[i];
			delay(100);
		}
		for(i=0;i<8;i++)
		{
			P0=table3[i];
			P3=table2[i];
			delay(100);
		}
		for(i=0;i<9;i++)
		{
			P0=table5[i];
			P3=table4[i];
			delay(100);
		}
	}
	delay(200);	
}

void disp32()			//状态32 LED自上而下逐步点亮，然后从下往上逐步熄灭
{
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		P2=table3[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P1=table2[i];
		P3=table3[i];
		delay(100);
	}
	for(i=0;i<9;i++)
	{
		P1=table5[i];
		P3=table4[i];
		delay(100);
	}
	for(i=0;i<9;i++)
	{
		P0=table4[i];
		P2=table4[i];
		delay(100);
	}
}

void disp33()			//状态33 LED从左往右开始点亮(直到全亮)，然后从左右两边开始熄灭(直到全部熄灭)
{
	for(i=0;i<8;i++)
	{
		P0=table2[i];
		P1=table2[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P2=table3[i];
		P3=table2[i];
		delay(100);
	}
	delay(500);
	for(i=0;i<9;i++)
	{
		P2=table4[i];
		P3=table5[i];
		P1=table4[i];
		P0=table4[i];
		delay(100);
	}
	delay(300);
}

void disp34()			//状态34 LED从左往右开始点亮(直到全亮)，然后从中间往左右边熄灭
{
	for(i=0;i<8;i++)
	{
		P2=table2[i];
		P3=table3[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		P1=table3[i];
		delay(100);
	}
	delay(500);
	for(i=0;i<9;i++)
	{
		P2=table5[i];
		P3=table4[i];
		P1=table5[i];
		P0=table5[i];
		delay(100);
	}
	delay(300);	
}

void disp35()			//状态35  从P2.7、P1.7开始同时顺时针旋转，直到LED全亮
{
	for(i=0;i<8;i++)
	{
		P2=table3[i];
		P1=table3[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P3=table3[i];
		P0=table2[i];
		delay(100);
	}
}

void disp36()			//状态36  从P0.7、P3.1开始同时逆时针旋转，直到LED全亮
{
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		P3=table2[i];
		delay(100);
	}
	for(i=0;i<8;i++)
	{
		P1=table2[i];
		P2=table2[i];
		delay(100);
	}
}

void disp37()			//状态37 2个LED自上而下移动，并点亮最下2个LED，然后2个LED再自下而上回去，同时4个LED（上下各2个LED）再来回移动一次
{
	for(i=0;i<8;i++)
	{
		P0=table1[i];
		P2=table1[i];
		delay(100);
	}
	P0=0xff; P2=0xff;
	for(i=0;i<8;i++)
	{
		P1=table0[i];
		P3=table1[i];
		delay(100);
	}
	
	for(i=0;i<8;i++)
	{
		P1=table7[i];
		P3=table6[i];
		delay(100);
	}
	P1=0x7f; P3=0xfe;
	for(i=0;i<8;i++)
	{
		P0=table0[i];
		P2=table0[i];
		delay(100);
	}
	
	for(i=0;i<8;i++)
	{
		P0=table1[i];
		P2=table1[i];
		P1=table1[i];
		P3=table0[i];
		delay(200);
	}
	
	for(i=0;i<8;i++)
	{
		P0=table0[i];
		P1=table0[i];
		P2=table0[i];
		P3=table1[i];
		delay(200);
	}	
	P0=0xff;P1=0xff; P2=0xff;P3=0xff;
}

void disp38()			//状态38 逐步顺时针点亮P2、P1的LED,熄灭后，再顺时针点亮P0、P3的LED
{
	P0=0xff; P1=0xff; P2=0xff; P3=0xff;
	for(j=0;j<2;j++)
	{
		for(i=0;i<8;i++)
		{
			P2=table3[i];
			P1=table3[i];
			delay(100);
		}
		P2=0xff; P1=0xff;
		for(i=0;i<8;i++)
		{
			P0=table2[i];
			P3=table3[i];
			delay(100);
		}
		P0=0xff; P3=0xff;
	}	
}

void disp39()			//状态39 P1、P3LED(下半部分)一直亮,上半部分LED向外扩张亮,然后闪烁3次
{
	P1=0x00; P3=0x00;
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		P2=table3[i];
		delay(150);
	}
	for(i=0;i<8;i++)
	{
		P0=table3[i];
		P2=table3[i];
		delay(300);
	}
	for(i=0;i<4;i++)
	{
		P0=~P0; P2=~P2;
		delay(300);
	}
	delay(1000);
}

void disp40()			//状态40 LED从P3.0、P0.7开始逆时针同时熄灭，直到LED全部熄灭
{
	for(i=0;i<9;i++)
	{
		P3=table4[i];
		P0=table5[i];
		delay(100);
	}
	for(i=1;i<9;i++)
	{
		P2=table4[i];
		P1=table4[i];
		delay(100);
	}
	delay(300);	
}

void disp41()			//状态41 LED从P2.7、P1.7开始顺时针同时熄灭，直到全部熄灭
{
	for(i=0;i<9;i++)
	{
		P2=table5[i];
		P1=table5[i];
		delay(100);
	}
	for(i=1;i<9;i++)
	{
		P0=table4[i];
		P3=table5[i];
		delay(100);
	}
	delay(300);		
}

void main()
{	
	while(1)
	{		
		disp22();		//装态22 2个LED自上而下移动(循环5次,且频率渐快,到最快时持续循环16次,然后循环5次频率再逐渐降低)
		disp0();		//状态0 所有LED闪烁3次
		disp1();		//状态1 LED顺时针转一圈
		disp2();		//状态2 LED逆时针转一圈
		disp3();		//状态3 4个LED同时顺时、逆时针移动1/4圈
		disp4();		//状态4  2个LED同时顺时针移动半圈
		disp5();		//状态5  2个LED同时逆时针移动半圈
		disp6();		//状态6 LED自上而下逐渐点亮(一半点亮一半不亮)
		disp7();		//状态7 LED自下而上逐渐点亮(直到全部点亮)
		disp0();		//状态0 所有LED闪烁3次
		disp8();		//状态8 间断8格的4个LED亮并逆时针旋转
		disp9();		//状态9 间断8格的4个LED亮，然后逆时针逐渐点亮(直到全部点亮)
		disp10();		//状态10 从LED全部亮到全不亮(间断8格的4个LED开始逆时针熄灭)
		disp11();		//状态11 间断8格的4个LED亮并顺时针旋转
		disp12();		//间断8格的4个LED亮，然后顺时针逐渐点亮(直到全部点亮)
		disp13();		//状态13 从LED全部亮到全不亮(间断8格的4个LED开始顺时针熄灭)
		disp14();		//状态14 从LED不亮到全亮(从P0.0、P1.0、P2.0、P3.7开始逐布点亮)   
		disp15();		//状态15 从LED全亮到全不亮(从P0.7、P1.7、P2.7、P3.0开始逐步熄灭)
		disp16();		//状态16 LED灯交替闪烁(频率有慢变快)
		disp17();		//状态17 LED从P0.7开始逆时针逐步点亮,并且速度会逐步提高
		disp18();		//状态18 LED从P0.7开始逆时针逐步点亮(每亮8位后就熄灭)
		disp19();		//状态19 LED从P2.7开始顺时针逐步点亮,(循环2次)并且速度会逐步提高
		disp20();		//状态20 LED从P2.7开始顺时针逐步点亮(每亮8位后就熄灭)
		disp21();		//状态21 2个LED开始上下移动半圈
		disp22();		//装态22 2个LED自上而下移动(循环5次,且频率渐快,到最快时持续循环16次,然后循环5次频率再逐渐降低
		disp23();		//状态23 LED上下交替闪烁
		disp39();		//状态39 P1、P3LED(下半部分)一直亮,上半部分LED向外扩张亮,然后闪烁3次
		disp24();		//状态24 LED左右交替闪烁
		disp25();		//状态25 LED交替闪烁  
		disp8();		//状态8 间断8格的4个LED亮并逆时针旋转
		disp9();		//状态9 间断8格的4个LED亮，然后逆时针逐渐点亮(直到全部点亮)
		disp10();		//状态10 从LED全部亮到全不亮(间断8格的4个LED开始逆时针熄灭)
		disp26();		//状态26 2个LED开始上下移动半圈(LED不亮流水)
		disp27();		//状态27 逐步顺时针点亮P2、P1的LED,熄灭后，再顺时针点亮P0、P3的LED
		disp28();		//状态28 P1、P2和P0、P3交替闪烁
		disp38();		//状态38 逐步顺时针点亮P2、P1的LED,熄灭后，再顺时针点亮P0、P3的LED
		disp29();		//状态29 逐步顺时针点亮P2、P1的LED,再顺时熄灭后，再顺时针点亮P0、P3的LED,再顺时熄灭
		disp30();		//状态30 逐步逆时针点亮P2、P1的LED,熄灭后，再逆时针点亮P0、P3的LED
		disp31();		//状态31 逐步逆时针点亮P2、P1的LED,再逆时熄灭后，再逆时针点亮P0、P3的LED,再逆时熄灭
		disp32();		//状态32 LED自上而下逐步点亮，然后从下往上逐步熄灭
		disp33();		//状态33 LED从左往右开始点亮(直到全亮)，然后从左右两边开始熄灭(直到全部熄灭)
		disp37();		//状态37 2个LED自上而下移动，并点亮最下2个LED，然后2个LED再自下而上回去，同时4个LED（上下各2个LED）再来回移动一次
		disp36();		//状态36  从P0.7、P3.0开始同时逆时针旋转，直到LED全亮
		disp40();		//状态40 LED从P3.0、P0.7开始逆时针同时熄灭，直到LED全部熄灭
		disp35();		//状态35  从P2.7、P1.7开始同时顺时针旋转，直到LED全亮
		disp41();		//状态41 LED从P2.7、P1.7开始顺时针同时熄灭，直到全部熄灭
		disp34();		//状态34 LED从左往右开始点亮(直到全亮)，然后从中间往左右边熄灭
		disp28();		//状态28 P1、P2和P0、P3交替闪烁
	}
}
