
#ifndef main_h
  #define main_h
//数据类型重定义
  #define uchar unsigned char
  #define uint  unsigned int
  #define ulint unsigned long int
//系统头文件
  #include <avr/io.h>
  #include "util/delay.h"
  #include <avr/interrupt.h>//中断
//项目头文件
  #include "cale.h"     //计算
  #include "display.h"  //显示
  #include "intree.h"   //中断
  #include "chk.h"      //串口
  #include "celiang.h"  //测量
//********
  #define f_cpu 16000000//晶振
  #define m_cpu m16     //cpu类型
  #define debug 1       //软件测试模式
  #define pc    1       //连接pc进行串口联调
  #define led_d 0       //指示灯调试
//**********
//*****************中断初始化
  #define mzd_on()  SREG=SREG|0X80  //打开总中断
  #define mzd_off() SREG=SREG&0X7F  //关闭总中断
  #define stop()    while(1)        //停止
//****************测试用的led指示灯,项目中没用到,全部是用串口调的
  #define led1 1   //连接到pc口
  #define led2 2
  #define led3 3
  #define led1_on() PORTC|=(1<<led1)
  #define led2_on() PORTC|=(1<<led2)
  #define led3_on() PORTC|=(1<<led3)
  #define led1_off() PORTC&=~(1<<led1)
  #define led2_off() PORTC&=~(1<<led2)
  #define led3_off() PORTC&=~(1<<led3)
//定义组合灯的含义,2^3-1=7
//~~~~~~~~~~~~~~
#endif 
//******************全局变量定义 除了main文件,其他的全部声明一次
#ifndef main_c
  #define main_c

  extern uint t0_cont;
  extern uint t1_cont;

  extern uchar flg1;

#endif
