typedef  unsigned int uint;
typedef  unsigned char uchar;
 
sbit RS=P2^0;	  //液晶控制引脚
sbit RW=P2^1;
sbit E=P2^2;


void delay3(uint z)
{
	while(z--);
}

void write_com(uchar com)	 //液晶写命令
{
	RS=0;
	RW=0;
	E=0;
	P0=com;
	delay3(50);
	E=1;
	delay3(50);
	E=0;
}

void write_dat(uchar dat)	 //液晶写数据
{
	RS=1;
	RW=0;
	E=0;
	P0=dat;
	delay3(50);
	E=1;
	delay3(50);
	E=0;
}

void write_string(uchar x,uchar y,uchar *s)	//显示字符串
{
	if(y==0)
		write_com(0x80+x);
	else
		write_com(0xc0+x);
	while(*s)
	{
		write_dat(*s);
		s++;
	}
}
								 
void LCD_Init()			 //初始化液晶时间显示
{
	write_com(0x38);

	write_com(0x0c);

	write_com(0x06);

//	write_string(0,0,"hello world");
}
