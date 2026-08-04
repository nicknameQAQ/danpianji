#include"IR.h"
#include"data.h"
#include "judge.h"
#include "delay.h"
#include "beep.h"

u8 IrValue[6];
u8 Times;



void IrInit()
{
	IT0=1;//下降沿触发
	EX0=1;//打开中断0允许
	EA=1;	//打开总中断

	IRIN=1;//初始化端口
}

void ReadIr() interrupt 0
{
	u8 j,k;
	u16 err;
	Times=0;					 
	delay(700);	//7ms
	if(IRIN==0)		//确认是否真的接收到正确的信号
	{	 
		
		err=1000;				//1000*10us=10ms,超过说明接收到错误的信号
	
		while((IRIN==0)&&(err>0))	//等待前面9ms的低电平过去  		
		{			
			delay(1);
			err--;
		} 
		if(IRIN==1)			//如果正确等到9ms低电平
		{
			err=500;
			while((IRIN==1)&&(err>0))		 //等待4.5ms的起始高电平过去
			{
				delay(1);
				err--;
			}
			for(k=0;k<4;k++)		//共有4组数据
			{				
				for(j=0;j<8;j++)	//接收一组数据
				{

					err=60;		
					while((IRIN==0)&&(err>0))//等待信号前面的560us低电平过去
					{
						delay(1);
						err--;
					}
					err=500;
					while((IRIN==1)&&(err>0))	 //计算高电平的时间长度。
					{
						delay(10);	 //0.1ms
						Times++;
						err--;
						if(Times>30)
						{
							return;
						}
					}
					IrValue[k]>>=1;	 //k表示第几组数据
					if(Times>=8)			//如果高电平出现大于565us，那么是1
					{
						IrValue[k]|=0x80;
					}
					Times=0;		//用完时间要重新赋值							
				}
			}
		}
		if(IrValue[2]!=~IrValue[3])
		{
			return;
		}
	}			
}

void infrared()				//红外遥控
{
	 if(IrValue[2]==69){  year++; p=p+number;	IrValue[2]=0; }
	 if(IrValue[2]==70){  year--; p=p-number;	IrValue[2]=0;}
 	 if(IrValue[2]==71){	month++;	p=p+maxday;	IrValue[2]=0;}
	 if(IrValue[2]==68){	month--;	p=p-maxday;	IrValue[2]=0;}
	 if(IrValue[2]==64){	day++;		p++;	IrValue[2]=0;}
 	 if(IrValue[2]==67){	day--;		p--;	IrValue[2]=0;}
	 if(IrValue[2]==7){		h++;	IrValue[2]=0;}
	 if(IrValue[2]==21){	h--;	IrValue[2]=0;}
 	 if(IrValue[2]==9){		min++;	IrValue[2]=0;}
	 if(IrValue[2]==22){	min--;	IrValue[2]=0;}
	 if(IrValue[2]==25){	h1++;	IrValue[2]=0;}
 	 if(IrValue[2]==13){	h1--;	IrValue[2]=0;}
	 if(IrValue[2]==12){	min1++;	IrValue[2]=0;}
	 if(IrValue[2]==24){	min--;	IrValue[2]=0;}
 	 if(IrValue[2]==94){	h2++;	IrValue[2]=0;}
	 if(IrValue[2]==8) {	h2--;	IrValue[2]=0;}
	 if(IrValue[2]==28){	min2++;	IrValue[2]=0;}
 	 if(IrValue[2]==90){	min2--;	IrValue[2]=0;}
	 if(IrValue[2]==66){	y++;	IrValue[2]=0;}
	 if(IrValue[2]==82){	y--;	IrValue[2]=0;}
 	 if(IrValue[2]==74){	p++;	IrValue[2]=0;}
}