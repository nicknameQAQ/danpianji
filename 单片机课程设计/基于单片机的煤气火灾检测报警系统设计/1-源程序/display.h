//宏定义
#define uint unsigned int 
#define uchar unsigned char

//LCD管脚声明
sbit LCDRS = P2^3;
sbit LCDEN= P2^4;

//初始画时显示的内容
uchar code Init1[]="MQ2:    MQ5 :   ";
uchar code Init2[]="H2 :    H5  :   ";

//LCD延时
void LCDdelay(uint z)
{
  uint x,y;
  for(x=z;x>0;x--)
    for(y=10;y>0;y--);
}
//写命令
void write_com(uchar com)
{
  LCDRS=0;
  P0=com;
  LCDdelay(5);
  LCDEN=1;
  LCDdelay(5);
  LCDEN=0;
}
//写数据
void write_data(uchar date)
{
  LCDRS=1;
  P0=date;
  LCDdelay(5);
  LCDEN=1;
  LCDdelay(5);
  LCDEN=0;
}
//1602初始化
void Init1602()
{
  uchar i=0;
  write_com(0x38);//屏幕初始化
  write_com(0x0c);//打开显示 无光标 无光标闪烁
  write_com(0x06);//当读或写一个字符是指针后一一位
  write_com(0x01);//清屏
  write_com(0x80);//设置位置

  for(i=0;i<16;i++)
  {
		write_data(Init1[i]);
  }								  
  write_com(0x80+0x40);//设置位置	 
  for(i=0;i<16;i++)
  {
		write_data(Init2[i]);
  }
}

void Display_1602(uchar NOW_GAS,uchar NOW_CO,uchar SET_GAS,uchar SET_CO)
{

	write_com(0x80+4);
	write_data('0'+NOW_GAS/100);
	write_data('0'+NOW_GAS/10%10);
	write_data('0'+NOW_GAS%10);

	write_com(0x80+0x40+4);
	write_data('0'+SET_GAS/100);
	write_data('0'+SET_GAS/10%10);
	write_data('0'+SET_GAS%10);

	write_com(0x80+13);
	write_data('0'+NOW_CO/100);
	write_data('0'+NOW_CO/10%10);
	write_data('0'+NOW_CO%10);

	write_com(0x80+0x40+13);
	write_data('0'+SET_CO/100);
	write_data('0'+SET_CO/10%10);
	write_data('0'+SET_CO%10);
}




