#include <stdio.h>
#include <intrins.h>
#include "Table.h"
#include "LCD12864.h"
#include "STC12C5A60S2.h"

#define SET 1;
#define RESET 0;

sbit SDA=P0^2;		//SI
sbit SCK=P0^0;		//SCL
sbit DI=P0^3;		//AO
sbit CS=P0^1;		//CS

extern unsigned char timpd;		
extern unsigned char hold;				//hold键标志位
extern unsigned char volpd;				//示波器 电压/每格 1--1V/d 2--0.2V/d
extern unsigned long freqin;			//示波器 信号频率,单位Hz
extern unsigned char idata wave[];	    //一屏波形
//extern unsigned char clrgrid;			//网格频闪
extern unsigned char shap;				    	//波形失真修正，上下轮流显示

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
/*========write one byte command==============*/
void write_command(unsigned char cmd)
{ 
	unsigned char i;
	DI=RESET;
	SCK=RESET;
	CS=RESET;
	for(i=0;i<8;i++)
	{
		SCK=RESET;
		//_nop_();
		SDA=cmd&0x80;
		cmd=cmd<<1;
		//_nop_();
		SCK=SET;
		//_nop_();
	}
	CS=SET;
}
/*========write one byte data==============*/
void write_data(unsigned char dat)
{ 
	unsigned char i;
	DI=SET;
	SCK=RESET;
	CS=RESET;
	for(i=0;i<8;i++)
	{
		SCK=RESET;
		//_nop_();
		SDA=dat&0x80;
		dat=dat<<1;
		//_nop_();
		SCK=SET;
		//_nop_();
	}
	CS=SET;
}
/*==========lcd12864初始化==============*/
void init_st7565(void)
{
	write_command(0xaf);				//0xaf开显示，0xae关显示
	write_command(0x2f);				//模块内部电源，必须0x2f	
	write_command(0x24);				//0x20--0x27粗调对比度
	write_command(0x81);				//与下边指令配合细调对比度			
	write_command(0x29);				//0x01--0x3f设定对比度
	write_command(0xa2);				//0xa2 固定1/9偏压，（0xa3 1/7 偏压）
	write_command(0xc8);				//0xc8上下正常显示，0xc0上下反向显示（纵移1行）
	write_command(0xa1);				//0xa0左右正常显示，0xa1左右反向显示（平移4列）
	write_command(0xa4);				//0xa4正常显示，0xa5全屏显示(+0xae进入低功耗)
	write_command(0xa6);				//0xa6字体正常显示，0xa7字体反白显示
	write_command(0xac);				//关闭闪烁（模块本身无此功能，关闭省电）
	write_command(0x40);				//0x40--0x7F设置起始行
	write_command(0xb0);        		//0xb0--0xb8设置页地址
	write_command(0x10);				//设定起始列地址高位
	write_command(0x00);        		//设定起始列地址低位
	disp_clear();						//清屏
}
/*=================清屏==================*/
void disp_clear(void)
{
	unsigned char i,j,column,page; 
	for(j=0x00;j<0x08;j++) 
	{ 
		page=j+0xb0; 
		write_command(page);
		column=0x01; 
		write_command(column);
		column=0x10; 
		write_command(column);
		for(i=0;i<134;i++)
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
	write_command(0xb0+page);					//page
	write_command(0x10+((0x04+column)>>4));		//column H
	write_command((0x04+column)&0x0f);    		//column L
	for(i=0;i<5;i++)
	{
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
	unsigned char i,j,page; 
	unsigned int dz=0;
	for(j=0;j<7;j++) 				//*write 7 page
	{ 
		page=j+0xb0; 					//*set page address
		write_command(page);              
		write_command(0x10);			//*set column higher address
		write_command(0x04);       		//*set column lower address
		for(i=0;i<127;i++)	
		{
			dz++;
			if(gridmode==0){write_data(grid0[dz-1]);}
			if(gridmode==1){write_data(grid1[dz-1]);}
			if(gridmode==2){write_data(grid2[dz-1]);}
			if(gridmode==3){write_data(grid3[dz-1]);}
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
	if(hold==0)
	{	
		write_string8(7,0,"          ");		//清除原来Hold Mode残存
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
	}
	if(hold==1)
	{
		write_string8(7,2,"Hold Mode ");
	}
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
		default: break;
	}
	switch(volpd)
	{
		case  1:{write_string8(7,97,"  1V/d");break;}
		case  2:{write_string8(7,97,"0.5V/d");break;}
		case  3:{write_string8(7,97,"0.2V/d");break;}
		case  4:{write_string8(7,97,"0.1V/d");break;}
		default: break;
	}
}
/*==============显示一屏幕波形(一列一点)==============*/
void dispwave(unsigned char gridmode)
{
	unsigned char i;
	disp_osc_settings();
	disp_grid(gridmode);
	for(i=0;i<127;i++)
	{
		if(wave[i]>47)
		{
			write_command(0xb0);              
			write_command(0x10+((i+0x04)>>4));	//*set column higher address
			write_command((0x04+i)&0x0f);       //*set column lower address
			if(gridmode==0){write_data((0x80>>(wave[i]-48))|grid0[i]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-48))|grid1[i]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-48))|grid2[i]);}
			if(gridmode==3){write_data((0x80>>(wave[i]-48))|grid3[i]);}
		}
		else if(wave[i]>39)
		{
			write_command(0xb1);              
			write_command(0x10+((i+0x04)>>4));	//*set column higher address
			write_command((0x04+i)&0x0f);       //*set column lower address
			if(gridmode==0){write_data((0x80>>(wave[i]-40))|grid0[i+127]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-40))|grid1[i+127]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-40))|grid2[i+127]);}
			if(gridmode==3){write_data((0x80>>(wave[i]-40))|grid3[i+127]);}
		}
		else if(wave[i]>31)
		{
			write_command(0xb2);              
			write_command(0x10+((i+0x04)>>4));	//*set column higher address
			write_command((0x04+i)&0x0f);       //*set column lower address
			if(gridmode==0){write_data((0x80>>(wave[i]-32))|grid0[i+127*2]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-32))|grid1[i+127*2]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-32))|grid2[i+127*2]);}
			if(gridmode==3){write_data((0x80>>(wave[i]-32))|grid3[i+127*2]);}
		}
		else if(wave[i]>23)
		{
			write_command(0xb3);              
			write_command(0x10+((i+0x04)>>4));	//*set column higher address
			write_command((0x04+i)&0x0f);       //*set column lower address
			if(gridmode==0){write_data((0x80>>(wave[i]-24))|grid0[i+127*3]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-24))|grid1[i+127*3]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-24))|grid2[i+127*3]);}
			if(gridmode==3){write_data((0x80>>(wave[i]-24))|grid3[i+127*3]);}
		}
		else if(wave[i]>15)
		{
			write_command(0xb4);              
			write_command(0x10+((i+0x04)>>4));	//*set column higher address
			write_command((0x04+i)&0x0f);       //*set column lower address
			if(gridmode==0){write_data((0x80>>(wave[i]-16))|grid0[i+127*4]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-16))|grid1[i+127*4]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-16))|grid2[i+127*4]);}
			if(gridmode==3){write_data((0x80>>(wave[i]-16))|grid3[i+127*4]);}
		}
		else if(wave[i]>7)
		{
			write_command(0xb5);              
			write_command(0x10+((i+0x04)>>4));	//*set column higher address
			write_command((0x04+i)&0x0f);       //*set column lower address
			if(gridmode==0){write_data((0x80>>(wave[i]-8))|grid0[i+127*5]);}
			if(gridmode==1){write_data((0x80>>(wave[i]-8))|grid1[i+127*5]);}
			if(gridmode==2){write_data((0x80>>(wave[i]-8))|grid2[i+127*5]);}
			if(gridmode==3){write_data((0x80>>(wave[i]-8))|grid3[i+127*5]);}
		}
		else if(wave[i]>=0)
		{
			write_command(0xb6);              
			write_command(0x10+((i+0x04)>>4));	//*set column higher address
			write_command((0x04+i)&0x0f);       //*set column lower address
			if(gridmode==0){write_data((0x80>>(wave[i]))|grid0[i+127*6]);}
			if(gridmode==1){write_data((0x80>>(wave[i]))|grid1[i+127*6]);}
			if(gridmode==2){write_data((0x80>>(wave[i]))|grid2[i+127*6]);}
			if(gridmode==3){write_data((0x80>>(wave[i]))|grid3[i+127*6]);}
		}
	}
}
/*==============显示一屏幕波形（列空白补齐）==============*/
void dispwave2(unsigned char gridmode)
{
	unsigned char i,j;
	unsigned char min,mid,max;	
	unsigned char srt,lgth,end;		//起始点，长度，终点（规定屏幕下为起点，上为终点）
	unsigned char pnts[7];			//一列的点组，pnts[0]第6页，pnts[6]第0页
	disp_osc_settings();
	//disp_grid(gridmode);			//不需要，画点时顺带画了网格
	for(i=0;i<127;i++)
	{
		if((i==0)||(i==126))
		{
			if(wave[i]>47)
			{
				write_command(0xb0);              
				write_command(0x10+((i+0x04)>>4));	//*set column higher address
				write_command((0x04+i)&0x0f);       //*set column lower address
				if(gridmode==0){write_data((0x80>>(wave[i]-48))|grid0[i]);}
				if(gridmode==1){write_data((0x80>>(wave[i]-48))|grid1[i]);}
				if(gridmode==2){write_data((0x80>>(wave[i]-48))|grid2[i]);}
				if(gridmode==3){write_data((0x80>>(wave[i]-48))|grid3[i]);}
			}
			else if(wave[i]>39)
			{
				write_command(0xb1);              
				write_command(0x10+((i+0x04)>>4));	//*set column higher address
				write_command((0x04+i)&0x0f);       //*set column lower address
				if(gridmode==0){write_data((0x80>>(wave[i]-40))|grid0[i+127]);}
				if(gridmode==1){write_data((0x80>>(wave[i]-40))|grid1[i+127]);}
				if(gridmode==2){write_data((0x80>>(wave[i]-40))|grid2[i+127]);}
				if(gridmode==3){write_data((0x80>>(wave[i]-40))|grid3[i+127]);}
			}
			else if(wave[i]>31)
			{
				write_command(0xb2);              
				write_command(0x10+((i+0x04)>>4));	//*set column higher address
				write_command((0x04+i)&0x0f);       //*set column lower address
				if(gridmode==0){write_data((0x80>>(wave[i]-32))|grid0[i+127*2]);}
				if(gridmode==1){write_data((0x80>>(wave[i]-32))|grid1[i+127*2]);}
				if(gridmode==2){write_data((0x80>>(wave[i]-32))|grid2[i+127*2]);}
				if(gridmode==3){write_data((0x80>>(wave[i]-32))|grid3[i+127*2]);}
			}
			else if(wave[i]>23)
			{
				write_command(0xb3);              
				write_command(0x10+((i+0x04)>>4));	//*set column higher address
				write_command((0x04+i)&0x0f);       //*set column lower address
				if(gridmode==0){write_data((0x80>>(wave[i]-24))|grid0[i+127*3]);}
				if(gridmode==1){write_data((0x80>>(wave[i]-24))|grid1[i+127*3]);}
				if(gridmode==2){write_data((0x80>>(wave[i]-24))|grid2[i+127*3]);}
				if(gridmode==3){write_data((0x80>>(wave[i]-24))|grid3[i+127*3]);}
			}
			else if(wave[i]>15)
			{
				write_command(0xb4);              
				write_command(0x10+((i+0x04)>>4));	//*set column higher address
				write_command((0x04+i)&0x0f);       //*set column lower address
				if(gridmode==0){write_data((0x80>>(wave[i]-16))|grid0[i+127*4]);}
				if(gridmode==1){write_data((0x80>>(wave[i]-16))|grid1[i+127*4]);}
				if(gridmode==2){write_data((0x80>>(wave[i]-16))|grid2[i+127*4]);}
				if(gridmode==3){write_data((0x80>>(wave[i]-16))|grid3[i+127*4]);}
			}
			else if(wave[i]>7)
			{
				write_command(0xb5);              
				write_command(0x10+((i+0x04)>>4));	//*set column higher address
				write_command((0x04+i)&0x0f);       //*set column lower address
				if(gridmode==0){write_data((0x80>>(wave[i]-8))|grid0[i+127*5]);}
				if(gridmode==1){write_data((0x80>>(wave[i]-8))|grid1[i+127*5]);}
				if(gridmode==2){write_data((0x80>>(wave[i]-8))|grid2[i+127*5]);}
				if(gridmode==3){write_data((0x80>>(wave[i]-8))|grid3[i+127*5]);}
			}
			else if(wave[i]>=0)
			{
				write_command(0xb6);              
				write_command(0x10+((i+0x04)>>4));	//*set column higher address
				write_command((0x04+i)&0x0f);       //*set column lower address
				if(gridmode==0){write_data((0x80>>(wave[i]))|grid0[i+127*6]);}
				if(gridmode==1){write_data((0x80>>(wave[i]))|grid1[i+127*6]);}
				if(gridmode==2){write_data((0x80>>(wave[i]))|grid2[i+127*6]);}
				if(gridmode==3){write_data((0x80>>(wave[i]))|grid3[i+127*6]);}
			}
		}
		else
		{
			//比较相邻三列三个点的位置，min=最低点，mid=中间点，max=最高点
			if(wave[i]>=wave[i-1]){min=wave[i-1];max=wave[i];}
			else{min=wave[i];max=wave[i-1];}
			if(min>=wave[i+1])
			{
				mid=min;				//两个赋值语句顺序不能颠倒
				min=wave[i+1];	
			}
			else
			{
				if(max>=wave[i+1]){mid=wave[i+1];}
				else{mid=max;max=wave[i+1];}
			}
			//确定该列画点的始末位置及线段长度
			if((wave[i+1]<wave[i])&&(wave[i-1]<wave[i]))
			{
				if(((min+max)%2)){srt=((min+max)/2)+1;}
				else{srt=((min+max)/2);}
				end=max;
				lgth=end-srt+1;
			}
			else if((wave[i+1]>wave[i])&&(wave[i-1]>wave[i]))
			{
				srt=min;
				end=((min+max)/2);
				lgth=end-srt+1;
			}
			else
			{
				//注意 要防止显示越界@@@@@@@@@@@@@@@@@@@@@@1
				if(((min+mid)/2)>=1){srt=((min+mid)/2)-1;}
				else{srt=((min+mid)/2);}
				if(((mid+max)/2)<=54){end=((mid+max)/2)+1;}
				else{end=((mid+max)/2);}
			}
			//根据以上判断赋值该列的显示数组
			for(j=0;j<7;j++)
			{
				pnts[j]=0;
			}
			//注意 要防止显示越界@@@@@@@@@@@@@@@@@@@@@@2
			if(end<=55)							//正常，越界时防止第一行显示
			{
				if((end==1)&&(wave[i-1]==0)&&(wave[i+1]==0)){end=0;}
	            for(j=srt;j<=end;j++)
				{
					pnts[j/8]|=pnts[j/8]|(0x80>>(j%8));
				}
			}
			else
			{
				if((srt<55)&&(srt>28))        //上界超过屏幕时画线到LCD上边界
				{
					for(j=srt;j<=55;j++)
					{
						pnts[j/8]|=pnts[j/8]|(0x80>>(j%8));
					}
				}
			}
			
			for(j=0;j<7;j++)
			{
				//if((pnts[6-j]|grid0[i+127*j])!=0)
				{
					write_command(0xb0+j);              
					write_command(0x10+((i+0x04)>>4));	//*set column higher address
					write_command((0x04+i)&0x0f);       //*set column lower address
					if(gridmode==0){write_data(pnts[6-j]|grid0[i+127*j]);}
					if(gridmode==1){write_data(pnts[6-j]|grid1[i+127*j]);}
					if(gridmode==2){write_data(pnts[6-j]|grid2[i+127*j]);}
					if(gridmode==3){write_data(pnts[6-j]|grid3[i+127*j]);}
				}
			}
		}
	}
}
/*==============显示16*16汉字或者16*8字母==============*/
//页，列，汉字（16）或字母（8），显示个数，字模数组
//hz是汉字或字符的的宽度（字符是汉字的一半）
//该函数也能显示12*12（12*6）的字符，但4行空白无法填充
void dispchar16(unsigned char page,unsigned char column,unsigned char hz,unsigned char n,unsigned char *p)
{
	unsigned char i,j;
	for(j=0;j<n;j++)
	{
		write_command(0xb0+page);		        
		write_command(0x10+((column+0x04+hz*j)>>4));//*set column higher address	
		write_command((column+0x04+hz*j)&0x0f);    	//*set column lower address
		for(i=0;i<hz;i++)
		{
		    write_data(*(p+i+hz*2*j));
		}
		write_command(0xb1+page);		        
		write_command(0x10+((column+0x04+hz*j)>>4));		
		write_command((column+0x04+hz*j)&0x0f); 
		for(i=0;i<hz;i++)
	    {
			write_data(*(p+i+hz*2*j+hz));
		}
	}
}
//------------------------------------------end----------------------------------------------//
/*			//确定该列画点的始末位置及线段长度
			if(wave[i]>=28)
			{
				if((wave[i+1]<wave[i])&&(wave[i-1]<wave[i]))
				{
					if(((min+max)%2)){srt=((min+max)/2)+1;}
					else{srt=((min+max)/2);}
					end=max;
					lgth=end-srt+1;
				}
				else
				{
					srt=((min+mid)/2);
					end=((mid+max)/2);
					lgth=end-srt+1;
					//lgth=max-min;
				}
			}
			else if(wave[i]<28)
			{
				if((wave[i+1]>wave[i])&&(wave[i-1]>wave[i]))
				{
					srt=min;
					end=((min+max)/2);
					lgth=end-srt+1;
				}
				else
				{
					if(((min+mid)%2)){srt=((min+mid)/2)+1;}
					else{srt=((min+mid)/2);}
					if(((mid+max)%2)){end=((mid+max)/2)+1;}
					else{end=((mid+max)/2);}
					lgth=end-srt+1;
				}
			}
			else
			{
				srt=((min+mid)/2);
				if(((mid+max)%2)){end=((mid+max)/2)+1;}
				else{end=((mid+max)/2);}
				lgth=end-srt+1;
			}
			//确定该列画点的始末位置及线段长度
			if(wave[i-1]<wave[i])
			{
				if(wave[i+1]<wave[i])
				{
					srt=((min+max)/2);
					end=max;
					lgth=end-srt+1;
				}
				else if(wave[i+1]>wave[i])
				{
					srt=((min+mid)/2);
					end=((mid+max)/2);
					lgth=end-srt+1;
					//lgth=max-min;
				}
				else
				{
					srt=((min+mid)/2);
					end=((mid+max)/2);
					lgth=end-srt+1;
					//lgth=max-min;
				}
			}
			else if(wave[i-1]>wave[i])
			{
				if(wave[i+1]<wave[i])
				{
					srt=((min+max)/2);
					end=max;
					lgth=end-srt+1;
				}
				else if(wave[i+1]>wave[i])
				{
		  			if(((min+mid)%2)){srt=((min+mid)/2)+1;}
					else{srt=((min+mid)/2);}
					if(((mid+max)%2)){end=((mid+max)/2)+1;}
					end=((mid+max)/2);
					lgth=end-srt+1;
					//lgth=max-min;
				}
				else
				{
					srt=((min+mid)/2);
					end=((mid+max)/2);
					lgth=end-srt+1;
					//lgth=max-min;
				}
			}
			else
			{
				if(wave[i+1]<wave[i])
				{
					srt=((min+mid)/2)+1;
					lgth=max-min;
					end=(mid+max)/2;
				}
				else if(wave[i+1]>wave[i])
				{
					srt=((min+mid)/2);
					lgth=max-min;
					end=((mid+max)/2)+1;
				}
				else
				{
					srt=((min+mid)/2)+1;
					lgth=max-min;
					end=((mid+max)/2)+1;
				}
			}
//确定该列画点的始末位置及线段长度
			if(wave[i-1]<wave[i])
			{
				if(wave[i+1]<wave[i])
				{
					srt=((min+mid)/2)+1;
					lgth=max-min;
					end=(mid+max)/2;
				}
				else if(wave[i+1]>wave[i])
				{
					srt=((min+mid)/2);
					lgth=max-min;
					end=((mid+max)/2)+1;
				}
				else
				{
					srt=((min+mid)/2)+1;
					lgth=max-min;
					end=((mid+max)/2)+1;
				}
			}
			else if(wave[i-1]>wave[i])
			{
				srt=((min+mid)/2)+1;
				lgth=max-min;
				end=((mid+max)/2)+1;
			}
			else
			{
				if(wave[i+1]<wave[i])
				{
					srt=((min+mid)/2)+1;
					lgth=max-min;
					end=(mid+max)/2;
				}
				else if(wave[i+1]>wave[i])
				{
					srt=((min+mid)/2);
					lgth=max-min;
					end=((mid+max)/2)+1;
				}
				else
				{
					srt=((min+mid)/2)+1;
					lgth=max-min;
					end=((mid+max)/2)+1;
				}
			}
*/
