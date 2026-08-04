#include <reg52.h>
unsigned long int fre; 
unsigned char time;
unsigned int count;
unsigned int count1;
sbit   LCD_RS=P1^0;	  //片选信号 
sbit   LCD_RW=P1^1;	  //读写信号 
sbit   LCD_E=P1^2; 	  //使能信号 
#define LCD_DB  P0	//数据信号 
unsigned char character[10]={0};//在屏幕上显示的字符串 
unsigned char character_1[]={"fre= Hz"};
unsigned char FLAG = 0;  //是否分频选择标志
void LCD_init(void);//初始化函数   
void LCD_write_command(unsigned char command);//写指令函数	 
void LCD_write_data(unsigned char dat);//写数据函数	   
void LCD_disp_char(unsigned char x,unsigned char y,unsigned char dat);//在某个屏幕位置上显示一个字符,X（0-15),y(1-2)
void delay_n40us(unsigned int n);//延时函数
void timer_init();            //中断初始化函数  
//-----------------------------延时函数---------------------------
void delay_n40us(unsigned int n)		  //延时函数 
{
	unsigned int i;
	unsigned char j;            
	for(i=n;i>0;i--)
	for(j=0;j<2;j++);          
} 
void delay_1s()
{
	unsigned int i,j;
	for(i = 0;i<100;i++)
		for(j = 0;j<1000;j++);
} 
//---------------------lcd1602液晶显示函数--------------------                                     
void LCD_init(void)	 //液晶初始化函数 
{
	LCD_write_command(0x38);//设置8位格式，2行，5x7
	LCD_write_command(0x38);//设置8位格式，2行，5x7
	LCD_write_command(0x38);//设置8位格式，2行，5x7
	LCD_write_command(0x0c);//整体显示，关光标，不闪烁 
	LCD_write_command(0x06);//设定输入方式，增量不移位
	LCD_write_command(0x01);//清除屏幕显示	  
	delay_n40us(100);//清屏延时 
} 
void LCD_write_command(unsigned char dat)  //写命令函数 
{
	LCD_DB=dat;
	LCD_RS=0;//指令	 
	LCD_RW=0;//写入	 
	LCD_E=1; //使能 	 
	LCD_E=0;
	delay_n40us(1);//写命令延时   
}	 
void LCD_write_data(unsigned char dat)	  //写数据函数 
{
	LCD_DB=dat;
	LCD_RS=1;//数据	  
	LCD_RW=0;//写入	  
	LCD_E=1;//使能  
	LCD_E=0;
	delay_n40us(1);	 //写数据延时 
}
void LCD_disp_char(unsigned char x,unsigned char y,unsigned char dat)//显示一个字符 
{
	unsigned char address;
	if(y==1)
	         address=0x80+x;	 //显示在第一排的时候的x的地址 
	else
	         address=0xc0+x;	 //显示在第二排的时候的x的地址 
	LCD_write_command(address);	 //输入地址 
	LCD_write_data(dat);		 //输入数据 
}
void LCD_disp_num(unsigned char x,unsigned char y,unsigned char dat)//显示一个数字
{
	unsigned char address;
	if(y==1)
	         address=0x80+x;	 //显示在第一排的时候的x的地址 
	else
	         address=0xc0+x;	 //显示在第二排的时候的x的地址 
	LCD_write_command(address);	 //输入地址 
	LCD_write_data(dat+48);		 //输入数据 
}
void dis_num(void)
{
	unsigned char i=0,j=0,k=0;
	LCD_write_command(0x01);//清除屏幕显示
	character[0] = fre/10000000;
	character[1] = fre/1000000%10;
	character[2] = fre/100000%10;
	character[3] = fre/10000%10;
	character[4] = fre/1000%10;
	character[5] = fre/100%10;
	character[6] = fre/10%10;
	character[7] = fre%10;
    character[8] = 'H';
	character[9] = 'z';
	for(i = 0;i<4;i++)		 //显示“fre=”
	{
	 	LCD_disp_char(i+0,1,character_1[i]);
	}
	for(i = 0;i<10;i++)	 //判断第一个不为0的数
	{
		if(character[i]!=0)
			break;
	}
	k = 8-i;
	for(j = 0;j<k;j++)	 //显示所有的数字
	{
			LCD_disp_num(4+j,1,character[i++]);	
	}
    for(i = 5;i<7;i++)	 //显示“Hz”
	{
	 	LCD_disp_char(j+4,1,character_1[i]);
		j++;
	}
}
//----------------主函数--------------------
void main()
{ 
	unsigned char i;
	LCD_init();
	timer_init();             //定时/计数器初始化 
	for(i = 0;i<4;i++)
	{
	 	LCD_disp_char(i+0,1,character_1[i]);
	}
	while(1)
	{
		dis_num();         //显示
		delay_1s();
	}
}
//-------------------定时/计数器初始化--------------
void timer_init(void)         //定时/计数器初始化
{ 
	TMOD=0x66;                //计数器0和计数器1工作工作方式2，自动重装初值 
	TH0=0;                    //计数器初值为0
	TL0=0;
	TR0=1;                    //计数器开始计数	    
	ET0=1;                    //打开计数器0中断   
	TH1=0;                    //计数器初值为0
	TL1=0;
	TR1=1;                    //计数器开始计数	    
	ET1=1;                    //打开计数器1中断   
	RCAP2H=(65536-62500)/256; //在程序初始化的时候给RCAP2L和RCAP2H赋值， 
	RCAP2L=(65536-62500)%256; //TH2和TL2将会在中断产生时自动使TH2=RCAP2H,TL2=RCAP2L。 
	TH2=RCAP2H;               //12M晶振下每次中断62.5ms(1s=1000ms=62.5ms×16)
	TL2=RCAP2L;
	ET2=1;                    //打开定时器2中断	 
	TR2=1;                    //定时器2开始计时  
	EA=1;                     //开总中断   
} 
//------------------中断函数----------------------
void timer2(void) interrupt 5 //定时器2中断(62.5ms)
{
	time++;
	TF2=0;                    //定时器2的中断标志位TF2不能够由硬件清零，所以要在中断服务程序中将其清零
	if (time==16)             //定时1s时间到  
	{
	   time=0;                //计时清0
	   EA=0;                  //关中断		 
	   fre=(long)count1*256+TL1;     //count*256强制转换成long型，否则将不产生进位~先判断分频后(计数器1)
	   FLAG = 0; 
	  
	   if(fre<2000)//如果不到200KHz则读取分频前(计数器0)频率(200K÷100=2000)
	   {
	   		fre = (long)count*256+TL0;
			FLAG = 1;          
	   } 
		if(!FLAG)
		{
			fre = fre *100;//100分频
		}
	   TL0=0;                 //清零计数器0计数	    
	   TH0=0;
	   TL1=0;
	   TH1=0; 
	   count=0;               //清零计数器0计数	  
	   count1=0;
	   EA=1;                  //开中断	    
	}
} 
//----------------------------------------------------------------
void timer0(void) interrupt 1 //计数器0中断(100分频前)	 
{
	count++;
}		
//----------------------------------------------------------------
void timer1(void) interrupt 3 //计数器1中断(100分频后) 
{
	count1++;
}