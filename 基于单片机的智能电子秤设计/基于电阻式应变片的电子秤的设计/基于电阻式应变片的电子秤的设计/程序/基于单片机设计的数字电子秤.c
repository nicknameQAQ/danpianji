#include<reg52.h>
typedef unsigned char uint8;
typedef unsigned int uint16;
sbit rs=P2^6;
sbit rw=P2^5;
sbit e=P2^7;
sbit scl=P2^1;
sbit sda=P2^0;
uint8 num[]="0123456789";
uint8 d1[]="RMB/kg: ";
uint16 ad[5],m[5];
uint8 kn=10;
uint8 keynum[3];
uint16 pric=0;
void delay(uint16 i)
{
	while(i--);
}
void wrc(uint8 c)
{
	delay(1000);
	rs=0;
	rw=0;
	e=0;
	P0=c;
	e=1;
	delay(10);
	e=0;
}
void wrd(uint8 dat)
{
	delay(1000);
	rs=1;
	rw=0;
	e=0;
	P0=dat;
	e=1;
	delay(10);
	e=0;
	rs=0;
}
void init()
{
	delay(1000);
	wrc(0x38);
	wrc(0x38);
	wrc(0x38);
	wrc(0x06);
	wrc(0x0c);
	wrc(0x01);
}
void iicinit()
{
	sda=1;
	scl=1;	
}
void iicstart()
{
	sda=1;
	scl=1;
	delay(10);
	sda=0;
	delay(10);
	scl=0;
	delay(10);
}
void iicstop()
{
	sda=0;
	scl=1;
	delay(10);
	sda=1;
	delay(10);
	sda=0;
	delay(10);
}
void ack()
{
	sda=1;
	scl=0;
	delay(2);
	sda=0;
	delay(2);
	scl=1;
	delay(10);
	scl=0;
	delay(2);
	sda=1;
	delay(5);
}
void noack()
{
	sda=0;
	scl=0;
	delay(2);
	sda=1;
	delay(2);	
	scl=1;
	delay(10);
	scl=0;
	delay(2);
	sda=0;
	delay(5);
}

void iicwrbyte(uint8 dat)
{
	uint8 i;
	scl=0;
	for(i=0;i<8;i++)				
	{
		if(dat&0x80)	
		{
			sda=1;
		}
		else
		{
			sda=0;
		}
		dat<<=1;
		scl=1;
		delay(10);
		scl=0;
		delay(10);
	}
	sda=1;
	delay(10);
}

uint8 iicread()
{
	uint8 i,dat;
	scl=0;
	sda=1;
	for(i=0;i<8;i++)
	{
		dat<<=1;
		if(sda==1)		
		{
			dat|=0x01;	
		}
		scl=1;
		delay(10);
		scl=0;
		delay(10);
	}
	return 	dat;
}

void pcf8591wrbyte(uint8 channel)
{
	iicinit();
	iicstart();
	iicwrbyte(0x90);	   //pcf8591写数据  1001 A2A1A0 R/W
	ack();
	iicwrbyte(channel|0x70);   //通过打开通道channelX，模拟输出使能打开
	noack();
	iicstop();
}
uint8 pcf8591read()
{
	uint8 dat;
	iicinit();
	iicstart();
	iicwrbyte(0x90);
	ack();
	iicstart();
	iicwrbyte(0x91);	 //	pcf8591读数据  1001 A2A1A0 R/W
	ack();
	dat=iicread();
	noack();
	iicstop();
	return dat;	
}

void pcf8591dac(uint8 value)	   //DA转换
{
	iicinit();
	iicstart();
	iicwrbyte(0x90);
	ack();
	iicwrbyte(0x70);		//不需要打开通道channelX，模拟输出使能打开
	ack();
	iicwrbyte(value);
	noack();
	iicstop();	
}
uint8 keyscan()		 //按键扫描程序
{
	uint8 h,l,value;
	P1=0x0f;
	h=P1&0x0f;
	if(h!=0x0f)
	{
		delay(1);
		if(h!=0x0f)
		{
			h=P1&0x0f;
			l=P1|0xf0;
			P1=l;
			l=P1&0xf0;
			h=P1&0x0f;
			value=h+l;	
		}
		return value;
	}
}
void keypros()		  //按键处理函数
{
	uint8 key;
	key=keyscan();
	switch(key)
	{
		case 0xee: kn=0;break;
		case 0xde: kn=1;break;
		case 0xbe: kn=2;break;
		case 0x7e: kn=3;break;
		case 0xed: kn=4;break;
		case 0xdd: kn=5;break;
		case 0xbd: kn=6;break;
		case 0x7d: kn=7;break;
		case 0xeb: kn=8;break;
		case 0xdb: kn=9;break;
		case 0xbb: break;   
		case 0x7b: break;
		case 0xe7: kn=12;break; //小数点按下标志位
		case 0xd7: kn=11;break; //计价确认按键标志 
		case 0xb7: kn=13;break; //clear,清空计价输入
		case 0x77: break;
		default :  break;
	}	
}
void datapros()
{
	uint8 i,count=0;
	do					   //将按键值保存在数组keynum中
	{
		P1=0x0f;
		if(P1!=0x0f)		
		{
			keypros();
			P1=0x0f;
			while(P1!=0x0f);//等待按键松开
			if(kn>=0&&kn<=9)
			{
				keynum[count]=kn;
				wrc(0x0b+count+0x80);  //移位显示
				wrd(keynum[count]+0x30);
				count++;
			}
			if(kn==13)	  //清除按键计价数
			{
				wrc(0x01);//清屏处理	
				pric=0;
			}
			if(kn==11)	 //当计价确认按键按下时候退出循环
			{
				goto xian;
			}	
		}			
	}
	while(count<=2);
	if(count>=2)
	{
		goto xian;
	}
	xian:	for(i=0;i<count;i++)
	{
		pric=pric*10+keynum[i];	
		if(i==count)count=0;//当进行一次转换后清空计数器的值	
	}
}
void display()
{
	float v;
	uint8 i;
	uint16 price;
	pcf8591wrbyte(0);	  //电位器转换
	ad[0]=pcf8591read();	//读取第一次之前的那次数据
	delay(10);
	v=ad[0]*0.01953;//将电位器的数据转换成电压值
	v=v*100;//取2位有效数值
	ad[0]=v;
	price=ad[0]*pric;	 //每千克多少钱如果我们定义每千克2元那么就可以乘以2，如果为其他我们可以乘以其他数,最大定义的价格是200元每千克
	wrc(0x00+0x80);
	for(i=0;i<8;i++)
	{
		wrd(d1[i]);
	}
	wrc(0x40+0x80);
	wrd('m');
	wrd(':');
	wrd(num[ad[0]/100]);
	wrd('.');
	wrd(num[ad[0]%100/10]);
	wrd(num[ad[0]%100%10]);
//	wrd(num[ad[0]%1000%100%10]);
	wrd('K');
//	wrd('G');
	ad[4]=ad[0];
	pcf8591dac(ad[4]/2);//将数字量转换为模拟量由pcf8591的AOUT输出

	wrd(' ');
	wrd('P');
	wrd(':');
	wrd(num[price/10000]);//为什么是除以10000，本来不计价的话是除以100，但是计价最大可以使百位数，所以除以的数也得乘以100
	
	wrd(num[price%10000/1000]);
	
	wrd(num[price%10000%1000/100]);
	wrd('.');
	if((price%10000%1000%100%10)>=5)  //进价，四舍五入法
		wrd(num[(price%10000%1000%100/10)+1]);
	else	
		wrd(num[price%10000%1000%100/10]);	
	wrd('$');
}

void main()
{
	init();
	while(1)
	{
		display();
		datapros();	
	}		
}