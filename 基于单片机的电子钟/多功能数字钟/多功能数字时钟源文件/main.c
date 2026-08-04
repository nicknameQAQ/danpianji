/*************多功能数字时钟源程序**********/
/*设计者：万亚雄 谭晓婉 朱梅英
设计时间：2012.11.23
班级：1012341（10电工班）*/
/*************多功能数字时钟源程序**********/
#include<reg52.h>
#include"ds12cr887.h"
#include"1602.h"
#define uint unsigned int
#define uchar unsigned char

sbit key1=P3^0;
sbit key2=P3^1;
sbit key3=P3^2;
sbit key4=P3^7;
sbit speaker=P2^3;

uchar code t3[]={" alarm setting: "};
uchar code t4[]={"Now is the time!"};
uchar code t5[]={"Get up quickly! "};
uchar key1num,key4num,flag;

char shi,fen,miao,nian,yue,ri,day,amiao,afen,ashi;

void writetime(uchar add,uchar time)//将时间用1602液晶显示的函数
{
 uchar si,ge;
 si=time/10;
 ge=time%10;
 writelcd_cmd(0xc0+add);
 writelcd_dat(0x30+si);
 writelcd_dat(0x30+ge);
}

void writedate(uchar add,uchar date)//将日期用1602液晶显示的函数
{
 uchar si,ge;
 si=date/10;
 ge=date%10;
 writelcd_cmd(0x80+add);
 writelcd_dat(0x30+si);
 writelcd_dat(0x30+ge);
}

void writeday(uchar day)//将星期用1602液晶显示的函数
{
  switch(day)
  {
  case 1:writelcd_cmd(0x80+12);writelcd_dat('M');writelcd_dat('O');writelcd_dat('N');break;
    case 2:writelcd_cmd(0x80+12);writelcd_dat('T');writelcd_dat('U');writelcd_dat('E');break;
	  case 3:writelcd_cmd(0x80+12);writelcd_dat('W');writelcd_dat('E');writelcd_dat('N');break;
	    case 4:writelcd_cmd(0x80+12);writelcd_dat('T');writelcd_dat('H');writelcd_dat('U');break;
		  case 5:writelcd_cmd(0x80+12);writelcd_dat('F');writelcd_dat('R');writelcd_dat('I');break;
		    case 6:writelcd_cmd(0x80+12);writelcd_dat('S');writelcd_dat('A');writelcd_dat('T');break;
			  case 7:writelcd_cmd(0x80+12);writelcd_dat('S');writelcd_dat('U');writelcd_dat('N');break;
  }
}

void settime()	//系统时间设定函数
{
 if(key4num==0)
  {	 if(key1==0)
	 {
		 delay(10);
		 if(key1==0)
		 {
			  while(key1==0);
			 
			  key1num++;
			  switch(key1num)
			  {
			  case 1:writelcd_cmd(0xc0+11);writelcd_cmd(0x0f);break;
			  case 2:writelcd_cmd(0xc0+10);writelcd_cmd(0x0f);break;
			  case 3:writelcd_cmd(0xc0+8);writelcd_cmd(0x0f);break;
			  case 4:writelcd_cmd(0xc0+7);writelcd_cmd(0x0f);break;
			  case 5:writelcd_cmd(0xc0+5);writelcd_cmd(0x0f);break;
			  case 6:writelcd_cmd(0xc0+4);writelcd_cmd(0x0f);break;
			  case 7:writelcd_cmd(0x80+14);writelcd_cmd(0x0f);break;
			  case 8:writelcd_cmd(0x80+10);writelcd_cmd(0x0f);break;
			  case 9:writelcd_cmd(0x80+9);writelcd_cmd(0x0f);break;
			  case 10:writelcd_cmd(0x80+7);writelcd_cmd(0x0f);break;
			  case 11:writelcd_cmd(0x80+6);writelcd_cmd(0x0f);break;
			  case 12:writelcd_cmd(0x80+4);writelcd_cmd(0x0f);break;
			  case 13:writelcd_cmd(0x80+3);writelcd_cmd(0x0f);break;
			default:key1num=0;writelcd_cmd(0x0c);break;
			  }
		}
	}



	if(key1num!=0)
	{   
		if(key2==0)
		{
			delay(10);
			if(key2==0)
				{
				  while(key2==0);
				  switch(key1num)
					  {
					  case 1:miao=miao+1;if(miao>=60)miao=miao-60;writetime(10,miao);writelcd_cmd(0xc0+11);break;
					  case 2:miao=miao+10;if(miao>=60)miao=miao-60;writetime(10,miao);writelcd_cmd(0xc0+10);break;
					  case 3:fen=fen+1;if(fen>=60)fen=fen-60;writetime(7,fen);writelcd_cmd(0xc0+8);break;
					  case 4:fen=fen+10;if(fen>=60)fen=fen-60;writetime(7,fen);writelcd_cmd(0xc0+7);break;
					  case 5:shi=shi+1;if(shi>=24)shi=shi-24;writetime(4,shi);writelcd_cmd(0xc0+5);break;
					  case 6:shi=shi+10;if(shi>=24)shi=shi%10;writetime(4,shi);writelcd_cmd(0xc0+4);break;
					  case 7:day=day+1;if(day>7)day=day-7;writeday(day);writelcd_cmd(0x80+14);break;
					  case 8:ri=ri+1;if(ri>31)ri=ri-31;writedate(9,ri);writelcd_cmd(0x80+10);break;
					  case 9:ri=ri+10;if(ri>31)ri=ri%10;writedate(9,ri);writelcd_cmd(0x80+9);break;
					  case 10:yue=yue+1;if(yue>12)yue=yue-12;writedate(6,yue);writelcd_cmd(0x80+7);break;
					  case 11:yue=yue+10;if(yue>12)yue=yue%10;writedate(6,yue);writelcd_cmd(0x80+6);break;
					  case 12:nian=nian+1;writedate(3,nian);writelcd_cmd(0x80+4);break;
					  case 13:nian=nian+10;writedate(3,nian);writelcd_cmd(0x80+3);break;
					default:break;
				     }
			    }
	    }

	  if(key3==0)
	  {
	  delay(10);
	  if(key3==0)
	  {
	    while(key3==0);
				  switch(key1num)
					  {
					  case 1:miao=miao-1;if(miao<0)miao=miao+60;writetime(10,miao);writelcd_cmd(0xc0+11);break;
					  case 2:miao=miao-10;if(miao<0)miao=miao+60;writetime(10,miao);writelcd_cmd(0xc0+10);break;
					  case 3:fen=fen-1;if(fen<0)fen=fen+60;writetime(7,fen);writelcd_cmd(0xc0+8);break;
					  case 4:fen=fen-10;if(fen<0)fen=fen+60;writetime(7,fen);writelcd_cmd(0xc0+7);break;
					  case 5:shi=shi-1;if(shi<0)shi=shi+24;writetime(4,shi);writelcd_cmd(0xc0+5);break;
					  case 6:shi=shi-10;if(shi<0)shi=shi+30;if(shi>=24)shi=shi-10;writetime(4,shi);writelcd_cmd(0xc0+4);break;
					  case 7:day=day-1;if(day<0)day=day+7;writeday(day);writelcd_cmd(0x80+14);break;
					  case 8:ri=ri-1;if(ri<0)ri=31;writedate(9,ri);writelcd_cmd(0x80+10);break;
					  case 9:ri=ri-10;if(ri<0)ri=ri+30;writedate(9,ri);writelcd_cmd(0x80+9);break;
					  case 10:yue=yue-1;if(yue<0)yue=yue+12;writedate(6,yue);writelcd_cmd(0x80+7);break;
					  case 11:yue=yue-10;if(yue<0)yue=yue+10;writedate(6,yue);writelcd_cmd(0x80+6);break;
					  case 12:nian=nian-1;writedate(3,nian);writelcd_cmd(0x80+4);break;
					  case 13:nian=nian-10;writedate(3,nian);writelcd_cmd(0x80+3);break;
					default:break;
				     }
	  }
	  }
	writeds(0,miao);
	writeds(2,fen);
	writeds(4,shi);
	writeds(6,day);
	writeds(7,ri);
	writeds(8,yue);
	writeds(9,nian);
   }
   }
}

void alarmoff()//按任意键关闭闹铃函数
{

if(flag==1)
{
if(key1==0||key2==0||key3==0||key4==0)
{
delay(10);
if(key1==0||key2==0||key3==0||key4==0)
{
while(key1==0||key2==0||key3==0||key4==0)
flag=0;TR1=0;lcd_init();
}
}
}

}

void setalarm()//设定闹钟函数
{
uchar i;
if(key1num==0)
{
 if(key4==0)
{
delay(10);
if(key4==0)
{
while(key4==0);
writelcd_cmd(0x80);
for(i=0;i<16;i++)
{
writelcd_dat(t3[i]);
}
key4num++;
if(key4num!=0)
{
        amiao=readds(1);
		afen=readds(3);
		ashi=readds(5);
		writetime(10,amiao);
		writetime(7,afen);
		writetime(4,ashi);
}
 switch(key4num)
 {
              case 1:writelcd_cmd(0xc0+11);writelcd_cmd(0x0e);break;
			  case 2:writelcd_cmd(0xc0+10);writelcd_cmd(0x0e);break;
			  case 3:writelcd_cmd(0xc0+8);writelcd_cmd(0x0e);break;
			  case 4:writelcd_cmd(0xc0+7);writelcd_cmd(0x0e);break;
			  case 5:writelcd_cmd(0xc0+5);writelcd_cmd(0x0e);break;
			  case 6:writelcd_cmd(0xc0+4);writelcd_cmd(0x0e);break;
		default:key4num=0;lcd_init();break;
 }
}
}
if(key4num!=0)
	{   
		if(key2==0)
		{
			delay(10);
			if(key2==0)
				{
				  while(key2==0);
				  switch(key4num)
					  {
					  case 1:amiao=amiao+1;if(amiao>=60)amiao=amiao-60;writetime(10,amiao);writelcd_cmd(0xc0+11);break;
					  case 2:amiao=amiao+10;if(amiao>=60)amiao=amiao-60;writetime(10,amiao);writelcd_cmd(0xc0+10);break;
					  case 3:afen=afen+1;if(afen>=60)afen=afen-60;writetime(7,afen);writelcd_cmd(0xc0+8);break;
					  case 4:afen=afen+10;if(afen>=60)afen=afen-60;writetime(7,afen);writelcd_cmd(0xc0+7);break;
					  case 5:ashi=ashi+1;if(ashi>=24)ashi=ashi-24;writetime(4,ashi);writelcd_cmd(0xc0+5);break;
					  case 6:ashi=ashi+10;if(ashi>=24)ashi=ashi%10;writetime(4,ashi);writelcd_cmd(0xc0+4);break;
					default:break;
				     }
			    }
	    }

	  if(key3==0)
	  {
	  delay(10);
	  if(key3==0)
	  {
	    while(key3==0);
				  switch(key4num)
					  {
					  case 1:amiao=amiao-1;if(amiao<0)amiao=amiao+60;writetime(10,amiao);writelcd_cmd(0xc0+11);break;
					  case 2:amiao=amiao-10;if(amiao<0)amiao=amiao+60;writetime(10,amiao);writelcd_cmd(0xc0+10);break;
					  case 3:afen=afen-1;if(afen<0)afen=afen+60;writetime(7,afen);writelcd_cmd(0xc0+8);break;
					  case 4:afen=afen-10;if(afen<0)afen=fen+60;writetime(7,afen);writelcd_cmd(0xc0+7);break;
					  case 5:ashi=ashi-1;if(ashi<0)ashi=ashi+24;writetime(4,ashi);writelcd_cmd(0xc0+5);break;
					  case 6:ashi=ashi-10;if(ashi<0)ashi=ashi+30;if(ashi>=24)ashi=ashi-10;writetime(4,ashi);writelcd_cmd(0xc0+4);break;
					default:break;
				     }
	  }
	  }
writealarm(ashi,afen,amiao);
   }
   }
}

void keyscan()		   //键盘扫描函数
{
alarmoff();	//任意键关闭闹铃
settime();	//设定时间
setalarm();	//设定闹钟函数
}

void alarm()//闹钟函数
{
uchar i;
TR1=1;
writelcd_cmd(0x80);
 for(i=0;i<16;i++)
 {
 writelcd_dat(t4[i]);
 }
 writelcd_cmd(0xc0);
 for(i=0;i<16;i++)
 {
 writelcd_dat(t5[i]);
 }
TR1=0;
delay(100);
}



void main()//主函数部分
{
EA=1;
EX1=1;
IT1=1;
ET1=1;
TMOD=0X10;
TH1=0XFD;
TL0=0X81;
lcd_init();
initds();
while(1)
	{
	keyscan();
	if(flag==1)
	{alarm();}
else if(flag==0&&key4num==0&&key1num==0)
	{
		
		miao=readds(0);
		fen=readds(2);
		shi=readds(4);
		day=readds(6);
		ri=readds(7);
		yue=readds(8);
		nian=readds(9);
		writetime(10,miao);
		writetime(7,fen);
		writetime(4,shi);
		writeday(day);
		writedate(9,ri);
		writedate(6,yue);
		writedate(3,nian);
	
	}

	}
}


void exter()interrupt 2	//外部中断1函数部分
{
 uchar c;
flag=1;
c=readds(0x0c);
}

void timer1() interrupt 3//定时器1函数部分
{
TH1=0XFD;
TL0=0X81;
speaker=~speaker;
}
