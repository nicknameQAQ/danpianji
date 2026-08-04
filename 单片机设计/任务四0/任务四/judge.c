#include "judge.h"
#include"data.h"

int number;

void time()						//时间月份函数
{
if((year%400==0)||((year%4==0)&&(year%100!=0)))
{
	number=	366;
if(month==1)
	maxday=31;
if(month==2)
	maxday=29;							 //29天
if(month==3)
	maxday=31;
if(month==4)
	maxday=30;
if(month==5)
	maxday=31;
if(month==6)
	maxday=30;
if(month==7)
	maxday=31;
if(month==8)
	maxday=31;	
if(month==9)
	maxday=30;	
if(month==10)
	maxday=31;	
if(month==11)
	maxday=30;	
if(month==12)
	maxday=31;
					
}
else								 //两种不同的形式
{
	number=365;
if(month==1)
	maxday=31;
if(month==2)
	maxday=28;						   //28天
if(month==3)
	maxday=31;
if(month==4)
	maxday=30;
if(month==5)
	maxday=31;
if(month==6)
	maxday=30;
if(month==7)
	maxday=31;
if(month==8)
	maxday=31;	
if(month==9)
	maxday=30;	
if(month==10)
	maxday=31;	
if(month==11)
	maxday=30;	
if(month==12)
	maxday=31;
}
}

