/*****************************************************************************
程序名：	GSM接听电话                             
*****************************************************************************/

#include <reg52.h>
#include "UART.h"
#include "Delay.h"
#include "LCD1602.h"
#include "FMQ.h"
#include "28BYJ.h"
#define Yes		1
#define No		0


sbit Power_key=P2^7;
sbit led=P2^ 6;

/********测试GSM是否启动**********/
int test_boot;
/********测试GSM是否注册网络**********/
int test_net_register;
/********测试是否有电话打入**********/
int test_have_call;
/********GSM串口接收数据缓存**********/
unsigned char GSM_receive[60];
/********GSM串口接收计数器**********/
unsigned char GSMDATA_count;
/********号码缓存*******************/
unsigned char number_get;
unsigned char door;
/********短信发送程序********/

void message()
{
	Send_String("AT+CMGF=1\r\n");//指定信息的输入输出格式为文本格式
	Delay_Ms(100);
	
	Send_String("AT+CSMP=17,167,2,25\r\n");
	Delay_Ms(100);
	
	Send_String("AT+CSCS=\"UCS2\"\r\n");
	Delay_Ms(100);
	
	Send_String("AT+CMGS=\"00310033003200300038003000330038003000300035\"\r\n");
	Delay_Ms(100);
	
	Send_String("4E3B94F6007E95E85F004E86007E\r\n");
	Delay_Ms(100);
	Send_Char(0x1A);
	Delay_Ms(200);
}

void main()
{
	unsigned char m;
	//初始化状态
	Power_key=0;
	test_boot=No;
	test_net_register=No;
	test_have_call=No;
	door=No;
	fmq=1;
	P2=0x00;
	number_get=No;
	//GSM启动
	Power_key=1;
	Delay_Ms(1000);
	Power_key=0;
	
	Uart_Init();				//串口初始化
	LcdInitiate();
	Print_String(line_1,0x00,"calling test--HM");
		//测试是否已经开机
		while(test_boot==No)
		{
			Send_String("AT\r\n");
			Delay_Ms(1000);
		}
		//测试是否注册到网络
		while(test_net_register==No)
		{
			Send_String("AT+COPS?\r\n");
			Delay_Ms(1000);	
		}
		//等待电话打入
		Send_String("AT+CLIP=1\r\n");
		Delay_Ms(100);
		while(1)
		{
			while(test_have_call==Yes)
			{
				if(number[10]=='5'&&number[9]=='0'&&number[8]=='0'&&number[7]=='8')
				{
//					if(door==No)
//					{
						for(m=0;m<200;m++)
						{
							forward();
							
						}
						Print_String(line_2,0x0B,"open!");
						FMQ_0();
						Delay_Ms(100);
						FMQ_1();
						message();					

						//						door=Yes;
//					}	
				}
				else
				{
					Print_String(line_2,0x0B,"close");	
					led=1;
				}				
			}
			
		}	
		
}

/*****************************************************************************
串口中断
/****************************************************************************/
void uart(void) interrupt 4
{
	unsigned char UART_data;
	unsigned char i;

	if(RI)
	{
		UART_data=SBUF;
		if(UART_data=='\n')
		{
			if(GSM_receive[0]=='O'&&GSM_receive[1]=='K')
				test_boot=Yes;
			else
				test_boot=No;
			
			if(GSM_receive[12]=='C')
				test_net_register=Yes;
			else
				test_net_register=No;
			
			if(GSM_receive[0]=='R'&&GSM_receive[1]=='I'&&GSM_receive[2]=='N'&&GSM_receive[3]=='G')
			{
				test_have_call=Yes;				
			}
			else
			{
				test_have_call=No;
			}
			
			if(GSM_receive[0]=='+'&&GSM_receive[1]=='C'&&GSM_receive[2]=='L'&&GSM_receive[3]=='I'&&GSM_receive[4]=='P')
			{
				for(i=0;i<11;i++)
				{
					number[i]=GSM_receive[i+8];
				}
				display_number(line_2,0x00);	
				
				
			}
			GSMDATA_count=0;
		}
		else
		{
			GSM_receive[GSMDATA_count]=UART_data;
			GSMDATA_count++;
		}
	}
	RI=0;
}

