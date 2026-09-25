/*------------------------------------------------------------------------------------------------
程序功能：采用LCD1602液晶显示，显示项目有：年月日、星期、时分秒，带按键调整功能。
按键操作说明：
1、S3按键：是调整时间日期键，该按键选择对秒、分、时、星期、日、月、年选中操作，
当其中一个被选中的时候，可以进行时间调整和日期的调整。每按一次调整一个时间或日期，直到"年"再按一次退出设置界面。
2、S1按键：是时间、日期增加操作。
3、S2按键：是时间、日期减少操作。

------------------------------------------------------------------------------------------------*/

#include <reg51.h>
#include <intrins.h>
#define uchar unsigned char
#define uint unsigned int
sbit ds1302_sclk= P1^4;    //DS1302的Sclk时钟线位定义
sbit ds1302_io  = P1^5;    //DS1302的i/o数据线位定义
sbit ds1302_ce = P1^6;     //DS1302的ce片选线位定义
sbit lcd1602_rs = P1^2; 	 //位定义1602液晶数据命令选择端
sbit lcd1602_rw = P1^1;		 //位定义1602液晶读写选择端
sbit lcd1602_en  = P1^0; 	 //位定义1602液晶使能端
sbit set = P3^4;	       //定义S9按键_时间设置选择键
sbit up  = P3^2;	       //定义S13按键_增加键
sbit down= P3^3;	       //定义S17按键_减小键

uchar write_add[]={0x8c,0x8a,0x88,0x86,0x84,0x82,0x80};  //年、周、月、日、时、分、秒的寄存器写入地址 

uchar code table1[]={"20  -  -   Week"}; //液晶一直显示的字符
uchar code table2[]={"      :  :      "}; //用空格补齐16位
uchar miao,fen,shi,nian,yue,ri,week;
uchar flag=1,count=0,flag_up=0,flag_down=0,done=0;

void delay(uchar x)  //延时函数
{
	uchar y,z;
	for(z=x;z>0;z--)
		for(y=110;y>0;y--);
}

/*****************************与ds1302相关函数*********************************/

void write_ds1302_byte(uchar dat)   //ds1302写一字节
{
	uchar i;
	for(i=0;i<8;i++)   //分8次循环，把（add）一位一位的写入
	{
		ds1302_sclk=0;	
		ds1302_io =dat&0x01;   //dat和0x01进行按位与，保留最低位。即从低位开始写入
		dat=dat>>1;   //add右移一位
		ds1302_sclk=1; 
	}
}

void ds1302_write(uchar add,uchar date)  //单字节写操作函数（包含地址和数据）
{
	ds1302_ce=0;      
	ds1302_sclk=0;     
	ds1302_ce=1;     //ce变为高电平，允许单片机向DS1302中的寄存器写入地址命令和数据
	_nop_();         
	write_ds1302_byte(add);    //写入控制命令（指明写入的寄存器地址及后续的操作为写操作）
	write_ds1302_byte(date);   //写入一个字节数据
	ds1302_ce=0;     //地址命令和数据写完之后，把ce拉低，防止对写入的数据进行误操作
	_nop_();         
	ds1302_io =1;    //释放IO数据线
	ds1302_sclk=1;   //释放sclk时钟线
}
 
uchar ds1302_read(uchar add)  //单字节读操作函数
{
	uchar i,value;
	ds1302_ce=0;     
	ds1302_sclk=0;    
	ds1302_ce=1;  //ce变为高电平，单片机向DS1302中的寄存器写入地址命令，并读出数据
	_nop_();        //稍微延时（一个机器周期）
	write_ds1302_byte(add);   //写入控制命令,（指明要读取的寄存器地址及后续的操作为读操作）
	for(i=0;i<8;i++)   //分8次循环，把数据一位一位的读出
	{
		value=value>>1;     //value右移一位
		ds1302_sclk=0;    //把sclk时钟置为低电平，开始读取数据
		if(ds1302_io)
		{
			value=value|0x80;    //把I0读到的值，放到value 的最高位
		}
		ds1302_sclk=1; 
	}
	ds1302_ce=0;      //一个字节的数据读完之后，把ce拉低
	_nop_();
	ds1302_sclk=0;     //释放sclk时钟线
	_nop_();
	ds1302_sclk=1;
	ds1302_io=1;      //释放io数据线
	return value;    //把 value值返回
}

void set_rtc()   //设置时间初值函数
{
	uchar i,j,k;
	for(i=0;i<7;i++)  //把数组time_data[]中的7个十进制数转换成BCD码
	{
		j=time_data[i]/10;
		k=time_data[i]%10;
		time_data[i]=k+j*16;
	}
	ds1302_write(0x8e,0x00);  //在往寄存器写之前，要去除写保护（8e为写保护寄存器的地址）
	for(i=0;i<7;i++)   //分7次循环，向年、周、月、日、时、分、秒的寄存器写入时间数据
	{
		ds1302_write(write_add[i],time_data[i]);
	}
	ds1302_write(0x8e,0x80);  //加写保护
}


/***************************与lcd1602相关函数***************************/
void lcd_write_com(uchar com)   //写命令函数
{
	lcd1602_rs=0;     //数据命令选择端，写命令时设为0
	lcd1602_rw=0;    //读写选择端，写时设为0
	lcd1602_en=0;  
	P0=com;        //将要写的命令送到数据总线上
	lcd1602_en=1;   //使能端电平变换，形成一个高脉冲将命令写入到液晶控制器
	delay(5);       //延时
	lcd1602_en=0;		
}

void lcd_write_date(uchar date)    //写数据函数
{
    lcd1602_rs=1;      //数据命令选择端，写数据时设为1
	lcd1602_rw=0;     //读写选择端，写时设为0
	lcd1602_en=0;
	P0=date;       //将要写的数据送到数据总线上
	lcd1602_en=1;  //使能端电平变换，形成一个高脉冲将命令写入到液晶控制器
	delay(5);       //延时
	lcd1602_en=0;	
}

void lcd1602_init()	         //LCD1602初始化函数	
{
	uchar lcdnum;
	lcd_write_com(0x38);  
	lcd_write_com(0x0c);   
	lcd_write_com(0x06);   
	lcd_write_com(0x01);   

	lcd_write_com(0x80);                 //写第一行数据 
	for(lcdnum=0;lcdnum<16;lcdnum++)	      
	{
		lcd_write_date(table1[lcdnum]);
		delay(2);
	}
	lcd_write_com(0x80+0x40);			 //写入第二行数据 
	for(lcdnum=0;lcdnum<16;lcdnum++)
	{
		lcd_write_date(table2[lcdnum]);
		delay(2);
	}
}

/***********************************显示部分********************************/

void write_time1(uchar add,uchar date)    //日期送显示
{	
	uchar shi,ge;
	shi=date/10;
	ge=date%10;
	lcd_write_com(0x80+add);
	lcd_write_date(0x30+shi);
	lcd_write_date(0x30+ge);
}

void write_time2(uchar add,uchar date)    //时间送显示
{	
	uchar shi,ge;
	shi=date/10;
	ge=date%10;
	lcd_write_com(0x80+0x44+add);
	lcd_write_date(0x30+shi);
	lcd_write_date(0x30+ge);
}

uchar bcd_decimal(uchar bcd)              //bcd转换十进制
{
	uchar decimal;
	decimal=bcd>>4;
	decimal=decimal*10+(bcd&=0x0f);
	return decimal; 	
}

void write_week(uchar we)        //星期送显示
{
	lcd_write_com(0x80+0x0f);


	switch(we)
	{
		case 1: lcd_write_date('1');
		 		break;
		case 2:	lcd_write_date('2');
		 		break;
		case 3:	lcd_write_date('3');
		 		break;
		case 4: lcd_write_date('4');
		 		break;	
		case 5: lcd_write_date('5');
		 		break;
		case 6: lcd_write_date('6');
		 		break;
		case 7: lcd_write_date('7');
		 		break;		 					 				 			
	}	
}

/******************************按键处理******************************/

uchar key_bcd(uchar key_decimal)    //转成ds1302所需的BCD码 
{
	uchar temp;
	temp=(((key_decimal/10)&0x0f)<<4)|(key_decimal%10);
	return temp;	
}

void key_up_down()     //加减键处理函数
{
	if(up==0)   //加键处理
	{
		delay(50);
		flag_up=1;     //"加"更新标志
		while(!up);
		switch(count)
		{
			case 1:
					miao++;
					if(miao>59)
						miao=0;
					break;
			case 2:
					fen++;
					if(fen>59)
						fen=0;
					break;
			case 3:
					shi++;
					if(shi>23)
						shi=0;
					break;				
			case 4:
					week++;
					if(week>7)
						week=1;
					break;				
			case 5:
					ri++;
					if(ri>31)
						ri=1;
					break;					
			case 6:
					yue++;
					if(yue>12)
						yue=1;
					break;					
			case 7:	
					nian++;
					if(nian>99)
						nian=0;
					break;												
		}
	}
	if(down==0)    //减键处理
	{
		delay(50);
		flag_down=1;	   //"减"更新标志			
		while(!down);
		switch(count)
		{
			case 1:
					miao--;
					if(miao==255)
						miao=59;
					break;
			case 2:
					fen--;
					if(fen==255)
						fen=59;
					break;
			case 3:
					shi--;
					if(miao==255)
						shi=23;
					break;				
			case 4:
					week--;
					if(week<1)
						week=7;
					break;				
			case 5:
					ri--;
					if(ri<1)
						ri=31;
					break;					
			case 6:
					yue--;
					if(yue<1)
						yue=12;
					break;					
			case 7:	
					nian--;
					if(nian==255)
						nian=99;
					break;												
		}
	}			
}


void key_set()   //ste键扫描函数
{
	if(set==0 )
	{
		delay(50);
		if(set==0)
		{	
			while(!set);      //等待set释放
			count++;        //按set一下，count加1，进入时间调速扫描程序  
			lcd_write_com(0x0f);  //把1602液晶显示光标并闪烁
			if(flag==1)
				{
					done=1;
					flag=0;
					ds1302_write(0x8e,0x00);	//写允许	
					ds1302_write(0x80,key_bcd(miao)|0x80); //BIT7为1，晶振停止工作
					ds1302_write(0x8e,0x80);        //写保护	
				}		
		}
	}

}

void keyjpress()	//按键处理
{	
	key_set();
	if(count!=0)           //count不为0，进入时间调整扫描
	{
			
		switch(count)
		{
			case 1:do    
				   {
						lcd_write_com(0x80+0x40+11);     //调整秒
						key_up_down();
						if(flag_up || flag_down)
						{	
							flag_up=0;
							flag_down=0;
							ds1302_write(0x8e,0x00);			
							ds1302_write(0x80,key_bcd(miao) | 0x80);
							ds1302_write(0x8e,0x80);        
	    					write_time2(6,miao);
	    					lcd_write_com(0x80+0x40+7);  //液晶写字符后光标自动
                                                         //向右移动一位，把光标
                                                         //左移一位
	    				}
	    		   }
	    			while(count==2);
					break;

			case 2:do   
				   {
						lcd_write_com(0x80+0x40+8);      //调整分
						key_up_down();						
						if(flag_up || flag_down)
						{	
							flag_up=0;				   	
							flag_down=0;
							ds1302_write(0x8e,0x00);			
							ds1302_write(0x82,key_bcd(fen));
							ds1302_write(0x8e,0x80);        
	    					write_time2(3,fen);
	    					lcd_write_com(0x80+0x40+4);
	    				}	
	    			}
	    			while(count==3);	
					break;

			case 3:do                                 
				   {	
						lcd_write_com(0x80+0x40+5);   //调整时
						key_up_down();
						if(flag_up || flag_down)
						{	
							flag_up=0;				   	
							flag_down=0;
							ds1302_write(0x8e,0x00);			
							ds1302_write(0x84,key_bcd(shi));
							ds1302_write(0x8e,0x80);        
	    					write_time2(0,shi);
	    					lcd_write_com(0x80+0x40+1);
	    				}
	    			}
	    			while(count==4);
					break;			
			case 4: do
				   {
						lcd_write_com(0x80+0x0f);    //调整星期
						key_up_down();
						if(flag_up || flag_down)
						{	
							flag_up=0;				   	
							flag_down=0;						
							ds1302_write(0x8e,0x00);			
							ds1302_write(0x8a,key_bcd(week));
							ds1302_write(0x8e,0x80);        
	    					write_week(week);
	    					lcd_write_com(0x80+0x0e);
	    				}	
	    			}
	    			while(count==5);
					break;			
			case 5:do
				   {	
						lcd_write_com(0x80+9);   //调整日
						key_up_down();
						if(flag_up || flag_down)
						{	
							flag_up=0;				   	
							flag_down=0;
							ds1302_write(0x8e,0x00);			
							ds1302_write(0x86,key_bcd(ri));
							ds1302_write(0x8e,0x80);        
	    					write_time1(8,ri);
	    					lcd_write_com(0x80+9);
	    				}
	    			}
					while(count==6);
					break;			
			case 6:do
				   {		    
						lcd_write_com(0x80+6);   //调整月
						key_up_down();
						if(flag_up || flag_down)
						{	
							flag_up=0;				   	
							flag_down=0;						
							ds1302_write(0x8e,0x00);			
							ds1302_write(0x88,key_bcd(yue));
							ds1302_write(0x8e,0x80);        
	    					write_time1(5,yue);
	    					lcd_write_com(0x80+6);
	    				}
	    			}
	    			while(count==7);
					break;			
			case 7:
						lcd_write_com(0x80+3);     //调整年
						key_up_down();
						if(flag_up || flag_down)
						{	
							flag_up=0;				   	
							flag_down=0;
							ds1302_write(0x8e,0x00);			
							ds1302_write(0x8c,key_bcd(nian));
							ds1302_write(0x8e,0x80);        
	    					write_time1(2,nian);
	    					lcd_write_com(0x80+3);
	    				}
	    			break;
			case 8:	
					lcd_write_com(0x0c);	 //调整结束，关闭显示光标
			 		flag=1;
			 		done=0;
					count=0;
					ds1302_write(0x8e,0x00);			
					ds1302_write(0x80,key_bcd(miao)&0x7f); //BIT7为0,晶振开始工作
					ds1302_write(0x8e,0x80); 
					break;	    						
			default:break;											
		}	
	}

}

void xianshi()  //1602液显示函数
{
  //读秒分时，日月年，星期		
	miao=bcd_decimal(ds1302_read(0x81));  
	fen =bcd_decimal(ds1302_read(0x83));
	shi =bcd_decimal(ds1302_read(0x85));
	ri  =bcd_decimal(ds1302_read(0x87));
	yue =bcd_decimal(ds1302_read(0x89));
	nian=bcd_decimal(ds1302_read(0x8d));
	week=bcd_decimal(ds1302_read(0x8b));
  //送液晶显示		
	write_time2(6,miao);
	write_time2(3,fen);
	write_time2(0,shi);
	write_time1(8,ri);
	write_time1(5,yue);
	write_time1(2,nian);
	write_week(week);	
 
}

main()  //主函数
{
	uchar ch;  
	lcd1602_en=0;  //关闭1602液晶使能，防止1602数据端影响单片机P0口输出
	P0=0xff;	
	lcd1602_init();    //1602液晶初始化

	ch=ds1302_read(0x81)>>7;  //读取DS1302时钟的秒寄存器的值，并保留最高位
	if(ch)   //判断：若ch为0，不设置初值；若ch为1，重新设置初值
	{
	set_rtc();   //对DS1302设置时间初值
	}
	while(1)
	{
		if(done==1)
		{	
			keyjpress();
		}
		if(done==0)
		{
			xianshi();        //取得并显示日历、时间
			key_set();
		
		}				
	}
}  