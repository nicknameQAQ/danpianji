/*作者：王先生
51单片机：清华电子的板
联系方式：QQ：2019529860*/
#include<reg52.h>
#include<intrins.h>
#include<math.h>

typedef unsigned char uchar;
typedef unsigned int  uint;

sbit DS = P2^2;//DS18B20接口
sbit BP = P2^3;//蜂鸣器
sbit DU = P2^6;//数码管段选码
sbit WE = P2^7;//数码管位选码
sbit key_s1 = P3^0;
sbit key_s2 = P3^1;
sbit key_s3 = P3^2;
sbit EN = P3^4;//LCD使能端
sbit RS = P3^5;//LCD数据命令选择端
sbit RW = P3^6;//LCD读写选择端

uint FF;
uint DD,LL;
uint K=0;
uint M=0;
uint s=0;

void delayus(uchar us)
{
	while(us--);//us--要6.5us,进入一次函数11.95us;
} //微秒延时	
void LCD_READ_BUSY() //忙不忙
{
	uchar busy;
	P0 = 0XFF;
	RS = 0;
	RW = 1;
	do
	{
		EN = 1;
		busy = P0;
		EN = 0;
	}while(busy&0X80);
	EN = 1;
}	  
void LCD_WRITE_CMD(uchar cmd)//写一个字节命令流程
{
	LCD_READ_BUSY();//判断忙不忙
	RS = 0;
	RW = 0;
	P0 = cmd;
	EN = 1;
	EN = 0;
}
void LCD_WRITE_DATE(uchar dat)//写一个字节数据
{
	LCD_READ_BUSY();
	RS = 1;
	RW = 0;
	P0 = dat;
	EN = 1;
	EN = 0;
}
void LCD_OneChar(uchar x, uchar y,uchar dat)//指定位置显示字符
{
	if(y)	
	x |= 0x40;//位选或
	x |= 0x80;
	LCD_WRITE_CMD(x);
	LCD_WRITE_DATE(dat);		
}
void LCD_Str(uchar x, uchar y, uchar *str)//指定位置显示字符串
{
	if(y) 
	x |= 0x40;
	x |= 0x80;
	LCD_WRITE_CMD(x);
	while(*str != '\0')
	{
		LCD_WRITE_DATE(*str++);
	}
}
void LCD_Init()	 //	初始化
{
	LCD_WRITE_CMD(0x38); //	设置16*2显示，5*7点阵，8位数据接口
	LCD_WRITE_CMD(0x0c); //开显示
	LCD_WRITE_CMD(0x06); //读写一字节后地址指针加1
	LCD_WRITE_CMD(0x01); //清除显示
}

bit ds_init()//单总线初始化程序
{
	bit  i;
	DS = 1;
	_nop_();
	DS = 0;
	delayus(75);//拉低总线;保持480us以上，75*6.5+11.95=499.45ms
	DS = 1;//释放总线
	delayus(1);//1*6.5+11.95=18.45us等待发回信号(15-60us)
	i = DS;
	delayus(20);//读取存在信号的时间20*6.5+11.95=141.95us
	DS = 1;	//释放总线
	_nop_(); 
	return(i);//i=0代表DS18B20已经准备好了
}
void displayN(int c)//温度正常显示
{	
	uint i;
	uchar M1,M2,M3,M4;
	uchar TestStr[] = {"Tem:"};
	uchar TestStr1[] = {"DB:WYX"};
	/*if(c<0)
	{m = 2;}
	else if(c>=0)
	{m = 0;}*/
	i = abs(c);
	M1 = i/1000;//千位
	M2 = i%1000/100;//百位
	M3 = i%1000%100/10;//十位 
	M4 = i%10;	//个位
			
	DU = 0;
	WE = 0;//关闭数码管显示
	LCD_Init();//1602初始化
	LCD_Str(0, 0, &TestStr[0]);	//显示字符串
	LCD_Str(0, 1, &TestStr1[0]);	//显示字符串
	LCD_OneChar(6 , 0, 43+s);//正负号
	LCD_OneChar(7, 0, M1+48); //显示温度
	LCD_OneChar(8, 0, M2+48);
	LCD_OneChar(9 , 0, M3+48);//正负号
	LCD_OneChar(10, 0, 46);
	LCD_OneChar(11, 0, M4+48);
	LCD_OneChar(12, 0, 39);
	LCD_OneChar(13, 0, 67);
	LCD_OneChar(15, 0, 78+DD*10);  //显示蜂鸣器开关
	LCD_OneChar(15, 1, 78+LL*10);  //显示指示灯开关
}
void displayM(int c)
{	
	uchar M1,M2,M3,M4;
	int i,m;
	uchar TestStr[] = {"Tem:"}; 
	uchar TestStr1[] = {"High"};
	if(c<0)
	{m = 2;}
	else if(c>=0)
	{m = 0;}
	i = abs(c);

	M1 = i/1000;//千位
	M2 = i%1000/100;//百位
	M3 = i%1000%100/10;//十位 
	M4 = i%10;	//个位
			
	DU = 0;
	WE = 0;//关闭数码管显示
	LCD_Init();//1602初始化
	LCD_Str(0, 0, &TestStr[0]);	//显示字符串
	LCD_Str(7, 1, &TestStr1[0]);	//显示字符串
	LCD_OneChar(6 , 0, 43+m);//正负号
	LCD_OneChar(7, 0, M1+48); //显示温度
	LCD_OneChar(8, 0, M2+48);
	LCD_OneChar(9 , 0, M3+48);//正负号
	LCD_OneChar(10, 0, 46);
	LCD_OneChar(11, 0, M4+48);
	LCD_OneChar(12, 0, 39);
	LCD_OneChar(13, 0, 67);
	LCD_OneChar(15, 0, 78+DD*10); //显示蜂鸣器开关
	LCD_OneChar(15, 1, 78+LL*10);  //显示指示灯开关
}
void displayL(int c)
{	
	uchar M1,M2,M3,M4;
	int i,m;
	uchar TestStr[] = {"Tem:"}; 
	uchar TestStr1[] = {"Low"};
	if(c<0)
	{m = 2;}
	else if(c>=0)
	{m = 0;}
	i = abs(c);
	M1 = i/1000;//千位
	M2 = i%1000/100;//百位
	M3 = i%1000%100/10;//十位 
	M4 = i%10;	//个位
		
	DU = 0;
	WE = 0;//关闭数码管显示
	LCD_Init();//1602初始化
	LCD_Str(0, 0, &TestStr[0]);	//显示字符串
	LCD_Str(7, 1, &TestStr1[0]);	//显示字符串
	LCD_OneChar(6 , 0, 43+m);//正负号
	LCD_OneChar(7, 0, M1+48); //显示温度
	LCD_OneChar(8, 0, M2+48);
	LCD_OneChar(9 , 0, M3+48);//正负号
	LCD_OneChar(10, 0, 46);
	LCD_OneChar(11, 0, M4+48);
	LCD_OneChar(12, 0, 39);
	LCD_OneChar(13, 0, 67);
	LCD_OneChar(15, 0, 78+DD*10); //显示蜂鸣器开关
	LCD_OneChar(15, 1, 78+LL*10);  //显示指示灯开关
}
void displayNF(int c)
{	
	uchar M1,M2,M3,M4;
	uchar TestStr[] = {"Tem:"};
	uchar TestStr1[] = {"DB:WYX"};
	uint p,i;
	/*if(c<0)
	{m = 2;}
	else
	{m = 0;}*/ 
	i = abs(c);
	p = i*9/5+320;
	M1 = p/1000;
	M2 = p%1000/100;//百位
	M3 = p%1000%100/10;//十位 
	M4 = p%10;	//个位
			
	DU = 0;
	WE = 0;//关闭数码管显示
	LCD_Init();//1602初始化
	LCD_Str(0, 0, &TestStr[0]);	//显示字符串
	LCD_Str(0, 1, &TestStr1[0]);	//显示字符串
	LCD_OneChar(6 , 0, 43+s);//显示正负
	LCD_OneChar(7, 0, M1+48); //显示温度
	LCD_OneChar(8, 0, M2+48);
	LCD_OneChar(9, 0, M3+48);
	LCD_OneChar(10, 0, 46);
	LCD_OneChar(11, 0, M4+48);
	LCD_OneChar(12, 0, 39);
	LCD_OneChar(13, 0, 67+FF);
	LCD_OneChar(15, 0, 78+DD*10);  //显示蜂鸣器开关
	LCD_OneChar(15, 1, 78+LL*10);  //显示指示灯开关
}
void displayMF(int c)
{	
	uchar M1,M2,M3,M4;
	uchar TestStr[] = {"Tem:"}; 
	uchar TestStr1[] = {"High"};
	uint p,m,i;
	if(c<0)
	{m = 2;}
	else
	{m = 0;}
	i = abs(c);
	p = i*9/5+320;
	M1 = p/1000;
	M2 = p%1000/100;//百位
	M3 = p%1000%100/10;//十位 
	M4 = p%10;	//个位
			
	DU = 0;
	WE = 0;//关闭数码管显示
	LCD_Init();//1602初始化
	LCD_Str(0, 0, &TestStr[0]);	//显示字符串
	LCD_Str(7, 1, &TestStr1[0]);	//显示字符串
	LCD_OneChar(6 , 0, 43+m);//显示正负
	LCD_OneChar(7, 0, M1+48); //显示温度
	LCD_OneChar(8, 0, M2+48);
	LCD_OneChar(9, 0, M3+48);
	LCD_OneChar(10, 0, 46);
	LCD_OneChar(11, 0, M4+48);
	LCD_OneChar(12, 0, 39);
	LCD_OneChar(13, 0, 67+FF);
	LCD_OneChar(15, 0, 78+DD*10); //显示蜂鸣器开关
	LCD_OneChar(15, 1, 78+LL*10);  //显示指示灯开关
}
void displayLF(int c)
{	
	uchar M1,M2,M3,M4;
	uchar TestStr[] = {"Tem:"}; 
	uchar TestStr1[] = {"Low"};
	uint p,m,i;
	if(c<0)
	{m = 2;}
	else
	{m = 0;}
	i = abs(c);
	p = i*9/5+320;
	M1 = p/1000;
	M2 = p%1000/100;//百位
	M3 = p%1000%100/10;//十位 
	M4 = p%10;	//个位
			
    DU = 0;
	WE = 0;//关闭数码管显示
	LCD_Init();//1602初始化
	LCD_Str(0, 0, &TestStr[0]);	//显示字符串
	LCD_Str(7, 1, &TestStr1[0]);	//显示字符串
	LCD_OneChar(6 , 0, 43+m);//显示正负
	LCD_OneChar(7, 0, M1+48); //显示温度
	LCD_OneChar(8, 0, M2+48);
	LCD_OneChar(9, 0, M3+48);
	LCD_OneChar(10, 0, 46);	  //.
	LCD_OneChar(11, 0, M4+48);
	LCD_OneChar(12, 0, 39);	  //'
	LCD_OneChar(13, 0, 67+FF);//C或F
	LCD_OneChar(15, 0, 78+DD*10);//X或者N
	LCD_OneChar(15, 1, 78+LL*10);  //显示指示灯开关
}
void displayLE()
{
	 uchar TestSt1[] = {"Error"};
	 uchar TestSt2[] = {"Low#High"};
	 LCD_Init();
	 LCD_Str(5, 0, &TestSt1[0]);
	 LCD_Str(4, 1, &TestSt2[0]);
	 LCD_OneChar(15, 0, 78+DD*10); //显示蜂鸣器开关
	 LCD_OneChar(15, 1, 78+LL*10);  //显示指示灯开关
	 
}
void write_byte(uchar dat)//写一个字节
{
	uchar i;
	for(i=0;i<8;i++)//一次写一位，一个字节循环8次
	{
		DS = 0;
		_nop_();//产生写时序（15US内满足要求）
		DS = dat&0X01; //与
		delayus(9);//延时大于60US 9*6.5+11.95=70.45us 同时满足写0写1
		DS = 1;//释放总线
		_nop_();
		dat>>=1;//右移一位
	}
}

uchar read_byte()//读一个字节
{
	uchar i,j,dat;
	for(i=0;i<8;i++)
	{
		DS = 0;
		_nop_();//产生读时序
		DS = 1;
		_nop_();//释放总线
		j = DS;
		delayus(9);//读取数据大于60US 9*6.5+11.95=70.45us
		DS = 1;
		_nop_();
		dat = (j<<7)|(dat>>1);
	}
	return(dat);
}
void beep(int m,gao,di)//按键与警报程序
{	
	if(m>=gao)
	{
		if(di<=gao)	  //判断最低报警温度是否低于最高报警温度
		{
			BP = DD;
			P1 = 0XEE+LL*0X11;
		}
		else if(di>gao)
		{
			BP = 1;
			P1 = 0X7E+LL*0X81;
		}
	}
	else if(m<=di) 
	{
		if(di<=gao)	  //判断最低报警温度是否低于最高报警温度
		{
			BP = DD;
			P1=0X77+LL*0X88;
		}
		else if(di>gao)
		{
			BP = 1;
			P1 = 0X7E+LL*0X81;
		}

	}
	else
	{
		if(di<=gao)	 //判断最低报警温度是否低于最高报警温度
		{
			BP = 1;
			P1=0XBB+LL*0X44;
		}
		else if(di>gao)
		{
			BP = 1;
			P1 = 0X7E+LL*0X81;
		}
	}
}

void main()
{
	int i,h;
	uint j=0;
	float t;
	int gao=330,di=270;
	uchar L,H;
	while(1)
	{
		ds_init();//初始化函数
		write_byte(0XCC);//发送跳跃ROM指令(0xcc)
		write_byte(0X44);//发送温度转换指令（0X44）
		ds_init();//初始化DS18B20
		write_byte(0XCC);//发送跳跃ROM指令(0xcc)
		write_byte(0XBE);//读取DS18B20暂存器的值（0Xbe）
		L = read_byte(); //温度低字节
		H = read_byte();//温度高字节
		i = H;
		i <<=8;
		i|=L;//与，把L写到低字节里
		if(H >= 0x08)	//判断是否为负数
		{ 
			i = ~i + 1;//负数是以补码的形式存放的需要我们需要取反加1
			s = 2;  //显示负数符号
		}
		else s = 0;
		t=i*0.0625;
		i = t*10;//10.125--101.25--101.75--10.2（四舍五入)
		
		if(key_s1 == 0)	//第一位选程序
		{
			delayus(10);//防抖
			if(key_s1 == 0)
			{
				j=j+1 ;
				if(j>=3)
				j=0;
			}
			while(!key_s1);
		}
		if(j==1) //调最高温度
		{
			h = gao;
			if(key_s2 == 0)//按键2增加温度
			{
				delayus(20);//防抖
				if(key_s2 == 0)
				{
					h=h+1 ;
				}
				while(!key_s2);
			}
			else if(key_s3 == 0)//按键3减小温度
			{
				delayus(20);
				if(key_s3 == 0)
				{
					h=h-1;
				}
				while(!key_s3);
			}
			if(K==0)
			{
				displayM(h);
			}
			else if(K==1)
			{
			//	p = h*(5/9)+320;
				displayMF(h);
			}
			gao = h;
		}	
		else if(j==2)//调最低温度		
		{
			h = di;
			if(key_s2 == 0)//按键2增加温度
			{
				delayus(20);//防抖
				if(key_s2 == 0)
				{
					h=h+1 ;
				}
				while(!key_s2);
			}
			else if(key_s3 == 0)//按键3减小温度
			{
				delayus(20);
				if(key_s3 == 0)
				{
					h=h-1;
				}
				while(!key_s3);
			}
			if(K==0)
			{
				displayL(h);
			}
			else if(K==1)
			{
			//	p = h*(5/9)+320;
				displayLF(h);
			}
			di = h;
		 }
		else if(j==0) //显示温度
		{	
			if(key_s2 == 0)//温度转化
			{
				delayus(20);//防抖
				if(key_s2 == 0)
				{
					K=K+1 ;
					if(K>1)
					K=0;
				}
				while(!key_s2);
			}
			if(key_s3 == 0)//蜂鸣器与指示灯开关
			{
				delayus(20);//防抖
				if(key_s3 == 0)
				{
					M=M+1 ;
					if(M>3)
					M=0;
				}
				while(!key_s3);
			}
			if(K==0)
			{
			 	FF=0;	   //摄氏度
			}
			else if(K==1)
			{
				FF=3;	   //华氏
			}
/****************判断蜂鸣器与指示灯开关程序*******************/
			if(M==0)	   //蜂鸣器关指示灯开
			{
			 	DD=1;
				LL=0;	   //蜂鸣器关
			}
			else if(M==1)  //蜂鸣器开指示灯开
			{
				DD=0;
				LL=0;	   
			}
			else if(M==2)  //蜂鸣器开指示灯关
			{
				DD=0;
				LL=1;
			}
			else if(M==3)  //蜂鸣器关指示灯关
			{
				DD=1;
				LL=1;
			}
/*****************温度显示及判断程序执行*****************/
			if(K==0)	   //选择显示摄氏度
			{
				if(di>gao)
				displayLE();
				else if(di<=gao)
				displayN(i);
			}
			else if(K==1)  //选择显示华氏
			{
				if(di>gao)
				displayLE();
				else if(di<=gao)
				displayNF(i);
			}
			beep(i,gao,di);//报警判断
		}
	}
}
//打开是85.0，因为初始值是85.
