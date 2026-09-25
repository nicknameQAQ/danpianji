#include <stdio.h>
#include <intrins.h>
#include "Table.h"
#include "LCD12864.h"
#include "STC12C5A60S2.h"

#define DATA P0

sbit RS=P2^0;		
sbit RW=P2^1;		
sbit EN=P2^2;		
sbit CS1=P2^3;		
sbit CS2=P2^4;		
sbit RST=P2^5;		

extern unsigned char timpd;		
//示波器 时间/每格 1-5ms 2-2ms 3-1ms 4-500us 5-200us 6-100us 7-50us 8-20us 9-10us 10-5us 11-2us 12-1us 13-500ns
extern unsigned char volpd;				//示波器 电压/每格 1--1V/d 2--0.2V/d
extern unsigned long freqin;				//示波器 信号频率,单位Hz
extern unsigned char idata wave[];	    //一屏波形
extern unsigned char clrgrid;			//网格频闪

//=============整型x的n次方==============//
unsigned long power(unsigned char x,unsigned char n)
{
	unsigned char i;
	unsigned long l=1;
	if(n==0)
	{
		return 1;
	}
	else
	{
		for(i=0;i<n;i++)
		{
			l = l*x;
		}
		return l;
	}
}
void delay100ns(unsigned char ns)
{
	for(;ns>0;ns--);
}
//*********************************************//
void Read_busy(void)
{
	DATA=0x80;
	RS=0;				//读状态
	RW=1;
	EN=1;
	delay100ns(80);
	while(DATA&0x8);
	EN=0;
	delay100ns(8);
}
/*========write one byte command==============*/
void write_command(unsigned char cmd)
{ 
	Read_busy();
	RS=0;				//选择命令
	RW=0;				//读操作
	DATA=cmd;
	EN=1;				//EN由1----0锁存有效数据
	delay100ns(8);
	EN=0;
	delay100ns(8);
}
/*========write one byte data==============*/
void write_data(unsigned char dat)
{ 
	Read_busy();
	RS=1;	
	RW=0;
	DATA=dat;
	EN=1;				//EN由1----0锁存有效数据
	delay100ns(8);
	EN=0;
	delay100ns(8);
}
/*============set column==================*/
//列设置包括了片选,先设置列，再设置页
//否则列和页可能设置在两块芯片上，显示错误
void set_column(unsigned char column)
{ 
	if(column<64)
	{
		CS1=1;CS2=0;
		write_command(0x40|column);
	}
	else if(column<128)
	{
		CS1=0;CS2=1;
		write_command(0x40|(column-64));
	}
}
/*===============set page================*/
void set_page(unsigned char page)
{ 
	write_command(0xb8|page);
}
/*==========lcd12864初始化==============*/
void init_ks0108(void)
{
	write_command(0x3f);				//0x3f开显示，0x3e关显示
	write_command(0xc0);				//设置起始行为0
	disp_clear();						//清屏
}
/*=================清屏==================*/
void disp_clear(void)
{
	unsigned char i,j,column,page; 
	CS1=1;CS2=1; 
	for(j=0;j<8;j++) 
	{ 
		page=0xb8+j; 
		write_command(page);
		column=0x40;
		write_command(column);
		for(i=0;i<64;i++)
		{
			write_data(0x00);
		}
	}
}
/*========write a char 5*7==============*/
//直接写字符，例如'A','1','/'等
void write_char8(unsigned char page,unsigned char column,unsigned char chr)
{
	unsigned char i;
	for(i=0;i<5;i++)
	{
		set_column(column+i);					//必须先设置列，否则列页有可能设置在不同芯片上
		set_page(page);							//page
		//write_data(nAscii[(chr-0x20)*5+i]);	//5*8字模
		write_data(Fonts[(chr-0x20)*5+i]<<1);	//5*7字模
	}
}
/*========write strings 5*7==============*/
//直接在指定位置写字符串，如"Good Luck!"
void write_string8(unsigned char page,unsigned char column,unsigned char *chr)
{
	unsigned char i,col;
	col=column;
	for(i=0;(i<((127-col)/5))&&(*(chr+i)!='\0');i++)
	{
		write_char8(page,column,*(chr+i));
		column = column+5;
	}
}
/*===============显示示波器网格===============*/
void disp_grid(unsigned char gridmode)
{
	unsigned char i,j,page=0; 
	unsigned int dz=0;
	for(j=0;j<7;j++) 				//*write 7 page
	{ 
		for(i=0;i<127;i++)	
		{
			if(i==0)
			{
				set_column(0);		//先设置列
				set_page(page+j);	//page
			}
			else if(i==64)			//切换芯片，页、列重新设置
			{
				set_column(64);		//先设置列
				set_page(page+j);	//page
			}
			dz++;
			if(gridmode==0){write_data(grid0[dz-1]);}
			if(gridmode==1){write_data(grid1[dz-1]);}
			if(gridmode==2){write_data(grid2[dz-1]);}
			//if(gridmode==3){write_data(grid3[dz-1]);}
			//if(gridmode==4){write_data(grid4[dz-1]);}
			//if(gridmode==5){write_data(grid5[dz-1]);}
			//if(gridmode==6){write_data(grid6[dz-1]);}

		}
	}
}
/*==============显示示波器参数==============*/
void disp_osc_settings(void)
{
	unsigned char i,j,k;
	unsigned long freq0;
	j=0;
	k=0;
	freq0=freqin;
	for(i=0;i<7;i++)
	{
		if((i==1)||(i==4)){j++;j++;}
		if((k==0)&&((freq0/power(10,6-i))==0))
		{
			if(i==6){write_char8(7,j,'0');}	//全0最后一位显示0
			else{write_char8(7,j,' ');}		//开头0不显示
		}
		else
		{
			write_char8(7,j,(unsigned char)(freq0/power(10,6-i))+0x30);
			k=1;
		}
		freq0 = freq0%power(10,6-i);
		j+=5;
	}
	write_char8(7,40,'H');write_char8(7,45,'z');
	switch(timpd)		
	{
		case  3:{write_string8(7,56," 10ms/d");break;}
		case  4:{write_string8(7,56,"  5ms/d");break;}
		case  5:{write_string8(7,56,"  2ms/d");break;}
		case  6:{write_string8(7,56,"  1ms/d");break;}
		case  7:{write_string8(7,56,"500us/d");break;}
		case  8:{write_string8(7,56,"200us/d");break;}
		case  9:{write_string8(7,56,"100us/d");break;}
		case  10:{write_string8(7,56," 50us/d");break;}
		case  11:{write_string8(7,56," 20us/d");break;}
		case  12:{write_string8(7,56," 10us/d");break;}
		case 13:{write_string8(7,56,"  5us/d");break;}
		case 14:{write_string8(7,56,"  2us/d");break;}
		case 15:{write_string8(7,56,"  1us/d");break;}
		case 16:{write_string8(7,56,"500ns/d");break;}
	}
	switch(volpd)
	{
		case  1:{write_string8(7,97,"  1V/d");break;}
		case  2:{write_string8(7,97,"0.5V/d");break;}
		case  3:{write_string8(7,97,"0.2V/d");break;}
	}
}
/*==============显示一屏幕波形==============*/
void dispwave(unsigned char gridmode)
{
	unsigned char i;
	disp_osc_settings();
	disp_grid(gridmode);
	for(i=0;i<127;i++)
	{
		if(wave[i]>47)
		{
			if(i<64)
			{
				set_column(i);					//列
				set_page(0);					//page
			}
			else 								//切换芯片
			{
				set_column(i);					//列
				set_page(0);					//page
			}
			if(gridmode==0){write_data((0x80>>(wave[i]-48))|grid0[i]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-48))|grid1[i]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-48))|grid2[i]);}
/*			if(gridmode==3){write_data((0x80>>(wave[i]-48))|grid3[i]);}
			if(gridmode==4){write_data((0x80>>(wave[i]-48))|grid4[i]);}
			if(gridmode==5){write_data((0x80>>(wave[i]-48))|grid5[i]);}
			if(gridmode==6){write_data((0x80>>(wave[i]-48))|grid6[i]);}
*/		}
		else if(wave[i]>39)
		{
			if(i<64)
			{
				set_column(i);					//列
				set_page(1);					//page
			}
			else 								//切换芯片
			{
				set_column(i);					//列
				set_page(1);					//page
			}
			if(gridmode==0){write_data((0x80>>(wave[i]-40))|grid0[i+127]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-40))|grid1[i+127]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-40))|grid2[i+127]);}
/*			if(gridmode==3){write_data((0x80>>(wave[i]-40))|grid3[i+127]);}
			if(gridmode==4){write_data((0x80>>(wave[i]-40))|grid4[i+127]);}
			if(gridmode==5){write_data((0x80>>(wave[i]-40))|grid5[i+127]);}
			if(gridmode==6){write_data((0x80>>(wave[i]-40))|grid6[i+127]);}
*/		}
		else if(wave[i]>31)
		{
			if(i<64)
			{
				set_column(i);					//列
				set_page(2);					//page
			}
			else 								//切换芯片
			{
				set_column(i);					//列
				set_page(2);					//page
			}
			if(gridmode==0){write_data((0x80>>(wave[i]-32))|grid0[i+127*2]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-32))|grid1[i+127*2]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-32))|grid2[i+127*2]);}
/*			if(gridmode==3){write_data((0x80>>(wave[i]-32))|grid3[i+127*2]);}
			if(gridmode==4){write_data((0x80>>(wave[i]-32))|grid4[i+127*2]);}
			if(gridmode==5){write_data((0x80>>(wave[i]-32))|grid5[i+127*2]);}
			if(gridmode==6){write_data((0x80>>(wave[i]-32))|grid6[i+127*2]);}
*/		}
		else if(wave[i]>23)
		{
			if(i<64)
			{
				set_column(i);					//列
				set_page(3);					//page
			}
			else 								//切换芯片
			{
				set_column(i);					//列
				set_page(3);					//page
			}
			if(gridmode==0){write_data((0x80>>(wave[i]-24))|grid0[i+127*3]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-24))|grid1[i+127*3]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-24))|grid2[i+127*3]);}
/*			if(gridmode==3){write_data((0x80>>(wave[i]-24))|grid3[i+127*3]);}
			if(gridmode==4){write_data((0x80>>(wave[i]-24))|grid4[i+127*3]);}
			if(gridmode==5){write_data((0x80>>(wave[i]-24))|grid5[i+127*3]);}
			if(gridmode==6){write_data((0x80>>(wave[i]-24))|grid6[i+127*3]);}
*/		}
		else if(wave[i]>15)
		{
			if(i<64)
			{
				set_column(i);					//列
				set_page(4);					//page
			}
			else 								//切换芯片
			{
				set_column(i);					//列
				set_page(4);					//page
			}
			if(gridmode==0){write_data((0x80>>(wave[i]-16))|grid0[i+127*4]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-16))|grid1[i+127*4]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-16))|grid2[i+127*4]);}
/*			if(gridmode==3){write_data((0x80>>(wave[i]-16))|grid3[i+127*4]);}
			if(gridmode==4){write_data((0x80>>(wave[i]-16))|grid4[i+127*4]);}
			if(gridmode==5){write_data((0x80>>(wave[i]-16))|grid5[i+127*4]);}
			if(gridmode==6){write_data((0x80>>(wave[i]-16))|grid6[i+127*4]);}
*/		}
		else if(wave[i]>7)
		{
			if(i<64)
			{
				set_column(i);					//列
				set_page(5);					//page
			}
			else 								//切换芯片
			{
				set_column(i);					//列
				set_page(5);					//page
			}
			if(gridmode==0){write_data((0x80>>(wave[i]-8))|grid0[i+127*5]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-8))|grid1[i+127*5]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-8))|grid2[i+127*5]);}
/*			if(gridmode==3){write_data((0x80>>(wave[i]-8))|grid3[i+127*5]);}
			if(gridmode==4){write_data((0x80>>(wave[i]-8))|grid4[i+127*5]);}
			if(gridmode==5){write_data((0x80>>(wave[i]-8))|grid5[i+127*5]);}
			if(gridmode==6){write_data((0x80>>(wave[i]-8))|grid6[i+127*5]);}
*/		}
		else if(wave[i]>=0)
		{
			if(i<64)
			{
				set_column(i);					//列
				set_page(6);					//page
			}
			else 								//切换芯片
			{
				set_column(i);					//列
				set_page(6);					//page
			}
			if(gridmode==0){write_data((0x80>>(wave[i]))|grid0[i+127*6]);}
			if(gridmode==1){write_data((0x80>>(wave[i]))|grid1[i+127*6]);}
			if(gridmode==2){write_data((0x80>>(wave[i]))|grid2[i+127*6]);}
/*			if(gridmode==3){write_data((0x80>>(wave[i]))|grid3[i+127*6]);}
			if(gridmode==4){write_data((0x80>>(wave[i]))|grid4[i+127*6]);}
			if(gridmode==5){write_data((0x80>>(wave[i]))|grid5[i+127*6]);}
			if(gridmode==6){write_data((0x80>>(wave[i]))|grid6[i+127*6]);}
*/		}
	}

}
//------------------------------------------end----------------------------------------------//

//////////////////////////////////////////////////////////////////////////////
	  /*write_command(0xb0);		//page
		write_command(0x10);		//clon H
		write_command(0x04);        //clon L
		for(j=0;j<8;j++)
		{
			write_command(0xb0);		        //page
			write_command(0x10+((0x04+16*j)>>4));		//clon H
			write_command((0x04+(16*j))&0x0f);    //clon L
			for(i=0;i<16;i++)
			{
			    write_data(test[i+32*j]);
			}
			write_command(0xb1);		        //page
			write_command(0x10+((0x04+16*j)>>4));		//clon H
			write_command((0x04+(16*j))&0x0f); 
			for(i=0;i<16;i++)
	        {
				write_data(test[i+32*j+16]);
			}
		}*/
/////////////////////////////////////////////////////////////////////////////
