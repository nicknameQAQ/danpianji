#include"serial.h"
#include"data.h"
#include "beep.h"
#include "judge.h"
#include "dalay.h"

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

unsigned char UART_buff;  

bit New_rec = 0, Send_ed = 1; 


void UsartInit()											              
{
	SCON=0X50;     	    // 串行口工作方式寄存器 设置为工作方式1   0101 0000
	TMOD=0X20;			//设置计数器工作方式2 ，8位自动重装载
	PCON=0X80;			//波特率加倍  1000 0000
	TH1=0XF3;			//计数器初始值设置，注意波特率是4800的
	TL1=0XF3;
	
	ES=1;						//打开接收中断
	EA=1;						//打开总中断
	TR1=1;						//打开计数器
}

void Usart() interrupt 4
{
	u8 receiveData;
	if(RI == 1)
	{
	receiveData=SBUF;//出去接收到的数据
	RI = 0;          //清除接收中断标志位
	SBUF=receiveData;//将接收到的数据放入到发送寄存器
	while(!TI);			 //等待发送数据完成//等待一帧数据(一个字符)发完，TI置1，
	TI=0;				//让TI为0就继续接收，		 //清除发送完成标志位


	if(receiveData == '0')  year++,p=p+number;  
    if(receiveData == '1')  month++,p=p+maxday;  
    if(receiveData == '2')  day++,p++; 
	if(receiveData == '3')  h++;  
    if(receiveData == '4')  min++;  
    if(receiveData == '5')  h1++; 
	if(receiveData == '6')  min1++;  
    if(receiveData == '7')  h2++;  
    if(receiveData == '8')  min2++; 
	if(receiveData == '9')  y++; 
	if(receiveData == 'a')  year--,p=p-number;
    if(receiveData == 'b')  month--,p=p-maxday;  
    if(receiveData == 'c')  day--,p--; 
	if(receiveData == 'd')  h--;  
    if(receiveData == 'e')  min--;  
    if(receiveData == 'f')  h1--; 
	if(receiveData == 'g')  min1--;  
    if(receiveData == 'h')  h2--;  
    if(receiveData == 'i')  min2--; 
	if(receiveData == 'j')  y--;
	if(receiveData == 'k')  year0=year0+1;
	if(receiveData == 'o')  year0=year0-1;
	if(receiveData == 'p')  month0=month0+1;
	if(receiveData == 'q')  month0=month0-1;
	if(receiveData == 'z')  day0=day0+1;
	if(receiveData == 'x')  day0=day0-1;

    }  
    else  				  //如果送毕. 
	{         
      TI = 0;      //清除标志.  
      Send_ed = 1;  
    }  
}

void send()
{
	
	if(sec%3==0) 
	sends();

}

extern char w[7];
extern int t[14],c[16];
char m1[]="时间";
char m2[]="年"	;
char m3[]="月"	;
char m4[]="日"	;

void wei(char term)			//单个位输入
{
	ES=0;
	SBUF=term;
	while(!TI);
	TI=0;
	ES=1;
}

void duowei(char m[])		   //多位输入
{
	u8 i=0;
	while(m[i]!='\0')
	{
		wei(m[i]);
		i++;
	}
}
void sends()
{
	duowei(m1);
	wei(t[0]+'0');
	wei(t[1]+'0');
	wei(':');
	wei(t[2]+'0');
	wei(t[3]+'0');
	wei(':');
	wei(t[4]+'0');
	wei(t[5]+'0');
	wei(' ');
	duowei(w);
	wei(' ');
	wei(c[0]+'0');
	wei(c[1]+'0');
	wei(c[2]+'0');
	wei(c[3]+'0');
	duowei(m2);
	wei(c[4]+'0');
	wei(c[5]+'0');
	duowei(m3);
	wei(c[6]+'0');
	wei(c[7]+'0');
	duowei(m4);
	wei('\n');	
}



