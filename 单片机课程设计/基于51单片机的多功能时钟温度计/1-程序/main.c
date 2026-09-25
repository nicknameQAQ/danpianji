#include<reg52.h>
#include<intrins.h>
#define uchar unsigned char 
#define uint unsigned int
#define     LCDIO      P0
sbit rs=P2^0;  
sbit rd=P2^1;
sbit lcden=P2^2;
sbit acc0=ACC^0;             //移位时的第0位
sbit acc7=ACC^7;             //移位时用的第7位
sbit ct=P2^3;
sbit speaker=P3^5;
sbit DQ=P2^5;//ds18b20与单片机连接口
int djs11=0,js,dsmin=0,dshour=0;

uchar second,minute,hour,day,month,year,week,count=0;    
uchar ReadValue,num,time;    
uint tvalue;//温度值
uchar tflag;
uchar code	table[]={" 2000-00-00 FRI"};
uchar code table1[]={" 00:00:00       "};
uchar code table2[]= "THUFRISATSUNMONTUEWES";
char szdata[8];
char szdata1[8];
uchar data disdata[5];
sbit DATA=P3^3;   //时钟数据接口
sbit RST=P3^4;    
sbit SCLK=P3^2;   
sbit menu=P2^7;              //菜单
sbit add=P2^6;               //加一
sbit dec=P2^4;
sbit set=P2^3 ;              //减一
sbit djs=P3^6;
void miaobiao();
void yx();
void delayms(int x)
{
char i;
while(x--)
 {
  for(i=150;i>0;i--); 
 }
}
void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=110;y>0;y--);
}
void delay1(uint z)
{
	for(;z>0;z--);
}
void write_com(uchar com)
{
	rs=0;
	rd=0;
	lcden=0;
	LCDIO=com;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;	
}

void write_date(uchar date)
{
	rs=1;
	rd=0;
	lcden=0;
	LCDIO=date;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;	
}

void init()
{
	uchar num;
	lcden=0;

	write_com(0x38);
	write_com(0x0c);
	write_com(0x06);
	write_com(0x01);
	write_com(0x80);
	delay(5);
	write_com(0x80);
	for(num=0;num<15;num++)
		{
			write_date(table[num]);
			delay(5);
		}
	write_com(0x80+0x40);
	for(num=0;num<15;num++)
		{
			write_date(table1[num]);
			delay(5);
		}       
	

}

void Write1302(uchar dat)
{
  uchar i; 
  SCLK=0;           //拉低SCLK，为脉冲上升沿写入数据做好准备
  delay1(2);       //稍微等待，使硬件做好准备
  for(i=0;i<8;i++)      //连续写8个二进制位数据
    {
     DATA=dat&0x01;    //取出dat的第0位数据写入1302
  delay(2);       //稍微等待，使硬件做好准备
   SCLK=1;           //上升沿写入数据
   delay1(2);      //稍微等待，使硬件做好准备
   SCLK=0;           //重新拉低SCLK，形成脉冲
   dat>>=1;          //将dat的各数据位右移1位，准备写入下一个数据位
   }
 }

 void WriteSet1302(uchar Cmd,uchar dat)
 {
   RST=0;           //禁止数据传递
   SCLK=0;          //确保写数居前SCLK被拉低
   RST=1;           //启动数据传输
   delay1(2);     //稍微等待，使硬件做好准备
   Write1302(Cmd);  //写入命令字
   Write1302(dat);  //写数据
   SCLK=1;          //将时钟电平置于已知状态
   RST=0;           //禁止数据传递
 }

uchar Read1302(void)
 {
   uchar i,dat;
   delay(2);       //稍微等待，使硬件做好准备
   for(i=0;i<8;i++)   //连续读8个二进制位数据
   {
    dat>>=1;       //将dat的各数据位右移1位，因为先读出的是字节的最低位
    if(DATA==1)    //如果读出的数据是1
    dat|=0x80;    //将1取出，写在dat的最高位
    SCLK=1;       //将SCLK置于高电平，为下降沿读出
    delay1(2);  //稍微等待
    SCLK=0;       //拉低SCLK，形成脉冲下降沿
    delay1(2);  //稍微等待
    }  
  return dat;        //将读出的数据返回
  }

uchar  ReadSet1302(uchar Cmd)
 {
  uchar dat;
  RST=0;                 //拉低RST
  SCLK=0;                //确保写数居前SCLK被拉低
  RST=1;                 //启动数据传输
  Write1302(Cmd);       //写入命令字
  dat=Read1302();       //读出数据
  SCLK=1;              //将时钟电平置于已知状态
  RST=0;               //禁止数据传递
  return dat;          //将读出的数据返回
}
void Init_DS1302(void)
{ 
 WriteSet1302(0x8E,0x00);                 //根据写状态寄存器命令字，写入不保护指令 
 WriteSet1302(0x80,((0/10)<<4|(0%10)));   //根据写秒寄存器命令字，写入秒的初始值
 WriteSet1302(0x82,((12/10)<<4|(12%10)));   //根据写分寄存器命令字，写入分的初始值
 WriteSet1302(0x84,((11/10)<<4|(11%10))); //根据写小时寄存器命令字，写入小时的初始值
 WriteSet1302(0x86,((25/10)<<4|(25%10))); //根据写日寄存器命令字，写入日的初始值
 WriteSet1302(0x88,((1/10)<<4|(1%10))); //根据写月寄存器命令字，写入月的初始值
 WriteSet1302(0x8c,((13/10)<<4|(13%10)));   //nian
 WriteSet1302(0x8a,((1/10)<<4|(1%10)));
}

void DisplaySecond(uchar x)
{
 uchar i,j;    
 i=x/10;
 j=x%10;
 write_com(0xc7);   
 write_date(0x30+i);   
 write_date(0x30+j);      
 }
void DisplayMinute(uchar x)
{
 uchar i,j;    
 i=x/10;
 j=x%10; 
 write_com(0xc4);    
 write_date(0x30+i);  
 write_date(0x30+j);        
 }
 void DisplayHour(uchar x)
{
 uchar i,j;   
 i=x/10;
 j=x%10;
 write_com(0xc1);    
 write_date(0x30+i);  
 write_date(0x30+j);        
 }
 
void DisplayDay(uchar x)
{
 uchar i,j;   
 i=x/10;
 j=x%10;    
 write_com(0x89);    
 write_date(0x30+i);  
 write_date(0x30+j);      
 }

void DisplayMonth(uchar x)
{
 uchar i,j;     
 i=x/10;
 j=x%10;  
 write_com(0x86);    
 write_date(0x30+i);  
 write_date(0x30+j);  
 }

void DisplayYear(uchar x)
{
 uchar i,j;  
  i=x/10;
 j=x%10; 
 write_com(0x83);    
 write_date(0x30+i);
 write_date(0x30+j);  
 }   
void DisplayWeek(uchar x)
{ uchar i;
   x=x*3;
 write_com(0x8c);    
 for(i=0;i<3;i++)
   {
   	write_date(table2[x]);
	x++;  
   }
 }   

void  read_date(void)
{
     ReadValue = ReadSet1302(0x81);   
     second=((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F);
     ReadValue = ReadSet1302(0x83);  
     minute=((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F); 
     ReadValue = ReadSet1302(0x85);  
     hour=((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F); 
     ReadValue = ReadSet1302(0x87);  
     day=((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F); 
     ReadValue = ReadSet1302(0x89);  
     month=((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F); 
     ReadValue = ReadSet1302(0x8d);  
     year=((ReadValue&0x70)>>4)*10 + (ReadValue&0x0F); 
	 ReadValue=ReadSet1302(0x8b);                     //读星期
     week=ReadValue&0x07;
	 if((year==szdata[1])&&(month==szdata[2])&&(day==szdata[3])&&(week==szdata[4])&&(hour==szdata[5])&&(minute==szdata[6])&&(second==szdata[7]))   	  speaker=0;
	 DisplaySecond(second);           
     DisplayMinute(minute);        
     DisplayHour(hour);             
     DisplayDay(day);                
     DisplayMonth(month);            
     DisplayYear(year);          
	 DisplayWeek(week);  
}

void turn_val(char newval,uchar flag,uchar  newaddr,uchar s1num)
    {
                   newval=ReadSet1302(newaddr);                     //读取当前时间
                   newval=((newval&0x70)>>4)*10+(newval&0x0f);    //将bcd码转换成十进制
                   if(flag)                  //判断是加一还是减一
                   { 
				     newval++;
                     switch(s1num)
                       { case 1: if(newval>99) newval=0;
					              DisplayYear(newval);      
								  break;
					     case 2: if(newval>12) newval=1;
						 		  DisplayMonth(newval); 
								  break;
					     case 3: if(newval>31) newval=1;
						          DisplayDay(newval);  
								  break;
					     case 4: if(newval>6) newval=0;
						          DisplayWeek(newval); 
								  break;
                         case 5: if(newval>23) newval=0;
						          DisplayHour(newval); 
								  break;
                         case 6: if(newval>59) newval=0;
						          DisplayMinute(newval); 
								  break;
                         case 7: if(newval>59) newval=0;
						          DisplaySecond(newval); 
								  break;
                         default:break;
                       } 
					 }   
				else
                   {
				    newval--;
					switch(s1num)
                       { case 1: if(newval<0) newval=99;
					              DisplayYear(newval); 
								  break;
					     case 2: if(newval<0) newval=12;
						 		  DisplayMonth(newval); 
						          break;
					     case 3: if(newval<0) newval=31;
						 		  DisplayDay(newval); 
						          break;
					     case 4: if(newval<0) newval=6;
						          DisplayWeek(newval); 
								  break;
                         case 5: if(newval<0) newval=23;
						          DisplayHour(newval); 
								  break;
                         case 6: if(newval<0) newval=59;
						          DisplayMinute(newval); 
								  break;
                         case 7: if(newval<0) newval=59;
						          DisplaySecond(newval);
								  break;
						 default:break;
				       }
					 }
                WriteSet1302((newaddr-1),((newval/10)<<4)|(newval%10));  //将新数据写入寄存器
           
	}   


void turn_val11(char newval,uchar flag,uchar  newaddr,uchar s1num)
    {
                   newval=szdata[s1num];                     //读取当前时间
                   //newval=((newval&0x70)>>4)*10+(newval&0x0f);    //将bcd码转换成十进制
                   if(flag)                  //判断是加一还是减一
                   { 
				     newval++;
                     switch(s1num)
                       { case 1: if(newval>99) newval=0;
					              DisplayYear(newval);      
								  break;
					     case 2: if(newval>12) newval=1;
						 		  DisplayMonth(newval); 
								  break;
					     case 3: if(newval>31) newval=1;
						          DisplayDay(newval);  
								  break;
					     case 4: if(newval>6) newval=0;
						          DisplayWeek(newval); 
								  break;
                         case 5: if(newval>23) newval=0;
						          DisplayHour(newval); 
								  break;
                         case 6: if(newval>59) newval=0;
						          DisplayMinute(newval); 
								  break;
                         case 7: if(newval>59) newval=0;
						          DisplaySecond(newval); 
								  break;
                         default:break;
                       } 
					 }   
				else
                   {
				    newval--;
					switch(s1num)
                       { case 1: if(newval<0) newval=99;
					              DisplayYear(newval); 
								  break;
					     case 2: if(newval<0) newval=12;
						 		  DisplayMonth(newval); 
						          break;
					     case 3: if(newval<0) newval=31;
						 		  DisplayDay(newval); 
						          break;
					     case 4: if(newval<0) newval=6;
						          DisplayWeek(newval); 
								  break;
                         case 5: if(newval<0) newval=23;
						          DisplayHour(newval); 
								  break;
                         case 6: if(newval<0) newval=59;
						          DisplayMinute(newval); 
								  break;
                         case 7: if(newval<0) newval=59;
						          DisplaySecond(newval);
								  break;
						 default:break;
				       }
					 }
                 szdata[s1num]= newval;
				 szdata1[s1num]=newval;
				//WriteSet1302((newaddr-1),((newval/10)<<4)|(newval%10));  //将新数据写入寄存器
           
	}   

//键盘扫描程序
//*******************************************
void key_scan(void)
{	uchar miao,s1num=0;	

	if(menu==0)
     { 
       delay(5);
	   if(menu==0)
	   {
	     while(!menu);	  
		  s1num++;
		  			       write_com(0xc9);
	   write_date(' ');//显示符号位
	   write_com(0xca);
	   write_date(' ');//显示百位
	   write_com(0xcb);
	   write_date('T');//显示十位 	
	   write_com(0xcc);
	   write_date('-');//显示个位 	
	   write_com(0xcd);
	   write_date('S');//显示小数点 	
	   write_com(0xce);
	   write_date('E');//显示小数位
	   write_com(0xcf);
	   write_date('T');
		while(1)
	    {

		    if(menu==0)
             { 
                delay(5);
	            if(menu==0)
	              { 
				     while(!menu);
	                 s1num++;
		          }
	         }

	        rd=0;
			miao=ReadSet1302(0x81);
			second=miao;
			WriteSet1302(0x80,miao|0x80);
		    write_com(0x0f);//光标闪射
			
			if(s1num==1)
			{	year=ReadSet1302(0x8d);		 
				write_com(0x80+4);	//年光标

			    if(add==0)
			     {
			       delay(3);
			       if(add==0)
			         { 	while(!add);
		               turn_val(year,1,0x8d,1);			
               
			         }
			      }
	            if(dec==0)
		         {
			        delay(3);
			        if(dec==0)
			         {	while(!dec);
			           turn_val(year,0,0x8d,1);				      
			         }
			      }
			 }
			if(s1num==2)
			{
			  month=ReadSet1302(0x89);
				write_com(0x80+7);	//月光标
				 if(add==0)
			{
			  delay(3);
			  if(add==0)
			   {  while(!add);
			      turn_val(month,1,0x89,2);
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   {  while(!dec);
			      turn_val(month,0,0x89,2);
			   }
			}
			}
			if(s1num==3)
			{	day=ReadSet1302(0x87);
				write_com(0x80+10);//日光标
				if(add==0)
			  {
			  delay(3);
			  if(add==0)
			   { while(!add);
		         turn_val(day,1,0x87,3);
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
			     turn_val(day,0,0x87,3);				//写入日寄存器
			   }
			}
			}
			if(s1num==4)
			{  week=ReadSet1302(0x8b);
				write_com(0x80+14);	//星期光标
				  if(add==0)
		     	{
			  delay(3);
			  if(add==0)
			   { while(!add);
			     turn_val(week,1,0x8b,4);
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
	             turn_val(week,0,0x8b,4);
			   }
			}
			}
			if(s1num==5)
			{	hour=ReadSet1302(0x85);
				write_com(0x80+0x40+2);	//时光标
				 if(add==0)
			{
			  delay(3);
			  if(add==0)
			   { while(!add);
                 turn_val(hour,1,0x85,5);
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
		         turn_val(hour,0,0x85,5);
			   }
			}
			}
			if(s1num==6)//调时间分
			{  minute=ReadSet1302(0x83);
				write_com(0x80+0x40+5);
				if(add==0)
			{
			  delay(5);
			  if(add==0)
			   { while(!add);
		         turn_val(minute,1,0x83,6);				//写入分寄存器   
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
		         turn_val(minute,0,0x83,6);				//写入分寄存器
			   }
			}
			}
			if(s1num==7)//调时间秒
			{	second=ReadSet1302(0x81);
				write_com(0x80+0x40+8);//秒光标
				if(add==0)
			{
			  delay(3);
			  if(add==0)
			   { while(!add);
                 if(second==0x60)
                    second=0x00;
		         	turn_val(second,1,0x81,7);	
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
		         turn_val(second,0,0x81,7);				
			   }
			}
			}
	    

			if(s1num==8)
			{	miao=ReadSet1302(0x81);
				second=miao;
				WriteSet1302(0x80,second&0x7f);
				s1num=0;//s1num清零//
				write_com(0x0c);//光标不闪烁//
				break;
			}
		}
	}
	}

  if(ct==0)
     { 
       delay(5);
	   if(ct==0)
	   {
	     while(!ct);	  
		  s1num++;
       write_com(0xc9);
	   write_date(' ');//显示符号位
	   write_com(0xca);
	   write_date(' ');//显示百位
	   write_com(0xcb);
	   write_date('C');//显示十位 	
	   write_com(0xcc);
	   write_date('-');//显示个位 	
	   write_com(0xcd);
	   write_date('S');//显示小数点 	
	   write_com(0xce);
	   write_date('E');//显示小数位
	   write_com(0xcf);
	   write_date('T');
	 DisplaySecond(szdata1[7]);           
     DisplayMinute(szdata1[6]);        
     DisplayHour(szdata1[5]);             
     DisplayDay(szdata1[3]);                
     DisplayMonth(szdata1[2]);            
     DisplayYear(szdata1[1]);          
	 DisplayWeek(szdata1[4]); 
		while(1)
	    {

		    if(ct==0)
             { 
                delay(5);
	            if(ct==0)
	              { 
				     while(!ct);
	                 s1num++;
		          }
	         }

	        rd=0;
			miao=ReadSet1302(0x81);
			second=miao;
			WriteSet1302(0x80,miao|0x80);
		    write_com(0x0f);//光标闪射
			
			if(s1num==1)
			{	year=0x00;		 
				write_com(0x80+4);	//年光标

			    if(add==0)
			     {
			       delay(3);
			       if(add==0)
			         { 	while(!add);
		               turn_val11(year,1,0x8d,1);			
               
			         }
			      }
	            if(dec==0)
		         {
			        delay(3);
			        if(dec==0)
			         {	while(!dec);
			           turn_val11(year,0,0x8d,1);				      
			         }
			      }
			 }
			if(s1num==2)
			{
			  month=0x00;	
				write_com(0x80+7);	//月光标
				 if(add==0)
			{
			  delay(3);
			  if(add==0)
			   {  while(!add);
			      turn_val11(month,1,0x89,2);
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   {  while(!dec);
			      turn_val11(month,0,0x89,2);
			   }
			}
			}
			if(s1num==3)
			{	day=0x00;	
				write_com(0x80+10);//日光标
				if(add==0)
			  {
			  delay(3);
			  if(add==0)
			   { while(!add);
		         turn_val11(day,1,0x87,3);
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
			     turn_val11(day,0,0x87,3);				//写入日寄存器
			   }
			}
			}
			if(s1num==4)
			{  week=0x00;	
				write_com(0x80+14);	//星期光标
				  if(add==0)
		     	{
			  delay(3);
			  if(add==0)
			   { while(!add);
			     turn_val11(week,1,0x8b,4);
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
	             turn_val11(week,0,0x8b,4);
			   }
			}
			}
			if(s1num==5)
			{	hour=0x00;	
				write_com(0x80+0x40+2);	//时光标
				 if(add==0)
			{
			  delay(3);
			  if(add==0)
			   { while(!add);
                 turn_val11(hour,1,0x85,5);
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
		         turn_val11(hour,0,0x85,5);
			   }
			}
			}
			if(s1num==6)//调时间分
			{  minute=0x00;	
				write_com(0x80+0x40+5);
				if(add==0)
			{
			  delay(5);
			  if(add==0)
			   { while(!add);
		         turn_val11(minute,1,0x83,6);				//写入分寄存器   
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
		         turn_val11(minute,0,0x83,6);				//写入分寄存器
			   }
			}
			}
			if(s1num==7)//调时间秒
			{	second=0x00;	
				write_com(0x80+0x40+8);//秒光标
				if(add==0)
			{
			  delay(3);
			  if(add==0)
			   { while(!add);
                 if(second==0x60)
                    second=0x00;
		         	turn_val11(second,1,0x81,7);	
			    }
			}
			if(dec==0)
			{
			   delay(3);
			   if(dec==0)
			   { while(!dec);
		         turn_val11(second,0,0x81,7);				
			   }
			}
			}
	    

			if(s1num==8)
			{	miao=ReadSet1302(0x81);	
				second=miao;
				WriteSet1302(0x80,second&0x7f);
				s1num=0;//s1num清零//
				write_com(0x0c);//光标不闪烁//
				break;
			}
		}
	}
	}
}
 
void Timer0_init()
{
      js=0;
	  TMOD=0x01;	  			//T0为定时器方式1
      TH0=0x4c;
	  TL0=0x00;			
     //IT1=1;
	  ET0=1;//EX1=1;
	  EA=1;	
}

void timer0() interrupt 1 using 1	
{
	    TF0=0;
	 	//TH0=0x4c;
		//TL0=0x00;
		TH0=0xb3;
		TL0=0x4c;
		js++;
		if(js==1) 
		  { js=0;
		    djs11++;
			//led1=~led1;
			if(djs11>59) 
			  {
			    djs11=0;
				dsmin=dsmin+1;
				if(dsmin>59) 
				 { 
				   dsmin=0;
				   dshour=dshour+1;
				   if(dshour>24) dshour=0;
				 }
			}
		   }
}
void miaobiao()
{		int i,j;

            write_com(0x80);write_date(' '); 
			write_com(0x81);write_date(' ');
			write_com(0x82);write_date(' ');
			write_com(0x83);write_date(' ');
			write_com(0x84);write_date(' ');
			write_com(0x85);write_date(' ');
			write_com(0x86);write_date(' ');
			write_com(0x87);write_date(' ');
		    write_com(0x88);write_date(' '); 
			write_com(0x89);write_date(' ');
			write_com(0x8a);write_date(' ');
			write_com(0x8b);write_date(' ');
			write_com(0x8c);write_date(' ');
			write_com(0x8d);write_date(' ');
			write_com(0x8e);write_date(' ');
			write_com(0x8f);write_date(' ');
			write_com(0xc0);write_date(' '); 
			write_com(0xc1);write_date(' ');
			write_com(0xc2);write_date(' ');
			write_com(0xc3);write_date(' ');
			write_com(0xc4);write_date(' ');
			write_com(0xc5);write_date(' ');
			write_com(0xc6);write_date(' ');
			write_com(0xc7);write_date(' ');
		    write_com(0xc8);write_date(' '); 
			write_com(0xc9);write_date(' ');
			write_com(0xca);write_date(' ');
			write_com(0xcb);write_date(' ');
			write_com(0xcc);write_date(' ');
			write_com(0xcd);write_date(' ');
			write_com(0xce);write_date(' ');
			write_com(0xcf);write_date(' ');
		   while(1)
		    {	if(add==0)
			      {
				  delayms(30);
				    if(add==0)
					  {
					    TR0=1;
					  }

				  }
				if(dec==0)
			      {
				  delayms(30);
				    if(dec==0)
					  {
					    TR0=0;
					  }
					  }
				if(set==0)
			      {
				  delayms(30);
				    if(set==0)
					  {
					    djs11=0;
						TR0=0;
												dsmin=0;
						dshour=0;
					  }
					  }
			   	if(djs==0)
			      {
				  delayms(30);
				    if(djs==0)
					  {
						yx();
					  }
					  }
			write_com(0xc4); 
						 i=dshour/10;
             j=dshour%10;    
             write_date(0x30+i);  
             write_date(0x30+j); 

			 write_date(':');  
			 i=dsmin/10;
             j=dsmin%10;    
             write_date(0x30+i);  
             write_date(0x30+j);
			 write_date(':');   

			 			 i=djs11/10;
             j=djs11%10; 
             write_date(0x30+i);  
             write_date(0x30+j);
			}
}
void delay_18B20(unsigned int i)//延时1微秒
{
 	while(i--);
}
void ds1820rst()/*ds1820复位*/
 {  unsigned char x=0;
	 DQ = 1;          //DQ复位
	 delay_18B20(4);  //延时
	 DQ = 0;          //DQ拉低
	 delay_18B20(100); //精确延时大于480us
	 DQ = 1;          //拉高
	 delay_18B20(40);	 
   }  
  
   uchar ds1820rd()/*读数据*/
  { unsigned char i=0;
	 unsigned char dat = 0;
	 for (i=8;i>0;i--)
	 {   DQ = 0; //给脉冲信号
		  dat>>=1;
		  DQ = 1; //给脉冲信号
		  if(DQ)
		  dat|=0x80;
		  delay_18B20(10);
	 }
 	return(dat);
  }

  void ds1820wr(uchar wdata)/*写数据*/
  {unsigned char i=0;
    for (i=8; i>0; i--)
   { DQ = 0;
     DQ = wdata&0x01;
     delay_18B20(10);
     DQ = 1;
     wdata>>=1;
   }
 }
read_temp()/*读取温度值并转换*/
 {uchar a,b;
  ds1820rst();    
  ds1820wr(0xcc);//*跳过读序列号*/
  ds1820wr(0x44);//*启动温度转换*/
  ds1820rst();    
  ds1820wr(0xcc);//*跳过读序列号*/ 
  ds1820wr(0xbe);//*读取温度*/ 
  a=ds1820rd();
  b=ds1820rd();
  tvalue=b;
  tvalue<<=8;
  tvalue=tvalue|a;
    if(tvalue<0x0fff)
   tflag=0;
    else
   {tvalue=~tvalue+1;
	 tflag=1;
   }
  tvalue=tvalue*(0.625);//温度值扩大10倍，精确到1位小数
	return(tvalue);
  }
void ds1820disp()//温度值显示
	{ uchar flagdat;
	  disdata[0]=tvalue/1000+0x30;//百位数
     disdata[1]=tvalue%1000/100+0x30;//十位数
     disdata[2]=tvalue%100/10+0x30;//个位数
     disdata[3]=tvalue%10+0x30;//小数位
    
     if(tflag==0)
	    flagdat=0x20;//正温度不显示符号
     else
       flagdat=0x2d;//负温度显示负号:-

     if(disdata[0]==0x30)
	   {disdata[0]=0x20;//如果百位为0，不显示
		 if(disdata[1]==0x30)
		  {disdata[1]=0x20;//如果百位为0，十位为0也不显示
		  }
		}

	   write_com(0xc9);
	   write_date(flagdat);//显示符号位
	   write_com(0xca);
	   write_date(disdata[0]);//显示百位
	   write_com(0xcb);
	   write_date(disdata[1]);//显示十位 	
	   write_com(0xcc);
	   write_date(disdata[2]);//显示个位 	
	   write_com(0xcd);
	   write_date(0x2e);//显示小数点 	
	   write_com(0xce);
	   write_date(disdata[3]);//显示小数位
	   write_com(0xcf);
	   write_date('C');
   }

void yx()
{
init();
while(1)
	{ 
	 read_date(); 
	 read_temp();//读取温度
	 ds1820disp();//显示
	 key_scan(); 
	 if(add==0)
			     {
				  delayms(30);
				    if(add==0)
					  {
					     speaker=1;
					  }

				  }
				if(dec==0)
			      {
				  delayms(30);
				    if(dec==0)
					  {
					     speaker=1;
					  }
					  }
	 if(djs==0)
	  {
	   delay(5);
	    if(djs==0)
		  {
		   miaobiao();		 
		  }	
		}
	}

}
 
void main()
{
	init();
	Timer0_init();
//	Init_DS1302();       //将1302初始化 
     speaker=1;
     szdata1[1]=0x00;
	 szdata1[2]=0x00;
	 szdata1[3]=0x00;
	 szdata1[4]=0x00;
	 szdata1[5]=0x00;
	 szdata1[6]=0x00;
	 szdata1[7]=0x00;
	yx();
}
