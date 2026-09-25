#include<reg51.h>
#include"DS18B20.h"
uchar a,b,miao,shi,fen,ri,yue,nian,week,flag,key1n,temp,miao1,shi1=12,fen1=1,miao1=0,clock=0 ;
//flag用于读取头文件中的温度值，和显示温度值
#define yh 0x80 //LCD第一行的初始位置,因为LCD1602字符地址首位D7恒定为1（100000000=80）
#define er 0x80+0x40 //LCD第二行初始位置（因为第二行第一个字符位置地址是0x40）
//液晶屏的与C51之间的引脚连接定义（显示数据线接C51的P0口）
sbit rs=P2^0;
sbit en=P2^2;
sbit rw=P2^1; //如果硬件上rw接地，就不用写这句和后面的rw=0了
sbit led=P2^6; //LCD背光开关
//DS1302时钟芯片与C51之间的引脚连接定义
sbit IO=P1^1;
sbit SCLK=P1^0;
sbit RST=P1^2;
sbit CLO=P1^4;
sbit ACC0=ACC^0;
sbit ACC7=ACC^7;
/************************************************************
ACC累加器=A
ACC.0=E0H 

ACC.0就是ACC的第0位。Acc可以位寻址。

累加器ACC是一个8位的存储单元，是用来放数据的。但是，这个存储单元有其特殊的地位，
是单片机中一个非常关键的单元，很多运算都要通过ACC来进行。以后在学习指令时，
常用A来表示累加器。但有一些地方例外，比如在PUSH指令中，就必须用ACC这样的名字。
一般的说法，A代表了累加器中的内容、而ACC代表的是累加器的地址。 
***************************************************************/
//校时按键与C51的引脚连接定义
sbit key1=P1^5;  //设置键
sbit key2=P1^6;  //加键
sbit key3=P1^7;  //减键
sbit buzzer=P1^3;//蜂鸣器，通过三极管9012驱动，端口低电平响
/**************************************************************/
uchar code tab1[]={"20  -  -   "}; //年显示的固定字符
uchar code tab2[]={"  :  :  "};//时间显示的固定字符
uchar code tab3[]={"    HELLO WELCOME"};//开机动画
//延时函数，后面经常调用
void delay(uint xms)//延时函数，有参函数
{
	uint x,y;
	for(x=xms;x>0;x--)
		for(y=110;y>0;y--);
}
/********液晶写入指令函数与写入数据函数，以后可调用**************/

/*在这个程序中，液晶写入有关函数会在DS1302的函数中调用，所以液晶程序要放在前面*/

void write_1602com(uchar com)//****液晶写入指令函数****
{
	rs=0;//数据/指令选择置为指令
	rw=0; //读写选择置为写
	P0=com;//送入数据
	delay(1);
	en=1;//拉高使能端，为制造有效的下降沿做准备
	delay(1);
	en=0;//en由高变低，产生下降沿，液晶执行命令
}
void write_1602dat(uchar dat)//***液晶写入数据函数****
{
	rs=1;//数据/指令选择置为数据
	rw=0; //读写选择置为写
	P0=dat;//送入数据
	delay(1);
	en=1; //en置高电平，为制造下降沿做准备
	delay(1);
	en=0; //en由高变低，产生下降沿，液晶执行命令
}
void lcd_init()//***液晶初始化函数****
{
	uchar j;
    write_1602com(0x0f|0x08);
	for(a=0;a<17;a++)
		write_1602dat(tab3[a]);
	j=17;
	while(j--)
	{
		write_1602com(0x1c);//循环左移
		delay(700);
	} 
	write_1602com(0x01);
	delay(10);
	write_1602com(0x38);//设置液晶工作模式，意思：16*2行显示，5*7点阵，8位数据
	write_1602com(0x0c);//开显示不显示光标
	write_1602com(0x06);//整屏不移动，光标自动右移
	write_1602com(0x01);//清显示
	/***开机动画显示hello welcome dianzizhong****/
	write_1602com(yh+1);//日历显示固定符号从第一行第1个位置之后开始显示
	for(a=0;a<14;a++)
	{
		write_1602dat(tab1[a]);//向液晶屏写日历显示的固定符号部分
		//delay(3);
	}
	write_1602com(er+2);//时间显示固定符号写入位置，从第2个位置后开始显示	
	for(a=0;a<8;a++)
	{
		write_1602dat(tab2[a]);//写显示时间固定符号，两个冒号
		//delay(3);
	}
	write_1602com(er+0);
	write_1602dat(0x24);
	write_1602com(er+1);
	write_1602dat(0x20);
}
/***************DS1302有关子函数********************/
void write_byte(uchar dat)//写一个字节
{
	ACC=dat;
	RST=1;
	for(a=8;a>0;a--)
	{
		IO=ACC0;
		SCLK=0;
		SCLK=1;
		ACC=ACC>>1;
	}
}
uchar read_byte()//读一个字节
{
	RST=1;
	for(a=8;a>0;a--)
	{
		ACC7=IO;
		SCLK=1;
		SCLK=0;
		ACC=ACC>>1;
	}
	return(ACC);
}
//----------------------------------------
void write_1302(uchar add,uchar dat)//向1302芯片写函数，指定写入地址，数据
{
	RST=0;
	SCLK=0;
	RST=1;
	write_byte(add);
	write_byte(dat);
	SCLK=1;
	RST=0;
}
uchar read_1302(uchar add)//从1302读数据函数，指定读取数据来源地址
{
	uchar temp;
	RST=0;
	SCLK=0;
	RST=1;
	write_byte(add);
	temp=read_byte();
	SCLK=1;
	RST=0;
	return(temp);
}
uchar BCD_Decimal(uchar bcd)//BCD码转十进制函数，输入BCD，返回十进制
{
 	uchar Decimal;
 	Decimal=bcd>>4;
 	return(Decimal=Decimal*10+(bcd&=0x0F));
}
//--------------------------------------
void ds1302_init() //1302芯片初始化子函数(2010-01-07,12:00:00,week4)
{
	RST=0;
	SCLK=0;
	write_1302(0x8e,0x00); //允许写，禁止写保护 
	miao=BCD_Decimal(read_1302(0x81));
	fen=BCD_Decimal(read_1302(0x83));
	shi=BCD_Decimal(read_1302(0x85));
	ri=BCD_Decimal(read_1302(0x87));
	yue=BCD_Decimal(read_1302(0x89));
	nian=BCD_Decimal(read_1302(0x8d));
	week=BCD_Decimal(read_1302(0x8b));
	
	//写入初始年份数据10
	write_1302(0x8e,0x80); //打开写保护
}
//------------------------------------
//温度显示子函数
void write_temp(uchar add,uchar dat)//向LCD写温度数据,并指定显示位置
{
	uchar gw,sw,bw;
	if(dat>=0&&dat<=128)
	{
		gw=dat%10;//取得个位数字
		sw=dat%100/10;//取得十位数字
		bw=dat/100	;//取得百位数字	
	}
	else
	{
		dat=256-dat;
		gw=dat%10;//取得个位数字
		sw=dat%100/10;//取得十位数字
		bw=-3;		 //0x30-3表示为负号
	}
	write_1602com(er+add);//er是头文件规定的值0x80+0x40
	write_1602dat(0x30+bw);//数字+30得到该数字的LCD1602显示码
	write_1602dat(0x30+sw);//数字+30得到该数字的LCD1602显示码
	write_1602dat(0x30+gw);//数字+30得到该数字的LCD1602显示码
    write_1602dat(0xdf);//显示温度的小圆圈符号，0xdf是液晶屏字符库的该符号地址码
  	write_1602dat(0x43);	//显示"C"符号，0x43是液晶屏字符库里大写C的地址码		
}
//------------------------------------
//时分秒显示子函数
void write_sfm(uchar add,uchar dat)//向LCD写时分秒,有显示位置加、现示数据，两个参数
{
	uchar gw,sw;
	gw=dat%10;//取得个位数字
	sw=dat/10;//取得十位数字
	write_1602com(er+add);//er是头文件规定的值0x80+0x40
	write_1602dat(0x30+sw);//数字+30得到该数字的LCD1602显示码
	write_1602dat(0x30+gw);//数字+30得到该数字的LCD1602显示码				
}
//-------------------------------------
//年月日显示子函数
void write_nyr(uchar add,uchar dat)//向LCD写年月日，有显示位置加数、显示数据，两个参数
{
	uchar gw,sw;
	gw=dat%10;//取得个位数字
	sw=dat/10;//取得十位数字
	write_1602com(yh+add);//设定显示位置为第一个位置+add
	write_1602dat(0x30+sw);//数字+30得到该数字的LCD1602显示码
	write_1602dat(0x30+gw);//数字+30得到该数字的LCD1602显示码	
}
//-------------------------------------------
void write_week(uchar week)//写星期函数
{
	write_1602com(yh+0x0c);//星期字符的显示位置
	switch(week)
	{
		case 1:	write_1602dat('M');//星期数为1时，显示
			   	write_1602dat('O');
			   	write_1602dat('N');
			   	break;
	   
		case 2:	write_1602dat('T');//星期数据为2时显示
			   	write_1602dat('U');
			   	write_1602dat('E');
			   	break;
		
		case 3:	write_1602dat('W');//星期数据为3时显示
			   	write_1602dat('E');
			   	write_1602dat('D');
			   	break;
		
		case 4:	write_1602dat('T');//星期数据为4是显示
			   	write_1602dat('H');
			   	write_1602dat('U');
			   	break;
		
		case 5:	write_1602dat('F');//星期数据为5时显示
			   	write_1602dat('R');
			   	write_1602dat('I');
			   	break;
		
		case 6:	write_1602dat('S');//星期数据为6时显示
			   	write_1602dat('T');
			   	write_1602dat('A');
			   	break;
		
		case 7:	write_1602dat('S');//星期数据为7时显示
			   	write_1602dat('U');
			   	write_1602dat('N');
			   	break;
	}
}
//****************键盘扫描有关函数**********************
void keyscan()
{
	if(key1==0)//---------------key1为功能键（设置键）--------------------
	{
		delay(9);//延时，用于消抖动
		if(key1==0)//延时后再次确认按键按下
		{
    		buzzer=0;//蜂鸣器短响一次
    		delay(20);
    		buzzer=1;
			while(!key1);
			key1n++;
			if(key1n==12)
				key1n=1;//设置按键共有秒、分、时、星期、日、月、年、返回，8个功能循环
			switch(key1n)
			{		
				case 1: TR0=0;//关闭定时器
						//TR1=0;
						write_1602com(er+0x09);//设置按键按动一次，秒位置显示光标
					   	write_1602com(0x0f);//设置光标为闪烁
					   	temp=(miao)/10*16+(miao)%10;//秒数据写入DS1302
					   	write_1302(0x8e,0x00);
					   	write_1302(0x80,0x80|temp);//miao
					    write_1302(0x8e,0x80);
					   	break;
				case 2: write_1602com(er+6);//按2次fen位置显示光标						
					    //write_1602com(0x0f);
						break;
				case 3: write_1602com(er+3);//按动3次，shi
					    //write_1602com(0x0f);
						break;
				
				case 4: write_1602com(yh+0x0e);//按动4次，week
					    //write_1602com(0x0f);
						break;
				case 5: write_1602com(yh+0x0a);//按动5次，ri
					    //write_1602com(0x0f);
						break;
				case 6: write_1602com(yh+0x07);//按动6次，yue
					    //write_1602com(0x0f);
						break;
				case 7: write_1602com(yh+0x04);//按动7次，nian
					    //write_1602com(0x0f);
						break;
				case 8: write_1602com(er+1);
				        write_1602dat(0x4d);
						write_1602com(er+1);
				        break;
				case 9: write_1602com(er+1);
				        write_1602dat(0x46);
						write_1602com(er+1);
				        break;
				case 10:write_1602com(er+1);
				        write_1602dat(0x53);
						write_1602com(er+1);
						break;
				case 11:
				        write_1602com(er+1);
			        	write_1602dat(0x20);
						write_1602com(0x0c);//按动到第8次，设置光标不闪烁
						TR0=1;//打开定时器
			         	temp=(miao)/10*16+(miao)%10;
					   	write_1302(0x8e,0x00);
					   	write_1302(0x80,0x00|temp);//miao数据写入DS1302
					   	write_1302(0x8e,0x80);
			            break;																		
			}
		} 
	}
	//------------------------------加键key2----------------------------		
	if(key1n!=0)//当key1按下以下。再按以下键才有效（按键次数不等于零）
	{
		if(key2==0)  //上调键
		{
			delay(10);
			if(key2==0)
			{
    			buzzer=0;//蜂鸣器短响一次
    			delay(20);
    			buzzer=1;
				while(!key2);
				switch(key1n)
				{
					case 1:	miao++;//设置键按动1次，调秒
							if(miao==60)
								miao=0;//秒超过59，再加1，就归零
							write_sfm(0x08,miao);//令LCD在正确位置显示"加"设定好的秒数
							temp=(miao)/10*16+(miao)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00); //允许写，禁止写保护 
						   	write_1302(0x80,temp); //向DS1302内写秒寄存器80H写入调整后的秒数据BCD码
						   	write_1302(0x8e,0x80); //打开写保护
							write_1602com(er+0x09);//因为设置液晶的模式是写入数据后，光标自动右移，所以要指定返回
							//write_1602com(0x0b);
							break;
					case 2:	fen++;
							if(fen==60)
								fen=0;
							write_sfm(0x05,fen);//令LCD在正确位置显示"加"设定好的分数据
							temp=(fen)/10*16+(fen)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护 
						   	write_1302(0x82,temp);//向DS1302内写分寄存器82H写入调整后的分数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(er+6);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
							break;
					case 3:	shi++;
							if(shi==24)
								shi=0;
							write_sfm(2,shi);//令LCD在正确的位置显示"加"设定好的小时数据
							temp=(shi)/10*16+(shi)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护 
						   	write_1302(0x84,temp);//向DS1302内写小时寄存器84H写入调整后的小时数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(er+3);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 4:	week++;
							if(week==8)
								week=1;
				            write_1602com(yh+0x0C);//指定'加'后的周数据显示位置
							write_week(week);//指定周数据显示内容
				            temp=(week)/10*16+(week)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护 
						   	write_1302(0x8a,temp);//向DS1302内写周寄存器8aH写入调整后的周数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(yh+0x0e);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 5:	ri++;
							switch(yue)
							{
				            	case 1:
								case 3:
								case 5:
								case 7:
								case 8:
								case 10:
								case 12:
										if(ri>31) 
											ri=1;
						        		break;
				                case 2: 
										if(nian%4==0||nian%400==0)
						  	       		{ 
								     		if(ri>29) 
												ri=1;
								    	}	
						           		else					  	
						         		{					    
								    		if(ri>28) 
												ri=1;
						           		}
										break;
				               case 4:
							   case 6:
							   case 9:
							   case 11:
							      		ri++;
								    	if(ri>30) 
											ri=1;
							         	break;
				            }		 
							write_nyr(9,ri);//令LCD在正确的位置显示"加"设定好的日期数据
							temp=(ri)/10*16+(ri)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护
						   	write_1302(0x86,temp);//向DS1302内写日期寄存器86H写入调整后的日期数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(yh+10);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 6:	yue++;
							if(yue==13)
								yue=1;
							write_nyr(6,yue);//令LCD在正确的位置显示"加"设定好的月份数据
							temp=(yue)/10*16+(yue)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护
						   	write_1302(0x88,temp);//向DS1302内写月份寄存器88H写入调整后的月份数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(yh+7);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 7:	nian++;
						 	if(nian==100)
								nian=0;
							write_nyr(3,nian);//令LCD在正确的位置显示"加"设定好的年份数据
				            temp=(nian)/10*16+(nian)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护
						   	write_1302(0x8c,temp);//向DS1302内写年份寄存器8cH写入调整后的年份数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(yh+4);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 8:	write_1602com(er+9); //设置闹钟的秒定时
					        miao1++;
							if(miao1==60)
							   miao1=0;
							write_sfm(0x08,miao1);//令LCD在正确位置显示"加"设定好秒的数据
							write_1602com(er+9);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
							break;
					case 9:	write_1602com(er+6); //设置闹钟的分钟定时
					        fen1++;
					        if(fen1==60)
								fen1=0;
							write_sfm(0x05,fen1);//令LCD在正确位置显示"加"设定好的分数据
							write_1602com(er+6);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
							break;
					case 10:write_1602com(er+3); //设置闹钟的小时定时
					        shi1++;
					        if(shi1==24)
								shi1=0;
							write_sfm(0x02,shi1);//令LCD在正确的位置显示"加"设定好的小时数据
							write_1602com(er+3);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
				}
			}
		}
		//------------------减键key3，各句功能参照'加键'注释---------------
		if(key3==0)
		{
			delay(10);//调延时，消抖动
			if(key3==0)
			{
	    		buzzer=0;//蜂鸣器短响一次
	    		delay(20);
			    buzzer=1;
				while(!key3);
				switch(key1n)
				{
					case 1:	miao--;
							if(miao==-1)
								miao=59;//秒数据减到-1时自动变成59
							write_sfm(0x08,miao);//在LCD的正确位置显示改变后新的秒数
				            temp=(miao)/10*16+(miao)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00); //允许写，禁止写保护 
						   	write_1302(0x80,temp); //向DS1302内写秒寄存器80H写入调整后的秒数据BCD码
						   	write_1302(0x8e,0x80); //打开写保护
							write_1602com(er+0x09);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
							//write_1602com(0x0b);
							break;
					case 2:	fen--;
							if(fen==-1)
								fen=59;
							write_sfm(5,fen);
							temp=(fen)/10*16+(fen)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护 
						   	write_1302(0x82,temp);//向DS1302内写分寄存器82H写入调整后的分数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(er+6);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
							break;
			
					case 3:	shi--;
						   	if(shi==-1)
								shi=23;
							write_sfm(2,shi);
							temp=(shi)/10*16+(shi)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护 
						   	write_1302(0x84,temp);//向DS1302内写小时寄存器84H写入调整后的小时数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(er+3);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 4:	week--;
							if(week==0)
								week=7;						 
			            	write_1602com(yh+0x0C);//指定'加'后的周数据显示位置
							write_week(week);//指定周数据显示内容
						   	temp=(week)/10*16+(week)%10;//十进制转换成DS1302要求的DCB码
					   		write_1302(0x8e,0x00);//允许写，禁止写保护 
					   		write_1302(0x8a,temp);//向DS1302内写周寄存器8aH写入调整后的周数据BCD码
					   		write_1302(0x8e,0x80);//打开写保护
						   	write_1602com(yh+0x0e);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 5:	ri--;
							switch(yue)
							{ 
			                	case 1:
								case 3:
								case 5:
								case 7:
								case 8:
								case 10:
								case 12: 
							 			if(ri==0) 
											ri=31;
					         			break;
			                  	case 2: if(nian%4==0||nian%400==0)
					  	       			{ 
							    			if(ri==0) 
												ri=29;
							    		}			
					           			else	
						        		{
							        		if(ri==0) 
												ri=28;
					         			}
										break;
			                 	case 4:
								case 6:
								case 9:
								case 11:
							      		if(ri==0) 
											ri=30;
						         		break;
			             	}
							write_nyr(9,ri);
							temp=(ri)/10*16+(ri)%10;//十进制转换成DS1302要求的DCB码
					   		write_1302(0x8e,0x00);//允许写，禁止写保护
					   		write_1302(0x86,temp);//向DS1302内写日期寄存器86H写入调整后的日期数据BCD码
					   		write_1302(0x8e,0x80);//打开写保护
							write_1602com(yh+10);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 6:	yue--;
							if(yue==0)
								yue=12;
							write_nyr(6,yue);
							temp=(yue)/10*16+(yue)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护
						   	write_1302(0x88,temp);//向DS1302内写月份寄存器88H写入调整后的月份数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(yh+7);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;	
					case 7:	nian--;
					 		if(nian==-1)
								nian=99;
							write_nyr(3,nian);
			         		temp=(nian)/10*16+(nian)%10;//十进制转换成DS1302要求的DCB码
						   	write_1302(0x8e,0x00);//允许写，禁止写保护
						   	write_1302(0x8c,temp);//向DS1302内写年份寄存器8cH写入调整后的年份数据BCD码
						   	write_1302(0x8e,0x80);//打开写保护
							write_1602com(yh+4);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;
					case 8:	write_1602com(er+9); //设置闹钟的秒定时
				        	miao1--;
							if(miao1==-1)
						   		miao1=59;
							write_sfm(0x08,miao1);//令LCD在正确位置显示"加"设定好秒的数据
							write_1602com(er+9);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
							break;
					case 9:	write_1602com(er+6); //设置闹钟的分钟定时
				        	fen1--;
				        		if(fen1==-1)
							fen1=59;
							write_sfm(0x05,fen1);//令LCD在正确位置显示"加"设定好的分数据
							write_1602com(er+6);//因为设置液晶的模式是写入数据后，指针自动加一，在这里是写回原来的位置
							break;
				    case 10:write_1602com(er+3); //设置闹钟的小时定时
				        	shi1--;
				        	if(shi1==-1)
								shi1=23;
							write_sfm(0x02,shi1);//令LCD在正确的位置显示"加"设定好的小时数据
							write_1602com(er+3);//因为设置液晶的模式是写入数据后，指针自动加一，所以需要光标回位
							break;			
				}
			}
		}
	}
}
//定时器0初始化程序
void init() //定时器、计数器设置函数
{
	TMOD=0x11; //指定定时/计数器的工作方式为1
	TH0=0;  //定时器T0的高四位=0
	TL0=0;  //定时器T0的低四位=0
	EA=1;  //系统允许有开放的中断
	ET0=1; //允许T0中断
	TR0=1; //开启中断，启动定时器
}
//*******************主函数**************************
//***************************************************
void main()
{	
	lcd_init();      //调用液晶屏初始化子函数
	ds1302_init();   //调用DS1302时钟的初始化子函数
	init();          //调用定时计数器的设置子函数
	led=0;           //打开LCD的背光电源
    buzzer=0;//蜂鸣器长响一次
    delay(80);
    buzzer=1; 
	while(1)  //无限循环下面的语句：
	{		
    	keyscan();      //调用键盘扫描子函数
    }
}
/*************通过定时中断实现定是独处并显示数据******************/
void t0() interrupt 1  //取得并显示日历和时间
{	
   	//Init_DS18B20();//温度传感器DS18b2初始化子函数，在头文件中
   	flag=ReadTemperature();//将18b2头文件运行返回的函数结果送到变量FLAG中，用于显示
  	//读取秒时分周日月年七个数据（DS1302的读寄存器与写寄存器不一样）：
	miao=BCD_Decimal(read_1302(0x81));
	fen=BCD_Decimal(read_1302(0x83));
	shi=BCD_Decimal(read_1302(0x85));
	ri=BCD_Decimal(read_1302(0x87));
	yue=BCD_Decimal(read_1302(0x89));
	nian=BCD_Decimal(read_1302(0x8d));
	week=BCD_Decimal(read_1302(0x8b));
	//显示温度、秒、时、分数据：
	write_temp(12,flag);//显示温度，从第二行第12个字符后开始显示
	write_sfm(8,miao);//秒，从第二行第8个字后开始显示（调用时分秒显示子函数）
	write_sfm(5,fen);//分，从第二行第5个字符后开始显示
	write_sfm(2,shi);//小时，从第二行第2个字符后开始显示
	//显示日、月、年数据：
	write_nyr(9,ri);//日期，从第二行第9个字符后开始显示
	write_nyr(6,yue);//月份，从第二行第6个字符后开始显示
	write_nyr(3,nian);//年，从第二行第3个字符后开始显示
	write_week(week-1);
	/***********整点报时程序************/
	if(fen==0&&miao==0)
		if(shi<22&&shi>6 )
		{
			buzzer=0;//蜂鸣器短响一次
			delay(20);
			buzzer=1;
		}
		/**************闹钟程序: 将暂停键按下停止蜂鸣********************/
		if(shi1==shi&&fen1==fen&&miao==0)
		{		
			clock=1; 
		}	
		if(clock==1)
		{
			buzzer=0;//蜂鸣器短响一次
			delay(20);
			buzzer=1;
		}
		if(CLO==0)	/*按下p1.4停止蜂鸣*/
			clock=0;	
}


