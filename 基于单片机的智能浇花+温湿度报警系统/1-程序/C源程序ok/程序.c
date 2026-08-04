#include <reg52.h>
#include <intrins.h>
#define uchar unsigned char		//宏定义
#define uint unsigned int
#include"eeprom.h"
typedef unsigned char unint8;
typedef unsigned char unint16;
typedef unsigned int   U16; 
uchar code table[] = {"moisture:       "};
uchar code table1[] ={"                "};
uchar code table3[] ={" H:  %   H:   C "};
uchar code table2[] ={"Set The:         "};
sbit TRH = P1^4;//温湿度传感器DHT11数据接入
sbit rs = P2^6;             
sbit en = P2^7;
sbit beep=P2^0;//蜂鸣器控制端

sbit k1=P3^1;
sbit k2=P3^2;
sbit k3=P3^3;

sbit jdq=P3^4;

sbit CS   = P1^0; 
sbit Clk = P1^1; 
sbit DATI = P1^2; 
sbit DATO = P1^2;
uchar stemp=35,shr=50,tr=80;	   //温湿度上下线初始化
bit yyp=1,lalarm=0;  						//设置标志位，报警标志位
int key_delay=0;
const int key_count=400; 
unsigned char dat = 0x00;       
unsigned char CH;   
uchar s1num,num,shis,shig,wens,weng;
unint8 TH_data,TL_data,RH_data,RL_data,CK_data;
unint8 TH_temp,TL_temp,RH_temp,RL_temp,CK_temp;
unint8 com_data,untemp,temp;
unint8 respond;
void delay(uint x) //毫秒级延时函数
{
  uint i,j;
  for(i=x;i>0;i--)
     for(j=110;j>0;j--);
}
void di()	  		 //蜂鸣器报警
{
	beep=0;
	delay(100);
	beep=1;
}
void write_com(uchar com)
{
	rs=0;//命令
	P0=com;
	delay(5);
	en=1;
	delay(5);
	en=0;
}
void write_date(uchar date)
{
	rs=1;//数据
	P0=date;
	delay(5);
	en=1;
	delay(5);
	en=0;
}
void init1602()
{
	write_com(0x38);	//设置工作方式
	write_com(0x0c);	//设置光标
	write_com(0x06);	//设置输入方式
	write_com(0x01);	//清屏
	write_com(0x80);
	for(num=0;num<16;num++)
	{
		write_date(table[num]);
	}
	write_com(0x80+0x40);
	for(num=0;num<16;num++)
	{
		write_date(table1[num]);
	}
}
void init16021()		//设置温度上下限显示初始化函数
{
	write_com(0x80);
	for(num=0;num<16;num++)
	{
		write_date(table2[num]);
	}
	write_com(0x80+0x40);
	for(num=0;num<16;num++)
	{
		write_date(table3[num]);
	}
	write_com(0x80+0x40+13);
	write_date(0xdf);		 
	
}
void write_H(uchar add,uchar date)
{
	uchar shi,ge;
	shi=date/10;
	ge=date%10;
	write_com(0x80+0x40+add);
	write_date(0x30+shi);
	write_date(0x30+ge);
}
void write_tr(uchar add,uchar Data)
{
	uchar bai,shi,ge;
	bai=Data/100;
	shi=Data/10%10;
	ge=Data%10;
	write_com(0x80+add);
	write_date(0X30+bai);
	write_date(0X30+shi);
	write_date(0X30+ge);
}
/////////////////////////////////
/***********DHT11子程序*************************/
/*********************************************************************************/

/*********************************************************************************/
// 毫秒级延时子程序
/*********************************************************************************/
void delay_ms(U16 j)
{      unint8 i;
	    for(;j>0;j--)
	  { 	
		for(i=0;i<27;i++);

	  }
}
/*********************************************************************************/

/*********************************************************************************/
//5us级延时程序
/*********************************************************************************/
void delay_us()
{
          unint8 i;
          i--;
          i--;
          i--;
          i--;
          i--;
          i--;
} 
/********************************************************************************/

/****************************************************************************/
//收发信号检测，数据读取
/****************************************************************************/
char receive()
{    
          unint8 i;
          for(i=0;i<8;i++)    
         {
                    respond=2;
                    while((!TRH)&&respond++);
                    delay_us();
                    delay_us();
                    delay_us();

					temp=0;


                    if(TRH)temp=1;
                     respond=2;
                     while((TRH)&&respond++); 
					 //超时则跳出for循环		  
					 if(respond==1)break;

					 	//判断数据位是0还是1	 
	   	   
		// 如果高电平高过预定0高电平值则数据位为 1 
                    com_data<<=1;
                    com_data|=temp;    
          }
          return(com_data);  
}
/****************************************************************************/

/****************************************************************************/
//湿度读取子程序 
//温度高8位== TL_data
//温度低8位== TH_data
//湿度高8位== RH_data
//湿度低8位== RH_data
//校验 8位 == CK_data
//调用的程序有 delay();, Delay_5us();,RECEIVE(); 
/***************************************************************************/
void GET_TRH()
{
         //主机拉低18ms 
         TRH=0;
         delay_ms(180);
         TRH=1;
         //DATA总线由上拉电阻拉高 主机延时20us
         delay_us();
         delay_us();
         delay_us();
         delay_us();
         //主机设为输入 判断从机响应信号 
         TRH=1;
         //判断DHT11是否有低电平响应信号 如不响应则跳出，响应则向下运行   
         if(!TRH)   
         {
                respond=2;
                //判断DHT11发出 80us 的低电平响应信号是否结束 
                while((!TRH)&& respond++);
                respond=2;
                //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
                while(TRH && respond++);
               	//数据接收状态   
                RH_temp = receive(); 
                RL_temp = receive(); 
                TH_temp = receive(); 
                TL_temp = receive(); 
                CK_temp = receive();
                TRH=1;//ST=1;     
                //数据校验 
                untemp=(RH_temp+RL_temp+TH_temp+TL_temp);
                if(untemp==CK_temp)
                {
                     RH_data = RH_temp;
                     RL_data = RL_temp;
                     TH_data = TH_temp; 
                     TL_data = TL_temp;
                     CK_data = CK_temp;
                }
        }
				//湿度整数部分
		         shis= (char)(0X30+RH_data/10);  //湿度的十位
		         shig= (char)(0X30+RH_data%10);	 //湿度的个
		         //温度整数部分
		         wens= (char)(0X30+TH_data/10);   //温度的十位
		         weng= (char)(0X30+TH_data%10);	 //温度的个位
		       	 //湿度整数部分
		         shis= (char)(0X30+RH_data/10);  //湿度的十位
		         shig= (char)(0X30+RH_data%10);	 //湿度的个
		         //温度整数部分
		         wens= (char)(0X30+TH_data/10);   //温度的十位
		         weng= (char)(0X30+TH_data%10);	 //温度的个位
		       	 //温湿度显示函数
				 write_com(0x80+0x40+2);
				 write_date(shis);
				 write_date(shig);
				 write_date('%');	 //湿度符号
				 write_com(0x80+0x40+9);
				 write_date(wens);
				 write_date(weng);
				 write_date(0xdf);	//温度符号
				 write_date('C');


}
/****按键扫描******/
void keyscan()
{
	if(k1==0)
	{
		delay(10);
		if(k1==0)
		{
			s1num++;
			while(!k1);			//等待按键释放
			yyp=0;
			di();			
			switch(s1num)
			{
				case 1:	init16021();  				//设置温度上下限初始化
						tr=byte_read(0x2000); 
		  				shr=byte_read(0x2200);
		  				stemp=byte_read(0x2400);
						write_tr(9,tr);
						write_H(3,shr);
						write_H(11,stemp);
						write_com(0x80+11);
						write_com(0x0f);			//打开光标
						break;
				case 2:	write_com(0X40+0x80+4);
						break;
				case 3:	write_com(0X40+0x80+12);
						break;
				case 4:	s1num=0;
						write_com(0x0c);			//关闭光标
						yyp=1;
						init1602();					//上电后显示初始化
						break;
			}		
		}
	}
	if(s1num!=0)
	{
		if(k2==0)
		{
			delay(10);
			if(k2==0)
			{
				
				while(!k2);		//等待按键释放
				di();
				switch(s1num)
        		{
					case 1: tr++;
							if(tr==255)tr=0;
							write_tr(9,tr);
							write_com(0x80+11);
							SectorErase(0x2000);//擦除扇区
   							byte_write(0x2000,tr);//重新写入数据	
							break;
					case 2: shr++;
							if(shr==85)shr=0;
							write_H(3,shr);
							write_com(0X40+0x80+4);	
							SectorErase(0x2200);//擦除扇区
   							byte_write(0x2200,shr);//重新写入数据	
							break;
					case 3: stemp++;
							if(stemp==45)stemp=0;
							write_H(11,stemp);
							write_com(0X40+0x80+12); 
							SectorErase(0x2400);//擦除扇区
   							byte_write(0x2400,stemp);//重新写入数据
				    		break;
				} 
			}
		}
		if(k3==0)
		{
			delay(10);
			if(k3==0)
			{
			
				while(!k3);		//等待按键释放
				di();
				switch(s1num)
        		{
					case 1: tr--;
							if(tr==0)tr=255;
							write_tr(9,tr);
							write_com(0x80+11);
							SectorErase(0x2000);//擦除扇区
   							byte_write(0x2000,tr);//重新写入数据		
							break;
					case 2: shr--;
							if(shr==0)shr=85;
							write_H(3,shr);
							write_com(0X40+0x80+4);	
							SectorErase(0x2200);//擦除扇区
   							byte_write(0x2200,shr);//重新写入数据
							break;
					case 3: stemp--;
							if(stemp==0)stemp=45;
							write_H(11,stemp);
							write_com(0X40+0x80+12); 
							SectorErase(0x2400);//擦除扇区
   							byte_write(0x2400,stemp);//重新写入数据
				    		break;
				} 
			}
		}
	}
}

unsigned char adc0832(unsigned char CH) 
{        
	unsigned char i,test,adval;      
	adval = 0x00;      
	test = 0x00;       
	Clk = 0;       //初始化      
	DATI = 1;     
	_nop_();     
	CS = 0;     
	_nop_();     
	Clk = 1;    
	_nop_();       
	if ( CH == 0x00 )      //通道选择    
	{         
		Clk = 0;         
		DATI = 1;      //通道0的第一位       
		 _nop_();       
		 Clk = 1;         
		 _nop_();          
		 Clk = 0;        
		 DATI = 0;      //通道0的第二位       
		 _nop_();  
		 Clk = 1;       
		 _nop_();     
	}      
	else     
	{         
		Clk = 0;        
		DATI = 1;      //通道1的第一位        
		_nop_();        
		Clk = 1;        
		_nop_();        
		Clk = 0;        
		DATI = 1;      //通道1的第二位     
		_nop_();      
		Clk = 1;      
		_nop_();     
	}        
		Clk = 0;       
		DATI = 1;        
		for( i = 0;i < 8;i++ )      //读取前8位的值     
		{         
			_nop_();         
			adval <<= 1;        
			Clk = 1;        
			_nop_();        
			Clk = 0;        
			if (DATO)            
			adval |= 0x01;       
			else            
			adval |= 0x00;     
		}          
		for (i = 0; i < 8; i++)      //读取后8位的值       
		{             
			test >>= 1;            
			if (DATO)                
			test |= 0x80;            
			else                 
			test |= 0x00;            
			_nop_();           
			Clk = 1;           
			_nop_();           
			Clk = 0;       
		}        
		if (adval == test)      //比较前8位与后8位的值，如果不相同舍去。若一直出现显示为零，请将该行去掉             
		dat = test;        
		_nop_();         
		CS = 1;        //释放ADC0832        
		DATO = 1;        
		Clk = 1;       
		return dat; 
} 
void display(uchar Data)
{
	uchar bai,shi,ge;
	bai=Data/100;
	shi=Data/10%10;
	ge=Data%10;
	write_com(0x80+9);
	write_date(0X30+bai);
	write_date(0X30+shi);
	write_date(0X30+ge);
}
void baojing()
{
			if(TH_data>stemp||RH_data>shr)//判断温湿度
			{
				lalarm=1;
				if(RH_data>shr) //	报警
				{
						
				}
				 if(TH_data>stemp) //	报警
				{
					
				}
			}
			else							 //不报警
			{
				lalarm=0;
			  	if(yyp==1)
				{
				} 
			}
			if(lalarm==1)
			{
				di();
				delay(100);
				di();
				delay(100);
			} 
			if(100-(adc0832(0x00)/2.55)<tr)//浇花
			{
				jdq=0;beep=0;
			}
			else//停止浇花
			{
				jdq=1;beep=1;
			}		
} 
/****************************************************************************/
//主函数
//TH,TL,RH,RL分别代表温湿度的整数和小数部分
/****************************************************************************/
void main()
{
		  init1602();
		  
		  tr=byte_read(0x2000); 
		  shr=byte_read(0x2200);
		  stemp=byte_read(0x2400);
          while(1)
          {


		

		 	keyscan();
			if(yyp==1)
			{
				display(100-adc0832(0X00)/2.55);
             	keyscan();
				if(yyp==1)GET_TRH();
				baojing();	
				
			 }      		 	
          }      
} 
