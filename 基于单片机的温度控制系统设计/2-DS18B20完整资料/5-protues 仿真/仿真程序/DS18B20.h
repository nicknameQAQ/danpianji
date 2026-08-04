#ifndef _DS18B20_H_
#define _DS18B20_H_

#define uchar unsigned char 
#define uint unsigned int
sbit DS1820_dat=P3^7;


void DS18B20_delay(uint T)							   //????
{
	while(T--);
}

 
void Initial()			 
{
    DS1820_dat=1;
   	DS18B20_delay(8);		  //??16??
    DS1820_dat=0;
    DS18B20_delay(80);
    DS1820_dat=1;
    DS18B20_delay(15);
} 
 
void Input(uchar f)			//18B20???????
{
    uchar j;
    for (j=0;j<8;j++)
    {
        DS1820_dat=0;
        DS1820_dat=f&0x01;
        DS18B20_delay(10);
        DS1820_dat=1;
        f>>=1;
    }
}

uchar Output()		   //18b20 ????????
{
    uchar j,b;
    for (j=0;j<8;j++)
    {
        DS1820_dat=0;
        b>>=1;
        DS1820_dat=1;
        if (DS1820_dat)
        {
            b=b|0x80;
        }
        DS18B20_delay(10);
    }
    return(b);
}
int Temper()
{	  
	int Store;
	int Store_2,Store_1;
	Initial();				   //对 DS18B20 复位
	Input(0xcc);			  	//跳过 ROM
	Input(0x44);			   //启动温度转换
	Initial();				   //对 DS18B20 复位
	Input(0xcc);			   //跳过 ROM
	Input(0xbe);			   //读 RAM 命令
	Store_1=Output();		   //读温度的低字节
	Store_2=Output();	 	   //读温度的高字节	 
	Store_2=(Store_2*256)+Store_1;	   //合起来为温度的整数部分
	Store=(int) Store_2*0.625;
	return Store;
}

#endif