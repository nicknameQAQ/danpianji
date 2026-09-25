#include <REGX51.H>
#include<intrins.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
sbit RS = P2^5; 
sbit RW = P2^6; 
sbit E  = P2^7; 

#define Data  P0//数据端口
//全局变量
float num1;//第一操作数num1     （初始为零）
float num2;//第二操作数num2     （初始为零）
char yun_flag='+';//运算符号 yun_flag    （默认为+ ）
char key_last;//上次按键状态标志 key_last
char sqrt_flag;//开方键按下标志   sqrt_flag
unsigned long pos_flag=1;//小数位权pos_flag      (默认为1)
char neg_flag;//正负标志 neg_flag
char data c_num1[15]=" ";//c_num1 字符型第一操作数 (数组不定义大小与其他变量冲突)
char data c_num2[15]=" ";//c_num2 字符型第二操作数
char error;//运算法则错误；
char len;//小数点后位数
/*                    微秒延时函数                                */
/******************************************************************/
void DelayUs(unsigned char us)//delay us
{
 unsigned char uscnt;
 uscnt=us>>1;        /*12MHz频率*/
 while(--uscnt);
}
/******************************************************************/
/*                    毫秒函数声明                                */
/******************************************************************/
void DelayMs(unsigned char ms)
{
 while(--ms)
   {
     DelayUs(250);
     DelayUs(250);
	 DelayUs(250);
	 DelayUs(250);
   }
}
/******************************************************************/
/*                   写入命令函数                                 */
/******************************************************************/
void WriteCommand(unsigned char c)
{
 DelayMs(5);//操作前短暂延时，保证信号稳定
 E=0;
 RS=0;
 RW=0;
 _nop_();
 E=1;
 Data=c;
 E=0;
}
/******************************************************************/
/*                   写入数据函数                                 */
/******************************************************************/
void WriteData(unsigned char c)
{
 DelayMs(5);  //操作前短暂延时，保证信号稳定
 E=0;
 RS=1;
 RW=0;
 _nop_();
 E=1;
 Data=c;
 E=0;
 RS=0;
}
/******************************************************************/
/*                   写入字节函数                                 */
/******************************************************************/
void ShowChar(unsigned char pos,unsigned char c)
{
 unsigned char p;
 if (pos>=0x10)
    p=pos+0xb0; //是第二行则命令代码高4位为0xc
 else 
    p=pos+0x80; //是第二行则命令代码高4位为0x8
 WriteCommand (p);//写命令
 WriteData (c);   //写数据
}
/******************************************************************/
/*                   写入字符串函数                               */
/******************************************************************/
void ShowString (unsigned char line,char *ptr)
{
 unsigned char l,i;
 l=line<<4;
 for (i=0;*(ptr+i)!='\0';i++)
  ShowChar (l++,*(ptr+i));//循环显示16个字符
}
/******************************************************************/
/*                   初始化函数                                   */
/******************************************************************/
void InitLcd()
{
 DelayMs(15);
 WriteCommand(0x38); //display mode
 WriteCommand(0x38); //display mode
 WriteCommand(0x38); //display mode
 WriteCommand(0x06); //显示光标移动位置
 WriteCommand(0x0c); //显示开及光标设置
 WriteCommand(0x01); //显示清屏

}
/*************按键扫描****************************/
char scan()
{

  char h_data,l_data,i,key_num;
  P2=P2&0XE0;//P2低5位送0
  P1=P1|0X0F;//P1低4位送1
  while((P1|0xf0)==0xff)//判断P1低4位是否全为1
  {
   while((P1|0xf0)==0xff);
   DelayMs(10);//延时10ms
  }
   l_data=~(P1|0xf0);//记入列标志
   P2=P2|0X1F;//P2低五位送1
   P1=P1&0XF0;//P1低四位送0
   h_data=~(P2|0xe0);//记入行标志
   for(i=0;i<=4;i++)//计算行号(0~4)
   	 {
	   if(h_data==1)break;
	   h_data=h_data>>1;
	 }
 h_data=i;//行号
  for(i=1;i<=4;i++)//计算列号(1~4)
 	{
	  if(l_data==1)break;
	  l_data=l_data>>1;
	}
 l_data=i;//列号
   key_num=h_data*4+l_data;	
 
  P2=P2&0XE0;//P2低5位送0
  P1=P1|0X0F;//P1低4位送1
  while((P1|0xf0)!=0xff)//判断P1低4位是否全为1
  {
   while((P1|0xf0)!=0xff);
   DelayMs(10);//延时10ms
  }
  return key_num;
}
void float_to_char(float a,char* p)
{
  char i,flag,length;
  flag=0;
  sprintf(p,"%f",a);
  length=strlen (p);
  for(i=0;i<length;i++)
  {
    if(*(p+i)=='.')flag=1;
  }
  if(flag==1)
  for(i=length-1;i>=0;i--)
  {
    if(*(p+i)=='.'){*(p+i)='\0';break;}
    if(*(p+i)!='0'){*(p+i+1)='\0';break;}
  }
}
/**********************更新液晶*************************/
void refresh()//更新液晶
{
  char length,i,j;
  char dot;
  dot=0;
  	
  float_to_char(num2,c_num2);//num2转为字符型
  length=strlen(c_num2);
  for(i=0,j=0;i<length;i++)	
  {
    if(c_num2[i]=='.')
	dot=1;
	if(dot==1)
    j++;
  } 
  if(j<len)
  {
	  if(dot==1)
	  {	
	    for(i=length;i<(length+len-j);i++)
		 {
		   c_num2[i]='0';
	
		 }
		 c_num2[i]='\0';
	  }
	  else
		{
		  
		  c_num2[length]='.';
		  if(len-j>1)
		  {
		    for(i=length+1;i<(length+len-j);i++)
			 c_num2[i]='0';
		  }
		  c_num2[length+len-j]='\0';
		}  
  }

  if(neg_flag==1)//'+/-'按下首位加‘-’
   {
      length=strlen(c_num2);//计算c_num2长度	  
		    for(i=length-1;i>=0;i--)
		  {
		  	c_num2[i+1]=c_num2[i];
		  }
		  c_num2[length+1]='\0';
		  c_num2[0]='-';
   }
   
   if(sqrt_flag==1)
   	{
      length=strlen(c_num2);//计算c_num2长度
   	  for(i=length-1;i>=0;i--)
	  {
	  	c_num2[i+1]=c_num2[i];
	  }
	  c_num2[length+1]='\0';
	  c_num2[0]=0xe8;//字符根号
   }
   
   
   if(error==0)
   {
	   float_to_char(num1,c_num1);//num1转为字符型
	   WriteCommand(0x01); //显示清屏 
	   ShowString(0,c_num1);
	   ShowString(1,c_num2);  
	   ShowChar(15,yun_flag);
   }else
   { 
       WriteCommand(0x01); //显示清屏 
	   ShowString(0,"error!!!");
   }
}
//键值处理
void operation1(char keynum)//按下 ’+、-、*、/处理
{
  if(key_last==1)//上次按键为 数字、小数点、+/-、sqrt
   {
     if(neg_flag==1)num2=-num2;//'+/-'按下
	 if(sqrt_flag==1)//sqrt按下
	 {
	 if(num2>=0)
	 num2=sqrt(num2);
	 else
	 error=1;
	 }
	 if(yun_flag=='+')num1=num1+num2; //按下的是‘+’
	 if(yun_flag=='-')num1=num1-num2;//按下的是‘-’
	 if(yun_flag=='*')num1=num1*num2;//按下的是‘*’
	 if(yun_flag=='/')//按下的是‘/’
	 {
	   if(num2!=0)
	   num1=num1/num2;
	   else
	   error=1;
	 }
	 num2=0;//num2清零
	 sqrt_flag=0;//sqrt_flag清零
     neg_flag=0;//neg_flag清零
	 pos_flag=1;//pos_flag回1
   }
   if(keynum==4)yun_flag='+';//yun_flag更新
   if(keynum==8)yun_flag='-';
   if(keynum==12)yun_flag='*';
   if(keynum==16)yun_flag='/';
   len=0;
   key_last=0;//key_last更新   
   refresh();
}
//////////////////////////////////////////////////////
void operation2(char keynum)//输入数字
{
  float Data1;
   
  if(keynum==1)Data1=7;//分析输入数字
  if(keynum==2)Data1=8;
  if(keynum==3)Data1=9;
  if(keynum==5)Data1=4;
  if(keynum==6)Data1=5;
  if(keynum==7)Data1=6;
  if(keynum==9)Data1=1;
  if(keynum==10)Data1=2;
  if(keynum==11)Data1=3;
  if(keynum==13)Data1=0; 
  if(pos_flag==1)//更新num2
  num2=num2*10+Data1;
  else
  {	    
    num2=num2+(Data1/pos_flag);
	pos_flag=pos_flag*10;
	len++;
  }
  key_last=1;//更新key_last	
  refresh();
}
////////////////////////////////////////////////////////////
void operation3()//输入小数点
{
  if(pos_flag==1)//首次出现小数点
  {
   pos_flag=pos_flag*10;//小数位权*10
   len++;
  }
  key_last=1;//更新key_last 
  refresh();//更新液晶
}
/////////////////////////////////////////////////////
void operation4()//输入'='
{
     if(neg_flag==1)num2=-num2;//'+/-'按下
	 if(sqrt_flag==1)//sqrt按下
	 {
	 if(num2>=0)
	 num2=sqrt(num2);
	 else
	 error=1;
	 }
	 if(yun_flag=='+')num1=num1+num2; //按下的是‘+’
	 if(yun_flag=='-')num1=num1-num2;//按下的是‘-’
	 if(yun_flag=='*')num1=num1*num2;//按下的是‘*’
	 if(yun_flag=='/')//按下的是‘/’
	 {
	   if(num2!=0)
	   num1=num1/num2;
	   else
	   error=1;
	 }
	 num2=0;//num2清零
	 sqrt_flag=0;//sqrt_flag清零
     neg_flag=0;//neg_flag清零
	 pos_flag=1;//pos_flag回1
     yun_flag='+';//yun_flag更新
	 len=0;
   key_last=0;//key_last更新   
   refresh();
}
////////////////////////////////////////////////////
void operation5()//输入clear all
{

   num1=0;// num1清零
   num2=0;//num2清零
   sqrt_flag=0;//清sqrt_flag
   neg_flag=0;// 清neg_flag
   pos_flag=1;// Pos_flag=1
   yun_flag='+';// yun_flag(为'+')
   error=0;//清error
   len=0;
   key_last=0;//更新key_flag
   refresh();//更新液晶
}
///////////////////////////////////////////////////////
void operation6()//输入'C'
{
 num2=0;//num2清零
 sqrt_flag=0;//清sqrt_flag
 neg_flag=0;//清neg_flag
 pos_flag=1;//pos_flag=1
 len=0;
 key_last=0;//key_last
 refresh();//更新液晶 
}
////////////////////////////////////////////////////////////
void operation7()//输入'+/-'
{
  if(neg_flag==0)//neg_flag反转
  neg_flag=1;
  else neg_flag=0;
  key_last=1;//key_last
  refresh();// 更新液晶
}
/////////////////////////////////////////////////////////////
void operation8()//输入'sqrt'
{
 if(sqrt_flag==0)//sqrt_flag反转
  sqrt_flag=1;
 else sqrt_flag=0;
 key_last=1;//更新key_last
 refresh();//更新液晶
}
//键值分析
void key(char keynum)
{
  switch(keynum)
  {
    case 4 :
	case 8 :
	case 12:
	case 16://+、-、*、/
	    	{
			 if(error==0)
	         operation1(keynum);
        	 break;}
			
	case 1  : //数字7
	case 2  : //数字8
	case 3  : //数字9
	case 5  : //数字4
	case 6  : //数字5
	case 7  : //数字6
	case 9  : //数字1
	case 10 : //数字2
	case 11 : //数字3
	case 13 : //数字0
	         {
			 if(error==0)
			 operation2(keynum);
			 break;}
			 
	case 14 ://小数点		  
			 {
			   if(error==0)
			   operation3();
			   break;}
			 
	case 15 :// '='	  
			 {
			   if(error==0)
			   operation4();
			   break;}
			 
	case 17 :// clear all  
			 {
			   operation5();
			   break;}

	case 18 :// 'C'  
			 {
			   if(error==0)
			   operation6();
			   break;}
	case 19 :// '+/-'  
			 {
			   if(error==0)
			   operation7();
			   break;}
	case 20 ://sqrt
			 {
			   if(error==0)
			   operation8();
			   break;}		   		   
			   			   		 
  }	  
}
main()
{
 
 InitLcd();         //初始化LCD
DelayMs(15);       //延时保证信号稳定
refresh();
 
        //延时保证信号稳定
while(1)
{
 key(scan());
}
}