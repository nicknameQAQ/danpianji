#include<reg52.h>				 //头文件
#include<intrins.h>
#include "DS18b20.H"
#include "eeprom52.h"
#include "Nixie tube.h"
#define uchar unsigned char		 //宏定义
#define uint unsigned int
uchar code ss[]=
 {
  0xc0,0xf9,0xa4,0xb0,0x99,
  0x92,0x82,0xf8,0x80,0x90
 };                           //数码管
uchar code ss1[]=
 {
  0x40,0x79,0x24,0x30,0x19,
  0x12,0x02,0x78,0x00,0x10,
  0xc7,0x89,0xbf               //0xc7=L   0x89=H   0xbf=- 负号
 };                            //0-9为带小数点的 
uchar ms;	                   //定时器0计数
sbit key1=P3^4;
sbit key2=P3^5;
sbit key3=P3^3;
sbit led_0=P1^0;                //上限温度报警灯
sbit led_2=P1^2;                //下限温度报警灯
char H,L;                       //用于设置上下限度  上下限度与要比较的数据类型最好相同
uchar flag; 
sbit beep=P1^6;                 //蜂鸣器
bit s0,beep_1=1;		 			//s0用于闪烁标志  beep_1用于在中断里改变蜂鸣器工作状态
int temp,i;                      //存储读出来的温度，与H，L上下限度数据类型相同
void delay(uint T)
{
  while(T--);
}

void display()
{ 	  
  if(flag==0&&temp>=0)          //温度为正数时执行
	{
     if(temp>999) Nixietube(ss[temp/1000%10],ss[temp/100%10],ss1[temp/10%10],ss[temp%10]);
	 if(temp>99)  Nixietube(0xff,ss[temp/100%10],ss1[temp/10%10],ss[temp%10]);
	 if(temp<99&&temp/100%10<=0)  Nixietube(0xff,0xff,ss1[temp/10%10],ss[temp%10]);

    }
  if(flag==0&&temp<0)          //温度为负数时
	{
     if(temp*-1/100%10<=0) Nixietube(0xff,ss1[12],ss1[temp*-1/10%10],ss[temp*-1%10]);
	 if(temp*-1/100%10>0)  Nixietube(ss1[12],ss[temp*-1/100%10],ss1[temp*-1/10%10],ss[temp*-1%10]);
	}
  if(flag==1)                 //flag==0显示温度  =1 显示上限   =2 显示下限
	{ 
     if(H>99)          Nixietube(ss1[11],ss[H/100],ss[H%100/10],ss[H%10]);   //温度范围是125~-55 ，当上限值大于99时执行  
	 if(H>=10&&H<=99)  Nixietube(ss1[11],0xff,ss[H/10],ss[H%10]);            //当上限值大于12小于99时执行  不显示最高位
	 if(H<10&&H>=0)    Nixietube(ss1[11],0xff,0xff,ss[H%10]);                //不显示百位 十位
	 if(H<0&&H>=-9)    Nixietube(ss1[11],0xff,ss1[12],ss[H*-1%10]);          //设置的为负数且介于0~-9之间时执行  不显示十位
     if(H<0&&H<-9)     Nixietube(ss1[11],ss1[12],ss[H*-1%100/10],ss[H*-1%10]);	 //最低可以到55  		 
    }		
if(flag==2)            //同上                                                
	{ 
     if(L>99)          Nixietube(ss1[10],ss[L/100],ss[L%100/10],ss[L%10]);
	 if(L>=10&&L<=99)  Nixietube(ss1[10],0xff,ss[L/10],ss[L%10]);
	 if(L<10&&L>=0)    Nixietube(ss1[10],0xff,0xff,ss[L%10]);
	 if(L<0&&L>=-9)  Nixietube(ss1[10],0xff,ss1[12],ss[L*-1%10]);
     if(L<0&&L<-9)  Nixietube(ss1[10],ss1[12],ss[L*-1%100/10],ss[L*-1%10]);	 		 
    }
					
  }
void key()                              //按键处理
{
	
if(!key1)                               //设置上限
{
   delay(500);
   if(!key1)
	{
      while(!key1); 
	  if(flag==1)
	  	{
         if(H<125)H++;
		 SectorErase(0x2000);          //每修改一次上下限数据就写进EEROM
		 byte_write(0x2000,H); 
        }
	  else if	(flag==2)
		{
          if(L<H-1) L++;                //原则是下限不超过上限
		  SectorErase(0x2200);
		  byte_write(0x2200,L);
        }	
    }	
}

if(!key2)                            //设置下限
{
    delay(500);
	if(!key2)
	 {
       while(!key2); 
	   if(flag==1)
	  	{
		  if(H>L+1)  H--;             //原则上限不低于下限
		  SectorErase(0x2000);        //每修改一次上下限数据就写进EEROM
		  byte_write(0x2000,H);
        }
	   else if	(flag==2)
		{
		  if(L>-55)L--;
		  SectorErase(0x2200);
	      byte_write(0x2200,L);
        }	
     }
}
if(!key3)                         //设置上限下限切换按键
	{
	 delay(1000);
	 if(!key3)
       { 
		while(!key3);
        flag=(flag+1)%3;			
       }
  }
}
void alarm()                          //报警
{
if(temp/10>=H||temp/10<=L)               //超过上下限蜂鸣器报警 ，正常的LED灯关闭
   {
    beep_1=0;
   }	
else
	{ 
	  beep_1=1;
    }                             //正常情况下，蜂鸣器关闭  
if(temp/10>=H)                    //上限，当测得的值，大于等于设定值得时候，
	{ 
	  led_0=0;     
	}
else
   {
    led_0=1;
   }
		
if(temp/10<=L)                        //下限 当实际值小于等于下限值时
   { 
	 led_2=0;     
   }
else
   {
     led_2=1;
   }	
}
void main()                        //主函数          
{  
    do
     {
	  temp=Temper();
     }
    while(temp==850);   	       //因为DS18B20上电之后会读出来850，但不是温度值，所以等待读出温度值    
	delay(1000);
	H=byte_read(0x2000);
	L=byte_read(0x2200);
	if((H>99)||(L>99)||(L>=H))     //读EEROM里面的数据 
      {
		H=35;
		L=16;                      //如果数据不对，赋予初始值 
	  }  
	TMOD=0x01;
	TH0=(65536-10000)/256;
	TL0=(65536-10000)%256;
	EA=1;
	ET0=1;
    TR0=1;                         //设置定时器与中断
	 
while(1)    
	{  
 
	  key();
	  alarm();
          	 
	 
	}
}



void UART_1() interrupt 1
{
	TH0=(65536-10000)/256;
	TL0=(65536-10000)%256;             //一次定时10MS   十进制65536-50000=15536   折合十六进制3cb0  高位3c低位b0   
	ms++;
	display();						    // 完全消影可以开启这个，屏蔽主函数大循环里的显示函数
 if(ms>100)                            //数据一秒刷新一次
	{ 
	  ms=0; 
	  temp=Temper();                   //读温度
	}	 
 if(ms%5==0)                  
	{
	 if(beep_1==0)                      //蜂鸣器设置
	  {
		beep=!beep;
	  }
	 else
      {
		beep=1;
	  }
    }  
}





