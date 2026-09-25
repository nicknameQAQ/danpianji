#include <REGX52.H>
sbit keyu=P3^4;//加按键
sbit keyl=P3^5;//减按键
sbit keyh=P3^6;//调节尺度
sbit keym=P3^7; //转换模式
sbit RS=P2^5;//数据/命令选择端（H/L）
sbit RW=P2^6;//读/写端(H/L)
sbit EN=P2^7;//使能
sbit DQ=P2^0;  //DS18B20接入口
sbit buzz=P2^1;//蜂鸣器接口	-
bit f=1;  //正负温度标志位
unsigned char  m=1,y=0,o=0;//寄存器
unsigned int sdata;//存放整数温度
unsigned char xiaoshu1;	//存放小数后第一位温度数值
unsigned char xiaoshu2;	 //存放小数后第二位温度数值
unsigned char code huany[]=" Welcome to use "; //欢迎使用
unsigned char code tt[]  =" ter thermometer"; //温度计
unsigned char code duqu[]="   loading...   "; //读取
unsigned char code tab1[]=" T:      C   M  ";	//框架
unsigned char code tab2[]="U:        L:    "; 
unsigned char code shuzu[]="0123456789+- .*";	//数字,,10加号，，，11减号，，，12空格，，，，13点，，，，14*
unsigned char u=40;//上限
char l=15,p;
			  //下限
void delay_50us(unsigned int t)	 //延时函数
{
 unsigned int j,k;
 for (;t>0;t--)
 {
  for(j=10;j>0;j--)
  {
   for(k=1;k>0;k--);
  }
 }
}
void button() //报警蜂鸣器
{
  unsigned char k=2;
  while(k--)
 {
  buzz=0;delay_50us(200);
  buzz=1;delay_50us(200);
 }
}
///lcd
void write_com(unsigned com)//写操纵
{
   EN=0;
   RS=0;
   RW=0;
   P0=com;
   delay_50us(1);
   EN=1;
   delay_50us(5);
   EN=0;
}
void write_data(unsigned char dat)//写数据
{
   EN=0;
   RS=1;
   RW=0;
   P0=dat;
   delay_50us(1);
   EN=1;
   delay_50us(5);
   EN=0;
}
void init(void)//初始化
{
     delay_50us(5);
	 write_com(0x38);
	 delay_50us(1);
	 write_com(0x38);
	 delay_50us(1);
	 write_com(0x38);
	 write_com(0x38);
	 write_com(0x08); //只开显示
	 write_com(0x01); //清屏
	 write_com(0x06);//地址加1，当写入数据的时候光标右移
	 write_com(0x0C); //开显示，不显示光标
}  
void delay_18B20(unsigned int i)	//18b20全局延时
{
  while(i--);
}
///////////////////////////////////18b20//////////////////////////////////////////////////
//初始化传感器函数
  Init_DS18B20(void)
  {
  unsigned char x=0;
  DQ=1;//DQ复位
  delay_18B20(10);//稍做延时
  DQ=0;//单片机将DQ拉低
  delay_18B20(80);//精确延时大于480us
  DQ=1;//拉高总线
  delay_18B20(20);
  x=DQ;//稍做延时后如果x=0则初始化成功x=1则初始化失败
  delay_18B20(30);
  return x;
}
//读一个字节
ReadOneChar(void)
{
   unsigned char i=0;
   unsigned char dat=0;
   for(i=8;i>0;i--)
   {
      DQ=0;//给脉冲信号
      dat>>=1;
      DQ=1;//给脉冲信号
      if(DQ)
      dat|=0x80;
      delay_18B20(4);
   }
   return(dat);
}
//写一个字节
WriteOneChar(unsigned char dat)
{
   unsigned char i=0;
   for(i=8;i>0;i--)
   {
      DQ=0;
      DQ=dat&0x01;
      delay_18B20(5);
      DQ=1;
      dat>>=1;
   }
   return(dat);
}
void ReadTemperature(void)
{
   unsigned char L=0;
   unsigned char H=0;
   Init_DS18B20();
   WriteOneChar(0xCC);//跳过读序号列号的操作
   WriteOneChar(0x44);//启动温度转换
   delay_18B20(100);
   Init_DS18B20();
   WriteOneChar(0xCC);//跳过读序号列号的操作
   WriteOneChar(0xBE);//读取温度寄存器等（共可读9个寄存器前两个就是 温度                
   L=ReadOneChar();
   H=ReadOneChar();
   if(H>0xf8)      	//高位前五位为1时温度是负
   {
		L=~L+1;
		H=~H;    
		f=0;      					//读取温度为负时fg=0
		sdata = L/16+H*16;      	//整数部分
	    xiaoshu1 = (L&0x0f)*10/16; 		//小数第一位
    	xiaoshu2 = (L&0x0f)*100/16%10;	//小数第二位
	}
//传感器返回值除16得实际温度值
//为了得到2位小数位，先乘100，再除16，考虑整型数据长度，
	else
   	{
	    f=1;
        sdata = L/16+H*16;           	//整数部分
    	xiaoshu1 = (L&0x0f)*10/16; 		//小数第一位
    	xiaoshu2 = (L&0x0f)*100/16%10;	//小数第二位
	}
}
///////////////////18b20结束/////////////
void display0()//显示welcome   to use the  
{
       unsigned char i,w,h=0x8f,i1,w1,h1=0xcf,w2,i2,h2,g=2;
       init();
	   for(w=0;w<16;w++)  //显示欢迎
	   {
	       write_com(h--);//第一行自减左移
	       for(i=0;i<16;i++)
	       {
	          write_data(huany[i]);
			  delay_50us(15);
	       }
	   }
	   for(w1=0;w1<16;w1++)//第二行自减左移
	   {
	       write_com(h1--);
	       for(i1=0;i1<16;i1++)
	       {
	          write_data(tt[i1]);
			  delay_50us(15);
	       }
	   }
       delay_50us(5000);//显示loading
	   init(); 
	   write_com(0x80);
	   for(w2=0;w2<16;w2++)//向右显示
	   {	    
           write_data(duqu[w2]);
		   delay_50us(200);	       
	   }
	   while(g--)//小数点循环亮灭
	   {   
	       write_com(0x8a);
	       for(i2=0;i2<3;i2++)
	       {
		      write_data(duqu[i2]);
			    
		   }
		   write_com(0x8a);
	       for(h2=10;h2<14;h2++)
		   {  
		      delay_50us(300);
		      write_data(duqu[h2]);
			  delay_50us(300);
		   }
	   }
      
}
void display1()	//显示框架
{
       unsigned char p,q;
       init();//全局清零
       write_com(0x80);
       for(q=0;q<16;q++)//显示TMHL
       {
          write_data(tab1[q]);
       }  
       write_com(0xc1);
       for(p=0;p<16;p++)  
       write_data(tab2[p]);
}
void display2()//显示上下限温度	,尺度
{
   //尺度
 
      write_com(0x8e);
      write_data(shuzu[m/10]);//十位
      write_com(0x8f);
      write_data(shuzu[m%10]);//个位 
   if(o==1)		   //上限选中标志*号可操作
   {
     write_com(0xc0);
     write_data(shuzu[14]);
     write_com(0xca);
     write_data(shuzu[12]);
   }
   else if(o==2)	//下限选中标志*号可操作
   {
      write_com(0xca);
      write_data(shuzu[14]);
      write_com(0xc0);
      write_data(shuzu[12]);
   }
   else if(o==0)	 //隐藏*号不可操作
   {
      write_com(0xca);
      write_data(shuzu[12]);
      write_com(0xc0);
      write_data(shuzu[12]);
   }
   if(u/100==0)	 //上限100内温度
   {
      write_com(0xc3);
      write_data(shuzu[12]);//空格
      write_com(0xc4);
      write_data(shuzu[u/10]);//十位
      write_com(0xc5);
      write_data(shuzu[u%10]);//个位
   }
   else if (u/100==1)//上限100外温度
   {
      write_com(0xc3);
      write_data(shuzu[u/100]);//百位
      write_com(0xc4);
      write_data(shuzu[(u-100)/10]);//十位
      write_com(0xc5);
      write_data(shuzu[u%10]);//个位
   }
 //下限温度
   if (l<0)//下限低于0的温度显示
   {
      p=-l;
      write_com(0xcD);
      write_data(shuzu[11]);//负号
      write_com(0xcE);
      write_data(shuzu[p/10]);
      write_com(0xcF);
      write_data(shuzu[p%10]);
   }
   else if(l>=0)//下限高于0的温度显示
   {  
      write_com(0xcD);
      write_data(shuzu[12]);	 
      write_com(0xcE);
      write_data(shuzu[l/10]);
      write_com(0xcF);
      write_data(shuzu[l%10]);
   } 
}  
void fuhao()	//温度正负子程序
{
  	  if(f==1) //温度为正
	 {
	    write_com(0x83);
        write_data(shuzu[12]);
	 }
	 else if(f==0) //温度为负，显示负号
	 {
	    write_com(0x83);
        write_data(shuzu[11]);
	 }
}
void display(unsigned int date)	 //显示温度，
{
      if(date/100==0) //100度内
      {
         write_com(0x84);
         write_data(shuzu[date/10]);//十位
         write_com(0x85);
         write_data(shuzu[date%10]);//个位
         write_com(0x86);
         write_data(shuzu[13]);//小数点
         write_com(0x87);
         write_data(shuzu[xiaoshu1]);//小数点后一位
         write_com(0x88);
         write_data(shuzu[xiaoshu2]); //小数点后二位
      }
      else if(date/100==1)	  //100度上
      {
         write_com(0x83);
         write_data(shuzu[date/100]);//百位
         write_com(0x84);
         write_data(shuzu[(date-100)/10]);//十位
         write_com(0x85);
         write_data(shuzu[date%10]);//个位
         write_com(0x86);
         write_data(shuzu[13]);//点
         write_com(0x87);
         write_data(shuzu[xiaoshu1]);//小数点后一位
         write_com(0x88);
         write_data(shuzu[xiaoshu2]);//小数点后二位
      }
}

void key()		//按键加减标尺

{ 
 //尺度
   if(keym==0)//判定
   {
     delay_50us(100);//延时
     if(keym==0)
     {	
       y++;//转换加减的数值
	   if(y>=3)y=0;
	   switch (y)
	   {
	   case 0:m=1;break;
	   case 1:m=5 ;break;
	   case 2:m=10 ;break;
	   }
	   while(keym==0);//自锁
     }
   }
 ///换
   if(keyh==0)//上下限转换按键
   {
     delay_50us(100);
     if(keyh==0)
     {
       o++;
   	   if(o>=3)o=0;//1为上限选中，2为下限选中，0为隐藏
  	   while(keyh==0);
     }
   }
/////上限	  
   if(keyu==0&o==1)//上限加按键
   {
     delay_50us(100);
     if(keyu==0&o==1)
     {
        u=u+m; while(keyu==0);
     }
   }
   if(keyl==0&o==1) //上限减按键

   {
      delay_50us(100);
      if(keyl==0&o==1)
      {
        u=u-m; while(keyl==0);
      }
   }
//// 下限
   if(keyu==0&o==2)//下限加按键
   {
      delay_50us(100);
      if(keyu==0&o==2)
      {
         l=l+m;while(keyu==0);
      }
   }
   if(keyl==0&o==2)//下限减按键
   {
      delay_50us(100);
      if(keyl==0&o==2)
      {
        l=l-m;while(keyl==0);
      }
   } 
}

 void  BJ()//温度报警子程序
 {
  	if(f==1)	 //温度大于0的温度比较
	 {
   	     
	        if(sdata>=u) //温度大于上限，报警
	        {
	            display(sdata);	button();
	        }
	        else if(l>0&sdata<=l)	//温度小于下限，报警
	        {
	           display(sdata);	button();
	        }
	        else 	 display(sdata);
	 }
	 else if(f==0)	//温度小于0的温度比较
	 {
		    if(l>=0) 
	        {
	            display(sdata);	button();
	        }
	        else if(l<0)	
	        {
	            if(sdata>=-l)  //温度于下限，报警
				{ 
					 display(sdata);	button();
				}
				else if(sdata<-l)  //温度大于下限不报警
				{
				     display(sdata);
				}
	        }
	        else 	 display(sdata);          	       
	 }
 }
void main()
{
  buzz=1;
  display0(); //欢迎，
  display1();//框架
  while(1)
  {
     display2(); //上下限，尺度
   	 ReadTemperature();	//测温程序
	 fuhao();//负号显示程序
	 key();	//按键
	 BJ();	//报警程序
  }
 
}
/*元器件列表
	材料				数量
   LCD1602                1
   蜂鸣器                 1
   USB母座                1	 
   89c52				  1
   9脚103排阻             1
   电阻10K                1
   电阻1k   			  1
   电阻4.7K               2
   电位器10k			  1
   电解电容10uf			  1
   电容30pf	              2
   11.0592晶振            1
   三极管pnp8550		  1
   四脚按键               5
   排针					  8
   排针母座20p			  1
   稳定传感器18B20		  1
   插座40脚               1
   覆铜板15X20            1
   三脚开关               1
 */