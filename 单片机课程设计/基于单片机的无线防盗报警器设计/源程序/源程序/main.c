#include<reg52.h>
#include<intrins.h>
#include "sim800.h"	
#define uchar unsigned char
#define uint  unsigned int
#define K_MG_MV    120/66 
typedef unsigned char  U8;       /* defined for unsigned 8-bits integer variable 	  无符号8位整型变量  */
typedef unsigned int   U16;      /* defined for unsigned 16-bits integer variable 	  无符号16位整型变量 */

/********测试GSM是否启动**********/
int test_boot;
/********测试GSM是否注册网络**********/
int test_net_register;
/********GSM串口接收数据缓存**********/
unsigned char idata GSM_receive[60];
/********GSM串口接收计数器**********/
unsigned char  GSMDATA_count;
unsigned char *content_temp =  "Warning: over temperature\r\n";
unsigned char *content_smoke = "Warning: excessive smoke concentration\r\n";
unsigned char *content_body =  "Warning: body entry\r\n";

unsigned char str_yw[12];//烟雾值
unsigned char T_baojing = 30;  //温度报警值
unsigned int  C_baojing = 500; //烟雾浓度报警值

unsigned char T_buff[3];  //报警缓存
unsigned char H_buff[3];
unsigned char C_buff[4];
bit sf_flag_buff=0; //
bit  sf_flag=0; //设防标志
unsigned char moshi=0;
unsigned char data1;
long Value;

sbit bADcs=P3^4;
sbit bADcl=P3^6;
sbit bADda=P3^5;

sbit  sf_led = P1^3;//   设防指示灯
sbit  shefang= P1^4;//   设防按键
sbit  shezhi = P1^5; //  设置
sbit  jia    = P1^6; //	 加
sbit  jian   = P1^7; //  减	
sbit  BUZZER = P2^0; //  LED蜂鸣器
sbit  Infra	 = P3^2; //  人体红外模块


unsigned char IntToString(unsigned char *str, int dat);
extern bit Start18B20();
extern bit Get18B20Temp(int *temp);				//	  ；；；；；、‘       
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
float intT, decT;  //温度值的整数和小数部分

 /*------------------------------------------------
                    定时器初始化子程序
------------------------------------------------*/
void Init_Timer0(void)
{
 TMOD |= 0x01;	  //使用模式1，16位定时器，使用"|"符号可以在使用多个定时器时不受影响		     
 TH0=(65536-1000)/256;		  //重新赋值 1ms
 TL0=(65536-1000)%256;
 EA=1;            //总中断打开
 ET0=1;           //定时器中断打开
 TR0=1;           //定时器开关打开
 PT0=1;           //优先级打开
}
long kssj()   //开始收集
 {
   unsigned char i;

     bADcs = 0;//当ADC0832未工作时其CS输入端应为高电平，此时芯片禁用.
	 bADcl=0;
	 bADda=1;
	 bADcl=1;
	 bADcl=0;//i down
	 bADda=1;
	 bADcl=1;
	 bADcl=0;	//   2 down
	 bADda=0;
	 bADcl=1;
	 bADcl=0;	//   3 down
	 bADda=1;
	 bADcl=1;
	 bADcl=0;	//   4 down

	 for(i=8;i>0;i--)
	 	{
		
	 	data1<<=1;
	 	bADcl=0;
		bADcl=1;
		if(bADda==1) data1|=0x01;
		   	bADda=1;
	 	}
		 bADcs=1;
	 	
       Value=data1*1.0/256*500;
	   
	   Value=Value*K_MG_MV;
	   Value=Value-5;
	   if(Value<0)Value=0;
	   return Value;
 }


void Key_set_scan()
{
  	
	if(shezhi==0)
	{
	   Delay_Ms(10);
		if(shezhi==0)
		{  
		   while(!shezhi);
		   InitLcd1602();     //初始化液晶
		   moshi++;
		   if(moshi >= 3)moshi = 0;
		   if(moshi == 0)
		   {
		     LcdShowStr(0, 0,"T:   C");
             LcdShowStr(7, 0,"C:    PPM");
		   }
		   else if(moshi == 1)
		   {
		      LcdShowStr(0, 1,"Set_T:   C");
		   }
		   else if(moshi == 2)
		   {
		   	   LcdShowStr(0, 1,"Set_C:    PPM");
		   }		   	
		}
	}
	
	if(jia==0)
	{
	   Delay_Ms(10);
		if(jia==0)
		{ 	 
		    while(!jia);
			if(moshi==1)
			{
			 T_baojing++ ;
			 if( T_baojing>=99 )T_baojing =99;
			}
			if(moshi==2)
			{
			 C_baojing++ ;
			 if( C_baojing>=999 )C_baojing =999;
			}		
		 }
	 }
	 if(jian == 0)
	 {
	    Delay_Ms(10);
	    if(jian == 0)
	    {
		    while(!jian);
	        if(moshi==1)
			{
			 T_baojing-- ;
			 if( T_baojing<=0 )T_baojing =0;
			}
			if(moshi==2)
			{
			 C_baojing-- ;
			 if( C_baojing<=0 )C_baojing =0;
			}
	   }
	 }
	 if(shefang == 0)
	 {
	   Delay_Ms(10);
	   if(shefang == 0)
	   { 
	     while(!shefang);
		
		 if(sf_flag == 0) //设防
		 {
		    sf_flag_buff = 1;
			sf_led = 0;
		 }		    
			  
		 else 			 //关闭设防
		 { 
		    sf_flag = 0;
			sf_flag_buff = 0;
			sf_led = 1;
		    BUZZER = 1;
		 }

	   }
	 }
		 
}
void wendu()
{ 
  bit res;
  int temp;  //读取到的当前温度值
  
  unsigned char len;
  unsigned char str_wd[5];   //温度缓冲区
            Get18B20Temp(&temp);  //读取当前温度
            res = Get18B20Temp(&temp);  //读取当前温度
            if (res)                    //读取成功时，刷新当前温度显示
            {		
			    intT = temp >> 4;          //分离出温度值整数部分
                decT = temp & 0xF;         //分离出温度值小数部分
                len = IntToString(str_wd, intT); //整数部分转换为字符串
                str_wd[len++] = '.';             //添加小数点
                decT = (decT*10) / 16;        //二进制的小数部分转换为1位十进制位
                str_wd[len++] = decT + '0';   //十进制小数位再转换为ASCII字符
                str_wd[len] = '\0';              //添加字符串结束符
                LcdShowStr(2, 0, str_wd);        //显示到液晶屏上
			 }
    		Start18B20();
}
/************主函数****************/
void main()
{ 
  unsigned char i,a,b,c;
  sf_flag = 0;
  Uart_Init();
  Init_Timer0();	//定时器初始化
  InitLcd1602();     //初始化液晶
  Start18B20();
  
  LcdShowStr(0,0,"  System init   ");
  LcdShowStr(0,1,"  please wait   ");
  for(i = 0;i < 15;i++)//等待网络稳定
	{
		Delay_Ms(1000);
	}
   InitLcd1602();  
  LcdShowStr(0, 0,"T:   C");
  LcdShowStr(7, 0,"C:    PPM");
//		//测试是否已经开机，同时自适应波特率
//	while(test_boot==No)
//	{
//		Send_String("AT\r\n");	
//		Delay_Ms(100);
//	}
//		test_boot=No;
//
//	//测试是否注册到网络
//	while(test_net_register==No)
//	{
//		Send_String("AT+CREG?\r\n");	 
//		Delay_Ms(100);	
//	}
//	test_net_register=No;
	Receive_message();      //配置成接受短信模式
 
 
  while(1)
 {
  Key_set_scan();//按键扫描
 if(moshi == 0)	  //设置模式切换
 {
   kssj();   //开始收集 ,电压采集检测烟雾浓度
   Value =  Value-100;//烟雾浓度校准
   if(Value<0)	Value = 0;
   IntToString(str_yw,Value); //转换成字符串
   LcdShowStr(9, 0, str_yw); //烟雾浓度显示到液晶屏上
   wendu();	 //温度检测并显示
   if(sf_flag == 1) //当开启设防时
   {
      if(Value>=C_baojing||intT>=T_baojing||Infra==1)   //有触发报警
     { 
   	   BUZZER = 0;
	   Delay_Ms(1000);
	   LcdShowStr(0, 1,"                ");												     
       if(intT >= T_baojing)
       { 			  /*1234567890123456*/
        LcdShowStr(0,1,"temp");
		if(a==0)
		{  
		   a=1;
		   Send_message(content_temp);	  //发温度超标短信
		}
   	    
       }  else  LcdShowStr(0, 1,"    ");	
       if(Value >= C_baojing)     
       { 				/*1234567890123456*/
         LcdShowStr(5,1,"smoke"); 
		 if(b == 0)
		 {
		   b=1;
		   Send_message(content_smoke);	  //发烟雾浓度超标短信
		 } 
   	     
       }  else  LcdShowStr(5, 1,"     ");
       if(Infra == 1)        
       { 			    /*1234567890123456*/
         LcdShowStr(11, 1,"body"); 
		 if(c == 0) 
		 {
		   c=1;
		   Send_message(content_body);	  //发人体进入短信
		 }
   	    
       }  else  LcdShowStr(11,1,"    ");

      }
     else 	//没有触发报警
       {
	      BUZZER = 1;	 /*1234567890123456*/
	      LcdShowStr(0, 1," System normal  ");
       }
	 }
   }
   else if(moshi == 1)	//设置温度上限模式
   {	
   T_buff[0] = T_baojing/10+0x30;
   T_buff[1] = T_baojing%10+0x30;
   T_buff[2] = '\0';
   LcdShowStr(6, 1,T_buff);
   }
   else if(moshi == 2)	 //设置烟雾浓度上限模式
   {
   C_buff[0] = C_baojing/100+0x30;
   C_buff[1] = C_baojing%100/10+0x30;
   C_buff[2] = C_baojing%10+0x30;
   C_buff[4] = '\0';
   LcdShowStr(6, 1,C_buff);
   }
				                                                                           
 }
}
/* 整型数转换为字符串，str-字符串指针，dat-待转换数，返回值-字符串长度 */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //如果为负数，首先
	
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //先转换为低位在前的十进制数组
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i最后的值就是有效字符的个数
    while (i-- > 0)   //将数组值转换为ASCII码反向拷贝到接收指针上
    {
        *str++ = buf[i] + '0';
    }

    *str = '\0';  //添加字符串结束符
    
    return len;   //返回字符串长度
}
/*****************************************************************************
串口中断
/****************************************************************************/
void uart(void) interrupt 4
{
unsigned char UART_data;
	if(RI)
	{
		UART_data=SBUF;
		if(UART_data=='\n')
		{
			if(GSM_receive[0]=='O'&&GSM_receive[1]=='K')
			{
			 	test_boot=Yes;
			}
		    //else
			//	test_boot=No;
		    if(GSM_receive[0]=='+'&&GSM_receive[1]=='C'&&GSM_receive[2]=='R'&&GSM_receive[3]=='E'&&GSM_receive[4]=='G')
		    {
		 	   if(GSM_receive[7]=='1'||GSM_receive[9]=='1'||GSM_receive[9]=='5')
			   {
			 	test_net_register=Yes;
			   }
		    }		   
		    //	else
		    //  test_net_register=No;

			if((GSM_receive[0]=='o')&&(GSM_receive[1]=='n'))	  //
		      {	
			    sf_flag_buff=1;  			 
				sf_led = 0;	  
			  }
			if((GSM_receive[0]=='o')&&(GSM_receive[1]=='f')&&(GSM_receive[2]=='f'))
		      {	
			    sf_flag = 0;
				sf_flag_buff =0;
			    sf_led = 1;
		        BUZZER = 1;
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
/*------------------------------------------------
    定时器中断子程序（定时1ms）
------------------------------------------------*/
void Timer0_isr(void) interrupt 1 
{ 
  static unsigned int i,j;
 TH0=(65536-1000)/256;	 //重新赋值 1ms
 TL0=(65536-1000)%256;
 TR0=0;
 if(sf_flag_buff == 1)
 {
    i++;
    if(i>=1000)
  {
     i=0;
	 sf_led =~ sf_led;
	 j++;
	 if(j>=10)
	 {
	   j=0;
	   sf_flag = 1;
	   sf_flag_buff = 0;
	   sf_led = 0;
	 }
	 
  }
 }
 
  TR0=1;
  
}

