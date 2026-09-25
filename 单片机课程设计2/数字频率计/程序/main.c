#include <reg51.h>
#define uint unsigned int
#define uchar unsigned char
#include "value.h"
#include "lcd12864bu.h"
#include<intrins.h>

uchar tt;
sbit K=P3^2;	//测量频宽
uint High,Low,n;

bit xitong_init=1;
bit jishu_init=1;

unsigned long temp_V; 

unsigned long temp;   
unsigned int shi =9;
unsigned int zhouqi_xiaoshu;
unsigned int zhouqi_zengshu;
/**********************TLC549***************************/
sbit DI  = P1^6;
sbit CS  = P1^7;
sbit CLK = P3^5;

/********************独立按键引脚*****************/
sbit button1=P1^0;
sbit button2=P1^1;
sbit button3=P1^2;
sbit button4=P1^3;
sbit button5=P1^4;


sbit X10=P1^5;

sbit s0_1=P2^6;
sbit s1=P2^7;
sbit s10=P3^0;

sbit us1 =P3^1;
sbit us10 =P3^6;
sbit ms1=P3^7;

uchar key_can;	 //按键值


 /************************定义变量***********************/
uchar i=0;
unsigned long T0count; 
unsigned long T1count ;
unsigned char timecount; 
bit flag=0; 
unsigned int x; 
unsigned char zhameng_time=20;
unsigned long zhouqi_s;

/****************************LCD12864显示**********************************/
void xiaoshu_qian(unsigned char x,unsigned long temp){  //x为第几行  temp为显示的数字
	write_shuzi(1,x,32+4*8,shuzi[temp % 10],0);temp/=10;
	write_shuzi(1,x,32+3*8,shuzi[temp % 10],0);temp/=10;
	write_shuzi(1,x,32+2*8,shuzi[temp % 10],0); temp/=10;
	write_shuzi(1,x,32+1*8,shuzi[temp % 10],0); temp/=10;
}
void xiaoshu_hou(unsigned char x,unsigned long temp){
    write_shuzi(1,x,32+8*8,shuzi[temp %10],0); temp/=10;
	write_shuzi(1,x,32+7*8,shuzi[temp %10],0);temp/=10;
}
void shuzixianshi(unsigned char x,unsigned long temp){
    write_shuzi(1,x,32+8*8,shuzi[temp %10],0); temp/=10;
	write_shuzi(1,x,32+7*8,shuzi[temp %10],0);temp/=10;
	write_shuzi(1,x,32+6*8,shuzi[temp % 10],0); temp/=10;
	write_shuzi(1,x,32+5*8,shuzi[temp % 10],0);temp/=10;
	write_shuzi(1,x,32+4*8,shuzi[temp % 10],0);temp/=10;
	write_shuzi(1,x,32+3*8,shuzi[temp % 10],0);temp/=10;
	write_shuzi(1,x,32+2*8,shuzi[temp % 10],0); temp/=10;
	write_shuzi(1,x,32+1*8,shuzi[temp % 10],0); temp/=10;
}
void ceping(unsigned char x,unsigned long temp){   //测频

	 write_yhanzi(1,x,0*16,hanzi[1]);   //测频
	 write_yhanzi(1,x,1*16,hanzi[0]);
	 write_shuzi(1,x,2*16,fuhao[1],0);	  //:
	 write_shuzi(1,x,32+10*8,fuhao[2],0); //HZ
	 write_shuzi(1,x,32+11*8,fuhao[3],0);	 
}
void chezou(unsigned char x,unsigned long temp){
     write_yhanzi(1,x,0*16,hanzi[1]);   //测周
	 write_yhanzi(1,x,1*16,hanzi[2]);
	 write_shuzi(1,x,2*16,fuhao[1],0);	  //:
	 write_shuzi(1,x,32+6*8,fuhao[4],0);	  //.
	 	  write_shuzi(1,x,32+10*8,fuhao[5],0); //HZ
	 write_shuzi(1,x,32+11*8,fuhao[5],0);	  
}
void maikuan(unsigned char x,unsigned long temp){
     write_yhanzi(1,x,0*16,hanzi[7]);   //脉宽
	 write_yhanzi(1,x,1*16,hanzi[8]);
	 write_shuzi(1,x,2*16,fuhao[1],0);	  //:  
	  write_shuzi(1,x,32+10*8,fuhao[5],0); //HZ
	 write_shuzi(1,x,32+11*8,fuhao[5],0);
}
void pinbi(unsigned char x,unsigned long temp){
     write_yhanzi(1,x,0*16,hanzi[0]);   //频幅
	 write_yhanzi(1,x,1*16,hanzi[10]);
	 write_shuzi(1,x,2*16,fuhao[1],0);	  //: 
	  write_shuzi(1,x,32+6*8,fuhao[4],0);	  //.
	   write_shuzi(1,x,32+10*8,fuhao[5],0); //HZ
	 write_shuzi(1,x,32+11*8,fuhao[5],0);

}
void jishu(unsigned char x,unsigned long temp){
     write_yhanzi(1,x,0*16,hanzi[5]);   //计数
	 write_yhanzi(1,x,1*16,hanzi[6]);
	 write_shuzi(1,x,2*16,fuhao[1],0);	  //: 
	  write_shuzi(1,x,32+10*8,fuhao[5],0); //HZ
	 write_shuzi(1,x,32+11*8,fuhao[5],0);
}


/*************************测频宽*********************/
void Init_T0()
{
	TMOD=0x09;
	TH0=0;
	TL0=0;
}
void Message_Width()	   
{
	while(K);
	TR0=1;
	while(!K);
	while(K);
	TR0=0;
	High=TH0;
	Low=TL0;
}
/************************tlc594*************************/
uchar ad_0;
uint AD;

//----------------------------------
//               延时子程序
//----------------------------------
void delay(uchar ms)    
{       
	 uchar i ;
	 while(ms--)
	 {
	  	for(i = 0 ; i<250;i++) ;
	 }
}	
	   
//-----------------------------------------
//            TLC549模数转换
//-----------------------------------------
uchar TLC549_ADC(void)
{
	   uchar n, tmp;

        CS  = 1;                     //CS置高,片选无效
        CLK = 0;

        CS  = 0;                     //CS置低,片选有效,同时DO输出高位
        _nop_();
        _nop_();                     //适当延迟时间1.4us Setup Time

        for(n = 0; n < 8; n++) //串行数据移位输入
        { 
                tmp <<= 1;
                tmp |=  DI;

                CLK = 1;        //0.4us
                _nop_();        //延迟0.1us
                CLK = 0;        //0.4us
        }
        CS = 1;            //CS置高,片选无效
        for(n = 17; n != 0; n--) _nop_();  //下一次转换需要延迟17us
        return (tmp);

}

/****************************************************/

void xitongchushihua(){	     //系统初始化
 	TMOD=0x15;
  	TH0=0; 
  	TL0=0; 
  	TH1=(65536-49989)/256; 
  	TL1=(65536-49989)%256; 
  	TR1=1;
  	ET0=1; 
  	ET1=1;
  	EA=1;
  	TR0=1;
}
void main()
{	
   // xitongchushihua();//定时器初始化				 
   Clr_Scr();
	
	
	while(1)
    {
		if(!button1){ //测频按键
		ceping(3,X10);
		}else if(!button2){	 //测周按键
		chezou(3,temp);
		}else if(!button3){	  //幅值按键
			pinbi(3,temp);
			EA=0;
  	        TR0=0;
			TR1=0;
			while(!button3){
			  ad_0=TLC549_ADC();
		      AD=ad_0*1.97;
	            write_shuzi(1,3,32+4*8,shuzi[AD%1000/100],0);  
				 write_shuzi(1,3,32+8*8,shuzi[AD%100/10],0); 
	            write_shuzi(1,3,32+7*8,shuzi[AD%10],0); 
			}
			EA=1;
  	        TR0=1;
			TR1=1;			  
		}else if(!button4){	 //计数按键
		     IT1=1; //设置外部中断1的触发方式为下降沿			 //顺序保证：最后开启总中断
	         EX1=1;	//开启外部中断1
		     jishu(3,temp);
			 while(!button4){
			    shuzixianshi(3,T1count);
			    TR1=0;	   //关闭定时器1的计数
			 }
			 EX1=0;	//关闭外部中断1
		 	 jishu_init=0;
		}else if(!button5){  //脉宽按键
		 Init_T0();
		  Message_Width();
		  shuzixianshi(3,High*256+TL0);
		    maikuan(3,temp);
			xitong_init=0;
		}

	if(xitong_init==0 && button5){	 //脉宽菜单退出后进行系统初始化
	  xitong_init=1;
	  chushihua();
	}
	if(jishu_init==0 && button4){   //计数菜单退出后进行系统初始化
	  jishu_init=1;
	  chushihua();
	}

    if(!s0_1)  //闸门时间0.1s
	 {
	   	zhameng_time=2;
	 }
	 else if(!s1) //闸门时间1s
	 {
	    zhameng_time=20;
	 }
	 else if(!s10) ////闸门时间10s

	 {
	    zhameng_time=200;
	 }


	 if(!us1){    //10us测周期
	  zhouqi_s=100000;
	 }
	 else if(!us10){//100us测周期
	  zhouqi_s=10000;
	 }
	 else if(!ms1){	//1ms测周期
	  zhouqi_s=1000;
	 }
		if(flag==1) //定时时间按到
       	{ 
         	flag=0; 
         	x=(T0count*65536+TH0*256+TL0);
         	timecount=0; 
         	T0count=0; 
         	TH0=0; 													
         	TL0=0; 
         	TR0=1;
			
			if(X10)x=x*100;	   //对分频检测后家伙是那个系数
			else x=x*10;

			if(zhameng_time==2)x=x*10;	  //对阀门时间的系数计算
			if(zhameng_time==200)x=x/10;
			
			
			if(!button1){	    // 测频数字量计算
			shuzixianshi(3,x);
			}else if(!button2){	  //小数位的处理
				zhouqi_xiaoshu =zhouqi_s%x;
				zhouqi_zengshu =zhouqi_s/x;
				xiaoshu_qian(3,zhouqi_zengshu);
				xiaoshu_hou(3,zhouqi_xiaoshu); 
			chezou(3,temp);
			}else if(!button3){//频率比数据量计算
			pinbi(3,temp);
			}else if(!button4){//计数数据量计算
			 jishu(3,temp);
			}
       	} 
	}
}

void t0(void) interrupt 1 using 0	//计脉冲个数
{ 
  T0count++; 
} 

void t1(void) interrupt 3 using 0	 //定时1s	  50ms
{ 
  TH1=(65536-50045)/256; 
  TL1=(65536-50045)%256; 
  timecount++; 
  if(timecount==zhameng_time) 	//阀门时间处理
    { 
      TR0=0;
      timecount=0; 
      flag=1;
    } 
}
void EX1_ISR(void) interrupt 2     //外部中断1的中断在此为2
{
	T1count++; 					//计脉冲个数
}