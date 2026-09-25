#ifndef _LCD1602_H_
#define _LCD1602_H_

#define uchar unsigned char 
#define uint unsigned int 

#define data_1602 P1
uchar code table_num[]="0123456789abcdefg";

sbit rs=P1^0;	 //寄存器选择信号 H:数据寄存器  	L:指令寄存器
sbit rw=P1^1;	 //寄存器选择信号 H:数据寄存器  	L:指令寄存器
sbit e =P1^2;	 //片选信号   下降沿触发

/***********************延时函数************************/
void delay_uint(uint q)
{
	while(q--);
}

/***********************lcd1602写命令函数************************/
void write_com(uchar com)
{
	e=0;
	rs=0;
	rw=0;
	P0=com;
	delay_uint(3);
	e=1;
	delay_uint(25);
	e=0;
}

/***********************lcd1602写数据函数************************/
void write_data(uchar dat)
{
	e=0;
	rs=1;
	rw=0;
	P0=dat;
	delay_uint(3);
	e=1;
	delay_uint(25);
	e=0;	
}

/***********************lcd1602初始化设置************************/
void init_1602()
{
	write_com(0x38);	//
	write_com(0x0c);
	write_com(0x06);
}


/***********************lcd1602上显示两位十进制数************************/
void write_sfm3_18B20(uchar hang,uchar add,uint date)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
	write_data(0x30+date/100%10);
	write_data(0x30+date/10%10);
	write_data(0x30+date%10);	
}

/***********************lcd1602上显示这字符函数************************/
void write_string(uchar hang,uchar add,uchar *p)
{
	if(hang==1)   
		write_com(0x80+add);
	else
		write_com(0x80+0x40+add);
		while(1)														 
		{
			if(*p == '\0')  break;
			write_data(*p);
			p++;
		}	
}

				 

#endif