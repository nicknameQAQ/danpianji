/********************************************************
                   压力测试仪
系统描述;输入  15--115kPA压力信号
         输出  00h--ffh数字信号（adc0832）
         在LCD上显示实际的压力值，如果超限则报警

线性区间标度变换公式：    y=(115-15)/(243-13)*X+15kpa   

作者：
单位：
日期：2008.3.7

********************************************************/

#include<reg51.h>
#include<intrins.h>
#include <absacc.h>
#include <math.h>

#define uchar unsigned char
#define uint   unsigned int
#define BUSY  0x80                               //常量定义
#define DATAPORT P0


//ADC0832的引脚
sbit ADCS =P3^5;  //ADC0832 chip seclect
sbit ADDI =P3^7;  //ADC0832 k in
sbit ADDO =P3^7;  //ADC0832 k out
sbit ADCLK =P3^6;  //ADC0832 clock signal

sbit LCM_RS=P2^0;
sbit LCM_RW=P2^1;
sbit LCM_EN=P2^2;

uchar ad_data;                                    //采样值存储
sbit Alarm_led_red =P3^1;                         //超过压力表量程最大值红色led报警定义
sbit Alarm_led_green=P3^2;                        //低于压力表量程最小值绿色led报警定义
                          //adc采样值存储单元
char press_data;                                  //标度变换存储单元
unsigned char ad_alarm;                           //报警值存储单元
unsigned char press_ge=0;                        //显示值百位
unsigned char press_shifen=0;                        //显示值十位
unsigned char press_baifen=0;                         //显示值个位
unsigned char press_qianfen=0;                        //显示值十分位

uchar code str0[]={"Weight:  .   Kg "};
uchar code str1[]={"Make by GUO TAO "};
uchar code str2[]={"Price:          "};
uchar code str3[]={"Total:          "};
void delay(uint);
void lcd_wait(void);
void delay_LCM(uint);             											//LCD延时子程序
void initLCM( void);               											//LCD初始化子程序
void lcd_wait(void);              											//LCD检测忙子程序
void WriteCommandLCM(uchar WCLCM,uchar BusyC);                              //写指令到ICM子函数
void WriteDataLCM(uchar WDLCM);                                             //写数据到LCM子函数
void DisplayOneChar(uchar X,uchar Y,uchar DData);                           //显示指定坐标的一个字符子函数
void DisplayListChar(uchar X,uchar Y,uchar code *DData);					 //显示指定坐标的一串字符子函数
  void keyscan();                   
void display(void);                                                         //系统显示子函数
uchar Adc0832(unsigned char channel);															
void alarm(void);
void data_pro(void);


/**********main funcation************/

void main(void)
{ 
    delay(500);                      //系统延时500ms启动
//	ad_data=0;                       //采样值存储单元初始化为0
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
				 
	}
}


/*********延时K*1ms,12.000mhz**********/

void delay(uint k)
{
    uint i,j;
    for(i=0;i<k;i++)
    {
        for(j=0;j<60;j++)
        	{;}
    }
}   
/**********写指令到ICM子函数************/

void WriteCommandLCM(uchar WCLCM,uchar BusyC)
{
    if(BusyC)lcd_wait();
	DATAPORT=WCLCM;
    LCM_RS=0;                   // 选中指令寄存器
    LCM_RW=0;               	// 写模式	
    LCM_EN=1;    
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
    LCM_EN=0;
}

/***********lcm内部等待函数*************/

void lcd_wait(void)
{
    DATAPORT=0xff;	     //读LCD前若单片机输出低电平,而读出LCD为高电平,则冲突,Proteus仿真会有显示逻辑黄色
	LCM_EN=1;
    LCM_RS=0;   
    LCM_RW=1;   
    _nop_();
    _nop_();
	_nop_();
    while(DATAPORT&BUSY)  
	{  LCM_EN=0;
	   _nop_();
	   _nop_();
	   LCM_EN=1;
	   _nop_();
	   _nop_();
	   }	 
   	LCM_EN=0;	
	
}

/**********LCM初始化子函数***********/

void initLCM( )
{   
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
}

/****显示指定坐标的一个字符子函数****/

void DisplayOneChar(uchar X,uchar Y,uchar DData)

{
   Y&=0x01;
    X&=0x0f;
    if(Y)X|=0x40;               //若y为1（显示第二行），地址码+0X40
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
	DisplayListChar(0,1,str2);	


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
      unsigned int temp;
      float  press;	   		  

	  if(14<ad_data<243)			               //当压力值介于15kpa到115kpa之间时，遵循线性变换
	 	{			    
		    int vary=ad_data;						//y=(115-15)/(243-13)*X+15kpa			
			press=((10.0/23.0)*vary)+9.3;			//测试时补偿值为9.3
														  
			temp=(int)(press*10);	                //放大10倍，便于后面的计算
	   									    	
		    											
			press_ge=temp/1000;			     	//取压力值百位
			press_shifen=(temp%1000)/100;		    	//取压力值十位
			press_baifen=((temp%1000)%100)/10;	    	//取压力值个位
			press_qianfen=((temp%1000)%100)%10;			//取压力值十分位

	   	   }	    
			 
}
/*****************报警子函数*******************/

void alarm(void)
{
	 if(ad_data>=243)                           //如果当前压力值大于115kpa，

          {  Alarm_led_red=0; }                 //则启动red led报警
        
	  else
	     {	  Alarm_led_red=1; }		        //关闭red led 报警
		   
	
	  if(ad_data<=14)							//如果当前压力值小于16kpa

	     {  Alarm_led_green=0;  }               //则启动green led报警

      else
	     {   Alarm_led_green=1; }      			//关闭green  led 报警
         
}



