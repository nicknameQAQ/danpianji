#include "TM1637.h"
#include "SysTick.h"

void TM1637_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	TM1637_RCC_Cmd(TM_DIO_CLK|TM_CLK_CLK|TM_VCC_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Pin = TM_DIO_PIN | TM_CLK_PIN | TM_VCC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//设置为推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	PBout(1)=1;
}

void TM1637_start( void )
{
	 CLK_1;
	 DIO_1;
	 delay_us(5);
	 DIO_0;
	 delay_us(5);
	 CLK_0;
	 delay_us(5);
}

void TM1637_stop( void )
{
 CLK_0;
 delay_us(5);
 DIO_0;
 delay_us(5);
 CLK_1;
 delay_us(5);
 DIO_1;
 delay_us(5);
}

void TM1637_write1Bit(unsigned char mBit )
{
 CLK_0;
 delay_us(5);
 if(mBit)
  DIO_1;
 else
  DIO_0;
 delay_us(5); 
 CLK_1;
 delay_us(5);
}

void TM1637_write1Byte(unsigned char mByte)
{
 char loop = 0;
 for(loop = 0; loop < 8; loop++)
 {
  TM1637_write1Bit((mByte>>loop)&0x01);
 }
 CLK_0;
 delay_us(5);
 DIO_1;
 delay_us(5);
 CLK_1;
 delay_us(5);
 while(GPIO_Pin_11==1);
}

void TM1637_writeCammand(unsigned char mData)
{
 TM1637_start();
 TM1637_write1Byte(mData);
 TM1637_stop(); 
}

void TM1637_writeData(unsigned char addr, unsigned char mData)
{
 TM1637_start();
 TM1637_write1Byte(addr);
 TM1637_write1Byte(mData);
 TM1637_stop(); 
}
