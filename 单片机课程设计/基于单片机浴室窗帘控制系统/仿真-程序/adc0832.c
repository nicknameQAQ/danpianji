#include<reg51.h>
#include"delay.h"
#include <intrins.h> 
#include"adc0832.h"
/************************端口定义**************************/
sbit cs=P3^0;
sbit clk=P3^1;
sbit dio=P3^2;
/************************************************************
		   adc0832
*************************************************************/
uchar adc0832()
{
  uint i;
  uchar dat;
  dat=0x00;

  cs=1;
  clk=0;

  cs=0;
  dio=1;
  clk=1;
  _nop_();
  _nop_();
  clk=0;
  _nop_();
  _nop_();

  dio=1;//1
  clk=1;
  _nop_();
  _nop_();
  clk=0;
  _nop_();
  _nop_();

  dio=0;//0
  clk=1;
  _nop_();
  _nop_();
  clk=0;
  _nop_();
  _nop_();

  dio=1;
  clk=1;
  _nop_();
   _nop_();
   clk=0;
     _nop_();
   _nop_();
  for(i=0;i<8;i++) //前8
	{
		dat<<=1; 
		if(dio==1)
	   dat=dat|0x01;
	    else dat=dat|0x00;
		 clk=1; 
		_nop_();
        _nop_();
		clk=0;
		_nop_();
        _nop_();	
	}
	cs=1;

	return dat; 	
}