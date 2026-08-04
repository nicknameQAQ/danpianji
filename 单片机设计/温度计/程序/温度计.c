#include<reg51.h>
#include<intrins.h>	     //shiyongyanshi
#include<math.h>									   
#define uchar unsigned char
#define uint unsigned int
sbit DQ=P3^3;
 
sbit RS =P2^0;
sbit RW =P2^1;
sbit EN =P2^2;
sbit k=P1^0;
sbit  k1=P1^4;
sbit  k2=P1^5;
sbit  k3=P1^6;
sbit  k4=P1^7;

sbit led_red=P2^5;
sbit led_blue=P2^6;

sbit BEEP=P3^7;

uchar bz=1;
uchar ng=0;  //fuhaobiaoshiwei  
uchar TempBuffer[] ={"TEMP:           "};
int temp_value;      //温度值
 
uchar code  dis_title[]={"--current temp--"};
 
void xianshi_huashi();

uchar gw=40;
char dw= 10;
uchar xianshi_title[]={"  TEMP ALARM     "};
uchar xianshi_baojing[]={"HI:    LO:    "};	  //345   10 `` 12

//----------------延时--------------------------------
 void delayxus(uint x)
 {
  uchar i;
  while(x--)  for(i=0;i<200;i++);
 }
//*********************LCD  控制********************
//读lcd 状态
uchar read_lcd_state()
{
  uchar state ;
  RS=0;RW=1;EN=1;delayxus(1);state=P0;EN=0;delayxus(1);
  return state;
}											                                 
//  忙等待
void lcd_busy_wait()
{
 	while((read_lcd_state()&0x80)==0x80);
	delayxus(5);
}
//向LCD写数据
 void write_lcd_data(uchar dat)
 {
   lcd_busy_wait();
   RS=1;RW=0;EN=0;P0=dat;EN=1;delayxus(1);EN=0;
 }
//向LCD写指令
 void write_lcd_cmd(uchar cmd)
 { lcd_busy_wait();
   RS=0;RW=0;EN=0;P0=cmd;EN=1;delayxus(1);EN=0;
 }
//-----------------------------------------------------------------------------------------------------
//LCD初始化
void init_lcd()
{
   write_lcd_cmd(0x38);	 delayxus(1);
   write_lcd_cmd(0x01) ; delayxus(1);
   write_lcd_cmd(0x06);	 delayxus(1);
   write_lcd_cmd(0x0C) ;  delayxus(1);
} 

 //-----------------------------------------------
 //设置液晶显示位置
 void set_lcd_pos(uchar p)
 {
  write_lcd_cmd(p| 0x80);
}
  //---------------------------------------
  //在LCD上显示字符串
  void dis_lcd_string(uchar p,uchar *s)	 //位置,字符指针
  {
   uchar i;
    set_lcd_pos(p);
	for(i=0;i<16;i++)      //16*2
	{
	  write_lcd_data(s[i]) ;
	  delayxus(1);
	}
  }
 //-------------------------------------------------------------------
 void delay_18B20(unsigned int i)
{
	while(i--);
}
//--------------------蜂鸣器---------------------------
 void beep()
   {
   	  uchar i;
	  for(i=0;i<100;i++)
	  {
	  	delayxus(1);
		BEEP=~BEEP;
	  }
   	 BEEP=1;
   }
//延时2
void delay2(uint x)
{
while(--x);
}
 //==================================================
 //初始化DS!*
 /**********ds18b20初始化函数**********************/
void Init_DS18B20(void) 
{
	 unsigned char x=0;
	 DQ = 1;          //DQ复位
	 delay_18B20(8);  //稍做延时
	 DQ = 0;          //单片机将DQ拉低
	 delay_18B20(80); //精确延时 大于 480us
	 DQ = 1;          //拉高总线
	 delay_18B20(14);
	 x=DQ;            //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	 delay_18B20(20);
}
/***********ds18b20读一个字节**************/  
 uchar ReadOneChar(void)
{
	uchar i=0;
	uchar dat = 0;
	for (i=8;i>0;i--)
	 {
		  DQ = 0; // 给脉冲信号
		  dat>>=1;
		  DQ = 1; // 给脉冲信号
		  if(DQ)
		  dat|=0x80;
		  delay_18B20(4);
	 }
 	return(dat);
}

/*************ds18b20写一个字节****************/  
void WriteOneChar(uchar dat)
{
 	unsigned char i=0;
 	for (i=8; i>0; i--)
 	{
  		DQ = 0;
 		DQ = dat&0x01;
    	delay_18B20(5);
 		DQ = 1;
    	dat>>=1;
 }
}
/**************读取ds18b20当前温度************/
void ReadTemp(void)
{
	unsigned char a=0;
	unsigned char b=0;
	unsigned char t=0;

	Init_DS18B20();
	WriteOneChar(0xCC);    	// 跳过读序号列号的操作
	WriteOneChar(0x44); 	// 启动温度转换

	delay_18B20(100);       // this message is wery important

	Init_DS18B20();
	WriteOneChar(0xCC); 	//跳过读序号列号的操作
	WriteOneChar(0xBE); 	//读取温度寄存器等（共可读9个寄存器） 前两个就是温度

	delay_18B20(100);

	a=ReadOneChar();    	//读取温度值低位 aaaaaaaaaaaaaaaaaaaa
	b=ReadOneChar();   		//读取温度值高位  bbbbbbbbbb   b.a
  temp_value= b<<4; 
 temp_value+=(a&0xf0)>>4; 
}
void temp_to_str()   //温度数据转换成液晶字符显示
{ 	 
   if((temp_value   &  0xE0)==0xE0)
	{
	 temp_value =~temp_value +1 ; 
	 ng=1;
	}
	else
	ng=0;
	 
	 if(k1==0) {//while(k1==0);
	  bz=(bz+1)  %2;	   }
	   if(bz==0)
	  {   
      temp_value =(int)(temp_value )*(9.0/5.0)+32;
	  }
   
	if(ng==1)
	{
		 TempBuffer[9]=temp_value/100+'0';
		   if(TempBuffer[9]= ' ') 
	        TempBuffer[9]='-'; 

 	 TempBuffer[10]=temp_value%100/10+'0';  //十位
		  
 	 TempBuffer[11]=temp_value%10+'0';  //个位
	  TempBuffer[12]=0xdf;              //温度符号
	  TempBuffer[13]='C';
 	 TempBuffer[14]='\0';	
 	//TempBuffer[1]=ng %10+ '\0'; 
	}
if(ng==0)
	{  
	 TempBuffer[9]=temp_value/100+'0';
	  if(temp_value/100==0)  TempBuffer[9] =' ';

     TempBuffer[10]=temp_value % 100/10+'0';  //十位
    	if(temp_value % 100/10==0) TempBuffer[10] =' ';

          TempBuffer[11]=temp_value%10+'0';  //个位
	 // if(temp_value%10==0) TempBuffer[11] =' ';

  TempBuffer[13]='C'; //温度符号
  if(bz==0)		  TempBuffer[13]='F';   
  TempBuffer[14]='\0';	
   //TempBuffer[1]=ng%10+'\0'; 
   }
}
void Delay1ms(unsigned int count)
{
	unsigned int i,j;
	for(i=0;i<count;i++)
	for(j=0;j<120;j++);
}

/*延时子程序*/
void mdelay(uint delay)
{	uint i;
 	for(;delay>0;delay--)
   		{
for(i=0;i<62;i++) ;      //1ms延时.
   		}
}
void show_time()   //液晶显示程序
{ 
  ReadTemp(); 
                  //开启温度采集程序
  temp_to_str();                 //温度数据转换成液晶字符
 dis_lcd_string(0x40,TempBuffer);            //显示温度
  dis_lcd_string(0x00,dis_title);  //
   
  Delay1ms(400);                 //扫描延时
}
void xianshi_dw()
{
  if(dw>=0)
  { xianshi_baojing[3]=gw/100+'0';
   xianshi_baojing[4]=gw%100/10+'0';
   xianshi_baojing[5]=gw%10+'0';

    xianshi_baojing[10]=dw/100+'0';
   xianshi_baojing[11]=dw%100/10+'0';
   xianshi_baojing[12]=dw%10+'0';

	dis_lcd_string(0x00,xianshi_title);
	dis_lcd_string(0x40,xianshi_baojing);
	}
	else
	if(dw<0)
	{
	  xianshi_baojing[3]=gw/100+'0';
   xianshi_baojing[4]=gw%100/10+'0';
   xianshi_baojing[5]=gw%10+'0';

    xianshi_baojing[10]= '-';
   xianshi_baojing[11]=abs(dw)%100/10+'0';
   xianshi_baojing[12]=abs(dw)%10+'0';

	dis_lcd_string(0x00,xianshi_title);
	dis_lcd_string(0x40,xianshi_baojing);
	}
}
 
void main()
{

led_red=0;
led_blue=0;
 BEEP=0;
 delayxus(10);  
 init_lcd();
//init_DS();
Init_DS18B20();
 delay2(500);
 delay2(500);
 while(1)
 { 
  	 if(  k2==1 &&k3==1 && k4==1)
		show_time();
	    }
 	 if(k2==0)
 	 {
	  while(k2==0);
 	  xianshi_dw();
   
 	 }
  	   
  	if(k3==0 && k2==0 &&k==1)
  	{	 while(k3==0);
  		gw++;
    }  	  
 	 if(k4==0 && k2==0 &&k==1)
 	 {
 	 while(k4==0);
  	dw++;
     }
		 
	  if(k3==0 && k2==0 &&k==0)
	  {	 
while(k3==0);
 	 	  gw--;
 }	  
  	if(k4==0 && k2==0 &&k==0)
	  {
	  while(k4==0);
 	 dw--;	
	 
	 if(dw<0) dw=0;
	  	/*	if(dw<0)
			{
			  xianshi_baojing[10]='-';
	        }         */
 	 }  
  
 	 delayxus(100);

   //报警设置
   if(bz==1)
  {	if(temp_value>gw  &&bz==1)
	 { beep(); delayxus(50);led_red=1;}
	 else
	 led_red=0;
 		 
 	if( temp_value<dw  &&bz==1) 
        { beep(); delayxus(50);led_blue=1;}
		else
		led_blue=0;

   if( (TempBuffer[9]=='-')&& abs( temp_value)>=abs(dw) && bz==1 )
 	 { beep(); delayxus(50);}
  if(  dw<=0 && abs( temp_value)>=abs(dw) &&bz==1 )
 	 { beep(); delayxus(50);}		 
  if((-abs(temp_value))<=(-abs(dw)) && dw<=0 &&bz==1) 
  
 		 { beep(); delayxus(50);}
	}
 }
                          
