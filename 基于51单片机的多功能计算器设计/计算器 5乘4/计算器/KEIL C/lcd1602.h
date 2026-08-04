#define uint unsigned int
#define uchar unsigned char
#define yh 0x80	 //第一行位置定义
#define er 0x80+0x40 	   //第二行位置定义
sbit rs=P2^0;			   //RS引脚
sbit en=P2^2;			   //EN引脚
sbit rw=P2^1;			   //RW引脚

//8位数据反转函数
//由于硬件接法问题，为方便布线，把液晶的数据口反转
//如输入0101 1110输出0111 1010
uchar fan_zhuan_dat(uchar dat)
{
	uchar i,j=0x01,a,b;
	for(i=0;i<8;i++)
	{
		a=j&dat;
		a>>=i;
		
		b<<=1;
		j<<=1;
		b=b|a;
	}
	return b;
}

//向液晶写命令
void write_1602com(uchar com)
{
	rs=0;
	rw=0;
	P0=fan_zhuan_dat(com);
	delay(1);
	en=1;
	delay(1);
	en=0;
}

//向液晶写数据
void write_1602dat(uchar dat)
{
	rs=1;
	rw=0; 
	P0=fan_zhuan_dat(dat);
	delay(1);
	en=1; 
	delay(1);
	en=0;
	weizhi++;		  //写完一位数据后，指针加一，标志为跟随加一
	if(weizhi==0x80+16)		  //指针加到第一行末尾，自动转到第二行
	{write_1602com(er);weizhi=er;}
	if(weizhi==(er+16))		  //指针加到第二行末尾，自动转到第一行
	{write_1602com(0x80);weizhi=yh;}

}

//液晶初始化
void lcd_init()	
{
	write_1602com(0x38);
	write_1602com(0x0f);
	write_1602com(0x06);
	write_1602com(0x01);

}
