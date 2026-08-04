#include "PCF8591.h"
#include "delay.h"


sbit SCL=P2^1;  //串行时钟输入端  
sbit SDA=P2^0;  //串行数据输入端
uchar flag=0;

void start()//开始信号  
{  
    SDA=1;  
    delay();  
    SCL=1;  
    delay();  
    SDA=0;  
    delay();  
}  
  
void stop() //停止信号  
{  
    SDA=0;  
    delay();  
    SCL=1;  
    delay();  
    SDA=1;  
    delay();  
}  
  
  
void respons()//应答    相当于一个智能的延时函数  
{  
    uchar i;  
    SCL=1;  
    delay();  
    while((SDA==1)&&(i<250))  
        i++;  
    SCL=0;  
    delay();  
}  
  
void init() //初始化  
{  
    SDA=1;  
    delay();  
    SCL=1;  
    delay();      
}  
  
void write_byte(uchar date) //写一字节数据  
{  
    uchar i,temp;  
    temp=date;  
    for(i=0;i<8;i++)  
    {  
        temp=temp<<1; //左移一位 移出的一位在CY中  
        SCL=0;          //只有在scl=0时sda能变化值  
        delay();  
        SDA=CY;  
        delay();  
        SCL=1;  
        delay();          
    }     
    SCL=0;  
    delay();  
    SDA=1;  
    delay();  
}  
  
  
  
void write_add(uchar control,uchar date)  
{  
    start();  
    write_byte(PCF8591);    //10010000  前四位固定 接下来三位全部被接地了 所以都是0 最后一位是写 所以为低电平  
    respons();  
    write_byte(control);  
    respons();  
    write_byte(date);  
    respons();  
    stop();  
  
}  

