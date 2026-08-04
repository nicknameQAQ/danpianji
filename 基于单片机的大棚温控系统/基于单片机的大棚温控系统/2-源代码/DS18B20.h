#include <math.H>
#include <intrins.h>

#define uchar unsigned  char
#define uint  unsigned   int 
uint temp,templ;
int Tp  ;  // 温度的整数位

sbit DQ = P3^3;                     //DS18B20的数据口位

void delayUs()		  //延时 微秒级
{
    _nop_();
}

void delayMs(uint a)	 //延时 毫秒级
{
	uint i, j;
	for(i = a; i > 0; i--)
		for(j = 100; j > 0; j--);
}


void DS18b20_Init()				 //初始化18B20
{
	uint i;  
	DQ = 0;
	i = 100;  
	while(i>0) i--;
	DQ = 1;   
	i = 4;
	while(i>0) i--;
}
void dsWait()	  //DS18B20等待信号
{
	uint i;
	while(DQ);  
	while(~DQ);
	i = 4;
	while(i > 0) i--;
}
bit readBit()  //DS18B2读取一位
{
    uint i;
    bit b;
    DQ = 0;
    i++;   
    DQ = 1; 
    i++; i++;  
    b = DQ;
    i = 8; 
    while(i>0) i--;
    return b;
}
uchar readByte() //ds18b20读一个字节
{
    uint i;
    uchar j, dat;
    dat = 0;
    for(i=0; i<8; i++)
    {
        j = readBit();   
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}


void write_18b20(uchar dat)	//写数据
{
    uint i;
    uchar j;
    bit b;
    for(j = 0; j < 8; j++)	   //八位数据
    {
        b = dat & 0x01;
        dat >>= 1;   
        if(b)   
        {
            DQ = 0;          i++; i++;  
            DQ = 1;    
            i = 8; while(i>0) i--;  
        }
        else  
        {
            DQ = 0;
            i = 8; while(i>0) i--;  
            DQ = 1;
           i++; i++;
        }
   }
}
void sendChangeCmd()    //发送启动会温度检测命令
{
    DS18b20_Init();    
    dsWait();   
    delayMs(1);    
    write_18b20(0xcc);
    write_18b20(0x44); //启动温度检测 
}

void sendReadCmd()		 //发送读取温度检测命令
{
    DS18b20_Init();
    dsWait();
    delayMs(1);
    write_18b20(0xcc);   //跳过ROM
    write_18b20(0xbe);   //读取温度值
}

int getTmpValue()
{
    uint tmpvalue;
    int value; 
    float t;
    uchar low, high;
    sendReadCmd();
    
    low = readByte(); 	   //读取温度低八位
    high = readByte();	   //读取温度高八位
   
    tmpvalue = high;
    tmpvalue <<= 8;
    tmpvalue |= low;
    value = tmpvalue;     //合并数值得到真实温度
    
    t = value * 0.0625;	  //消除误差得到小数

    value = t * 100 + (value > 0 ? 0.5 : -0.5); //大于0加0.5, 小于0减0.5
    return value;
}
void display(int v) 
{
    uchar count;
    uchar datas[] = {0, 0, 0, 0, 0};
    uint tmp = abs(v);
    datas[0] = tmp / 10000;
    datas[1] = tmp % 10000 / 1000;
    datas[2] = tmp % 1000 / 100;
    datas[3] = tmp % 100 / 10;
    datas[4] = tmp % 10;
	Tp=datas[0]*100+datas[1]*10+datas[2]; // 得到整数位
    write_com(0x80+6);
    if(v < 0)
    {		 //	write_dat('+');
        write_dat('-');
    }
    else
    {
       write_dat('+');
    }
    if(datas[0] != 0)
    {
        write_dat('0'+datas[0]);
    }
    for(count = 1; count != 5; count++)
    {
        write_dat('0'+datas[count]);
        if(count == 2)
        {
            write_dat('.');
        }
    }
    write_dat(0xDF);	write_dat('C');//	℃
}

