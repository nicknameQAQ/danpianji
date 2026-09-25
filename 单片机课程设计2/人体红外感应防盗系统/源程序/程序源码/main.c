


#include "STC15.h"
#include "intrins.h"
#include "ADC.h"
#include "lcd.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

#define FOSC 12000000          //系统频率
#define BAUD 9600             //串口波特率

#define NONE_PARITY     0       //无校验
#define ODD_PARITY      1       //奇校验
#define EVEN_PARITY     2       //偶校验
#define MARK_PARITY     3       //标记校验
#define SPACE_PARITY    4       //空白校验

#define PARITYBIT NONE_PARITY   //定义校验位



//sfr AUXR  = 0x8e;               //辅助寄存器
//sfr T2H   = 0xd6;               //定时器2高8位
//sfr T2L   = 0xd7;               //定时器2低8位

//sfr P_SW1   = 0xA2;             //外设功能切换寄存器1

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

//sbit P22 = P2^2;

bit busy;

unsigned char xdata PuZh[]=" Phone Number   ";
unsigned char xdata csh[]="Initialization...";

void SendData(BYTE dat);
void SendString(char *s);
void init(void);
void Delay500ms();
void debug(unsigned int a);
void senttext(void);
void	ADC_config(void);
void Delay10ms(char shu1);
void show(unsigned char num,unsigned char i);
void senttext2(void);
void senttext3(void);
void senttext4(void);
void cleardisplay(void);
void wbzd();
unsigned char Key_Scan();
unsigned int xdata a1;
unsigned int xdata a11;
unsigned  int xdata a3;
unsigned char xdata i;
unsigned char xdata i2=0;
unsigned int xdata number[11];
unsigned int xdata number2[11];
unsigned char xdata a2;
unsigned char  xdata qpbj=1;
unsigned char xdata zdfh[5];

void main()
{

		zdfh[0]=0;
		P1M1=0x03; P1M0=0xF0; //(00000001B,11110000B)
		init();
		ADC_config();
		LcdInit();
	for(i=0;i<20;i++)
	{
		LcdWriteData(csh[i]);	
	}
	
	for(i=0;i<80;i++)
	{Delay500ms();
	}
			i=0;

	P32=1;
	P36=1;
	zdfh[0]=0;
	LcdWriteCom(0x80);
	for(i=0;i<16;i++)
	{
		LcdWriteData(PuZh[i]);	
	}
	i=0;
    while(1){
			a1 = Get_ADC10bitResult(0);

								if(P32==0)
								{	Delay10ms(1);
									if(P32==0){
									if(zdfh[0])
									senttext4();
									zdfh[0]=0;
									
								}}
								
									if(P11==0)
								{	Delay10ms(1);
									if(P11==0){
									if(zdfh[1])
									senttext2();
									zdfh[1]=0;
									
								}}
								
									if(P36==0)
								{	Delay10ms(1);
									if(P36==0){
									if(zdfh[2])
									senttext3();
									zdfh[2]=0;
								}}
								
								
									if((a1>935)&&a1<990)
									{

										zdfh[0]=1;
										zdfh[1]=1;
										zdfh[2]=1;
									}
			
			
			
					
									if(a1<1000)
							{		a3 = Get_ADC10bitResult(0);
								while((a3 <1000)&&i2<50)
								{a3 = Get_ADC10bitResult(0);
								i2++;
									Delay10ms(1);
									
								}
									i2=0;
									a2=a1/85;
								show(a2,i);
								if(((number[i]<10)&&i<11))
									{ 
									number2[i]=number[i];
									LcdWriteCom(0xC0+i);
									LcdWriteData(number[i]+48);
									i=i+1;}

									if(number[i]==11)
									{
										
											LcdWriteCom(0x01);
											for(i=0;i<=10;i++)
											{
										number[i]=0;	
										}

										cleardisplay();
										
											i=0;
									}
					



							}



		
		
		
		
		
		
		}
	
}

/*----------------------------
UART 中断服务程序
-----------------------------*/
void Uart() interrupt 4 using 1
{
    if (RI)
    {
        RI = 0;                 //清除RI位
        P0 = SBUF;              //P0显示串口数据
        P22 = RB8;              //P2.2显示校验位
    }
    if (TI)
    {
        TI = 0;                 //清除TI位
        busy = 0;               //清忙标志
    }
}

/*----------------------------
发送串口数据
----------------------------*/
void SendData(BYTE dat)
{
    while (busy);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    if (P)                      //根据P来设置校验位
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //设置校验位为0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //设置校验位为1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //设置校验位为1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //设置校验位为0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //写数据到UART数据寄存器
}

/*----------------------------
发送字符串
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}


void init(void)
{
	
	    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
    
//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
//  ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
//  P_SW1 = ACC;  
//  
//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
//  ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
//  P_SW1 = ACC;  

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;                //8位可变波特率
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9位可变波特率,校验位初始为1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;                //9位可变波特率,校验位初始为0
#endif

    T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2
    AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
    ES = 1;                     //使能串口1中断
    EA = 1;
}



void Delay500ms()		//@12.000MHz
{
	unsigned char i, j, k;
	_nop_();
	_nop_();
	i = 23;
	j = 205;
	k = 120;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);


}




void debug(unsigned int a)
{ unsigned b1[3];
				b1[3]=a1/1000;	
				b1[2]=a1%1000/100;
				b1[1]=a1%100/10;
				b1[0]=a1%10;
		SendData(b1[3]+48);
		SendData(b1[2]+48);
		SendData(b1[1]+48);
		SendData(b1[0]+48);
				SendData(0x0D);
		SendData(0x0a);
}



void senttext(void)
{    SendString("AT+CMGF=1");
		SendData(0x0D); 
		Delay500ms();
	   SendString("AT+CMGS=");
		SendData(34);
		SendString("15992420917");
		SendData(34);
		SendData(0x0D);
		Delay500ms();
			Delay500ms();
			Delay500ms();
			SendString("this is a test");
			SendData(0x0D);
			Delay500ms();

			Delay500ms();
			Delay500ms();
//			SendData(0x1A);
}


void senttext2(void)
{    SendString("AT+CMGF=1");
		SendData(0x0D); 
		Delay500ms();
			Delay500ms();
//			Delay500ms();
//			Delay500ms();
	   SendString("AT+CMGS=");
		SendData(34);
//		SendString("15992420917");
												for(i=0;i<=10;i++)
									{
									SendData(number2[i]+48);	
									}
		SendData(34);
		SendData(0x0D);
		Delay500ms();
			Delay500ms();
//			Delay500ms();
			SendString("Warning!The door was open!");
			SendData(0x0D);
			Delay500ms();
			Delay500ms();
			Delay500ms();
			SendData(0x1A);
}

void senttext3(void)
{    SendString("AT+CMGF=1");
		SendData(0x0D); 
		Delay500ms();
			Delay500ms();
//			Delay500ms();
//			Delay500ms();
	   SendString("AT+CMGS=");
		SendData(34);
//		SendString("15992420917");
												for(i=0;i<=10;i++)
									{
									SendData(number2[i]+48);	
									}
		SendData(34);
		SendData(0x0D);
		Delay500ms();
			Delay500ms();
//			Delay500ms();
			SendString("Warning!Gas leakage!");
			SendData(0x0D);
			Delay500ms();
			Delay500ms();
			Delay500ms();
			SendData(0x1A);
}

void senttext4(void)
{    SendString("AT+CMGF=1");
		SendData(0x0D); 
		Delay500ms();
			Delay500ms();
//			Delay500ms();
//			Delay500ms();
	   SendString("AT+CMGS=");
		SendData(34);
//		SendString("15992420917");
												for(i=0;i<=10;i++)
									{
									SendData(number2[i]+48);	
									}
		SendData(34);
		SendData(0x0D);
		Delay500ms();
			Delay500ms();
//			Delay500ms();
			SendString("Warning!Someone invasion!");
			SendData(0x0D);
			Delay500ms();
			Delay500ms();
			Delay500ms();
			SendData(0x1A);
}


void	ADC_config(void)
{
	ADC_InitTypeDef		ADC_InitStructure;				//结构定义
	ADC_InitStructure.ADC_Px        = ADC_P10;	//设置要做ADC的IO,	ADC_P10 ~ ADC_P17(或操作),ADC_P1_All
	ADC_InitStructure.ADC_Speed     = ADC_360T;			//ADC速度			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;			//ADC功率允许/关闭	ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H8L2;		//ADC结果调整,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;		//优先级设置	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;			//中断允许		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);					//初始化
	ADC_PowerControl(ENABLE);							//单独的ADC电源操作函数, ENABLE或DISABLE
}





	

void Delay10ms(char shu1)		//@12.000MHz
{
	unsigned char i, j, kk;

	
for(kk=0;kk<shu1;kk++)
{

	i = 117;
	j = 184;
	do
	{
		while (--j);
	} while (--i);
}


}


void show(unsigned char num,unsigned char i)
{switch (num)
{
	case 0x00:number[i]=3;break;
	case 0x01:number[i]=6;break;
	case 0x02:number[i]=9;break;
	case 0x03:number[i]=11;break;
	case 0x04:number[i]=0;break;
	case 0x05:number[i]=8;break;
	case 0x06:number[i]=5;break;
	case 0x07:number[i]=2;break;
	case 0x08:number[i]=1;break;
	case 0x09:number[i]=4;break;
	case 0x0a:number[i]=7;break;
	case 0x0b:number[i]=10;break;
	default:break;
}
}



void cleardisplay(void)
{	char aaaa;

	LcdWriteCom(0x80);
	for(aaaa=0;aaaa<14;aaaa++)
	{
		LcdWriteData(PuZh[aaaa]);
  		
	}

}


void exint0() interrupt 0       //INT0中断入口
{
//    zdfh[0]=1;                 
//	if(zdfh[0]==1)
	senttext2();
}


void wbzd()
{

    P32 = 1;
    IT0 = 1;                    //设置INT0的中断类型 (1:仅下降沿 0:上升沿和下降沿)
    EX0 = 1;                    //使能INT0中断
    EA = 1;}