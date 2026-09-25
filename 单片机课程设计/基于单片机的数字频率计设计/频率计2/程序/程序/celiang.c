#ifndef celiang_c
  #define celiang_c

  #include "main.h"
//***************************************定时器设置
//t0只用来记时,固定的1s t1记数在try和f中,记时在t中
  void timer0_init() //CTC模式 记时1s
  {
    TCNT0 = bch; //补偿  在5mhz下得出的补偿系数
    OCR0 = 249;   //设定TOP值  每次250个
    TCCR0 = ( 1 << WGM01 ) | ( 0 << WGM00 ) | ( 0 << COM01 ) | ( 0 << COM00 ) |
      ( 0 << CS02 ) | ( 0 << CS01 ) | ( 0 << CS00 );
  }
//*************定时器1 16位
  void timer1_init( uchar moshi ) //三个，可以只更新其中某些
  {
    TCNT1 = 0X00; //清除
//尝试 记数
    if ( moshi == try )
    {
      OCR1A = 0XFFFE; //无意义
      TCCR1A = ( 0 << COM1A1 ) | ( 0 << COM1A0 ) | ( 0 << COM1B1 ) | ( 0 <<
        COM1B0 ) | ( 0 << WGM11 ) | ( 0 << WGM10 ); //无匹配  ctc模式
      TCCR1B = ( 0 << ICNC1 ) | ( 0 << ICES1 ) | ( 0 << WGM13 ) | ( 1 << WGM12 )
        | ( 0 << CS12 ) | ( 0 << CS11 ) | ( 0 << CS10 ); //关闭
    }
//测频率 记数
    else if ( moshi == cef )
    {
      OCR1A = 49999; //50k个脉冲 //低频<50k还要补偿一个脉冲 好象不要
      TCCR1A = ( 0 << COM1A1 ) | ( 0 << COM1A0 ) | ( 0 << COM1B1 ) | ( 0 <<
        COM1B0 ) | ( 0 << WGM11 ) | ( 0 << WGM10 ); //无匹配  ctc模式
      TCCR1B = ( 0 << ICNC1 ) | ( 0 << ICES1 ) | ( 0 << WGM13 ) | ( 1 << WGM12 )
        | ( 0 << CS12 ) | ( 0 << CS11 ) | ( 0 << CS10 ); //关闭
    }
//测周期 记时
    else
    {
      OCR1A = 49999; //50ms
      TCCR1A = ( 0 << COM1A1 ) | ( 0 << COM1A0 ) | ( 0 << COM1B1 ) | ( 0 <<
        COM1B0 ) | ( 0 << WGM11 ) | ( 0 << WGM10 ); //无匹配  ctc模式
      TCCR1B = ( 0 << ICNC1 ) | ( 0 << ICES1 ) | ( 0 << WGM13 ) | ( 1 << WGM12 )
        | ( 0 << CS12 ) | ( 0 << CS11 ) | ( 0 << CS10 ); //关闭
    }
  }

//-------------------------------------------------------------------------
//   //刷新所有的东西 为重新测量做准备    
//-------------------------------------------------------------------------
  void refish()
  {
    TIFR = 0Xff;     //清中断
    _delay_ms( 40 ); //延时
    _delay_ms( 40 );
    t0_cont = 0;     //数值
    t1_cont = 0;
    flg1 = 0;        //flag
	#if pc
send_char('F');
send_char('F');
    #endif
  }
//-------------------------------------------------------------------------
//   //估计范围   997为界
//-------------------------------------------------------------------------
  uchar hz_try() //估计范围 1k
  {
    timer1_init( try ); //设置模式
    TCCR1B |= 7;        
    _delay_ms( 10 );
    _delay_ms( 10 );
    _delay_ms( 10 );
    _delay_ms( 10 );
    _delay_ms( 10 );
    _delay_ms( 10 );
    _delay_ms( 10 );
    _delay_ms( 10 );
    _delay_ms( 10 );
    _delay_ms( 10 );
    TCCR1B &= ~7; 
    if ( t1_cont == 0 )
    {
      if ( TCNT1 > 99 )
      {
        return 1;
      }
//如果大于100
      else
      {
        return 0;
      }
//如果小于100
    }
    else
    {
      t1_cont = 0; 
      return 1;    
    }
  }
//-------------------------------------------------------------------------
//       //频率法
//-------------------------------------------------------------------------
  void cl_f()
  {
    uchar back[7]; //7位整数
    uchar time;

    showstr( "c_f", 1, 2, 3 ); //频率法

    timer1_init( cef );        //
    timer0_init();             //  

while( !( PINB& ( 1 << 4 )));//等待变1
while (( PINB& ( 1 << 4 )));//等待变0
    TCCR1B = 0X0e; //两个都开启 先开频率
    TCCR0 = 0X09;  //时间晚开
    for ( time = 9; time > 0; time-- )
    {
      _delay_ms( 10 );
    }
    _delay_ms( 9 );
    _delay_us( 800 );//这里要实际调试
#if pc
	send_char('A');
#endif
    while ( !flg1 )

    {
	#if pc
     send_char( 0x64 );
	#else
     send_char( 'w' );
	#endif
    }
//*************************查看数据
    #if pc
    send_char( t1_cont / 0x100 ); //查看倍数
    send_char( t1_cont % 0X100 );
    send_char( TCNT1 / 0X100 ); //查看余数
    send_char( TCNT1 % 0X100 );
	send_char('F');
    #endif
 cale_f(back);//计算
 showstr( ">1KHz", 11, 1, 5 ); //大于1k
 showstr( "             ", 4, 2, 10 );//清屏
 showstr(back,7,2,7);//显示
  }
//-------------------------------------------------------------------------
//       //周期法
//-------------------------------------------------------------------------
  void cl_t()
  {
    uchar xsh[6]; //两位小数 xxx.xxhz 

    showstr( "c_t", 1, 2, 3 );    

    timer1_init( cet ); //周期模式
//等待变成1
    while ( !( PINB& ( 1 << 4 )))
      ;
//等待变成0 开始计时
    while (( PINB& ( 1 << 4 )))
      ;
    TCCR1B = 0x09; //开启定时器记时
//等待变成1
    while ( !( PINB& ( 1 << 4 )))
      ;
//等待变成0 停止计数
    while (( PINB& ( 1 << 4 )))
      ;
    TCCR1B = 0X80; //关闭定时器
#if pc
send_char('B');
send_char(t1_cont/0x100);
send_char(t1_cont%0x100);
send_char(TCNT1/0X100); //
send_char(TCNT1%0x100); //
send_char('F');
#endif

cale_t(xsh);//计算
showstr( "<1KHz", 11, 1, 5 ); //小于1k
showstr( "             ", 4, 2, 10 );//清屏
showstr( xsh,8,2,6 ); //显示
  }

#endif 

