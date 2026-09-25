#include<reg52.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit DSPORT=P3^7;
void Delay1ms(uint );
uchar Ds18b20Init();
void Ds18b20WriteByte(uchar com);
uchar Ds18b20ReadByte();
void  Ds18b20ChangTemp();
void  Ds18b20ReadTempCom();
int Ds18b20ReadTemp();
#define LCD1602_DATAPINS P2
sbit lcde=P1^4;
sbit w=P1^5;
sbit rs=P1^6;
void Lcd1602_Delay1ms(uint c);   //误差 0us
void LcdWriteCom(uchar com);	 
void LcdWriteData(uchar dat);
void wendu();	
void LcdInit();
void LcdDisplay(int);
void init_jsq();
void init_sz();
void write_data(uchar date);
void delay(uint z);
void write_com(uchar com);
void write_sfm(uchar add,uchar date);


sbit k1=P3^3;
sbit k2=P3^4;
sbit k3=P3^5;
uchar num,count,k1num,miao1,miao2,ki;
char miao,fen,shi;
uchar code table2[]=" 2018-12-10 SAT" ;
uchar code table21[]=" 00:00:00";


char i,j,temp,num_1;
long a,a1,a2,b,b1,b2,c,c1,c2;//a，第一个数 b第二个数  c第三个数
uchar flag1,fuhao; //flag1表示是否有符号按下，fuhao表示按下哪个符号
uchar code table[]={
					1,2,3,0,
					4,5,6,0,
					7,8,9,0,
					0,0,0,0,
					};
uchar code table1[]={
					 1,2,3,0x2f-0x30,
					 4,5,6,0x2a-0x30,
					 7,8,9,0x2d-0x30,
					 0x01-0x30,0,0x3d-0x30,0x2b-0x30,					
					};
void jisuanqi();
void shizhong();
sbit ky1=P3^2;
sbit d1=P0^0;
sbit d2=P0^1;
sbit d3=P0^2;
void main()
{

	
		i=50;
		ki=0;
	while(1)
	{
		if(ky1==0)
		{
			while(ky1==0);
			ki++;
		}
				
		if(ki>5)
		{
			ki=0;
		}
		
///////////温度//////////////////		
		if(ki==0)
		{
			LcdInit();
			LcdWriteCom(0x88);	//写地址 80表示初始地址
			LcdWriteData('C'); 
			if(ki==0)
			{
				ki++;
			}
		}
		
		if(ki==1)
			{
				d1=0;
				d2=1;
				d3=1;
				wendu();
			}
			
			
////////////计算器////////////////
		if(ki==2)
		{
			init_jsq();
			if(ki==2)
			{
				ki++;
			}
		}	
			if(ki==3)
			{
				d1=1;
				d2=0;
				d3=1;
				jisuanqi();		
			}
			
//////////时钟////////////////
		if(ki==4)
		{
			init_sz(); //初始化
			for(num=0;num<16;num++)
			{
				write_data(table2[num]);
				delay(5);
			}

			write_com(0x80+0x40); //开始时第二行显s
			for(num=0;num<10;num++)
			{
				write_data(table21[num]);
				delay(5);
			}
			if(ki==4)
			{
				ki++;
			}
			
			write_sfm(2,shi);
			write_sfm(5,fen);
			write_sfm(8,miao);
		}
		
			if(ki==5)
			{
				d1=1;
				d2=1;
				d3=0;
				shizhong();
			}

	}
}

void wendu()
{
		LcdDisplay(Ds18b20ReadTemp());
}
void LcdDisplay(int temp) 	 //lcd显示
{
    
  unsigned char datas[] = {0, 0, 0, 0, 0}; //定义数组
	float tp;  
	if(temp< 0)				//当温度值为负数
  	{
	  	LcdWriteCom(0x80);		//写地址 80表示初始地址
	    LcdWriteData('-');  		//显示负
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
  	}
 	else
  	{			
	  	LcdWriteCom(0x80);		//写地址 80表示初始地址
	    LcdWriteData('+'); 		//显示正
		tp=temp;//因为数据处理有小数点所以将温度赋给一个浮点型变量
		temp=tp*0.0625*100+0.5;	
	}
	datas[0] = temp / 10000;
	datas[1] = temp % 10000 / 1000;
	datas[2] = temp % 1000 / 100;
	datas[3] = temp % 100 / 10;
	datas[4] = temp % 10;
 
	LcdWriteCom(0x82);		  //写地址 80表示初始地址
	LcdWriteData('0'+datas[0]); //百位 
	
	LcdWriteCom(0x83);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[1]); //十位

	LcdWriteCom(0x84);		//写地址 80表示初始地址
	LcdWriteData('0'+datas[2]); //个位 

	LcdWriteCom(0x85);		//写地址 80表示初始地址
	LcdWriteData('.'); 		//显示 ‘.’

	LcdWriteCom(0x86);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[3]); //显示小数点  

	LcdWriteCom(0x87);		 //写地址 80表示初始地址
	LcdWriteData('0'+datas[4]); //显示小数点  
}
	
void Delay1ms(uint y)
{
	uint x;
	for( ; y>0; y--)
	{
		for(x=110; x>0; x--);
	}
}
uchar Ds18b20Init()
{
	uchar i;
	DSPORT = 0;			 //将总线拉低480us~960us
	i = 70;	
	while(i--);//延时642us
	DSPORT = 1;			//然后拉高总线，如果DS18B20做出反应会将在15us~60us后总线拉低
	i = 0;
	while(DSPORT)	//等待DS18B20拉低总线
	{
		i++;
		if(i>5)//等待>5MS
		{
			return 0;//初始化失败
		}
		Delay1ms(1);	
	}
	return 1;//初始化成功
}

void Ds18b20WriteByte(uchar dat)
{
	uint i, j;

	for(j=0; j<8; j++)
	{
		DSPORT = 0;	     	  //每写入一位数据之前先把总线拉低1us
		i++;
		DSPORT = dat & 0x01;  //然后写入一个数据，从最低位开始
		i=6;
		while(i--); //延时68us，持续时间最少60us
		DSPORT = 1;	//然后释放总线，至少1us给总线恢复时间才能接着写入第二个数值
		dat >>= 1;
	}
}
uchar Ds18b20ReadByte()
{
	uchar byte, bi;
	uint i, j;	
	for(j=8; j>0; j--)
	{
		DSPORT = 0;//先将总线拉低1us
		i++;
		DSPORT = 1;//然后释放总线
		i++;
		i++;//延时6us等待数据稳定
		bi = DSPORT;	 //读取数据，从最低位开始读取
		byte = (byte >> 1) | (bi << 7);						  
		i = 4;		//读取完之后等待48us再接着读取下一个数
		while(i--);
	}				
	return byte;
}
void  Ds18b20ChangTemp()
{
	Ds18b20Init();
	Delay1ms(1);
	Ds18b20WriteByte(0xcc);		//跳过ROM操作命令		 
	Ds18b20WriteByte(0x44);	    //温度转换命令
//	Delay1ms(100);	//等待转换成功，而如果你是一直刷着的话，就不用这个延时了
   
}
void  Ds18b20ReadTempCom()
{	

	Ds18b20Init();
	Delay1ms(1);
	Ds18b20WriteByte(0xcc);	 //跳过ROM操作命令
	Ds18b20WriteByte(0xbe);	 //发送读取温度命令
}
int Ds18b20ReadTemp()
{
	int temp = 0;
	uchar tmh, tml;
	Ds18b20ChangTemp();			 	//先写入转换命令
	Ds18b20ReadTempCom();			//然后等待转换完后发送读取温度命令
	tml = Ds18b20ReadByte();		//读取温度值共16位，先读低字节
	tmh = Ds18b20ReadByte();		//再读高字节
	temp = tmh;
	temp <<= 8;
	temp |= tml;
	return temp;
}
void Lcd1602_Delay1ms(uint c)   //误差 0us
{
    uchar a,b;
	for (; c>0; c--)
	{
		 for (b=199;b>0;b--)
		 {
		  	for(a=1;a>0;a--);
		 }      
	}
    	
}
#ifndef 	LCD1602_4PINS	 //当没有定义这个LCD1602_4PINS时
void LcdWriteCom(uchar com)	  //写入命令
{
	lcde = 0;     //使能
	rs = 0;	   //选择发送命令
	w = 0;	   //选择写入
	
	LCD1602_DATAPINS = com;     //放入命令
	Lcd1602_Delay1ms(1);		//等待数据稳定

	lcde = 1;	          //写入时序
	Lcd1602_Delay1ms(5);	  //保持时间
	lcde = 0;
}
#else 
void LcdWriteCom(uchar com)	  //写入命令
{
	lcde = 0;	 //使能清零
	rs = 0;	 //选择写入命令
	w = 0;	 //选择写入

	LCD1602_DATAPINS = com;	//由于4位的接线是接到P0口的高四位，所以传送高四位不用改
	Lcd1602_Delay1ms(1);

	lcde = 1;	 //写入时序
	Lcd1602_Delay1ms(5);
	lcde = 0;

//	Lcd1602_Delay1ms(1);
	LCD1602_DATAPINS = com << 4; //发送低四位
	Lcd1602_Delay1ms(1);

	lcde = 1;	 //写入时序
	Lcd1602_Delay1ms(5);
	lcde = 0;
}
#endif
#ifndef 	LCD1602_4PINS		   
void LcdWriteData(uchar dat)			//写入数据
{
	lcde = 0;	//使能清零
	rs = 1;	//选择输入数据
	w = 0;	//选择写入

	LCD1602_DATAPINS = dat; //写入数据
	Lcd1602_Delay1ms(1);

	lcde = 1;   //写入时序
	Lcd1602_Delay1ms(5);   //保持时间
	lcde = 0;
}
#else
void LcdWriteData(uchar dat)			//写入数据
{
	lcde = 0;	  //使能清零
	rs = 1;	  //选择写入数据
	w = 0;	  //选择写入

	LCD1602_DATAPINS = dat;	//由于4位的接线是接到P0口的高四位，所以传送高四位不用改
	Lcd1602_Delay1ms(1);

	lcde = 1;	  //写入时序
	Lcd1602_Delay1ms(5);
	lcde = 0;

	LCD1602_DATAPINS = dat << 4; //写入低四位
	Lcd1602_Delay1ms(1);

	lcde = 1;	  //写入时序
	Lcd1602_Delay1ms(5);
	lcde = 0;
}
#endif
#ifndef		LCD1602_4PINS
void LcdInit()						  //LCD初始化子程序
{
 	LcdWriteCom(0x38);  //开显示
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x0c);  //开显示不显示光标
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x06);  //写一个指针加1
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x01);  //清屏
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x80);  //设置数据指针起点
	Lcd1602_Delay1ms(1);
}
#else
void LcdInit()						  //LCD初始化子程序
{
	LcdWriteCom(0x32);	 //将8位总线转为4位总线
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x28);	 //在四位线下的初始化
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x0c);  //开显示不显示光标
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x06);  //写一个指针加1
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x01);  //清屏
	Lcd1602_Delay1ms(1);
	LcdWriteCom(0x80);  //设置数据指针起点

}
#endif
void delay_jsq(uchar z)
{
	uchar y;
	for(z;z>0;z--)
		for(y=110;y>0;y--);
}
void delay_jsq1(uchar a)
{
	uchar i;

	while(a--)
	{
		for(i=0;i<250;i++)
		{
			_nop_();//延迟一周期
			_nop_();
			_nop_();
			_nop_();

		}
	}	
}
void lcd_mang()	// 判忙
{
	rs=0;
	w=1;
	lcde=1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	while(P2&0x80);
	lcde=0;

}
void writelcd(uchar cmd,uchar i)// rs=0，写命令 rs=1,写数据
{
	lcd_mang();
	rs=i;
	w=0;
	lcde=0;
	_nop_();
	_nop_();	
	P2=cmd;
	_nop_();
	_nop_();
	lcde=1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	lcde=0;
}
void init_jsq()
{
	writelcd(0x38,0);
	delay_jsq1(1);
	writelcd(0x0c,0);
	delay_jsq1(1);
	writelcd(0x06,0);
	delay_jsq1(1);
	writelcd(0x01,0);
	delay_jsq1(1);
	num_1=0;
	i=0;
	j=0;
	a=0;
	b=0;
	c=0;
	flag1=0;
	fuhao=0;
}
void keyscan_4_4()
{
	uchar i;
	P2=0x7f;
if(P2!=0x7f)
	{
		delay_jsq(5);
		if(P2!=0x7f)
		{
			temp=P2&0x0f;
			switch(temp)
			{
				case 0x07:num=0;break;
				case 0x0b:num=1;break;
				case 0x0d:num=2;break;
				case 0x0e:num=3;break;
			}
			while(P2!=0x7f);
			if(j!=0)
			{
				writelcd(0x01,0);
				delay_jsq1(1);
				j=0;

			}
			if(num==0||num==1||num==2)
			{
				if(flag1==0)
				{
					a=a*10+table[num];
				}
				else
				{
					b=b*10+table[num];
				}
			}
			else if(num==3)
			{
				flag1=1;
				fuhao=4;
			}
			i=table1[num];
			writelcd(0x30+i,1);
		}
	}
	P2=0xbf;
	if(P2!=0xbf)
	{
		delay_jsq(5);
		if(P2!=0xbf)
		{
			if(P2!=0xbf)
			{
				temp=P2&0x0f;
				switch(temp)
				{
					case 0x07:num=4;break;
					case 0x0b:num=5;break;
					case 0x0d:num=6;break;
					case 0x0e:num=7;break;
				}
			}	
			while(P2!=0xbf);
			if(j!=0)
			{
				writelcd(0x01,0);
				delay_jsq1(1);
				j=0;
	
			}
			if(num==4||num==5||num==6&&num!=7)
			{
				if(flag1==0)
				{
					a=a*10+table[num];
				}
				else
				{
					b=b*10+table[num];
				}
			}
			else
			{
				flag1=1;
				fuhao=3;
			}
			i=table1[num];
			writelcd(0x30+i,1);
		}
	}
	P2=0xdf;
	if(P2!=0xdf)

	{
		delay_jsq(5);
		if(P2!=0xdf)
		{
			if(P2!=0xdf)
			{
				temp=P2&0x0f;
				switch(temp)
				{
					case 0x07:num=8;break;
					case 0x0b:num=9;break;
					case 0x0d:num=10;break;
					case 0x0e:num=11;break;
				}
			}
			while(P2!=0xdf);
			if(j!=0)
			{
				writelcd(0x01,0);
				delay_jsq1(1);
				j=0;
	
			}
			if(num==8||num==9||num==10)
			{
					if(flag1==0)
					{
						a=a*10+table[num];
					}
					else
					{
						b=b*10+table[num];
					}
				}
			else if(num==11)
			{
				flag1=1;
				fuhao=2;
			}
			i=table1[num];
			writelcd(0x30+i,1);
			
		}	
	}
	P2=0xef;
	if(P2!=0xef)
	{
		delay_jsq(5);
		if(P2!=0xef)
		{
			if(P2!=0xef)
			{
				temp=P2&0x0f;
				switch(temp)
				{
					case 0x07:num=12;break;
					case 0x0b:num=13;break;
					case 0x0d:num=14;break;
					case 0x0e:num=15;break;
				}
			}
			while(P2!=0xef);
			switch(num)
			{
				case 12:
				{
					writelcd(0x01,0);
					a=0;
					b=0;
					
					flag1=0;
					fuhao=0;
				}
				break;
				case 13:
				{
					if(flag1==0)
					{
						a=a*10;
						writelcd(0x30,1);

					}
					else if(flag1==1)
					{
						b=b*10;
						writelcd(0x30,1);
					}	
				}
				break;
				case 14:
				{
					j=1;
					if(fuhao==1)
					{
						writelcd(0x04,0);
						delay_jsq1(1);
						writelcd(0x80+0x4f,0);
					//	writelcd(0x04,0);
						c=a+b;
						while(c!=0)
						{
							writelcd(0x30+c%10,1);
							c=c/10;
						}
						writelcd(0x3d,1);
						a=0;
						b=0;
						flag1=0;
						fuhao=0;

					}
					else if(fuhao==2)
					{
						writelcd(0x04,0);
						delay_jsq1(1);
						writelcd(0x80+0x4f,0);
					//	writelcd(0x04,0);
						if(a-b>0)
							c=a-b;
						else
							c=b-a;
						while(c!=0)
						{
							writelcd(0x30+c%10,1);
							c=c/10;
						}
						if(a-b<0)
							writelcd(0x2d,1);
							writelcd(0x3d,1);
							a=0;
							b=0;
							flag1=0;
							fuhao=0;
					}
					else if(fuhao==3)
					{
						writelcd(0x04,0);
						delay_jsq1(1);
						writelcd(0x80+0x4f,0);
					//	writelcd(0x04,0);
						a1=a/10000;
						a2=a%10000;
						b1=b/10000;
						b2=b%10000;
						a=a1*b2;
						b=a2*b1;
						a=a+b;
						c2=a/10000;
						a=a%10000;
						b=a2*b2;
						c1=a1*b1+c2;
						c2=a*10000+b;
						a=c2/100000000;
						c1=c1+a;
						c2=c2%100000000;
						c=c2;
						i=1;
						if(c1!=0)
						{
							while(c!=0||i<8)
							{
								writelcd(0x30+c%10,1);
								c=c/10;
								i++;
							}
						
							c=c1;						
							while(c!=0)
							{
								writelcd(0x30+c%10,1);
								c=c/10;
	
							}	
						}
						if(c1==0)
						{
							while(c!=0)
							{
								writelcd(0x30+c%10,1);
								c=c/10;
							}
						}
						writelcd(0x3d,1);
						a=0;
						b=0;
						flag1=0;
						fuhao=0;
					}
					else if(fuhao==4)
					{
						writelcd(0x04,0);
						delay_jsq1(1);
						writelcd(0x80+0x4f,0);
					//	writelcd(0x04,0);
						i=1;
						c=(long)(((float)a/b)*100000000);
						while(c!=0||i<8)
						{
							writelcd(0x30+c%10,1);
							c=c/10;
							if(i==8)
							writelcd(0x2e,1);
							i++;
						}
						if(a/b<=0)						
							writelcd(0x30,1);
							writelcd(0x3d,1);
							a=0;
							b=0;
							flag1=0;
							fuhao=0;
					}
					else if(fuhao==0)
					{
						writelcd(0x04,0);
						delay_jsq1(1);
						writelcd(0x80+0x4f,0);
						//writelcd(0x04,0);
						c=a;
						while(c!=0)
						{
							writelcd(0x30+c%10,1);
							c=c/10;
						}
						writelcd(0x3d,1);
						a=0;
						b=0;
						flag1=0;
						fuhao=0;
					}
				}
				break;
				case 15:
				{
				 	writelcd(0x30+table1[num],1);
					flag1=1;
					fuhao=1;
				}
				break;
			}
		}			
	}	
}
void jisuanqi()
{
		keyscan_4_4();
}
void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
	for(y=110;y>0;y--);
}

void write_com(uchar com) // 写命令
{
	w=0;
	rs=0; //RS选择写命令还是写数据，写命令
	P2=com; //将写入的命令送到数据总线上
	delay(5); //稍作延时以待数据稳定
	lcde=1; //使能端给一个高脉冲，因为初始化函数已将lcden置为0
	delay(5); //稍作延时
	lcde=0; //将使能端置0以完成高脉冲
}

void write_data(uchar date)//写入数据
{
	rs=1;
	P2=date;
	delay(5);
	lcde=1;
	delay(5);
	lcde=0;
}

void write_sfm(uchar add,uchar date) //将数据写到某个地址上
{
	uchar shi,ge;
	shi=date/10;
	ge=date%10;
	write_com(0x80+0x40+add-1); //地址指针指在第二行的第add+1个
	delay(5);
	write_data(0x30+shi); //送数据，将数据data的十位数送过去
	delay(5);
	write_data(0x30+ge); //送数据，将数据data的个位数送过去
}

void keyscan()
{
	if(k1==0)
	{
		delay(5);
		if(k1==0)//去抖
	{
	k1num++; //计数，计算k1按了几次
	while(!k1); //松手检测
	if(k1num==1) //当按了一次
	{
		TR0=0; //计时器停止，时间停止
		write_com(0x0f); //开显示，显示光标，光标闪烁
		delay(5);
		write_com(0x80+0x48); //数据指针指在第二行秒的个位
	}
	
	if(k1num==2) //当k1按了两次
	{
	
	// write_com(0x0f);
	write_com(0x80+0x44); //数据指针指在第二行分的个位
	
	}
	if(k1num==3) //当k1按了三次
	{
		// write_com(0x0f);
		write_com(0x80+0x41); //数据指针指在第二行时的个位
	}
	
	if(k1num==4) //如果被按了四次
	{
		TR0=1; //启动计时器
		k1num=0; //清零
		write_com(0x0c); //关闭光标
		}
			// if(k1num==5)
			// {
			// write_com(0x80+0x14)
			// }
		}
	}
	if(k1num!=0) //如果k1已经按下过
	{
		if(k2==0) //如果k2按下
		{
			delay(5);
			if(k2==0)
			{
				while(!k2); //松手检测
				if(k1num==1) //当k1按了一次，调秒
				{
					miao++; //每按一次k2秒加一
				if(miao==60)
					{
						miao=0;
					} 
				write_sfm(8,miao); //数据显示在第二行第9位和第10位
				delay(5);
				write_com(0x80+0x40+7); //k1中已设置光标，数据指针指在0x48
			
				}
	if(k1num==2) //如果k1被按过两次
	{
		fen++;
		if(fen==60)
		fen=0;
		write_sfm(5,fen); //数据送到数据指针指在第六位++
		delay(5);
		write_com(0x80+0x40+4); //数据指针指在第七位 
	} 
	if(k1num==3) //如果k1被按过3次
	{
		shi++;
		if(shi==24)
		shi=0;
		write_sfm(2,shi); // 数据送到第3++
		delay(5);
		write_com(0x80+0x40+1); //地址指针指在第四位
	}
		
		}
		}
		if(k3==0) //k1按过后，如果k3按下
		{
			delay(5);
			if(k3==0)
			{
				while(!k3);
				if(k1num==1) //如果k1按下一次
				{
					miao--;
					if(miao==-1)
					{
						miao=59;
					}
					write_sfm(8,miao); //数据送到第二行第九位++
					delay(5);
					write_com(0x80+0x40+7); //指针指在第十位
				
		
					}
		if(k1num==2)
		{
		fen--;
		if(fen==-1)
		fen=59;
		write_sfm(5,fen);
		delay(5);
		write_com(0x80+0x40+4); 
		} 
		if(k1num==3)
		{
		shi--;
		if(shi==-1)
		shi=23;
		write_sfm(2,shi);
		delay(5);
		write_com(0x80+0x40+1);
		} 
		}
		}
	}
}


void init_sz() //初始化函数
{
	lcde=0;
	write_com(0x38); //设置16*2显示，5*7点阵，8位数据接口
	delay(5);
	write_com(0x0c); //开显示，不显示光标
	delay(5);
	write_com(0x06); //当写一个数据后地址指针加1，且光标加1，当显示一个数据时，整屏显示不移动
	delay(5);
	write_com(0x01); //数据指针清0，显示清0
	delay(5);
	write_com(0x80); //显示在屏内
	delay(5);
	TMOD=0x01;
	TH0=0xd8;
	TL0=0xf0;
	EA=1;
	ET0=1;
	TR0=1;
}


void shizhong()
{ 

	
	keyscan();

}


void timer0() interrupt 1
{
	TH0=0xd8;
	TL0=0xf0;
	count++;
	if(count==100) //一秒时间到
	{ 
		count=0;
		miao++;
		if(miao==60)
		{
			miao=0;
			fen++;
			if(fen==60)
			{
				fen=0;
				shi++;
				if(shi==24)
				{
					shi=0;
				}
				if(ki==5)
				write_sfm(2,shi);
			}
			if(ki==5)
			write_sfm(5,fen);
		}
		if(ki==5)
		write_sfm(8,miao);
	}
}