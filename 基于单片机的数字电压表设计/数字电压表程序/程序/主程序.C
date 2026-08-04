#include<reg51.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit RS = P3^0;   //LED1602端口定义
sbit RW = P3^1;
sbit EN = P3^2;
sbit KEY = P3^3;	//通道切换按键定义
sbit LED = P3^4;	 //观察运行状态LED



/*ADC0809端口定义*/
sbit ADDA = P2^0;	  //模拟量输入控制端口
sbit ADDB = P2^1;
sbit ADDC = P2^2;
sbit ALE = P2^3; //地址锁存允许信号，输入，高电平有效。
sbit CLK = P2^4; //时钟脉冲输入端。取500khz
sbit ST = P2^5; //脉冲上升沿使0809复位，下降沿启动A/D转换
sbit EOC = P2^6; //	 A／D转换结束信号，输出，当A／D转换结束时，此端输出一个高电平（转换期间一直为低电平）。
sbit OE = P2^7;	//数据输出允许信号，输入，高电平有效。当A／D转换结束时，此端输入一个高电平，才能打开输出三态门，输出数字量。

unsigned char ti[]={'0','1',0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};	//lcd数字数组。
int t;//时间控制标志位

/*------------------------------------------------
 以下是定义LCD函数
------------------------------------------------*/
void DelayUs2x(unsigned int t)
{   
 while(--t);
}

void DelayMs(unsigned char t)
{
     
 while(t--)
 {
     //大致延时1mS
     DelayUs2x(2450);
	 DelayUs2x(2450);
 }
}
/*------------------------------------------------
              判忙函数
------------------------------------------------*/
 bit LCD_Check_Busy(void) 
 { 
P1= 0xFF; 
 RS=0; 
 RW=1; 
 EN=0; 
 _nop_(); 
 EN=1;
 return (bit)(P1 & 0x80);
 }
/*------------------------------------------------
              写入命令函数
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
 while(LCD_Check_Busy()); //忙则等待
 RS=0; 
 RW=0; 
 EN=1; 
P1= com; 
 _nop_(); 
 EN=0;
 }
/*------------------------------------------------
              写入数据函数
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 while(LCD_Check_Busy()); //忙则等待
 RS=1; 
 RW=0; 
 EN=1; 
P1= Data; 
 _nop_();
  _nop_();
   _nop_();
 EN=0;
 }

/*------------------------------------------------
                清屏函数
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              写入字符串函数
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);     //表示第一行
 	}
 else 
 	{      
 	LCD_Write_Com(0xC0 + x);      //表示第二行
 	}        
 while (*s) 
 	{     
 LCD_Write_Data( *s);     
 s ++;     
 	}
 }
/*------------------------------------------------
              写入字符函数
------------------------------------------------*/
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
 {     
 if (y == 0) 
 	{     
 	LCD_Write_Com(0x80 + x);     
 	}    
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);     
 	}        
 LCD_Write_Data( Data);  
 }
/*------------------------------------------------
              初始化函数
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*显示模式设置*/ 
   DelayMs(10); 
   LCD_Write_Com(0x38); 
   DelayMs(10); 
   LCD_Write_Com(0x38); 
   DelayMs(10); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*显示关闭*/ 
   LCD_Write_Com(0x01);    /*显示清屏*/ 
   LCD_Write_Com(0x06);    /*显示光标移动设置*/ 
   DelayMs(10); 
   LCD_Write_Com(0x0C);    /*显示开及光标设置*/
   }


 void display(uint temp)			//显示程序
{
   int A1,A2,A3;
   A1=temp/100;	//十位	假如是820,821/100=8.21，取整数部分就为8
   A2=temp%100/10;//个位 821除以100的余数等于21,20除以10的整数部分为2
   A3=temp%100%10; //小数  821除以100余数81.除以10余数。1

   	 LCD_Write_Char(4,0,ti[A1]);
	 LCD_Write_Char(5,0,ti[A2]);
     LCD_Write_Char(6,0,'.');
	 LCD_Write_Char(7,0,ti[A3]);
	 LCD_Write_Char(9,0,'V');


   }


/*------------------------------------------------
 以下是ADC0809函数
------------------------------------------------*/

  void ADbegin()
  {
     ST=0;
  	  _nop_();
   _nop_();
  	 ST=1;
  	  _nop_();
   _nop_();
  	 ST=0;
  	
 
  }


void  t0init()   //T0定时器初始化
{
	TMOD=0x02;
	TH0=(256-200)/256;
	TL0=(256-200)%256;
	ET0=1;
	TR0=1;
	EA=1;
	DelayMs(2);
}


float  Read_AD()  //读ADC0809数据
{
	    float temp=0; 	//存放AD采集的数据
    	while(EOC==0);	//等待转换完成
	 	OE=1;
		temp=P0;      //读取数据
		DelayMs(1);
		return temp;
}


void  keyscan()
{ 


		  ALE=1;  //允许写入地址
		  ADDA=0;
		  ADDB=0;
		  ADDC=0;
	      ALE=0;
	 
}






 float v;
 float q;
void main()	

{
 
  t0init() ; //定时器初始化
 
  LCD_Init(); //液晶初始化
  LCD_Clear();//清屏
   keyscan();//选择端口

 while(1)
 {
  // if(t>2500)	//间隔半秒检测一次
  {	 
   // LED=~LED;
   //	t=0;
    
	 ADbegin();
   v=(Read_AD()/256)*50;
   q=v+10*v	;
   display(q); 
   }   
    DelayMs(500); 
 }
 
 }	

 void timer0() interrupt 1    //中断处理函数，产生约500KHz时钟供0809工作
{
	
	CLK=~CLK;
 	t++;

}












