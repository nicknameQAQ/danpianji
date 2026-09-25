//************************************************************************************************************************************
//库调用.宏类型定义.位定义//
//************************************************************************************************************************************
#include <reg52.h>			            
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
#define ushort unsigned short

#define GPIO_KEY P1                 //矩阵键盘烧扫描端口
sbit RS = P2^2; 			              //液晶显示器控制端口
sbit RW = P2^3; 
sbit E = P3^7;
sbit shuchu=P2^0;			              //继电器控制端口
sbit beep=P2^1;				              //报警器端口
sbit DoorBell=P3^6;			            //门铃端口
sbit SCL = P2^6;                    //AT24C04的时钟
sbit SDA = P2^7;                    //AT24C04的数据

uchar idata upxs[16]={"Password:"}; //上排存放数组
uchar idata dwxs[16]={""};          //下排存放数组
uint idata tempword[6]={0}; 		    //临时密码数组
uint idata password[6]={1,1,1,1,1,1};//初始密码
uint idata now=0;					          //数组伪指针
uint idata key=0;                   //密钥
uint idata keyboard=0;              //键盘锁定标志
uint idata key1=0;				          //用于密码修改
uint idata KeyValue=16;             //用来存放读取到的键值
uint idata warn=0;				          //错误累计
uint idata time=0;				          //用于定时
uint idata time1=0;                 //用于定时
uint idata time2=0;                 //用于定时
uint idata moshi=0;				          //定时器内部模式
uchar idata BUF[8];                 //掉电数据缓存区
uchar idata TESTDATA[8]={0};        //掉电新密码存储
uint idata menu=0;			            //menu用于密码修改
uint idata tempword1[8]={0};        //用于密码修改
//*********************************************************************************************************************************
//函数声明//
//*********************************************************************************************************************************
void delete(void);
//*********************************************************************************************************************************
/*模块名称：液晶驱动显示模块*/
/*模块功能：驱动LCD1602的显示，并时时显示按键值*/
//*********************************************************************************************************************************
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
/*模块名称：矩阵键盘扫描模块*/
/*模块功能：时时扫描按键值*/
//*********************************************************************************************************************************
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
		Delay10ms();         //延时10ms进行消抖
		if(GPIO_KEY!=0x0f)   //再次检测键盘是否按下
		{
			
			//扫描列
			GPIO_KEY=0X0F;
			switch(GPIO_KEY)
			{
				case(0X07):	KeyValue=0;break;
				case(0X0b):	KeyValue=1;break;
				case(0X0d): KeyValue=2;break;
				case(0X0e):	KeyValue=3;break;
			}
			//扫描行
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
/*模块名称：数据处理模块*/
/*模块功能：（1）将按键值传递给各种缓存数祝（2）包含数据清除函数*/
//*******************************************************************************************************************************
void keysend(void)//键值传递//
{
		 switch(KeyValue)
	   {
	     case 0:{dwxs[now]='0';tempword[now]=0;TESTDATA[now]=0;now++;break;}
		   case 1:{dwxs[now]='1';tempword[now]=1;TESTDATA[now]=1;now++;break;}
		   case 2:{dwxs[now]='2';tempword[now]=2;TESTDATA[now]=2;now++;break;}
		   case 3:{dwxs[now]='3';tempword[now]=3;TESTDATA[now]=3;now++;break;}
		   case 4:{dwxs[now]='4';tempword[now]=4;TESTDATA[now]=4;now++;break;}
		   case 5:{dwxs[now]='5';tempword[now]=5;TESTDATA[now]=5;now++;break;}
		   case 6:{dwxs[now]='6';tempword[now]=6;TESTDATA[now]=6;now++;break;}
		   case 7:{dwxs[now]='7';tempword[now]=7;TESTDATA[now]=7;now++;break;}
		   case 8:{dwxs[now]='8';tempword[now]=8;TESTDATA[now]=8;now++;break;}
		   case 9:{dwxs[now]='9';tempword[now]=9;TESTDATA[now]=9;now++;break;}
		   case 11:{
		   		     upxs[12]='S';
				       upxs[13]='U';
				       upxs[14]='R';
				       upxs[15]='E'; break;
		   }
		   case 14:{
		           menu++;			 //密码修改模式启动模式一
				  if(menu>1)		     //再次按下跳出密码修改模式
				  {
				  	 upxs[0]='P';	   //显示“password”
		             upxs[1]='a';
		             upxs[2]='s';
		             upxs[3]='s';
		             upxs[4]='w';
		             upxs[5]='o';
		             upxs[6]='r';
		             upxs[7]='d';
			         upxs[8]=':';
					 delete();
					 menu=0;
				  }
				  break;
		   }
		   case 15:{
		          TR0=1;		     //定时器0打开
		   		  moshi=1;			   //模式1为门铃模式
				  KeyValue=16; break;//键值缓存清零
		   }
		  }
		  if(now==6) now=0;      //只存储8位，只显示八位
}
void delete(void)//清除函数//
{
     uchar i;
	     for(i=0;i<16;i++)
		 {
		    dwxs[i]=0x20;	    //密码字符清零
		 }
		 for(i=0;i<6;i++)
		 {
		    tempword[i]=0;	 //密码缓存数组清零
		 }
		  upxs[11]=0x20;
		  upxs[12]=0x20;   //SURE显示清零
			upxs[13]=0x20;
			upxs[14]=0x20;
			upxs[15]=0x20;
		 now=0;				       //伪指针复位
		 key=0;				       //密钥清零
		 KeyValue=16;		     //键值缓存初始
}
void condition_delete(void) //条件清除
{
     if(KeyValue==10)
	 {
		delete();            //按下DELE键清除数据
	 }
	 KeyValue=16;			     //键值缓存清零
}
//*********************************************************************************************************************************
/*模块名称：定时器模块*/
/*模块功能：定时器0用于产生门铃效果，定时器1用于各种倒计时*/
//*********************************************************************************************************************************
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
			   moshi=0;				     //跳出模式1
            }
			break;
	  }
	  case 2:{
	  		 beep=~beep;
			 time++;
			 if(time<400)             //若需要拖长声音，调整 400
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
/*模块名称帜：掉电模块*/
/*模块功能：支持门锁掉电后密码可保存而不丢失*/
//**********************************************************************************************************************************
//延时5微秒(STC90C52RC@12M)，此延时对于12M晶振
void Delay5us()
{
    _nop_();
    _nop_();
}
//延时5毫秒(STC90C52RC@12M)
void Delay5ms()
{
    ushort n = 560;
    while (n--);
}
//起始信号
void AT24C04_Start()
{
    SDA = 1;                    //拉高数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 0;                    //产生下降沿
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
}
//停止信号
void AT24C04_Stop()
{
    SDA = 0;                    //拉低数据线
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SDA = 1;                    //产生上升沿
    Delay5us();                 //延时
}
//发送应答信号
void AT24C04_SendACK(bit ack)
{
    SDA = ack;                  //写应答信号
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时
}
//接收应答信号
bit AT24C04_RecvACK()
{
    SCL = 1;                    //拉高时钟线
    Delay5us();                 //延时
    CY = SDA;                   //读应答信号
    SCL = 0;                    //拉低时钟线
    Delay5us();                 //延时

    return CY;
}
//向IIC总线发送一个字节数据
void AT24C04_SendByte(uchar dat)
{
    uchar i;

    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移出数据的最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    AT24C04_RecvACK();
}
//向AT24C04写1页(16字节)数据,将TESTDATA开始的16个测试数据写如设备的00~0F地址中
void AT24C04_WritePage()
{
    uchar i;

    AT24C04_Start();            //起始信号
    AT24C04_SendByte(0xa0);     //发送设备地址+写信号
    AT24C04_SendByte(0x00);     //发送存储单元地址
    for (i=0; i<16; i++)
    {
        AT24C04_SendByte(TESTDATA[i]);
    }
    AT24C04_Stop();             //停止信号
}
//从IIC总线接收一个字节数据
uchar AT24C04_RecvByte()
{
    uchar i;
    uchar dat = 0;

    SDA = 1;                    //使能内部上拉,准备读取数据
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        dat |= SDA;             //读数据
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }

    return dat;
}
//从AT24C04读取1页(16字节)数据,将设备的00~0F地址中的数据读出存放在DATA区的BUF中
void AT24C04_ReadPage()
{
    uchar i;

    AT24C04_Start();            //起始信号
    AT24C04_SendByte(0xa0);     //发送设备地址+写信号
    AT24C04_SendByte(0x00);     //发送存储单元地址
    AT24C04_Start();            //起始信号
    AT24C04_SendByte(0xa1);     //发送设备地址+读信号
    for (i=0; i<8; i++)
    {
        BUF[i] = AT24C04_RecvByte();
        if (i == 7)
        {
            AT24C04_SendACK(1); //最后一个数据需要会NAK
        }
        else
        {
            AT24C04_SendACK(0); //回应ACK
        }
    }
    AT24C04_Stop();             //停止信号
}
//********************************************************************************************************************************
/*模块名称：菜单模块*/
/*模块功能：用于密码修改，先输入旧密码，密码正确两次输入新密码*/
//********************************************************************************************************************************
void new_password(void)//密码修改函数
{
     uchar i,j;
	   uchar tempxs1[]={"old mima   MENUi"};//提示：输入旧密码
     uchar tempxs2[]={"new mima   FIRST"};//提示：输入新密码一次
     uchar tempxs3[]={"new mima   AGAIN"};//提示：输入新密码二次
     uchar tempxs4[]={"password:       "};//password显示
     uchar tempxs5[]={"success  change "};//修改成功显示
     switch(menu)
	 {
	  case 1:{
		for(j=0;j<16;j++){
    upxs[j]=tempxs1[j];
   }
		if(tempword[5]!=0)
		{
		  AT24C04_ReadPage();//读取密码
		  for(i=0;i<8;i++)
		  {
		 	if(BUF[i]!=0)//对比密码,判断是不是第一次使用
			{
			    key1++;
			}
		  }
		  if(key1==0)//如果是第一次使用
		  {
		     for(i=0;i<6;i++)
		    {
		 	    if(password[i]!=tempword[i])//对比初始密码
			   {
			    key++;
			   }
		    }
			if(key==0)//密码正确进入菜单二输入新密码
		    {
		       delete();
			   for(i=0;i<8;i++)
			   {
			   	  tempword[i]=0;//清空缓存数组
			   }
			   key=0;
			   key1=0;
		       menu=2;
		    }
		  }
		  if(key1!=0)//如果不是第一次使用
		  {
		  	for(i=0;i<8;i++)
		    {
			    password[i]=BUF[i];
		 	    if(password[i]!=tempword[i])//对比密码
			    {
			       key++;
			    }
		    }
			if(key==0)//密码正确进入菜单二输入新密码
		    {
		       delete();
			   for(i=0;i<8;i++)
			   {
			   	  tempword[i]=0;//清空缓存数组
			   }
			   key=0;
			   key1=0;
		       menu=2;
		    }
		  }
		}
		break;
		}
	  case 2:{
	  for(j=0;j<16;j++){
    upxs[j]=tempxs2[j];
   }
		if(tempword[5]!=0)
		{
		   for(i=0;i<8;i++)
		   {
		       tempword1[i]=tempword[i];
		   }
		   delete();
			   for(i=0;i<8;i++)
			   {
			   	  tempword[i]=0;//清空缓存数组
			   }
		       menu=3;			//进入菜单三再次输入新密码
		}
		break;
		}
	  case 3:{
	  for(j=0;j<16;j++){
    upxs[j]=tempxs3[j];
   }
		if(tempword[5]!=0)
		{
		   for(i=0;i<8;i++)
		   {
		      if(tempword1[i]!=tempword[i])//对比密码
			 {
			     key++;
			 }
		   }
		   if(key==0)//如果第二次输入的密码与第一次相同
		   {
		       AT24C04_WritePage();  //写入新密码
			   for(i=0;i<8;i++)
			   {
			   	  tempword[i]=0;//清空缓存数组
				  tempword1[i]=0;//清空缓存数组
			   }
			  for(j=0;j<16;j++){
    upxs[j]=tempxs4[j];
		dwxs[j]=tempxs5[j];
   }
			key=0;
			menu=0;      //跳出密码修改菜单
		  TR1=1;
			if(time2==2)	         //显示2秒
		 {
			 delete();
		   TR1=0; 	           //关闭定时器1
		 }	
		   }
		   if(key!=0)//如果第二次输入的密码与第一次不同
		   {
		   	   for(i=0;i<8;i++)
			   {
			   	  tempword[i]=0;//清空缓存数组
				  tempword1[i]=0;//清空缓存数组
			   }
			   dwxs[15]='n';
			   dwxs[14]='i';
		   	   dwxs[13]='a';
			   dwxs[12]='g';
			   dwxs[11]='a';
			  key=0;
			  menu=2;             //跳转到菜单2
		   }
		}
		}
	 }
	 KeyValue=16;             //键值初始
}
//********************************************************************************************************************************
/*模块名称：单函数区*/
/*模块功能：一些单独功能的函数*/
//********************************************************************************************************************************
void sure(void)//密码认证函数//
{
     uchar i;
    if(KeyValue==11)
	 {	  
	 AT24C04_ReadPage();
	     for(i=0;i<6;i++)
		 {	password[i]=BUF[i];
		 	if(password[i]!=tempword[i])//对比密码
			{
			    key++;
			}
		 }	 	  
		 if(key==0)                //密码正确，输出OK 
			   {
				dwxs[13]='O';
				dwxs[14]='K';
			  shuchu=1;		    //开门
				TR1=1;            //定时器1打开，启动倒计时
				moshi=0;          //输入正确密码可关闭报警模式
				beep=0;           //蜂鸣器复位
				TR0=0;			      //并且关闭定时器0
			   }
		 if(key!=0)           //密码错误，次数累加
			{
				dwxs[12]='E';
				dwxs[13]='R';
				dwxs[14]='O';
				dwxs[15]='R';
				warn++;
				if(warn==3)		    //输入错误3次，报警
				{
					 keyboard=1;    //三次错误则键盘锁住
				   TR0=1;		      //定时器0启动
			     moshi=2;		    //模式2启动蜂鸣器报警
				   warn=0;		    //累计清零
					 TR1=1;	        //启动键盘锁定倒计时
				}
			}
	 }
}
//************************************************************************************************************************************
//附功能函数//
//************************************************************************************************************************************
void Add_Function(void)
{
	  switch(time2)
	{
    case 4:{	             //门锁打开后4秒关闭
		   shuchu=0;           //关闭门锁电源
		   delete();		       //自动清除
		   if(keyboard==0)     //如果不是密码错误开启的定时器则关闭定时器1
			{
				 time2 =0;         //time2复位
				 TR1=0;            //定时器1关闭
      }
      break;			
		}
    case 60:{	             //键盘锁定一分钟
			 keyboard=0;         //键盘解锁
			 time2 =0;           //time2复位
		   TR1=0; 	           //定时器1关闭
			break;
		}
	}
}
//************************************************************************************************************************************
//系统初始化函数//
//************************************************************************************************************************************
void System_init(void)
{
    TMOD=0x10; 				      //0001 0000//定时器1方式1，定时器0方式0
	  TH0=(8192-700)/32;		  //定时器0 700us 定时
    TL0=(8192-700)%32;
    TH1=(65536-50000)/256;  //定时器1 50ms
    TL1=(65536-50000)%256;  
    EA=1;  
    ET0=1;
	  ET1=1;
    shuchu=0;			         //门锁初始化
	  beep=0;				         //警报初始化
	  DoorBell=0;				     //门铃初始化
    lcd_init();			       //液晶初始化
}
//************************************************************************************************************************************
/*模块名称：主函数*/
/*模块功能：程序入口*/
//************************************************************************************************************************************
void main(void)//主函数
{
    
	//AT24C04_WritePage();   //密码初始化（只需使用一次写入初始密码）
	System_init();           //系统初始化
	while(1)
	{	    
		if(keyboard==0)        //如果密码错误三次则锁定键盘
	 {
		  KeyDown();		       //按键检测
	 }
		keysend();		         //数据传递
		lcdxianshi();	         //输入显示
		sure();			           //密码认证
		condition_delete();    //按键清除
		new_password();        //密码修改
    Add_Function();	       //附功能函数
	}				
}
//************************************************************************************************************************************
//结束
//************************************************************************************************************************************