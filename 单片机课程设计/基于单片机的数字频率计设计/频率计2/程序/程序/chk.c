//SIGNAL 执行时全局中断触发位被清除、其他中断被禁止
//INTERRUPT 执行时全局中断触发位被置位、其他中断可嵌套执
#ifndef chk_c
  #define chk_c

  #include "main.h"

//*******串口初始化 波特率，倍速，位数，校验，中断，多机，同异步停止位，时钟极性
  void usart_init( uint botelv )
  {
    UBRRH = ( f_cpu / botelv / 16-1 ) / 256;
    UBRRL = ( f_cpu / botelv / 16-1 ) % 256; //计算波特
    UCSRA = 0X00; //
    UCSRB = ( 1 << RXCIE ) | ( 0 << RXEN ) | ( 1 << TXEN );
//使能接收中断，使能接收，使能发送  
    UCSRC = ( 1 << URSEL ) | 0x06;
//异步，8位数据，无奇偶校验，一个停止位，无倍速
  } //设置ucsrc时高位要为1，与ubrrh共地址

//***************发送一个字符
  void send_char( uchar zifu )
  {
    while ( !( UCSRA& ( 1 << UDRE )))
      ;
//死等 查询发送  
    UDR = zifu;
  }

//***************发送一个字符串
  void send_str( uchar* str )
  {
    while ( *str )
//一直到发送完即0为止  
    {
      send_char( *str++ );
    }
    send_char( 0x0D );
    send_char( 0x0A ); //结尾发送回车换行
  }

//****************中断接受  删除
/* SIGNAL( SIG_USART_RECV ) //串口接收中断服务程序
{

}*/

#endif
