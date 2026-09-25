#include<reg52.h> 
#include<intrins.h>  //包含_nop_();函数定义的头文件 
void timer0_int(); 
sbit LED=P3^7; 
sbit E=P3^2;  //1602使能引脚 
sbit RW=P3^1;  //1602读写引脚  
sbit RS=P3^0;  //1602数据/命令选择引脚 
unsigned char hour,minute,second; 
unsigned char n,count; 
unsigned long lengthbuf,lengthvalue; 
unsigned char code 
Table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};    
//0~9的共阴极七段管代码 
unsigned char code 
Table1[]={0xbf,0x86,0xdb,0xcf,0xe6,0xed,0xfd,0x87,0xff,0xef};   
//0~9的共阴极七段管代码带小数点 
void Delay0(int Times); 
void Display(unsigned int x); 
void delay()  //延时5US 
{ 
_nop_(); 
_nop_(); 
_nop_(); 
_nop_(); 
_nop_(); 
}
bit Busy(void)     //读状态函数判断液晶模块的忙碌状态 
{ 
bit busy_flag = 0; 
RS = 0; 
RW = 1; 
E = 1; 
delay(); 
busy_flag = (bit)(P0 & 0x80); 
E = 0; 
return busy_flag; 
} 
void wcmd(unsigned char del)  //将模式设置指令或显示地址写入液晶模
{ 
while(Busy()); 
RS = 0; 
RW = 0; 
E = 0; 
delay(); 
P0 = del; 
delay(); 
E = 1; 
delay(); 
E = 0; 
} 
void wdata(unsigned char del)   
///将数据字符ASCII码写入液晶
{ 
 while(Busy()); 
RS = 1; 
RW = 0; 
E = 0; 
delay(); 
P0 = del; 
delay(); 
E = 1; 
delay(); 
E = 0; 
} 
void L1602_init(void)   //初始化液晶模块 
{ 
 wcmd(0x38);  //功能设置8位字长2行5*7点阵 
 wcmd(0x0c);  //显示设置显示屏右移 
 wcmd(0x06);  //显示设置光标右移字符不移 
 wcmd(0x01);     //清屏命令 
} 

void L1602_string(unsigned char hang,unsigned char lie,unsigned char p)   
//将数据写入液晶模块 
{ 
unsigned char a; 
if(hang == 1) a = 0x80;      //显示在第一行 
if(hang == 2) a = 0xc0;      //显示在第一行 
a = a + lie -1; 
wcmd(a); 
wdata(p); 
} 
void timer0_int() interrupt 1 using 3     //中断用于构成时钟 
{ 
 n++; 
 if(n==100) 
 { 
 n=0; 
  count++; 
  second++; 
  if(second>=60) 
      {  
        second=0; 
         minute++; 
       if(minute>=60) 
      { 
     minute=0; 
      hour++; 
      if(hour>=24) hour=0; 
    } 
   } 
  } 
   if(count==5) 
     {   
       count=0; 
    lengthvalue=(TL1+TH1*256)*215-lengthbuf; 
    lengthbuf=(TL1+TH1*256)*215; 
     } 
   TH0=0xd8; 
   TL0=0xf0; 
    }  

int timer()             //以秒为单位计算运行总时间
{ 
   unsigned char a=100; 
   unsigned char b; 
   b=second+minute*60+hour*60*60; 
 return b; 
} 
 void Delay0(int Time)                      //延时子程序 
{
 int i; 
 unsigned char j; 
   for(i=0;i<Time;i++) 
      { 
       for(j=0;j<150;j++)  
         { 
          } 
       } 
} 
void Display(unsigned int x)       //在LED七段管中显示最高速度
{
 unsigned  char slc;  
 unsigned char a,b,c,d; 
 slc=0xef;           //LED七段管选通
 a= x/1000; 
 b=( x-a*1000)/100; 
 c=( x-a*1000-b*100)/10; 
 d= x%10; 
  {  
     P2=slc;           //分别将最大速度的各个位扫描显示
  slc=slc<<1; 
  P1=Table[a]; 
  Delay0(10); 
  P2=slc; 
  slc=slc<<1; 
  P1=Table1[b]; 
  Delay0(10); 
  P2=slc; 
  slc=slc<<1; 
  P1=Table[c]; 
  Delay0(10); 
  P2=slc; 

  slc=slc<<1; 
  P1=Table[d]; 
  Delay0(10); 
  }
 return; 
} 
void main() 
{ 
 unsigned char i=0 ; 
 unsigned int v=0;      //运行速度
 unsigned int vnow;      //当前速度
 unsigned int a,b,c,d;     //分别显示速度的十位个位十分位 百分位 
 unsigned long quanshu;     //定义车轮转动圈数 
 unsigned long 
length,length1,length2,length3,length4,length5,length6;  
unsigned int second1,second2,minute1,minute2,hour1,hour2; 
 L1602_init(); 
 TMOD=0x51;    //定时器T0计数器T1允许外部中断T1工作方式01
    TH1=0x00;    //计数器T1高位装初值 
    TL1=0x00;    //计数器T1低位装初值 
    ET1=1;     //计数器0开中断 
    TR1=1;     //计数器0开始工作 	
 TH0=0XD8;    //T0设置时间常数 
 TL0=0XF0; 
 ET0=1; 
 EA=1; 
 TR0=1; 
 IT1=1;     //外部中断1请求触发方式 
 EX1=1; 
 P1=0X00;     //P1、P2口装初值用于LED七段管显示用 
 P2=0xff; 
 while(1) 
{
quanshu=TL1+TH1*256;  //计算车轮旋转圈数 
 length=quanshu*215;  //骑行距离 
                  //计算里程各个进位
 length1=length/100000; 
 length2=(length-length1*100000)/10000; 
 length3=(length-length1*100000-length2*10000)/1000;
 length4=(length-length1*100000-length2*10000-length3*1000)/100; 
 length5=(length-length1*100000-length2*10000-length3*1000-length4
*100)/10; 
 length6=length%10; 
 if(timer()>0) 
 v=length/timer();       //计算速度 
    vnow=lengthvalue/5;       //计算当前速度5秒内
 if(vnow>=695) LED=0;else LED=1;    //速度超过6.95m/s(即 
 Display(vnow);        //在LED七段管上显示当前速度5秒内速度
 Delay0(20); 
 a=(v/1000);               //计算速度的各个进位 
 b=(v-a*1000)/100; 
 c=(v-a*1000-b*100)/10; 
 d=v%10; 
 second1=second/10;       //计算分秒时的十位与个位 
 second2=second%10;
 minute1=minute/10; 
 minute2=minute%10; 
 hour1=hour/10; 
 hour2=hour%10; 
 L1602_string(1,1,'s');         //在LCD相应位置显示变量或者常量
 L1602_string(1,2,'p'); 
 L1602_string(1,3,'e'); 
 L1602_string(1,4,'e'); 
 L1602_string(1,5,'d'); 
 L1602_string(1,6,' '); 
 L1602_string(1,7,'L'); 
 L1602_string(1,8,'='); 
 L1602_string(1,9,length1+48); 
 L1602_string(1,10,length2+48); 
 L1602_string(1,11,length3+48); 
 L1602_string(1,12,length4+48); 
 L1602_string(1,13,'.'); 
 L1602_string(1,14,length5+48); 
 L1602_string(1,15,length6+48); 
 L1602_string(1,16,'m'); 
 L1602_string(2,1,a+48);      
 L1602_string(2,2,b+48); 
 L1602_string(2,3,'.'); 
 L1602_string(2,4,c+48); 
 L1602_string(2,5,d+48); 
 L1602_string(2,6,'m'); 
 L1602_string(2,7,'/'); 
 L1602_string(2,8,'s'); 
 L1602_string(2,9,hour1+48); 
 L1602_string(2,10,hour2+48); 
 L1602_string(2,11,':'); 
 L1602_string(2,12,minute1+48); 
 L1602_string(2,13,minute2+48); 
 L1602_string(2,14,':'); 
 L1602_string(2,15,second1+48); 
 L1602_string(2,16,second2+48); 
 while(i<2)           //LCD显示延迟 
 {delay(); 
 i++; } 
 i=0;
 } 
 }