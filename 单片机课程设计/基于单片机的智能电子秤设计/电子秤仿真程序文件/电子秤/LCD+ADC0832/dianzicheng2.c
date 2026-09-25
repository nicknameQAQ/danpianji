#include<reg51.h>
#include<intrins.h>
#include <absacc.h>
#include <math.h>

#define uchar unsigned char
#define uint   unsigned int
#define BUSY  0x80                               //常量定义
#define DATAPORT P0

sbit ADCS =P3^5; 
sbit ADDI =P3^7;  
sbit ADDO =P3^7;  
sbit ADCLK =P3^6;  

sbit LCM_RS=P2^0;
sbit LCM_RW=P2^1;
sbit LCM_EN=P2^2;
uint x1,y1,z1=0,w1,temp1;
uchar ad_data,k,n,m,e,num,s;                              //采样值存储
sbit beep =P3^0;                         
                   
char press_data;                                  //标度变换存储单元
unsigned char ad_alarm;                           //报警值存储单元
unsigned char press_ge=0;                        //显示值百位
unsigned char press_shifen=0;                        //显示值十位
unsigned char press_baifen=0;                         //显示值个位
unsigned char press_qianfen=0;                        //显示值十分位

uchar code str0[]={"Weight:  .   Kg "};
uchar code str2[]={"Price:          "};
uchar code str3[]={"Total:             "};
uchar code table2[]={0x37,0x38,0x39,0xfd,0x34,0x35,0x36,0x78,0x31,0x32,0x33,0x2d,0x3d,0x30,0x2e,0x2b}; //键盘码

void delay(uint);
void lcd_wait(void);
void delay_LCM(uint);             											//LCD延时子程序
void initLCM( void);               											//LCD初始化子程序
void lcd_wait(void);              											//LCD检测忙子程序
void WriteCommandLCM(uchar WCLCM,uchar BusyC);                              //写指令到ICM子函数
void WriteDataLCM(uchar WDLCM);                                             //写数据到LCM子函数
void DisplayOneChar(uchar X,uchar Y,uchar DData);                           //显示指定坐标的一个字符子函数
void DisplayListChar(uchar X,uchar Y,uchar code *DData);					 //显示指定坐标的一串字符子函数
void weishu(uint m);  
void weishu1(uint m);           
void display(void); 
uchar Adc0832(unsigned char channel);															
void alarm(void);
void data_pro(void);
/**********main funcation************/
 
void main(void)
{ 
    delay(500);                      //系统延时500ms启动
	//ad_data=0;                       //采样值存储单元初始化为0
   	initLCM( );
        
   	WriteCommandLCM(0x01,1);		    //清显示屏
    DisplayListChar(0,0,str0);
   	DisplayListChar(0,1,str2);
	while(1)
    {
	   ad_data =Adc0832(0);	   //采样值存储单元初始化为0
	   alarm();
	   data_pro();
	   display();
	if(k==1)
     {
      DisplayOneChar((s+7),1,table2[num-1]);
	  x1=m;
	  y1=n;
	  y1=y1*10+x1;
     }
 
    if(k=='*')
   {
     data_pro();
	 WriteCommandLCM(0x01,1);
	 weishu(z1);
	 k=0;
    }
    if(k=='=')
	{  	
	    z1=z1*temp1;
	    WriteCommandLCM(0x01,1);
        DisplayListChar(0,1,str3);
		s=0;
    	weishu1(765);
	    k=0;
	}
	if(k==' ')
	{
	   WriteCommandLCM(0x80+0x40,1);
	   WriteCommandLCM(0x01,1);
	   z1=0; 
	   s=0;     //防止清零时指针后移动
	}
  }
}


/*********延时K*1ms,12.000mhz**********/

void delay(uint k)
{
   uint i,j;
   for(i=0;i<k;i++)
    for(j=0;j<100;j++);
}   
 /**********写指令到ICM子函数************/

void WriteCommandLCM(uchar WCLCM,uchar BusyC)
{
    if(BusyC)
	lcd_wait();
	DATAPORT=WCLCM;
    LCM_RS=0;                   // 选中指令寄存器
    LCM_RW=0;     
	LCM_RW=0;            	// 写模式	
    LCM_EN=1;    
	_nop_();   
	_nop_();
   _nop_();
   _nop_();
   _nop_();
   _nop_();
   _nop_();
    LCM_EN=0;
   
}

 /**********写数据到LCM子函数************/

void WriteDataLCM(uchar WDLCM)
{ 
    lcd_wait( );             //检测忙信号    
	DATAPORT=WDLCM;
    LCM_RS=1;                // 选中数据寄存器
    LCM_RW=0;           	 // 写模式
    LCM_EN=1;
    _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	_nop_();
    LCM_EN=0;
}
/***********lcm内部等待函数*************/

void lcd_wait(void)
{
    DATAPORT=0xff;	     //读LCD前若单片机输出低电平,而读出LCD为高电平,则冲突,Proteus仿真会有显示逻辑黄色
	LCM_EN=1;
    LCM_RS=0;     
	LCM_RW=0; 
    LCM_RW=1;   
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
	_nop_();
    while(DATAPORT&BUSY)  
	{  LCM_EN=0;
	   _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
	   LCM_EN=1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
	_nop_();
	_nop_();
	   }	 
   	LCM_EN=0;	
	
}

/**********LCM初始化子函数***********/

void initLCM( )
{  
    LCM_EN=0; 
	DATAPORT=0;	
	delay(15);
	WriteCommandLCM(0x38,0);    //三次显示模式设置，不检测忙信号
    delay(5); 
    WriteCommandLCM(0x38,0);
    delay(5); 
    WriteCommandLCM(0x38,0);
    delay(5); 

    WriteCommandLCM(0x38,1);    //8bit数据传送，2行显示，5*7字型，检测忙信号
    WriteCommandLCM(0x08,1);    //关闭显示，检测忙信号
    WriteCommandLCM(0x01,1);    //清屏，检测忙信号
    WriteCommandLCM(0x06,1);    //显示光标右移设置，检测忙信号
    WriteCommandLCM(0x0c,1);    //显示屏打开，光标不显示，不闪烁，检测忙信号
    TMOD=0x11;
	EA=1;
    ET1=1;
	TR1=1;
	k=0;
	x1=0;
	y1=0;
	z1=0;
}

/****显示指定坐标的一个字符子函数****/

void DisplayOneChar(uchar X,uchar Y,uchar DData)

{
    Y&=0x01;
    X&=0x0f;
    if(Y)
	X|=0x40;               //若y为1（显示第二行），地址码+0X40
    X|=0x80;                    //指令码为地址码+0X80
    WriteCommandLCM(X,1);
    WriteDataLCM(DData);
}

/*******显示指定坐标的一串字符子函数*****/
 
void DisplayListChar(uchar X,uchar Y,uchar code *DData)
{
    uchar ListLength=0;
    Y&=0x01;
    X&=0x0f;
    while(X<16)
    {
        DisplayOneChar(X,Y,DData[ListLength]);
        ListLength++;
        X++;
    }
}

 /*****************系统显示子函数*****************/

void display(void)
{
   	WriteCommandLCM(0x0c,1);    				//显示屏打开，光标不显示，不闪烁，检测忙信号	
	DisplayListChar(0,0,str0);	
	//DisplayListChar(0,1,str2);	


	DisplayOneChar(8,0,press_ge+0x30);
	DisplayOneChar(10,0,press_shifen+0x30);
	DisplayOneChar(11,0,press_baifen+0x30);
    DisplayOneChar(12,0,press_qianfen+0x30);
	delay(1000);                               //稳定显示
}
/************
读ADC0832函数
************/

//采集并返回
uchar Adc0832(unsigned char channel)     //AD转换，返回结果
{
     uchar i=0;
    uchar j;
    uint dat=0;
    uchar ndat=0;

    if(channel==0)channel=2;
    if(channel==1)channel=3;
    ADDI=1;
    _nop_();
    _nop_();
    ADCS=0;//拉低CS端
    _nop_();
    _nop_();
    ADCLK=1;//拉高CLK端
    _nop_();
    _nop_();
    ADCLK=0;//拉低CLK端,形成下降沿1
    _nop_();
    _nop_();
    ADCLK=1;//拉高CLK端
    ADDI=channel&0x1;
    _nop_();
    _nop_();
    ADCLK=0;//拉低CLK端,形成下降沿2
    _nop_();
    _nop_();
    ADCLK=1;//拉高CLK端
    ADDI=(channel>>1)&0x1;
    _nop_();
    _nop_();
    ADCLK=0;//拉低CLK端,形成下降沿3
    ADDI=1;//控制命令结束 
    _nop_();
    _nop_();
    dat=0;
    for(i=0;i<8;i++)
    {
        dat|=ADDO;//收数据
        ADCLK=1;
        _nop_();
        _nop_();
        ADCLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        dat<<=1;
        if(i==7)dat|=ADDO;
    }  
    for(i=0;i<8;i++)
    {
        j=0;
        j=j|ADDO;//收数据
        ADCLK=1;
        _nop_();
        _nop_();
        ADCLK=0;//形成一次时钟脉冲
        _nop_();
        _nop_();
        j=j<<7;
        ndat=ndat|j;
        if(i<7)ndat>>=1;
    }
    ADCS=1;//拉低CS端
    ADCLK=0;//拉低CLK端
    ADDO=1;//拉高数据端,回到初始状态
    dat<<=8;
    dat|=ndat;
    return(dat);            //return ad k         
}


void data_pro(void)
{
      unsigned int;
      float  press;	   		  

	  if(0<ad_data<256)			              
	 	{			    
		    int vary=ad_data;									
			press=(0.019531*vary);			
														  
			temp1=(int)(press*1000);	                //放大1000倍，便于后面的计算
	   									    	
		    											
			press_ge=temp1/1000;			     	//取压力值百位
			press_shifen=(temp1%1000)/100;		    	//取压力值十位
			press_baifen=((temp1%1000)%100)/10;	    	//取压力值个位
			press_qianfen=((temp1%1000)%100)%10;			//取压力值十分位

	   	   }	    
			 
}
/*****************报警子函数*******************/

void alarm(void)
{
	 if(ad_data>=256)                        

           beep=0;          //则启动报警
        
	  else
	       beep=1; 		  
}
void weishu(uint m)
{  
   uchar wei4,wei3,wei2,wei1,wei0;
   wei4=m/10000;
   wei3=m%10000/1000;
   wei2=m%1000/100;
   wei1=m%100/10;
   wei0=m%10;
   DisplayOneChar(7,1,0x30+wei4);
   DisplayOneChar(8,1,0x30+wei3);	
   DisplayOneChar(10,1,0x30+wei2);
   	//DisplayOneChar(10,1,'.');
   DisplayOneChar(11,1,0x30+wei1);
   DisplayOneChar(12,1,0x30+wei0);

}
void weishu1(uint m)
{  
   uchar wei5,wei4,wei3,wei2,wei1,wei0;
   wei5=m/100000;
   wei4=m%100000/10000;
   wei3=m%10000/1000;
   wei2=m%1000/100;
   wei1=m%100/10;
   wei0=m%10;
   DisplayOneChar(7,1,0x30+wei4);
   DisplayOneChar(8,1,0x30+wei3);
   DisplayOneChar(9,1,'.');	
   DisplayOneChar(10,1,0x30+wei2);
   //DisplayOneChar(10,1,'.');
   DisplayOneChar(11,1,0x30+wei1);
   DisplayOneChar(12,1,0x30+wei0);

}
 void temer1() interrupt 3
{ uchar temp;
  EX1=0;
  P1=0xfe;
  temp=P1;
  temp=temp&0xf0;
  if(temp!=0xf0)
  {
    delay(5);
	temp=P1;
    temp=temp&0xf0;
	while(temp!=0xf0)
	{
	  temp=P1;
	  switch(temp)
	  {
	    case 0xee:num=1;n=7;k=1,s++;break;
	    case 0xde:num=2;n=8;k=1,s++;break;
	    case 0xbe:num=3;n=9;k=1,s++;break;
	    case 0x7e:num=4;k='/',s++;break;
	  }
	  while(temp!=0xf0)
	  {
	   	temp=P1;
        temp=temp&0xf0;
	  }
	//DisplayOneChar((s+6),1,table2[num-1]);
	}
  } 
  P1=0xfd;
  temp=P1;
  temp=temp&0xf0;
  if(temp!=0xf0)
  {
    delay(5);
	temp=P1;
    temp=temp&0xf0;
	while(temp!=0xf0)
	{
	  temp=P1;
	  switch(temp)
	  {
	    case 0xed:num=5;n=4;k=1;s++;break;
	    case 0xdd:num=6;n=5;k=1;s++;break;
	    case 0xbd:num=7;n=6;k=1;s++;break;
	    case 0x7d:num=8;k='*';s++;break;
	  }
	  while(temp!=0xf0)
	  {
	   	temp=P1;
        temp=temp&0xf0;
	  }
	//DisplayOneChar(k+6,1,table2[num-1]);
	}
  } 
  P1=0xfb;
  temp=P1;
  temp=temp&0xf0;
  if(temp!=0xf0)
  {
    delay(5);
	temp=P1;
    temp=temp&0xf0;
	while(temp!=0xf0)
	{
	  temp=P1;
	  switch(temp)
	  {
	    case 0xeb:num=9;n=1;k=1;s++;break;
	    case 0xdb:num=10;n=2;k=1;s++;break;
	    case 0xbb:num=11;n=3;k=1;s++;break;
	    case 0x7b:num=12;k='-';s++;break;
	  }
	  while(temp!=0xf0)
	  {
	   	temp=P1;
        temp=temp&0xf0;
	  }
//	DisplayOneChar(k+6,1,table2[num-1]);

	}
  } 
  P1=0xf7;
  temp=P1;
  temp=temp&0xf0;
  if(temp!=0xf0)
  {
    delay(5);
	temp=P1;
    temp=temp&0xf0;
	while(temp!=0xf0)
	{
	  temp=P1;
	  switch(temp)
	  {
	    case 0xe7:num=13;k=' ';break;
	    case 0xd7:num=14;n=0;k=1;s++;;break;
	    case 0xb7:num=15;k='=';s++;break;
	    case 0x77:num=16;k='+';s++;;break;
	  }
	  while(temp!=0xf0)
	  {
	   	temp=P1;
        temp=temp&0xf0;
	  }
	//DisplayOneChar(k+6,1,table2[num-1]);
	 }
   
  } 
  EX1=1;
 }

