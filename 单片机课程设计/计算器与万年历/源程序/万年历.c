#include <reg52.h>
#include <intrins.h>  //包含_nop_()函数定义的头文件
#define uchar unsigned char
#define uint  unsigned int
//以下是对1302的定义 
sbit  clk= P2^3;              //实时时钟时钟线引脚 
sbit  IO= P2^2;              //实时时钟数据线引脚 
sbit  rst =P2^1;              //实时时钟复位线引脚
uchar second,minute,hour,week,day,month,year;       //8421码表示的时间  高四位用于表示十位，低四位用于表示个位
char second1,minute1,hour1,week1,day1,month1,year1;//十进制表示的时间  也即普通意义下的时间
uchar read_byte(uchar com);
uchar readyear_byte(uchar com);
void write_byte(uchar com,uchar date); 
void init_1302();
void readtime(); 
//以下是对lcd1602的定义
sbit wela=P2^6;
sbit dula=P2^7;
uchar table[]="0123456789";
sbit LCM_RS=P2^6;    //寄存器选择位，将RS位定义为P2.6引脚
sbit LCM_EN=P2^5;     //使能信号位，将E位定义为P2.5引脚
bit lcd_bz(void);
void write_com(unsigned char CMD);
void write_date(unsigned char dataW);
void init_1602();
void displaytime();
void delaynms(unsigned char n);
//以下是对按键调时程序的定义
sbit mode=P1^0;	 //调整闹钟或者时间时的选择按键
sbit plus=P1^1;	 //加1键
sbit minus=P1^2; //减1键
sbit s1=P1^3;	 //计算器和万年历切换键
void key();
void delay20ms(); 
//以下是对温度采集部分的定义 
sbit DS=P2^0; 			 //DS18B20数据端口
uchar code wendu[]="0123456789";        //利用一个温度表解决温度显示乱码 
void write_byte(uchar com,uchar date);	 //写一个字节
void tmpDelay(int num);					// 延时
void Init_DS18B20();					//初始化DS18B20
unsigned char ReadOneChar();		    //读一个字节
void WriteOneChar(unsigned char dat);	//写数据
unsigned int Readtemp();				//读温度
void display();							//显示温度

uchar num;	 
sbit lcden=P2^5; //定义引脚
sbit rs=P2^6;

sbit busy=P0^7;	 //LCD1602忙
//计算器有关变量定义
char i,j,temp,num_1,num1_2=0;
long a,b,c;     //a,第一个数 b,第二个数 c,得数
float a_c,b_c;
uchar flag,fuhao;//flag表示是否有符号键按下，fuhao表征按下的是哪个符号

uchar code table1[]={
7,8,9,0,
4,5,6,0,
1,2,3,0,
0,0,0,0};
uchar code table2[]={
7,8,9,0x2f-0x30,
4,5,6,0x2a-0x30,
1,2,3,0x2d-0x30,
0x01-0x30,0,0x3d-0x30,0x2b-0x30};

void delay2(uchar z) // 延迟函数
{
uchar y;
for(z;z>0;z--)
   for(y=0;y<110;y++);
}

void check() // 判断忙或空闲
{
do{
    P0=0xFF;    rs=0;     //指令
    lcden=0;     //禁止读写
    delay2(1); //等待，液晶显示器处理数据
    lcden=1;     //允许读写
	delay2(1);	busy=0;
    }while(busy==1); //判断是否为空闲，1为忙，0为空闲
}
void write_com1(uchar com) // 写指令函数
{
P0=com;    //com指令付给P0口
rs=0;lcden=0;
check();lcden=1;
}

void write_date1(uchar date) // 写数据函数
{
	P0=date;
	rs=1;
	lcden=0;
	check();
	lcden=1;
}

void init1() //初始化
{
	num=-1;
	lcden=1; //使能信号为高电平
	write_com1(0x38); //8位，2行
	delay2(5);
	write_com1(0x38); //8位，2行
	delay2(5);
	write_com1(0x0c); //显示开，光标关，不闪烁*/
	delay2(1);
	write_com1(0x06); //增量方式不移位
	delay2(1);
	write_com1(0x80); //检测忙信号
	delay2(1);
	write_com1(0x01); //显示开，光标关，不闪烁
	num_1=0;
	i=0;
	j=0;
	a=0;     //第一个参与运算的数
	b=0;     //第二个参与运算的数
	c=0;
	flag=0; //flag表示是否有符号键按下， 
	fuhao=0; // fuhao表征按下的是哪个符号
}
void keyscan1() // 键盘扫描程序
{
	P3=0xfe; 
	if(P3!=0xfe)
	{   delay2(20);// 延迟20ms
	   if(P3!=0xfe) { temp=P3&0xf0;
				    switch(temp)
				    {
				     case 0xe0:num=0;      break;
				     case 0xd0:num=1;      break; 
				     case 0xb0:num=2;      break;
				     case 0x70:num=3;      break;
				    }
	   } while(P3!=0xfe);
	   if(num==0||num==1||num==2)//如果按下的是'7','8'或'9
	   { 
	       if(j!=0){ write_com1(0x01);  j=0;  }
	       if(flag==0){  a=a*10+table1[num];  }//没有按过符号键	    
	       else{ b=b*10+table1[num];  }//如果按过符号键	     
	   }
	   else//如果按下的是'/'
	   {  flag=1; fuhao=4;}//4表示除号已按   
	   i=table2[num];   write_date1(0x30+i);
	}

	P3=0xfd;
	if(P3!=0xfd)
	{  delay2(20);
	   if(P3!=0xfd){ temp=P3&0xf0;
				    switch(temp)
				    {
				     case 0xe0:num=4;  break;
				     case 0xd0:num=5;  break;
				     case 0xb0:num=6;  break;
				     case 0x70:num=7; break;
				    }
	   } while(P3!=0xfd);
	   if(num==4||num==5||num==6&&num!=7)//如果按下的是'4','5'或'6'
	   { 
		    if(j!=0){write_com1(0x01); j=0;}
		    if(flag==0){  a=a*10+table1[num];   }//没有按过符号键  	    
		    else{  b=b*10+table1[num];   }//如果按过符号键	    
	   }
	   else//如果按下的是'/'
	   { flag=1;   fuhao=3;}//3表示乘号已按	   
	   i=table2[num]; 	   write_date1(0x30+i);
	}

	P3=0xfb;
	if(P3!=0xfb)
	{   delay2(20);
	   if(P3!=0xfb) { temp=P3&0xf0;
				    switch(temp)
				    {
				     case 0xe0:num=8;   break;
				     case 0xd0:num=9;   break;
				     case 0xb0:num=10;  break;
				     case 0x70:num=11;  break;
				    }
	   }  while(P3!=0xfb);
	   if(num==8||num==9||num==10)//如果按下的是'1','2'或'3'
	   { 
		    if(j!=0){ write_com1(0x01); j=0;  }
		    if(flag==0){ a=a*10+table1[num];  }//没有按过符号键		    
		    else//如果按过符号键
		    {   b=b*10+table1[num];    }
	   }
	   else if(num==11)//如果按下的是'-'
	   {  flag=1;   fuhao=2;}//2表示减号已按	   
	   i=table2[num];	   write_date1(0x30+i);
	}

	P3=0xf7;
	if(P3!=0xf7)
	{  delay2(20);
	   if(P3!=0xf7) {temp=P3&0xf0;
		    switch(temp)
		    {
		     case 0xe0:num=12; break;
		     case 0xd0:num=13; break;
		     case 0xb0:num=14; break;
		     case 0x70:num=15; break;
		    }
	   } while(P3!=0xf7);
	   switch(num)
	   {
	    case 12:{write_com1(0x01);a=0;b=0;flag=0;fuhao=0;}  break;//按下的是"清零"	     
	    case 13:{                //按下的是"0"
			       if(flag==0)//没有按过符号键
			       {   a=a*10;  write_date1(0x30);	P2=0;	 }
			       else if(flag>=1)//如果按过符号键
			       {    b=b*10;       write_date1(0x30);   }
		      }	     break;
	    case 14:{j=1;
	           if(fuhao==1)
			   {
			   		write_com1(0x80+0x4f);//按下等于键，光标前进至第二行最后一个显示处 
	           		write_com1(0x04);     //设置从后住前写数据，每写完一个数据，光标后退一格	
	           		c=a+b;
	           		while(c!=0){write_date1(0x30+c%10);c=c/10;  } 
	           		write_date1(0x3d);     //再写"="
	           		a=0;b=0;flag=0;fuhao=0;
	           }
	     	 else if(fuhao==2)
		  	   {
					write_com1(0x80+0x4f);//光标前进至第二行最后一个显示处
	            	write_com1(0x04);     //设置从后住前写数据，每写完一个数据，光标后退一格(这个照理说顺序不对，可显示和上段一样)
	           		if(a-b>0) 	c=a-b;
	           		else 	   	c=b-a;
	           		while(c!=0) {write_date1(0x30+c%10);c=c/10; } 
	           		if(a-b<0)     	 write_date1(0x2d);
	          		write_date1(0x3d);     //再写"="           
	          		a=0;b=0;flag=0;fuhao=0;
	           }
	      	 else if(fuhao==3)
			 	{write_com1(0x80+0x4f); write_com1(0x04);
	             	c=a*b;
	             	while(c!=0){ write_date1(0x30+c%10);c=c/10;} 
	             	write_date1(0x3d);  a=0;b=0;flag=0;fuhao=0;
	             }
	      	  else if(fuhao==4)
			  {	write_com1(0x80+0x4f); 	write_com1(0x04);
	            i=0;
					if(b!=0){
		            		c=(long)(((float)a/b)*1000);
		            		while(c!=0)	{ write_date1(0x30+c%10); c=c/10;
		           				i++;
		           				if(i==3)write_date1(0x2e); }
		           			 if(a/b<=0)	 {
							 	if(i<=2) {if(i==1) write_date1(0x30);
									write_date1(0x2e);	   //  .
									write_date1(0x30);}  							 	
		              			write_date1(0x30);	   //
							}
		            		write_date1(0x3d);  a=0;b=0;flag=0;fuhao=0;
						}
						else
						{
							write_date1('!'); 	write_date1('R');
							write_date1('O');	write_date1('R');
							write_date1('R'); 	write_date1('E');
						}
	           }
	      }  break;
	     case 15:{write_date1(0x30+table2[num]);flag=1;fuhao=1;}     break;
	   }
	}
}
/**************************************************************
时钟、计算器切换程序
*************************************************************/
void switchfunction()
{unsigned char ID;
  if(!s1)	//切换键按下
 {  delay2(20);	 //延时
 	if(!s1)		 //再判
	{ init1();	//计算器初始化
	  ID=1;	    //设置计算器工作模式标志
	  while(!s1) ;//等待切换键释放
		while(ID)
			{   
			   keyscan1();	//调矩阵键盘扫描 ，进入计算器工作模式
			   if(!s1)		//切换键按下
				 {  delay2(20);	if(!s1)
					{ ID=0;	 //退出计算器工作模式，进入万年历工作模式
					   init_1602();//万年历工作模式LCD1602初始化
					}
					while(!s1) ; //等待切换键按下
				 }
			}  					
	}
 }				
}  
/**************************************************************
主程序
*************************************************************/
main()
{	init_1602();   //LCD1602初始化
	while(1)
	{readtime();	//读时间 	
	 displaytime();	//显示时间
	 key();			//调整时间键盘
	 display();	   //读温度并显示
	 switchfunction();//调功能切换程序，若功能切换键按下，进入计算器显示模式
	 } 	
}  				

/*****************************************************
函数功能：延时若干毫秒
入口参数：n
***************************************************/
 void delaynms(unsigned char n) {    unsigned char t,i,j;
	for(t=0;t<n;t++)  for(i=0;i<10;i++)	    for(j=0;j<33;j++); }
/*********************************************************************
ds1302写一个字节的数据
参数：com 选择要写数据的寄存器地址
      date 向寄存器写入的数据
**********************************************************************/
void write_byte(uchar com,uchar date)
{
    uchar i;
    rst=0;  //没有先前的低电平不能工作,所以先将RST置成的低电平          
    clk=0;	//在写命令前clk也是低电平
    rst=1;	//开始写命令
	for(i=0;i<8;i++)
	{
	 if(com&0x01) IO=1;//获取com的最低位
	         else IO=0;
	 com=com>>1;	    //移位，为下一位的发送做好准备
	 clk=1;	 _nop_();		   //clk的高电平至少为250ns
	 clk=0;			   
	}                  //到此，命令传输完毕	
	for(i=0;i<8;i++)   //紧接着传输数据
	{
		 if(date&0x01) IO=1; 
		          else IO=0;
		 date=date>>1;	 clk=1;	 _nop_(); clk=0;			   
	 }
   rst=0;              //结束传输
}
/*********************************************************************
ds1302读一个字节的数据
参数：com 选择要读数据的寄存器地址
      date 从寄存器读出的数据  
**********************************************************************/
uchar read_byte(uchar com)
{
    uchar i; 
	uchar date; //用来存储读到的数据
    rst=0;	    //没有先前的低电平不能工作,所以先将RST置成的低电平
    clk=0;	    //在写命令前clk也是低电平
    rst=1;		//开始写命令
	for(i=0;i<8;i++)
	{
	 if(com&0x01) IO=1;//获取com的最低位
	         else IO=0;
	 com=com>>1;	    //移位，为下一位的发送做好准备
	 clk=0;	 _nop_();		   //clk的低电平至少为250ns
	 clk=1;			   
	}                  //到此，命令传输完毕
	/*********************************************************************
	 接收数据,接收的是1302发送的低7位，而dete的最高位为0，这样做的目的是避免了后边对
	 接收到数据的处理（因为日历的七个变量的最高位在计算时必须处理成0）。
	 这里通过少接收一次数据实现除去最高位。
	 *********************************************************************/
    for(i=0;i<8;i++)  
	{
	     if(IO==1) date|=0x80;//虽然也接收了八次，但第一次接收的数据不对（接收的肯定是1），且后来移位除掉了。实际上是接收了七位	
		 date=date>>1;		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
		 clk=1;	 _nop_();	 clk=0;             //时钟的下降沿,ds1302开始向主机发送数据
	 }
    rst=0;			   //结束传输
    return date;
} 
uchar readyear_byte(uchar com)
{
    uchar i; 
	uchar date; //用来存储读到的数据
    rst=0;	    //没有先前的低电平不能工作,所以先将RST置成的低电平
    clk=0;	    //在写命令前clk也是低电平
    rst=1;		//开始写命令
	for(i=0;i<8;i++)
	{
	 if(com&0x01) IO=1;//获取com的最低位
	         else IO=0;
	 com=com>>1;	    //移位，为下一位的发送做好准备
	 clk=0;
	 _nop_();		   //clk的低电平至少为250ns
	 clk=1;			   
	}                  //到此，命令传输完毕
    for(i=0;i<8;i++)  
	{
	 date=date>>1;  //由于空移了一次，所以没有浪费数据                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
	 clk=0;				   //时钟的下降沿,ds1302开始向主机发送数据
	 if(IO==1) date|=0x80;//这一次就能接收八次数据，没有无用的，也没有浪费
	 clk=1;             
	 }
    rst=0;			   //结束传输
	clk=0;
    return date;
}
 
void readtime()//读出时间  时间的每一部分的最高位都已经置零
{
year=readyear_byte(0x8d);
month=read_byte(0x89);
day=read_byte(0x87);
hour=read_byte(0x85);  
minute=read_byte(0x83); 
second=read_byte(0x81);
week=read_byte(0x8b); 
}
/*********************************************************************************
一下是对lcd1602的操作
***********************************************************************************/ 		  
/*******************************/ 	//lcd1602写命令		  
void write_com(unsigned char CMD)
{					
	delaynms(10);	LCM_RS=0;	LCM_EN=0;	_nop_();	_nop_();
	P0=CMD;		_nop_();	_nop_();	_nop_(); 	_nop_();	
	LCM_EN=1;_nop_();_nop_();_nop_(); _nop_();LCM_EN=0;
}					
/*******************************/	//lcd1602写数据	    	    
void write_date(unsigned char dataW) 
{					
	delaynms(10);	LCM_RS=1; 	LCM_EN=0;	_nop_();
	P0=dataW;  	_nop_();	_nop_(); 	_nop_(); 	_nop_();
	LCM_EN=1;	_nop_();  	_nop_(); 	_nop_();  	_nop_();
	LCM_EN=0;
}
void init_1602()//初始化液晶
{
	wela=0;dula=0;
	write_com(0x38);delay20ms();
	write_com(0x38); delay20ms();
	write_com(0x38);delay20ms();	 //初始化三次确保功能设定成功
	write_com(0x0c);  write_com(0x06);	write_com(0x01);
	
	write_com(0x85);//以下将液晶上不变的字符先显示出来
	write_date('/');
	write_com(0x88);write_date('/');
	write_com(0xc2);write_date(':');
	write_com(0xc5);write_date(':');
	write_com(0xc0+13);write_date(0xdf);  write_date('C');	
}
/***************************************************************************
显示时间  在计算时间（年 、月、日等等）的十位和个位时需要注意
这需要将8421码表示的两位十进制数的十位和个位分别求出来 十位=时间（比方说天）/16;个位=时间%16
*****************************************************************************/
void displaytime() 
{
	write_com(0x81); write_date(table[2]);write_date(table[0]);
	write_date(table[year/16]);	write_date(table[year%16]);//显示出年
	
	write_com(0x86);write_date(table[month/16]);write_date(table[month%16]);//显示出月
	
	write_com(0x89);write_date(table[day/16]); //显示出日
	write_date(table[day%16]);
	
	write_com(0x80+12);		   //显示出星期
	switch(week) {
	  case 1:write_date('M'); write_date('O');  write_date('N'); break;
	  case 2:write_date('T'); write_date('U');  write_date('E'); break;
	  case 3:write_date('W'); write_date('E');  write_date('D'); break;
	  case 4:write_date('T'); write_date('H'); 	write_date('U'); break;
	  case 5:write_date('F'); write_date('R');  write_date('T'); break;
	  case 6:write_date('S'); write_date('A');  write_date('T'); break;
	  case 7:write_date('S'); write_date('U');  write_date('N'); break;
	}
	write_com(0xc0);write_date(table[hour/16]);	write_date(table[hour%16]);//显示出小时
	write_com(0xc3);write_date(table[minute/16]); write_date(table[minute%16]);//显示出分钟
	write_com(0xc6);write_date(table[second/16]);write_date(table[second%16]);//显示出秒
}  					
/****************************************************************************
以下是有关键盘调时内容的程序
******************************************************************************/
/**************************************************************
函数功能：延时20ms的子程序
**************************************************************/
void delay20ms(){  unsigned char i,j;
  for(i=0;i<100;i++)   for(j=0;j<60;j++)  ;}

void delay1(uchar z){ uchar x,y;
	for(x=1000;x>1;x--)	for(y=z;y>1;y--);  } 
/**************************************************************
函数功能：扫描键盘调时程序
**************************************************************/	 
void key()
{	uchar timenum;		//调整时的标志符。当num=0时,正常工作状态。当num！=0时，就是调时状态。
	if(mode==0) 					//当按下模式选择键就开始进入调时模式，直到num=0为止
	{delay20ms();  if(mode==0) { timenum++;}
		while(!mode==1);	}	  
	if(timenum) //当num不为零时，就处于调时状态，就让1302停止计时。调时完成后，就重新打开计时					       
	{write_byte(0x8e,0); 		//调时过程中允许写入
	second=read_byte(0x81);		//先求出秒的数值
	write_byte(0x80,second|0x80);//再让1302停止         
	while(timenum) 					  //进入循环的调时状态，直到num=0，跳出调时模块
	{switch(timenum)
		 { case 1:write_com(0x84);   write_com(0x0f); //年
		  	     delaynms(200);		           //让年的个位闪烁需要足够的时间
				 year1=(year/16)*10+year%16;  //把当前从1302中读出的8424码表示的数，转化成十进制计算	   
		         if(plus==0)				  //扫描按键
					{ delay20ms();			  //延迟后再检测去除抖动
					  if(plus==0){	year1++;   //确认按下后就加1
					    if(year1==100) year1=0;	write_byte(0x8c,((year1/10)*16+year1%10));}//将更改后的十进制年转换成8421码存入1302的年寄存器中
					 } 	while(!plus)  ;								 //等待按键释放
					if(minus==0){delay20ms();
						if(minus==0){ 	year1--;   	if(year1==-1) year1=99;
						write_byte(0x8c,((year1/10)*16+year1%10));	}
					}while(!minus) ;
					year=readyear_byte(0x8d); 				   //将年读出来
					write_com(0x83);						   //将年显示出来，以使调整后能看到变化
					write_date(table[year/16]);   write_date(table[year%16]);   break;
			 case 2: write_com(0x87);	 			  //选择光标闪烁的位置，月的个位
			         delaynms(200);					  //让月的个位闪烁需要足够的时间
					 month1=(month/16)*10+month%16;	 //把当前从1302中读出的8424码表示的数，转化成十进制计算 
					 if(plus==0) { delay20ms();
					  if(plus==0) {month1++; if(month1==13) month1=1;write_byte(0x88,((month1/10)*16+month1%10));}				
					  }	while(!plus) ;
					if(minus==0){  delay20ms();
						if(minus==0){  month1--; if(month1==0) month1=12;
						write_byte(0x88,((month1/10)*16+month1%10)); }					
					}  while(!minus) ;
				 	month=read_byte(0x89);
	 				write_com(0x86); write_date(table[month/16]);
	                write_date(table[month%16]);//显示出月
					break;
		 	 case 3:write_com(0x80+10);	 
					delaynms(200);
					day1=(day/16)*10+day%16;		 //把当前从1302中读出的8424码表示的数，转化成十进制计算 
				   	if(plus==0){ delay20ms();
					  if(plus==0){			day1++;
							switch(month1)//对于日的调整比较麻烦1,3,5,7,8,10,12月有31天；而4,6,9,11有30天；闰年的2月29天，平年则有28天
						   {case 1:case 3:case 5:case 7:case 8:case 10:case 12: if(day1==32)   day1=1;break;  
						    case 4:case 6:case 9:case 11: if(day1==31) day1=1;break;
							case 2:if(year1%4==0) {if(day1==30)  day1=1;}
							       else {if(day1=29)  day1=1;} break;
						    }  
	                   		write_byte(0x86,((day1/10)*16+day1%10));
	                    }		             
			        }while(!plus)  ;
					if(minus==0){ delay20ms();
						if(minus==0){ day1--;
					    switch(month1)
					   {case 1:case 3:case 5:case 7:case 8:case 10:case 12: if(day1==0)   day1=31;break;  
					    case 4:case 6:case 9:case 11: if(day1==0) day1=30;break;
						case 2:if(year1%4==0) {if(day1==0)  day1=29;}
						         else {if(day1=0)  day1=28;} break;
					    }    
						write_byte(0x86,((day1/10)*16+day1%10));
	                  }		             
			        }while(!minus) ;
				 	day=read_byte(0x87);write_com(0x89);
	                write_date(table[day/16]);   write_date(table[day%16]);
					break; 
	         case 4:write_com(0x80+14);  //星期
					delaynms(200);
					week1=(week/16)*10+week%16;		 //把当前从1302中读出的8424码表示的数，转化成十进制计算
			     	 if(plus==0){ delay20ms();
					  if(plus==0) {	week1++;
						if(week1==8) week1=1;
						write_byte(0x8a,((week1/10)*16+week1%10));	}
					}	while(!plus)  ;
					if(minus==0){ delay20ms();
						if(minus==0) { week1--;
						if(week1==0) week1=7;
						write_byte(0x8a,((week1/10)*16+week1%10)); }
					}while(!minus) ;
				 	week=read_byte(0x8b);//显示出星期
					write_com(0x80+12);		   
					switch(week)
		 			{
		  				case 1:	write_date('M');write_date('O'); write_date('N');break;
	  					case 2:write_date('T');write_date('U');write_date('E'); break;
	  					case 3:write_date('W');write_date('E');write_date('D'); break;
	  					case 4:write_date('T');write_date('H'); write_date('U');break;
	  					case 5:write_date('F');write_date('R');write_date('T');break;
	  					case 6:write_date('S');write_date('A'); write_date('T');break;
	  					case 7:write_date('S');write_date('U');write_date('N');break;
					}	break;						 
	 	case 5:     write_com(0xc1);  //时
					delaynms(200); 
					hour1=(hour/16)*10+hour%16;     //把当前从1302中读出的8424码表示的数，转化成十进制计算
			     	if(plus==0){ delay20ms();
					  if(plus==0){ hour1++;	if(hour1==24) hour1=0;
						write_byte(0x84,((hour1/10)*16+hour1%10));	}
					}while(!plus)  ;
					if(minus==0)	{delay20ms();
						if(minus==0){ hour1--;	if(hour1==-1) hour1=23;
						write_byte(0x84,((hour1/10)*16+hour1%10));	}
					}	while(!minus) ;
				 	hour=read_byte(0x85);write_com(0xc0);
					write_date(table[hour/16]);	write_date(table[hour%16]);//显示出小时
	
					break;						   
			 case 6: write_com(0xc4);	 //分
					delaynms(200);
					minute1=(minute/16)*10+minute%16;	 //把当前从1302中读出的8424码表示的数，转化成十进制计算 
					if(plus==0){ delay20ms();
					  if(plus==0){minute1++;	if(minute1==60) minute1=0;
						write_byte(0x82,((minute1/10)*16+minute1%10));		}					
					}while(!plus)  ;
					if(minus==0){delay20ms();
						if(minus==0){ 	minute1--; 	if(minute1==-1) minute1=59;	}
						write_byte(0x82,((minute1/10)*16+minute1%10));
					}   while(!minus) ;
				 	minute=read_byte(0x83);	write_com(0xc3);
					write_date(table[minute/16]);write_date(table[minute%16]);//显示出分钟 
					break;
			  case 7: write_com(0xc7);	 //秒
					 delaynms(200);
					 second1=(second/16)*10+second%16;	 //把当前从1302中读出的8424码表示的数，转化成十进制计算 
				   	if(plus==0)	{ delay20ms(); 
					  if(plus==0){ second1++;if(second1==60) second1=0;
						write_byte(0x80,((second1/10)*16+second1%10)|0x80);//将更改后的十进制秒转换成8421码存入1302的年寄存器中
					     //由于read_byte()函数的最高位没读（为0）。虽然秒的最高位在停止时已经置成1，但是由于最高位的丢失，所以按前边的写进去必定又将计时启动了。
						 //这里在char型变量秒的最高位添了1以保证调秒时也停止计时
						}           
	                 }	while(!plus) ;
					if(minus==0){delay20ms();
						if(minus==0){second1--;	if(second1==-1) second1=59;
						write_byte(0x80,((second1/10)*16+second1%10)|0x80);	}
					} while(!minus) ;
				 	second=read_byte(0x81);	write_com(0xc6);
					write_date(table[second/16]);write_date(table[second%16]);//显示出秒  
					break;				 
			}
	  		if(mode==0) 					 //当按下模式选择键选择要调整的量，当num=0时跳出调时状态
		   	{	delay20ms();   
		    	if(mode==0)  	{ timenum++;	 	}
				while(!mode==1);
			}	  
	      	if(timenum==8){ timenum=0; 	} //当调整完时，num=0,就可以脱离调时状态
	   				
	  }  
	  second=read_byte(0x81);		//先求出秒的数值
	  write_byte(0x80,second&0x7f);	//再让1302启动
	  write_byte(0x8e,0x80);		//禁止写入
	  write_com(0x0c); 			    //光标在调整年（num=1）时开始闪烁,在调整其他位（num=2,3,4,5,6,7）时也在闪烁，。调整完（num=0）时,停止闪烁
	  }																				 
}

/*************DS18B20温度读取模块*************/
void tmpDelay(int num)//延时函数
{
	while(num--) ;
}

void Init_DS18B20()//初始化ds1820
{
	unsigned char x=0;
	DS = 1;    //DS复位
	tmpDelay(8);  //稍做延时
	DS = 0;    //单片机将DS拉低
	tmpDelay(80); //精确延时 大于 480us
	DS = 1;    //拉高总线
	tmpDelay(14);
	x=DS;      //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	tmpDelay(20);
}

unsigned char ReadOneChar()//读一个字节
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{
		DS = 0; // 给脉冲信号
		dat>>=1;
		DS = 1; // 给脉冲信号
		if(DS) 	dat|=0x80;
		tmpDelay(4);
	}
	return(dat);
}

void WriteOneChar(unsigned char dat)//写一个字节
{	char i;
	for (i=8; i>0; i--)	{
		DS = 0;	DS = dat&0x01;	tmpDelay(5);
		DS = 1;	dat>>=1;  }
}

unsigned int Readtemp()//读取温度
{
	unsigned char a=0;
	unsigned char b=0;
	unsigned int t=0;
	float tt=0;	
	Init_DS18B20();
	WriteOneChar(0xCC); // 跳过读序号列号的操作
	WriteOneChar(0x44); // 启动温度转换
	Init_DS18B20();
	WriteOneChar(0xCC); //跳过读序号列号的操作
	WriteOneChar(0xBE); //读取温度寄存器
	a=ReadOneChar();   //连续读两个字节数据   //读低8位 
	b=ReadOneChar();                          //读高8位
	t=b;
	t<<=8;
	t=t|a; 			//两字节合成一个整型变量。
	tt=t*0.0625;  	//得到真实十进制温度值，因为DS18B20可以精确到0.0625度，所以读回数据的最低位代表的是0.0625度
	t= tt*10+0.5; 	//放大十倍，这样做的目的将小数点后第一位也转换为可显示数字，同时进行一个四舍五入操作。
	return(t);
}

void display()
{
	uint num;                   //定义的时候用uchar宏定义就会出错
	uint shi,ge,xiaoshu;	    //这里的num,shi,ge,xiaoshu 必须用uint无符号整数来表示，用uchar字符型则显示错误
	num=Readtemp();
	shi=num/100; 	ge=num/10%10; 	xiaoshu=num%10;	  //
	write_com(0x80+0x40+0xa);	write_date(wendu[shi]);
	write_com(0x80+0x40+0xb);	write_date(wendu[ge]);
	write_com(0x80+0x40+0xc); 	write_date(0x2e);
	write_com(0x80+0x40+0xd);	write_date(wendu[xiaoshu]);	
	write_com(0x80+0x40+0xe); 	write_date(0xdf);
	write_com(0x80+0x40+0xf); 	write_date(0x43); 
}		 	
 
 
 