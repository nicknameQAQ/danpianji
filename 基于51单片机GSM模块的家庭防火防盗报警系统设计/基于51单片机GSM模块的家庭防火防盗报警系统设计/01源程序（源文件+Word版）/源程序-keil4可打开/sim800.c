#include <reg52.h> 
#include "sim800.h"	
/*****************************************************************************
UART初始化 11.0592
*****************************************************************************/
void Uart_Init(void)		
{
	EA=1;
	ES=1;
	SCON = 0x50;			 
	TMOD = 0x20;
	PCON=0x00;	
	TH1 = 0xFD;	
	TL1 = 0xfd;
	TR1 = 1;		
	RI = 0;	
}

/*****************************************************************************
单字符发送函数
/****************************************************************************/
void Send_Char (unsigned char UART_data)		
{ 		
	SBUF = UART_data;			
	while(TI == 0);			
	TI = 0;					  
}

/*****************************************************************************
字符串发送函数
/****************************************************************************/
void Send_String(unsigned char *str)		
{
	while(*str != '\0')		
	{
		Send_Char(*str);			
		*str=*str++;		
	}
	*str = 0;				  
}
/*****************************************************************************
延迟函数
/****************************************************************************/
void Delay_Ms (unsigned int a)
{
	unsigned int i;
	while( a-- != 0)
	{
	for(i = 0; i < 100; i++);	//	for(i = 0; i < 600; i++);
	}
} 
/*****配置为接收短信模式*************/
void Receive_message()
{
 	Send_String("AT+CMGF=1\r\n"); //配置英文模式
	Delay_Ms(500);	
	Send_String("AT+CSCS=\"GSM\"\r\n");     //配置字节集
	Delay_Ms(500);	
	Send_String("AT+CNMI=2,2,0,1\r\n"); //配置成：接受短信提醒，同时接受短信内容。（短信显示模式 新消息指示）
	Delay_Ms(1000);
}
 /*****发短信*******
  *content:发送内容*/
void Send_message(unsigned char *content)	  //
{
 	Send_String("AT+CMGF=1\r\n");
	Delay_Ms(200);
	Send_String("AT+CSMP=17,167,2,25\r\n");//设置短信文本模式参数
	Delay_Ms(200);
	Send_String("AT+CSCS=\"GSM\"\r\n");     //配置英文格式字节集
	Delay_Ms(200);
	Send_String("AT+CMGS=\"18395419662\"\r\n");	//接收短信的电话号码   （单片机里面的程序已经更换成客户的手机号）由于资料统一发送，资料里面这个地方没有改
	Delay_Ms(200);
	Send_String(content); // 发送短信内容！
	Delay_Ms(200);
	Send_Char(0x1A);
	Delay_Ms(5000);
}	