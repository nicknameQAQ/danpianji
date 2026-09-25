//************************************************************************************************************************************
//库调用.宏类型定义.位定义//
#include <reg52.h>			  
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
#define GPIO_KEY P1
sbit RS = P2^2; 			  //液晶显示器控制端口
sbit RW = P2^3; 
sbit E = P3^7;
sbit shuchu=P2^0;			  //继电器控制端口
sbit beep=P2^1;				  //报警器端口
sbit DoorBell=P3^6;			  //门铃端口
uchar upxs[16]={"Password:"}; //上排存放数组
uchar dwxs[16]={""};          //下排存放数组
int tempword[8]={0}; 		  //临时密码数组
int password[8]={1,1,2,2,3,3,4,4};//初始密码
uint now=0;					  //数组等效指针
uint key=0;                   //密钥
uint KeyValue=16;             //用来存放读取到的键值
uint warn=0;				  //错误累计
uint time=0;				  //用于定时
uint time1=0;                 //用于定时
uint time2=0;                 //用于定时
uint moshi=0;				  //定时器内部模式
//*********************************************************************************************************************************
//液晶驱动显示模块//
void delay(uint dell)//毫秒延时//
{
     uchar x;
     for(dell;dell>0;dell--)
     for(x=110;x>0;x--);
}
void wr_com(uchar temp)//写指令//
{
     RS=0;
     RW=0;
     P0=temp;
     E=1;
     E=0;
}
void wr_data(uchar num)//写数据//
{
     RS=1;
     RW=0;
     P0=num;
     E=1;
     E=0;
}
void disp_lcd(uchar addr,char *templ)//显示函数//
{
     uchar i;
     wr_com(addr);
	 delay(50);
     for(i=0;i<16;i++)
    {
     wr_data(templ[i]);
	 delay(50);
    }	 
}
void lcd_init()//初始化液晶//
{
     wr_com(0x30);
     delay(50);
     wr_com(0x38);
     delay(50);
     wr_com(0x01);
     delay(50);
     wr_com(0x06);
     delay(50);
     wr_com(0x0c);
     delay(50);
}
void lcdxianshi(void)//液晶总显示//
{
     disp_lcd(0x80,upxs);
     disp_lcd(0xc0,dwxs);
}
//*********************************************************************************************************************************
//按键扫描模块//
void Delay10ms(void)//延时函数//
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}
void KeyDown(void)//键盘检测函数//
{
	char a=0;
	GPIO_KEY=0x0f;
	if(GPIO_KEY!=0x0f)     //读取按键是否按下
	{
		Delay10ms();       //延时10ms进行消抖
		if(GPIO_KEY!=0x0f) //再次检测键盘是否按下
		{
			
			//检测列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//检测行
			GPIO_KEY=0XF0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=KeyValue;break;
				case(0Xb0):	KeyValue=KeyValue+4;break;
				case(0Xd0): KeyValue=KeyValue+8;break;
				case(0Xe0):	KeyValue=KeyValue+12;break;
			}
			while((a<10)&&(GPIO_KEY!=0xf0))//按键松手检测
			{
				Delay10ms();
				a++;
			}
		}
	}
}
//*******************************************************************************************************************************
//数据处理模块//
void keysend(void)//键值传递//
{
		  switch(KeyValue)
	      {
	       case 0:{dwxs[now]='0';tempword[now]=0;now++;break;}
		   case 1:{dwxs[now]='1';tempword[now]=1;now++;break;}
		   case 2:{dwxs[now]='2';tempword[now]=2;now++;break;}
		   case 3:{dwxs[now]='3';tempword[now]=3;now++;break;}
		   case 4:{dwxs[now]='4';tempword[now]=4;now++;break;}
		   case 5:{dwxs[now]='5';tempword[now]=5;now++;break;}
		   case 6:{dwxs[now]='6';tempword[now]=6;now++;break;}
		   case 7:{dwxs[now]='7';tempword[now]=7;now++;break;}
		   case 8:{dwxs[now]='8';tempword[now]=8;now++;break;}
		   case 9:{dwxs[now]='9';tempword[now]=9;now++;break;}
		   case 11:{
		   		  upxs[12]='S';
				  upxs[13]='U';
				  upxs[14]='R';
				  upxs[15]='E'; break;
		   }
		   case 15:{
		          TR0=1;		     //定时器0打开
		   		  moshi=1;			 //模式1为门铃模式
				  KeyValue=16; break;//键值缓存清零
		   }
		  }
		  if(now==8) now=0;          //只显示八位
}
void sure(void)//密码认证函数//
{
     uchar i;
     if(KeyValue==11)
	 {
	     for(i=0;i<8;i++)
		 {
		 	if(password[i]!=tempword[i])//对比密码
			{
			    key++;
			}
		 }	 	  
		 if(key==0)                //密码正确，输出OK 
			   {
				dwxs[13]='O';
				dwxs[14]='K';
			    shuchu=1;		  //开门
				TR1=1;            //定时器1打开，启动倒计时
				moshi=0;          //输入正确密码可关闭报警模式
				beep=0;           //蜂鸣器复位
				TR0=0;			  //并且关闭定时器0
			   }
		 if(key!=0)               //密码错误，次数累加
			{
				dwxs[12]='E';
				dwxs[13]='R';
				dwxs[14]='O';
				dwxs[15]='R';
				warn++;
				if(warn==3)		  //输入错误3次，报警
				{
				   TR0=1;		  //定时器0启动
			       moshi=2;		  //模式2启动报警
				   warn=0;		  //累计清零
				}
			}
	 }
}
void delete(void)//清除函数//
{
     uchar i;
	     for(i=0;i<16;i++)
		 {
		    dwxs[i]=0x20;	 //密码字符清零
		 }
		 for(i=0;i<8;i++)
		 {
		    tempword[i]=0;	 //密码缓存数组清零
		 }
		    upxs[12]=0x20;   //SURE显示清零
			upxs[13]=0x20;
			upxs[14]=0x20;
			upxs[15]=0x20;
		 now=0;				 //类指针复位
		 key=0;				 //密钥清零
		 KeyValue=16;		 //键值缓存清零
}
void condition_delete(void)  //条件清除
{
     if(KeyValue==10)
	 {
		delete();            //按下DELE键清除数据
	 }
	 KeyValue=16;			 //键值缓存清零
}
//********************************************************************************************************************************8
//定时模块//
void timer0() interrupt 1    //定时器0用于门铃
{
    switch(moshi)
   {
      case 1:{
	        DoorBell=~DoorBell;
            time++;
             if(time<400)             //若需要拖长声音，可以调整 400 和 800
            {
               TH0=(8192-700)/32;     //700us 定时
               TL0=(8192-700)%32;
            }
             else if(time<800)
            {
               TH0=(8192-1000)/32;    //1ms 定时
               TL0=(8192-1000)%32;
            }
             else
            {
               time=0;				  //计数清零
               TR0=0;				  //关闭定时器
			   moshi=0;				  //跳出模式1
            }
			break;
	  }
	  case 2:{
	  		 beep=~beep;
			 time++;
			 if(time<400)             //若需要拖长声音，可以调整 400
            {
               TH0=(8192-700)/32;     //700us 定时
               TL0=(8192-700)%32;
            }
			else
			{
			   time=0;				  //计数清零
			}
			break;
	  }
   }
}
void timer1() interrupt 3 	 //定时器1用于自动关门
{  
    TH0=(65536-50000)/256;  
    TL0=(65536-50000)%256;  
    time1++;   
    if(time1==20)//一秒  
    {      
      time2++;
	  time1=0;  
    } 
}
//**********************************************************************************************************************************
//主函数//
void main(void)//主函数
{
    TMOD=0x10; 				 //0001 0000//定时器1方式1，定时器0方式0
	TH0=(8192-700)/32;		 //定时器0 700us 定时
    TL0=(8192-700)%32;
    TH1=(65536-50000)/256;   //定时器1 50ms
    TL1=(65536-50000)%256;  
    EA=1;  
    ET0=1;
	ET1=1;
    shuchu=0;			    //门锁初始化
	beep=0;				    //警报初始化
	DoorBell=0;				//门铃初始化
    lcd_init();			    //液晶初始化
	while(1)
	{
		KeyDown();		    //按键检测
		keysend();		    //数据传递
		lcdxianshi();	    //输入显示
		sure();			    //密码认证
		condition_delete(); //按键清除
		if(time2==4)	    //门锁打开后4秒关闭
		{
		   shuchu=0;
		   time2=0;
		   delete();		//自动清除
		   TR1=0; 	        //关闭定时器1
		}		
	}				
}
//************************************************************************************************************************************8