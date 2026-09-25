#include <reg52.h>	         //调用单片机头文件
#define uchar unsigned char  //无符号字符型 宏定义	变量范围0~255
#define uint  unsigned int	 //无符号整型 宏定义	变量范围0~65535
#include<DS18N02.h>
#include <intrins.h>
#include "eeprom52.h"


#define DBPort P0		

unsigned char i=0,timecount=0,displayOK=0,rate=0,rate1=0,aa=0,time1=0,flat300ms=0;
unsigned int time[6]={0};
unsigned int oneminte=0;
unsigned int ReadTempDate;
unsigned char S_temp_H,S_temp_L,S_heart_H,S_heart_L;
unsigned char yemian=0,biaozhi=0;


sbit K1 = P1^0; //菜单
sbit K2 = P1^1;  //加
sbit K3 = P1^2;  //减
sbit Buzzer= P1^7;  //控制


sbit rs = P2^5 ; 
sbit rw = P2^6 ;
sbit ep = P2^7 ;


bit flag=1;
 

/******************把数据保存到单片机内部eeprom中******************/
void write_eeprom()
{
	SectorErase(0x2000);
	byte_write(0x2000, S_heart_H);
	byte_write(0x2001, S_heart_L);
	byte_write(0x2002, S_temp_H);
	byte_write(0x2003, S_temp_L);
	
	
    byte_write(0x2060, a_a);	
}

/******************把数据从单片机内部eeprom中读出来*****************/
void read_eeprom()
{
	S_heart_H = byte_read(0x2000);
	S_heart_L = byte_read(0x2001);
	S_temp_H = byte_read(0x2002);
	S_temp_L = byte_read(0x2003);
    a_a      = byte_read(0x2060);
}

/**************开机自检eeprom初始化*****************/
void init_eeprom()
{
	read_eeprom();		//先读
	if(a_a != 2)		//新的单片机初始单片机内问eeprom
	{
		S_heart_H = 120;
	    S_heart_L = 60;
	    S_temp_H = 38;
	    S_temp_L = 5;
		a_a = 2;
		write_eeprom();
	}	
}

/***********************1ms延时函数*****************************/
void delay_1ms(uint q)
{
	uint i,j;
	for(i=0;i<q;i++)
		for(j=0;j<110;j++);
}


/********************************************************************
* 名称 : delay_uint()
* 功能 : 小延时。
* 输入 : 无
* 输出 : 无
***********************************************************************/
void delay_uint(uint q)
{
	while(q--);
}


typedef bit BOOL  ; 


uchar data_byte;

void delay(uchar ms)
{       // 延时子程序
	 uchar i ;
	 while(ms--)
	 {
	  	for(i = 0 ; i<250;i++) ;
	 }
}

void delay1()//延时10us
{
	uchar i;
	i--;
	i--;
	i--;
	i--;
	i--;
	i--;
}
void longdelay(uchar s) //长延时
{
	 while(s--)
	 {
	  	delay(10) ;
	 }
}


/************************LCD模块******************************************/

BOOL lcd_bz()//测试LCD忙碌状态
{      
	 BOOL result ;
	 rs = 0 ;
	 rw = 1 ;
	 ep = 1 ;
	 result = (BOOL)(P0 & 0x80) ;
	 ep = 0 ;
	 return result ; 
}

void write_cmd(uchar cmd)// 写指令
{       
 	while(lcd_bz()) ;
    P0 = cmd ;
	ep = 1 ;
	ep = 0 ;  
}

void write_addr(uchar addr)//写地址
{       
 	write_cmd(addr|0x80) ;
}

void write_byte(uchar dat)//写字节
{      
 	while(lcd_bz()) ;
  	rs = 1 ;
  	rw = 0 ;
  	ep = 0 ;
  	P0 = dat ;
  	ep = 1 ;
  	ep = 0 ; 
}

void lcd_init()// 初始化
{       
	 write_cmd(0x38) ; 
	 delay(1);
	 write_cmd(0x08) ;  
	 delay(1);
	 write_cmd(0x01) ; 
	 delay(1);
	 write_cmd(0x06) ;
	 delay(1);
	 write_cmd(0x0c) ; 
	 delay(1);
}

 display(uchar addr, uchar q)//在某一地址上显示一字节
{ 	 
	 delay(1) ;
	 write_addr(addr) ;
   write_byte(q) ;
	// longdelay(1) ;
	 
}


void show1()
{     
	     lcd_init();// 初始化   
	     //  Welcome
         
		
		 display(0x05,'W');
         display(0x06,'e');
		 display(0x07,'l');
		 display(0x08,'c');
		 display(0x09,'o');
		 display(0x0a,'m');
         display(0x0b,'e');
	     longdelay(50);//短暂延时
		
  
		 display(0x42,' ');
         display(0x43,' '); 
		 display(0x44,' ');
		 display(0x45,' ');
		 display(0x46,' ');
		 display(0x48,' ');
	     display(0x49,' ');
	     display(0x4a,' ');
		 display(0x4b,' ');
	     longdelay(50);//短暂延时
	

}


void key() //按键程序 选择菜单模式
{
     
	  if(!K1)
		   {  
			   delay_1ms(20);
			    if(!K1)
		         {   
					 while(!K1)
				           ;
						  yemian++;
						  if(yemian>=5) { yemian=0; } 
								
             }
		   }
}




void shezhi()  //对里面的数值进行修改
{
   if(!K2)            //按下按键
  	  {
	   delay_1ms(20);
		if(!K2)
		  { 
            if(yemian==1) 
              {								
				S_heart_H+=1;       //没按下一次，数值加1
				if(S_heart_H>=200)  {S_heart_H=200;}
                write_eeprom();       //保存数据
				 
                 display(0x46,S_heart_H/100+'0');
	             display(0x47,S_heart_H%100/10+'0');
                 display(0x48,S_heart_H%100%10+'0');				
              }
							
           if(yemian==2) 
              {								
				S_heart_L+=1;       //没按下一次，数值加1
				if(S_heart_L>=200)  {S_heart_L=200;}
                write_eeprom();       //保存数据
				 
                 display(0x46,S_heart_L/100+'0');
	             display(0x47,S_heart_L%100/10+'0');
                 display(0x48,S_heart_L%100%10+'0');				
              }
							
			if(yemian==3) 
              {								
			    S_temp_H+=1;       //没按下一次，数值加1
				if(S_temp_H>=125)  {S_temp_H=125;}
                write_eeprom();       //保存数据
				   
				   display(0x46,S_temp_H/100+'0');
	               display(0x47,S_temp_H%100/10+'0');
                   display(0x48,S_temp_H%100%10+'0');
             }
			 
			if(yemian==4) 
              {								
			    S_temp_L+=1;       //没按下一次，数值加1
				if(S_temp_L>=125)  {S_temp_L=125;}
                write_eeprom();       //保存数据
				   
				   display(0x46,S_temp_L/100+'0');
	               display(0x47,S_temp_L%100/10+'0');
                   display(0x48,S_temp_L%100%10+'0');
             }	
			 	
           }
				  // while(!K2);      //等待按键 弹起  屏蔽此句，可以实现按键按下不放开时，连加技术功能，
					                  //   不过需要将上面延时去抖时间延长效果才好。建议改为： delay_LCD(50);  
      }
			 
			 
   if(!K3)            //按下按键
  	 {
	  delay_1ms(20);
		if(!K3)
		  { 
           if(yemian==1) 
             {								
			    
				if(S_heart_H<=1)  {S_heart_H=1;}
				S_heart_H-=1;       //没按下一次，数值加1
                write_eeprom();       //保存数据
				
				 display(0x46,S_heart_H/100+'0');
	             display(0x47,S_heart_H%100/10+'0');
                 display(0x48,S_heart_H%100%10+'0');				

			 }

			 if(yemian==2) 
             {								
			    
				if(S_heart_L<=1)  {S_heart_L=1;}
				S_heart_L-=1;       //没按下一次，数值加1
                write_eeprom();       //保存数据
				
				 display(0x46,S_heart_L/100+'0');
	             display(0x47,S_heart_L%100/10+'0');
                 display(0x48,S_heart_L%100%10+'0');				

			 }
							
			if(yemian==3) 
              {	
			    if(S_temp_H<=1)  {S_temp_H=1;}							
			    S_temp_H-=1;       //没按下一次，数值加1
                write_eeprom();       //保存数据

			    display(0x46,S_temp_H/100+'0');
	            display(0x47,S_temp_H%100/10+'0');
                display(0x48,S_temp_H%100%10+'0');

			
			 }
			 
		   if(yemian==4) 
              {	
			    if(S_temp_L<=1)  {S_temp_L=1;}							
			    S_temp_L-=1;       //没按下一次，数值加1
                write_eeprom();       //保存数据

			    display(0x46,S_temp_L/100+'0');
	            display(0x47,S_temp_L%100/10+'0');
                display(0x48,S_temp_L%100%10+'0');

			
			 }		
           }
				//   while(!K3);      //等待按键 弹起  //等待按键 弹起  屏蔽此句，可以实现按键按下不放开时，连加技术功能，
					                  //   不过需要将上面延时去抖时间延长效果才好。建议改为： delay_LCD(50);  
	  }

		
} 

void wendumaibo()
{

 	if(displayOK==0)//如果显示关
		{
		    display(0x06,'W');
            display(0x07,'a');
		    display(0x08,'i');
			display(0x09,'t');
			display(0x0a,'i');
			display(0x0b,'n');
			display(0x0c,'g');
			flag=1;
			oneminte=0;//加上这个，当displayOK标志位没有置1时，都清零计数，等待重新检测中计数。
		}
		
		if(displayOK==1 && oneminte<=200)//如果显示开
		{
			flag=0;
		    display(0x06,'T');
            display(0x07,'e');
		    display(0x08,'s');
			display(0x09,'t');
			display(0x0a,'i');
			display(0x0b,'n');
			display(0x0c,'g');
		}
	 
	    ReadTempDate=ReadTemperature();
        display(0x46,ReadTempDate/100+'0');
	    display(0x47,ReadTempDate%100/10+'0');
      	display(0x48,'.');
	    display(0x49,ReadTempDate%100%10+'0');
		display(0x4a,0xdf);
		display(0x4b,'C');		
}


void baojin()
{
  if(  ( (rate*6<S_heart_L  ||  rate*6>S_heart_H) &&  oneminte>200 )  ||  ReadTempDate>=S_temp_H*10 ||  ReadTempDate<=S_temp_L*10 )		{  Buzzer=0; }

  if(    oneminte>=0  &&  oneminte<200  &&    ReadTempDate<S_temp_H*10  &&  ReadTempDate>S_temp_L*10  )	    {  Buzzer=1; }
}

/*************定时器0初始化程序***************/
void time_init()	  
{
	EA   = 1;	 	  //开总中断
	TMOD = 0X01;	  //定时器0、定时器1工作方式1
	ET0  = 1;		  //开定时器0中断 
	TR0  = 1;		  //允许定时器0定时
}


/***********外部中断0初始化程序****************/
void init_int0()	  //外部中断0初始化程序
{
	EX0=1;			  //允许外部中断0中断
	EA=1;	 		  //开总中断
	IT0 = 1; 		  //外部中断0负跳变中断
}

/****************主函数***************/
void main()
{	
    ReadTemperature();
    show1();//显示问候语和其他信息
	time_init(); //初始化定时器 
	init_int0(); //外部中断0初始化程序
    init_eeprom();      			//读eeprom数据
	while(1)
	{	
      key(); //按键扫描
      if(flat300ms>=4)	//200ms
	   {
	    flat300ms=0;
//==================主界面===================
		  if(yemian==0)  
			{
			  if(biaozhi==0)
				 {
                   biaozhi=1;

				   lcd_init();// 初始化
				   display(0x00,'H');
				   display(0x01,'e');
				   display(0x02,'a');
				   display(0x03,'r');
				   display(0x04,'t');
				   display(0x05,':');

				   display(0x40,'T');
				   display(0x41,'e');
				   display(0x42,'m');
				   display(0x43,'p');
				   display(0x44,':');
                 }
               wendumaibo();
			   baojin();
	       }

//==================脉搏报警上限 设置===================
		  if(yemian==1)  
			{
			  if(biaozhi==1)
				 {
                   biaozhi=2;
				   Buzzer=1;
				   lcd_init();// 初始化
				   display(0x04,'H');
				   display(0x05,'e');
				   display(0x06,'a');
				   display(0x07,'r');
				   display(0x08,'t');
				   display(0x09,'-');
				   display(0x0a,'H');

				 display(0x46,S_heart_H/100+'0');
	             display(0x47,S_heart_H%100/10+'0');
                 display(0x48,S_heart_H%100%10+'0');				  
               }
                shezhi();
	       }

//==================脉搏报警下限 设置===================
		  if(yemian==2)  
			{
			  if(biaozhi==2)
				 {
                   biaozhi=3;
				   Buzzer=1;
				   lcd_init();// 初始化
				   display(0x04,'H');
				   display(0x05,'e');
				   display(0x06,'a');
				   display(0x07,'r');
				   display(0x08,'t');
				   display(0x09,'-');
				   display(0x0a,'L');

				 display(0x46,S_heart_L/100+'0');
	             display(0x47,S_heart_L%100/10+'0');
                 display(0x48,S_heart_L%100%10+'0');				  
               }
                shezhi();
	       }
		   		   
//==================温度报警 上限设置===================
		  if(yemian==3)  
			{
			  if(biaozhi==3)
				 {
                   biaozhi=4;
				   Buzzer=1;
				   lcd_init();// 初始化
				  
				   display(0x05,'T');
				   display(0x06,'e');
				   display(0x07,'m');
				   display(0x08,'p');
				   display(0x09,'-');
				   display(0x0a,'H');

				   display(0x46,S_temp_H/100+'0');
	               display(0x47,S_temp_H%100/10+'0');
                   display(0x48,S_temp_H%100%10+'0');				  
                }
                shezhi();
	       }	  	    

//==================温度报警 下限设置===================
		  if(yemian==4)  
			{
			  if(biaozhi==4)
				 {
                   biaozhi=0;
				   Buzzer=1;
				   lcd_init();// 初始化
				  
				   display(0x05,'T');
				   display(0x06,'e');
				   display(0x07,'m');
				   display(0x08,'p');
				   display(0x09,'-');
				   display(0x0a,'L');

				   display(0x46,S_temp_L/100+'0');
	               display(0x47,S_temp_L%100/10+'0');
                   display(0x48,S_temp_L%100%10+'0');				  
                }
                shezhi();
	       }	  	    
      
	        
	   }
    }
 }


void int0() interrupt 0
{
	if(time1>=8)
	 {
		  time1=0;
          if(i==3)//记录到超过等于3次时间
			{
				i=1;//计数从1开始
				displayOK=1;    //测得3次开始显示
            }	
			
		
			if( oneminte>200 && flag==0 && yemian==0) 
               { 
				 flag=1;
				 rate1=rate*6;
                
				 display(0x09,'/');
		         display(0x0a,'m');
  	             display(0x0b,'i');
                 display(0x0c,'n');
				 
                  display(0x06,rate1/100+'0');
			      display(0x07,rate1%100/10+'0');
			      display(0x08,rate1%100%10+'0');
              }
     	
		}
	
}


/*************定时器0中断服务程序***************/
void time0_int() interrupt 1
{	
	TH0 = 0x3c;
	TL0 = 0xb0;     // 50ms	 12M
	timecount++;//每50ms一次计数
    oneminte++;
	flat300ms++;
	if(timecount>50 &&yemian==0 )     //当超过50*50ms=2.5s左右没有检测到信号停止显示
	{
			i=0;//数据个数清零
			timecount=0;//50ms计数清零
			displayOK=0;//显示关
		    oneminte=0;
		    rate=0; 
		    display(0x06,'W');
            display(0x07,'a');
		    display(0x08,'i');
			display(0x09,'t');
			display(0x0a,'i');
			display(0x0b,'n');
			display(0x0c,'g');
		
		
  }
}