#include"test.h"
#define out1 1 //高频
#define out2 2 //低频
//-------------------------------------------------------------------------
//mcu初始化 端口 定时器 中断 外中断 变量
//-------------------------------------------------------------------------
void mcu_init()
{
//******************端口
  data_direc = out; //输出
  DDRB = out; //输出
  DDRC = out; //输出
  PORTC = 0X00; //输出0
  DDRB &= ~( 0 << 4 ); //输入
  PORTB &= ~( 0 << 4 );
  DDRB &= ~( 0 << 3 ); //输入
  PORTB |= ( 1 << 3 );
//********************定时器
  TIMSK |= 0x12; //定时中断允许位 只允许两个匹配
}


int main()
{
uchar a;
  mcu_init();                 //初始化
//  usart_init( 57600 );        //初始化
//  yj_set( 0x38, 0x0f, 0x06 ); //初始化
//  clearall(); //初始化
//  showstr( "cymometer", 1, 1, 9 ); //液晶显示

DDRA=0XFF;//OUT
PORTA=0XFF;
while(1)
{
for(a=0;a<200;a++)
{
_delay_us(10);
PORTA^=(1<<out1);
}
PORTA^=(1<<out2);

}

return 1;

}
