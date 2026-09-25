#include<reg51.h>
#include"delay.h"
#include"lcd602.h"
//**************端口定义******************* 
sbit en=P2^7; //6  液晶使能端
sbit rw=P2^6; //5 液晶读写选择端
sbit rs=P2^5; //4 液晶数据命令选择端
//**************变量定义*******************
//**************数组定义*******************
/*******************************************************************
                  写命令
************************************************************************/
void xiemingling(unsigned char mingling)
{
  en=0;
  rs=0;
  rw=0;
  P0=mingling;
  delayxms(5);
  en=1;
  delayxms(10);
  en=0;
}
/*******************************************************************
                  写数据
************************************************************************/
void xieshuju(unsigned char shuju)
{   
  en=0;
  rs=1;
  rw=0;
  P0=shuju;
  delayxms(5);
  en=1;
  delayxms(10);
  en=0;
}
/*******************************************************************
                  液晶初始化
************************************************************************/
void chushihua()
{
  delayxms(20); //给液晶一段时间
  en=0;
  xiemingling(0x38);//设置16*2显示，5*7点阵，8位数据接口
  delayxms(20);
  xiemingling(0x38);//
  delayxms(20);
  xiemingling(0x0c);//设置开显示，不显示光标
  delayxms(20);
  xiemingling(0x06);//写一个字符后地址指针加1
  delayxms(20);
  xiemingling(0x01);//显示清零，数据指针清零
  delayxms(20);
  xiemingling(0x80);//指定字符显示的实际地址，，，显示位置的确定方法规定为"80H+地址码x"
  delayxms(20);
}
/***********************lcd1602上显示这些字符函数************************/
void write_string(uchar hang,uchar add,uchar *p)
{
	if(hang==1)   
		xiemingling(0x80+add);
	else
		xiemingling(0x80+0x40+add);
		while(1)														 
		{
			if(*p == '\0')  break;
			xieshuju(*p);
			p++;
		}	
}
///*****************控制光标函数********************/
//void write_guanbiao(uchar hang,uchar add,uchar date)
//{		
//	if(hang==1)   
//		xiemingling(0x80+add);
//	else
//		xiemingling(0x80+0x40+add);
//	if(date == 1)
//		xiemingling(0x0f);     //显示光标并且闪烁	
//	else 
//		xiemingling(0x0c);   //关闭光标
//}	

/***********************lcd1602上显示特定的字符************************/
void write_zifu(uchar hang,uchar add,uchar date)
{
	if(hang==1)   
		xiemingling(0x80+add);
	else
		xiemingling(0x80+0x40+add);
	xieshuju(date);	
}
/***********************lcd1602上显示三位十进制数************************/
void write_num3(uchar hang,uchar add,uint date)
{
	if(hang==1)   
		xiemingling(0x80+add);
	else
	xiemingling(0x80+0x40+add);
	xieshuju(0x30+date/100%10);
//	xieshuju('.');
	xieshuju(0x30+date/10%10);
//	xieshuju('.');
	xieshuju(0x30+date%10);	
} 
/***********************lcd1602上显示2位十进制数************************/
void write_num2(uchar hang,uchar add,uint date)
{
	if(hang==1)   
		xiemingling(0x80+add);
	else
	    xiemingling(0x80+0x40+add);
	xieshuju(0x30+date/10);
	xieshuju(0x30+date%10);	
} 
/***********************lcd1602上显示2位十进制数************************/
//void write_num2(uchar hang,uchar add,uint date)
//{
//	if(hang==1)   
//		xiemingling(0x80+add);
//	else
//		xiemingling(0x80+0x40+add);
//	xieshuju(0x30+date/10);
//	
//	xieshuju(0x30+date%10);
//
//} 
///***********************lcd1602上显示五位十进制数************************/
//void write_num5(uchar hang,uchar add,uint date)
//{
//	if(hang==1)   
//		xiemingling(0x80+add);
//	else
//		xiemingling(0x80+0x40+add);
//	xieshuju(0x30+date/10000);
//	xieshuju(0x30+date%10000/1000);
//	xieshuju(0x30+date%10000%1000/100);
//	xieshuju(0x30+date%10000%1000%100/10);
//	xieshuju('.');
//	xieshuju(0x30+date%10);	
//} 
///***********************lcd1602上显示五位十进制数************************/
//void write_num51(uchar hang,uchar add,uint date)
//{
//	if(hang==1)   
//		xiemingling(0x80+add);
//	else
//		xiemingling(0x80+0x40+add);
//	xieshuju(0x30+date/10000);
//	xieshuju(0x30+date%10000/1000);
//	xieshuju(0x30+date%10000%1000/100);
//	xieshuju(0x30+date%10000%1000%100/10);
////	xieshuju('.');
//	xieshuju(0x30+date%10);	
//}
///***********************lcd1602上显示1位十进制数************************/
//void write_num1(uchar hang,uchar add,uint date)
//{
//	if(hang==1)   
//		xiemingling(0x80+add);
//	else
//	    xiemingling(0x80+0x40+add);
//	
//	xieshuju(0x30+date%10);	
//} 