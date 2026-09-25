  #include<reg52.h>
#include<intrins.h>
unsigned char code displaywelcome[]={" Welcome To My Lcd Timer"};//欢迎界面
unsigned char code displaywish[]={" Happy Every Day ^_^"}; //欢迎界面
unsigned char code overtemperature[]={"OVERTEMPERATURE!"};
unsigned char code digit[]={"0123456789"}; //数字代码
unsigned char mode,TH,TL,TN,TD,length,tempswitch,Maxtemp=40,amode,alarmmode,minutes,hours,
minutea,seconds,houra=12;
sbit SCLK=P1^0; //DS1302 时钟输入
sbit DATE=P1^1; //DS1302 数据输入
sbit REST=P1^2; //DS1302 复位端口
sbit SET=P1^4; //DS1302 设置模式选择位
sbit ADD=P1^5; //增加
sbit RED=P1^6; //减小
sbit CANL=P1^7;
void delay1ms(int i)//1 毫秒延时
{
int j,k;
while(i--)
for(j=76;j>1;j--);
for(k=29;k>1;k--);
}
void delaynus(unsigned char n) //延时若干微秒
{
unsigned char i;
for(i=0;i<n;i++);
}
/***********蜂鸣器模块************/
sbit beep=P3^6; //位定义，定义P.6 位fmp
void dely500(void)
{
unsigned char i;
for(i=250;i>0;i--)
{
_nop_();
}
}
void baojing(unsigned char n)
{
unsigned char x,i;
while(n--)
{
for(i=0;i<5;i++)
{
for(x=0;x<200;x++)
{
beep=~beep;
dely500();
}
}
for(i=0;i<3;i++)
{
for(x=0;x<200;x++)
{
beep=~beep;
dely500();
dely500();
}
}
}
}
/*******DS1302 模块*************/
void Write1302(unsigned char date)//向1302 写数据
{
unsigned char i;
SCLK=0;
delaynus(2);
for(i=0;i<8;i++)
{
DATE=date&0x01;
SCLK=1;
delaynus(2);
SCLK=0;
delaynus(2);
date>>=1;
}
}
void WriteSet1302(unsigned char cmd,unsigned char date) //根据相应的命令输入相应的数据
{
REST=0;
SCLK=0;
REST=1;
Write1302(cmd);
delaynus(5);
Write1302(date);
SCLK=1;
REST=0;
}
unsigned char Read1302(void) //读取1302 数据
{
unsigned char i,date;
delaynus(2);
for(i=0;i<8;i++)
{
date>>=1;
if(DATE==1)
date|=0x80;
SCLK=1;
delaynus(2);
SCLK=0;
delaynus(2);
}
return date;
}
unsigned char ReadSet1302(unsigned char cmd)//根据命令读取1302 相应的值
{
unsigned char date;
REST=0;
SCLK=0;
REST=1;
Write1302(cmd);
delaynus(2);
date=Read1302();
SCLK=1;
REST=0;
return date;
}
void IntDS1302(void) //DS1302 初始化
{
unsigned char flag;
flag= ReadSet1302(0x81);
if(flag&0x80) { //判断时钟芯片是否关闭
WriteSet1302(0x8E,0x00); //根据写状态寄存器命令字，写入不保护指令
WriteSet1302(0x80,((0/10)<<4|(0%10))); //根据写秒寄存器命令字，写入秒的初始值
WriteSet1302(0x82,((0/10)<<4|(0%10))); //根据写分寄存器命令字，写入分的初始值
WriteSet1302(0x84,((0/10)<<4|(0%10))); //根据写小时寄存器命令字，写入小时的初始值
WriteSet1302(0x86,((0/10)<<4|(0%10))); //根据写日寄存器命令字，写入日的初始值
WriteSet1302(0x88,((0/10)<<4|(0%10))); //根据写月寄存器命令字，写入月的初始值
WriteSet1302(0x8c,((10/10)<<4|(10%10))); //根据写年寄存器命令字，写入年的初始值
WriteSet1302(0x90,0xa5); //打开充电功能选择2K 电阻充电方式
WriteSet1302(0x8E,0x80); //根据写状态寄存器命令字，写入保护指令
}
}
/***************液晶显示模块*****************/
sbit RS=P2^5;
sbit RW=P2^6;
sbit E=P2^7;
sbit BF=P0^7;
/*液晶忙检测*/
bit BusyTest(void)
{
bit result;
RS=0;
RW=1;
E=1;
_nop_();
_nop_();
_nop_();
result=BF;
_nop_();
_nop_();
_nop_();
E=0;
return result;
}
/*写指令*/
void Write_com(unsigned char command)
{
while(BusyTest()!=0);
RS=0;
RW=0;
E=0;
_nop_();
_nop_();
_nop_();
P0=command;
_nop_();
_nop_();
_nop_();
E=1;
_nop_();
_nop_();
_nop_();
E=0;
}
/*写地址*/
void Write_Address(unsigned char address)
{
Write_com(address|0x80);
delay1ms(1);
}
/*写数据*/
void Write_Date(unsigned char date)
{
RS=1;
RW=0;
E=0;
_nop_();
_nop_();
_nop_();
P0=date;
_nop_();
_nop_();
_nop_();
E=1;
_nop_();
_nop_();
_nop_();
E=0;
delay1ms(1);
}
/*初始化*/
void Lcd_Int(void)
{
Write_com(0x38);
delay1ms(1);
Write_com(0x38);
delay1ms(1);
Write_com(0x06);
delay1ms(1);
Write_com(0x0c);
delay1ms(1);
Write_com(0x01);
}
void displaymainpart(void)//显示液晶主要部分（不变化部分）
{
Write_Address(0x01);
delay1ms(1);
Write_Date('D');
Write_Date('A');
Write_Date('T');
Write_Date('E');
Write_Date(':');
delay1ms(1);
Write_Address(0x0A);
delay1ms(1);
Write_Date('-');
Write_Address(0x0D);
Write_Date('-');
Write_Address(0x42);
Write_Date('T');
Write_Date('I');
Write_Date('M');
Write_Date('E');
Write_Date(':');
Write_Address(0x4A);
Write_Date('-');
Write_Address(0x4D);
Write_Date('-');
Write_Address(0x06);
Write_Date('2');
Write_Date('0');
}
void display_Second(unsigned char second) //在液晶上显示秒
{
unsigned char i,j;
i=second/10;
j=second%10;
Write_Address(0x4E);
Write_Date(digit[i]);
Write_Date(digit[j]);
delay1ms(1);
}
void display_Minute(unsigned char minute) //在液晶上显示分
{
unsigned char i,j;
i=minute/10;
j=minute%10;
Write_Address(0x4B);
Write_Date(digit[i]);
Write_Date(digit[j]);
delay1ms(1);
}
void display_Hour(unsigned char hour) //在液晶上显示时
{
unsigned char i,j;
i=hour/10;
j=hour%10;
Write_Address(0x48);
Write_Date(digit[i]);
Write_Date(digit[j]);
delay1ms(1);
}
void display_Day(unsigned char day) //在液晶上显示日
{
unsigned char i,j;
i=day/10;
j=day%10;
Write_Address(0x0E);
Write_Date(digit[i]);
Write_Date(digit[j]);
delay1ms(1);
}
void display_Month(unsigned char month) //在液晶上显示月
{
unsigned char i,j;
i=month/10;
j=month%10;
Write_Address(0x0B);
Write_Date(digit[i]);
Write_Date(digit[j]);
delay1ms(1);
}
void display_Year(unsigned char year) //在液晶上显示年
{
unsigned char i,j;
i=year/10;
j=year%10;
Write_Address(0x08);
Write_Date(digit[i]);
Write_Date(digit[j]);
delay1ms(1);
}
void display_houra(unsigned char x) //闹钟小时部分显示
{
unsigned char i,j;
i=x/10;
j=x%10;
Write_Address(0x44);
Write_Date(digit[i]);
Write_Date(digit[j]);
}
void display_minutea(unsigned char x)//闹钟分钟部分显示
{
unsigned char i,j;
i=x/10;
j=x%10;
Write_Address(0x47);
Write_Date(digit[i]);
Write_Date(digit[j]);
}
void display_Time(void) //显示实时时间
{
unsigned char value,day,month,year;
Write_com(0x0c);
value=ReadSet1302(0x81);
seconds=(((value&0x70)>>4)*10+(value&0x0f));
display_Second(seconds);
value=ReadSet1302(0x83);
minutes=(((value&0x70)>>4)*10+(value&0x0f));
display_Minute(minutes);
value=ReadSet1302(0x85);
hours=(((value&0x70)>>4)*10+(value&0x0f));
display_Hour(hours);
value=ReadSet1302(0x87);
day=(((value&0x70)>>4)*10+(value&0x0f));
display_Day(day);
value=ReadSet1302(0x89);
month=(((value&0x70)>>4)*10+(value&0x0f));
display_Month(month);
value=ReadSet1302(0x8D);
year=(((value&0xf0)>>4)*10+(value&0x0f));
display_Year(year);
}
void displaystar(void) //显示欢迎界面
{
unsigned char i,j;
Write_Address(0x0f);
while(displaywelcome[i]!='\0')
{
Write_Date(displaywelcome[i]);
i++;
delay1ms(1);
}
i=0;
Write_Address(0x4f);
while(displaywish[i]!='\0')
{
Write_Date(displaywish[i]);
i++;
delay1ms(1);
}
j=40;
while(j--)
{
Write_com(0x18);//循环左移
delay1ms(700);
}
Write_com(0x01);
delay1ms(10);
}
void gbdisplay(unsigned char address) //时间调整时光标闪烁
{
Write_Address(address);
delay1ms(5);
Write_com(0x0f);
delay1ms(5);
}
void displaymaxt(unsigned char x) //显示最大温度
{
unsigned char i,j,k;
Write_com(0x0c);
delay1ms(2);
Write_Address(0x44);
i=x/100;
j=x/10;
k=x%10;
Write_Date(digit[i]);
Write_Date(digit[j]);
Write_Date(digit[k]);
}
/*****************时间调整部分*********************/
void hourset(void) //调时
{
unsigned char timevalue,hour;
delay1ms(500); //防止多次触发
WriteSet1302(0x8e,0x00);//将写保护去掉，确保能正常将调整后的数值写入DS1302
timevalue=ReadSet1302(0x85); //读取此时的数值
hour=(((timevalue&0x70)>>4)*10+(timevalue&0x0f));
while(1)
{
if(ADD==0)
{
delay1ms(50);
if(ADD==0)
{
hour++;
delay1ms(300);
while(ADD==0);
}
}
if(RED==0)
{
delay1ms(50);
if(RED==0)
{
hour--;
delay1ms(300);
if(hour==0) hour=23;
while(RED==0);
}
}
timevalue=(((hour)/10)<<4|(hour%10));
WriteSet1302(0x84,timevalue);
delay1ms(1);
display_Hour(hour);
Write_Address(0x49);
delay1ms(5);
if(hour>=24) hour=0;
delay1ms(5);
if(CANL==0)
{
mode=0;
Write_com(0x0c);
break;
}
if(SET==0) break;
}
WriteSet1302(0x8e,0x80);
}
void minuteset(void) //调分
{
unsigned char timevalue,minute;
delay1ms(500);
WriteSet1302(0x8e,0x00);
timevalue=ReadSet1302(0x83);
minute=(((timevalue&0x70)>>4)*10+(timevalue&0x0f));
while(1)
{
if(ADD==0)
{
delay1ms(10);
if(ADD==0)
{
minute++;
if(minute>=60) timevalue=0;
while(ADD==0);
}
}
if(RED==0)
{
delay1ms(10);
if(RED==0)
{
minute--;
delay1ms(300);
if(minute==0) minute=59;
while(RED==0);
}
}
timevalue=((minute/10)<<4|(minute%10));
WriteSet1302(0x82,timevalue);
delay1ms(1);
display_Minute(minute);
Write_Address(0x4c);
delay1ms(5);
if(minute>=60) minute=0;
delay1ms(5);
if(CANL==0)
{
mode=0;
Write_com(0x0c);
break;
}
if(SET==0) break;
}
WriteSet1302(0x8e,0x80);
}
void secondset(void) //秒归零
{
unsigned char second;
delay1ms(500);
WriteSet1302(0x8e,0x00);
while(1)
{
if(ADD==0)
{
delay1ms(10);
if(ADD==0)
{
second=0;
WriteSet1302(0x80,0x00);
while(ADD==0);
}
}
delay1ms(1);
display_Second(second);
Write_Address(0x4f);
delay1ms(5);
if(CANL==0)
{
mode=0;
Write_com(0x0c);
break;
}
if(SET==0) break;
}
WriteSet1302(0x8e,0x80);
}
void yearset(void) //调年
{
unsigned char datevalue,year;
delay1ms(500);
WriteSet1302(0x8e,0x00);
datevalue=ReadSet1302(0x8d);
year=(((datevalue&0x70)>>4)*10+(datevalue&0x0f));
while(1)
{
if(ADD==0)
{
delay1ms(10);
if(ADD==0)
{
year++;
while(ADD==0);
}
}
if(RED==0)
{
delay1ms(10);
if(RED==0)
{
year--;
delay1ms(300);
while(RED==0);
}
}
datevalue=((year/10)<<4|(year%10));
WriteSet1302(0x8c,datevalue);
delay1ms(5);
display_Year(year);
delay1ms(5);
Write_Address(0x09);
delay1ms(5);
if(CANL==0)
{
mode=0;
Write_com(0x0c);
break;
}
if(SET==0) break;
}
WriteSet1302(0x8e,0x80);
}
void monthset(void) //调月
{
unsigned char datevalue,month;

delay1ms(500);
WriteSet1302(0x8e,0x00);
datevalue=ReadSet1302(0x89);
month=(((datevalue&0x70)>>4)*10+(datevalue&0x0f));
while(1)
{
if(ADD==0)
{
delay1ms(10);
if(ADD==0)
{
month++;
if(month>12) month=1;
while(ADD==0);
}
}
if(RED==0)
{
delay1ms(10);
if(RED==0)
{
month--;
delay1ms(300);
if(month==0) month=12;
while(RED==0);
}
}
datevalue=((month/10)<<4|(month%10));
WriteSet1302(0x88,datevalue);
delay1ms(1);
display_Month(month);
Write_Address(0x0c);
delay1ms(5);
if(CANL==0)
{
mode=0;
Write_com(0x0c);
break;
}
if(SET==0) break;
}
WriteSet1302(0x8e,0x80);
}
void dayset(void) //调日
{
unsigned char datevalue,day;
delay1ms(500);
WriteSet1302(0x8e,0x00);
datevalue=ReadSet1302(0x87);
day=(((datevalue&0x70)>>4)*10+(datevalue&0x0f));
while(1)
{
if(ADD==0)
{
delay1ms(10);
if(ADD==0)
{
day++;
if(day>31) day=1;
while(ADD==0);
}
}
if(RED==0)
{
delay1ms(10);
if(RED==0)
{
day--;
delay1ms(300);
if(day==0) day=31;
while(RED==0);
}
}
datevalue=((day/10)<<4|(day%10));
WriteSet1302(0x86,datevalue);
delay1ms(1);
display_Day(day);
Write_Address(0x0f);
delay1ms(5);
if(CANL==0)
{
mode=0;
Write_com(0x0c);
break;
}
if(SET==0) break;
}
WriteSet1302(0x8e,0x80);
}
void alarmhourset(void) //闹钟小时调整
{
unsigned char value;
value=houra;
while(1)
{
if(ADD==0)
{
delay1ms(20);
if(ADD==0)
{
Write_com(0x0c);
delay1ms(5);
value++;
if(value>23) value=0;
while(ADD==0);
}
}
if(RED==0)
{
delay1ms(20);
if(RED==0)
{
value--;
if(value==0) value=23;
while(RED==0);
}
}
houra=value;
display_houra(houra);
gbdisplay(0x45);
if(CANL==0)
{
delay1ms(500);
break;
}
}
}
void alarmminuteset(void) //闹钟分钟调整
{
while(1)
{
if(ADD==0)
{
delay1ms(20);
if(ADD==0)
{
minutea++;
if(minutea>59) minutea=0;
while(ADD==0) ;
}
}
if(RED==0)
{
delay1ms(20);
if(RED==0)
{
minutea--;
if(minutea==0) minutea=59;
while(RED==0) ;
}
}
display_minutea(minutea);
gbdisplay(0x48);
if(CANL==0)
{
delay1ms(500);
break;
}
}
}
void TimeSet(void) //时间调整函数
{
display_Time();
if(SET==0)
{
delay1ms(10);
if(SET==0)
{
mode++;
delay1ms(20);
switch(mode)
{
case(1):{gbdisplay(0x48);hourset(); delay1ms(500);} break;
case(2):{gbdisplay(0x4c);minuteset();delay1ms(500);} break;
case(3):{gbdisplay(0x4f);secondset();delay1ms(500);} break;
case(4):{gbdisplay(0x09);yearset();delay1ms(500);} break;
case(5):{gbdisplay(0x0c);monthset();delay1ms(500);} break;
case(6):{gbdisplay(0x0f);dayset();delay1ms(500);} break;
}
if(mode==7) mode=0;
}
}
}
/***************显示温度模块********************/
unsigned char code displayexplain[]={"Digit thermometer"};
unsigned char code displayerror[]={"DS18B20 ERROR"};
unsigned char code displayerror1[]={"PLEASE CHECK "};
unsigned char code displaypart1[]={"WenDu:"};
unsigned char code displaypart2[]={"Cent"};
unsigned char flag;
/**********************操作DS18B20 模块***************************/
sbit DQ=P3^3;
unsigned char time;
/**********************DS18B20 初始化****************************/
bit IntDS18B20(void)
{
bit temp;
DQ=1;
for(time=0;time<2;time++);
DQ=0;
for(time=0;time<200;time++);
DQ=1;
for(time=0;time<10;time++);
temp=DQ;
for(time=0;time<200;time++);
return temp;
}
/**************************读DS18B20**********************/
unsigned char ReadDS18B20(void)
{
unsigned char i;
unsigned char dat;
for(i=0;i<8;i++)
{
DQ=1;
_nop_();
DQ=0;
_nop_();
DQ=1;
for(time=0;time<2;time++);
dat>>=1;
if(DQ==1)
dat=dat|0x80;
else
dat=dat|0x00;
for(time=0;time<10;time++);
}
return dat;
}
/*********************向DS18B20 写数据**************************/
void WriteDS18B20(unsigned char date)
{
unsigned char i;
for(i=0;i<8;i++)
{
DQ=1;
_nop_();
DQ=0;
DQ=date&0x01;
for(time=0;time<10;time++);
DQ=1;
for(time=0;time<1;time++);
date>>=1;
}
for(time=0;time<4;time++);
}
/*******************为读取温度做好准备************************/
void ReadyreadDS18B20(void)
{
IntDS18B20();
WriteDS18B20(0XCC);
WriteDS18B20(0X44);
delay1ms(200);
IntDS18B20();
WriteDS18B20(0XCC);
WriteDS18B20(0XBE);
}
/*************在1602 液晶上显示相关信息模块**********************/
/****************当未检测到DS18B20 时显示信息模块****************/
void Display_Error(void)
{
unsigned char i=0;
Write_Address(0x00);
while(displayerror[i]!='\0')
{
Write_Date(displayerror[i]);
i++;
delay1ms(100);
}
delay1ms(5);
i=0;
Write_Address(0x40);
while(displayerror1[i]!='\0')
{
Write_Date(displayerror1[i]);
i++;
delay1ms(100);
}
delay1ms(3000) ;
}
/*************温度显示模块****************/
void Display_Explain(void)
{
unsigned char i=0;
Write_Address(0x00);
delay1ms(5);
while(displayexplain[i]!='\0')
{
Write_Date(displayexplain[i]);
i++;
delay1ms(5);
}
i=0;
Write_Address(0x40);
while(displaypart1[i]!='\0')
{
Write_Date(displaypart1[i]);
i++;
delay1ms(5);
}
i=0;
Write_Address(0x4C);
while(displaypart2[i]!='\0')
{
Write_Date(displaypart2[i]);
i++;
delay1ms(5);
}
Write_Address(0x49);
Write_Date('.');
delay1ms(5);
}
/*********************显示温度整数部分*************************/
void Display_Integer(unsigned char x)
{
unsigned char i,j,k;
i=x/100;
j=x%100/10;
k=x%10;
Write_Address(0x46);
if(flag==1) Write_Date('-');
Write_Date(digit[i]);
Write_Date(digit[j]);
Write_Date(digit[k]);
delay1ms(50) ;
}
/************显示温度小数部分*********************/
void Display_Decimal(unsigned char x)
{
Write_Address(0x4A);
delay1ms(5);
Write_Date(digit[x]);
delay1ms(50);
}
void displayovert(void)
{
unsigned char i;
Write_com(0x01);
delay1ms(10);
Write_Address(0x03);
delay1ms(10);
Write_Date('W');
Write_Date('A');
Write_Date('R');
Write_Date('N');
Write_Date('I');
Write_Date('N');
Write_Date('G');
Write_Date('!');
Write_Address(0x40);
while(overtemperature[i]!='\0')
{
Write_Date(overtemperature[i]);
i++;
delay1ms(10);
}
}
/*******************温度主函数********************/
void Temperature(void)
{
unsigned char i;
delay1ms(5);
if(IntDS18B20()!=0) Display_Error();
else
{
delay1ms(1);
Write_com(0x01);
delay1ms(5);
Display_Explain();
for(length=50;length>0;length--)
{
for(i=0;i<6;i++)
{
flag=0;
ReadyreadDS18B20();
TL=ReadDS18B20();
TH=ReadDS18B20();
if((TH&0XF8)!=0X00)
{
flag=1;
TL=~TL;
TH=~TH;
TL+=1;
if(TL>255) TH+=1;
TN=TH*16+TL/16;
TD=(TL%16)*10/16;
}
TN=TH*16+TL/16;
TD=(TL%16)*10/16;
Display_Integer(TN);
Display_Decimal(TD);
delay1ms(10);
}
if(tempswitch==1)
{
if(TN>=Maxtemp)
{
Write_Address(0x03);
delay1ms(5);
displayovert();
while(1)
{
baojing(1);
if(CANL==0)
{
Write_com(0x01);
delay1ms(5);
break;
}
}
}
}
if(CANL==0) break;
}
}
}
void tempset(void)
{
if(ADD==0)
{
delay1ms(10);
if(ADD==0)
{
Maxtemp++;
while(ADD==0);
}
}
if(RED==0)
{
delay1ms(10);
if(RED==0)
{
Maxtemp--;
while(RED==0);
}
}
}
void Maxtempset(void) //高温报警设置
{
Write_com(0x01);
delay1ms(10);
Write_Address(0x03);
delay1ms(10);
Write_Date('T');
Write_Date('E');
Write_Date('M');
Write_Date('P');
Write_Address(0x09);
Write_Date('S');
Write_Date('E');
Write_Date('T');
Write_Address(0x40);
Write_Date('M');
Write_Date('A');
Write_Date('X');
Write_Date(':');
while(1)
{
if(SET==0)
{
delay1ms(20);
if(SET==0)
{
Write_Address(0x4e);
Write_com(0x0f);
while(1)
{
if(ADD==0)
{
delay1ms(20);
if(ADD==0)
{
Write_Address(0x4b);
delay1ms(5);
Write_Date('O');
Write_Date('N');
Write_Date(' ');
tempswitch=1;
}
}
if(RED==0)
{
delay1ms(20);
if(RED==0)
{
Write_Address(0x4b);
Write_Date('O');
Write_Date('F');
Write_Date('F');
tempswitch=0;
}
}
if(CANL==0)
{
delay1ms(500);
break;
}
}
}
}
if(tempswitch==1)
{
Write_Address(0x4b);
Write_Date('O');
Write_Date('N');
Write_Date(' ');
tempset();
displaymaxt(Maxtemp);
if(CANL==0)
{
Write_com(0x01);
displaymainpart();
break;
}
}
if(tempswitch==0)
{
Write_com(0x0c);
Write_Address(0x4b);
Write_Date('O');
Write_Date('F');
Write_Date('F');
}
if(CANL==0)
{
Write_com(0x01);
displaymainpart();
break;
}
}
}
void alarmswitchset(void)
{
while(1)
{
if(ADD==0)
{
delay1ms(100);
if(ADD==0)
{
alarmmode=1;
Write_Address(0x4c);
Write_Date('O');
Write_Date('N');
Write_Date(' ');
}
}
if(RED==0)
{
alarmmode=0;
Write_Address(0x4c);
Write_Date('O');
Write_Date('F');
Write_Date('F');
}
if(CANL==0)
{
delay1ms(500);
break;
}
}
}
/**************** 设置模块******************/
void Set(void)
{
if(SET==0)
{
delay1ms(10);
if(SET==0)
{

Write_Address(0x40);
Write_Date('T');
Write_com(0x0f);
delay1ms(1000);
Write_com(0x0c);
while(1)
{
TimeSet();
if(ADD==0)
{
delay1ms(20);
if(ADD==0)
{
Write_com(0x01);
Write_Address(0x00);
delay1ms(5);
Write_Date('S');
delay1ms(10);
Maxtempset();
}
}
if(RED==0)
{
delay1ms(20);
if(RED==0)
{
Write_com(0x01);
delay1ms(5);
Write_Address(0x02);
Write_Date('A');
Write_Date('L');
Write_Date('A');
Write_Date('R');
Write_Date('M');
Write_Address(0x09);
Write_Date('S');
Write_Date('E');
Write_Date('T');
Write_Address(0x46);
Write_Date('-');
display_houra(houra);
display_minutea(minutea);
if(alarmmode==1)
{
Write_Address(0x4c);
Write_Date('O');
Write_Date('N');
Write_Date(' ');
}
if(alarmmode==0)
{
Write_Address(0x4c);
Write_Date('O');
Write_Date('F');
Write_Date('F');
}
while(1)
{
if(SET==0)
{
delay1ms(100);
if(SET==0)
{
amode++;
switch(amode)
{
case(1):{gbdisplay(0x45);alarmhourset();Write_com(0x0c);delay1ms(500);} break;
case(2):{gbdisplay(0x48);alarmminuteset();Write_com(0x0c);delay1ms(500);} break;
case(3):{gbdisplay(0x4c);alarmswitchset();Write_com(0x0c);delay1ms(500);} break;
}
if(amode==4) amode=0;
}
}
if(CANL==0)
{
Write_com(0x01);
delay1ms(5);
displaymainpart();
break;
}
}
}
}
if(CANL==0)
{
Write_Address(0x40);
Write_Date(' ');
Write_com(0x0c);
break;
}
}
}
}
}
/***************主函数**********************/
void main()
{
IE=0X82; //打开定时中断
TMOD=0X01; //选择定时器0 工作在方式1
TR0=0; //启动定时器0
IntDS1302(); //初始化DS1302
delay1ms(1);
Lcd_Int(); //1602 液晶初始化
delay1ms(2);
displaystar(); //显示欢迎界面
displaymainpart(); //显示主要部分（不变化）
while(1)
{
display_Time( );
Set( );
if(ADD==0)
{
Write_com(0x01);
delay1ms(5);
Temperature();
Write_com(0x01);
displaymainpart();
}
if((hours==houra)&&(minutes==minutea)&&(seconds==0))
{
if(alarmmode==1)
{
Write_com(0x01);
delay1ms(5);
Write_Address(0x03);
Write_Date('T');
Write_Date('I');
Write_Date('M');
Write_Date('E');
Write_Address(0x08);
Write_Date('U');
Write_Date('P');
Write_Date('!');
delay1ms(5);
while(1)
{
baojing(1);
if(CANL==0)
{
Write_com(0x01);
delay1ms(5);
displaymainpart();
break;
}
}
}
}
}
}
