#ifndef display_c
  #define display_c

  #include "main.h"

  uchar dzl( void ) //读指令 测试LCD忙碌状态
  {
    uchar value; //
    data_direc = in;
    PORTB &= ~( 1 << rs ); //0
    PORTB |= ( 1 << rw ); //1
    PORTB |= ( 1 << e ); //1
    _delay_us( 2 );
    value = data_in;
    PORTB &= ~( 1 << e ); //0
    _delay_us( 1 );
    return value;
  }

//-------------------------------
  void xzl( uchar zhilin ) //写指令
  {
    uchar busy;
    busy = dzl(); //查忙
//如果为1等待
    while ( busy >= 80 )
    {
      busy = dzl();
    }
    data_direc = out;
    PORTB &= ~( 1 << rs ); //0
    PORTB &= ~( 1 << rw ); //0
    PORTB &= ~( 1 << e ); //0
    data_out = zhilin; //发送
    PORTB |= ( 1 << e ); //1
    _delay_us( 2 );
    PORTB &= ~( 1 << e ); //0
  }

//------------------------
  void xsj( uchar shujv ) //写数据
  {
    uchar busy;
    busy = dzl(); //查忙
//最高位如果为1
    while ( busy >= 80 )
    {
      busy = dzl();
    }
    data_direc = out;
    PORTB |= ( 1 << rs );
    PORTB &= ~( 1 << rw );
    PORTB |= ( 1 << e );
    data_out = shujv; //发送
    _delay_us( 2 );
    PORTB &= ~( 1 << e ); //0
  }


//----------------------
  void clearall() //清屏指令
  {
    xzl( 0x01 );
  }

//------------------------------
  void yj_set( uchar xsh, uchar gb, uchar dt ) //显示设置
  {
//设置，清屏，显示模式，光标模式，动态模式
    xzl( 0x01 );

    xzl( xsh );

    xzl( gb );

    xzl( dt );
  }

//----------------------------------------
  void makecg( uchar address, uchar* cg ) //自造字库
  {
//首地址，字库指针
    uchar num;
//8字节为一个图形 
    for ( num = 0; num < 8; num++ )
    {
      xzl( address++ ); //一个地址一个数据
      xsj( *cg++ );
    }
  }

//------------------------------
  void showstr( uchar* str, uchar xpot, uchar ypot, uchar num ) //液晶显示
  {
//字符指针，坐标，个数 坐标1--16，1--2
//第一行
    if ( ypot == 1 )
    {
      xzl( 0x80 + xpot - 1 );
    }
//第二行 
    else
    {
      xzl( 0xc0 + xpot - 1 );
    }
//写数据 不要超过16-xpot个
    if ( num > 16-xpot )
    {
      num = 16-xpot;
    }
    for ( ; num > 0; num-- )
    {
      xsj( *str++ );
    }
  }

#endif
