#ifndef _TM1673_H
#define _TM1673_H
//引脚时钟使能函数的宏定义
#define TM1637_RCC_Cmd    RCC_APB2PeriphClockCmd
//DIO和CLK相关引脚的宏定义
#define    TM_DIO_PORT    GPIOB		                 
#define    TM_DIO_CLK 	  RCC_APB2Periph_GPIOB		
#define    TM_DIO_PIN			GPIO_Pin_11			        
#define    TM_CLK_PORT    GPIOB			              
#define    TM_CLK_CLK 	  RCC_APB2Periph_GPIOB		
#define    TM_CLK_PIN		  GPIO_Pin_10
#define    TM_VCC_PORT    GPIOB
#define    TM_VCC_CLK 	  RCC_APB2Periph_GPIOB		
#define    TM_VCC_PIN 	  GPIO_Pin_1		
//时钟线和数据线高低电平宏定义
#define      CLK_1        GPIO_SetBits(TM_CLK_PORT,TM_CLK_PIN)
#define      CLK_0        GPIO_ResetBits(TM_CLK_PORT,TM_CLK_PIN)
#define      DIO_1        GPIO_SetBits(TM_DIO_PORT,TM_DIO_PIN)
#define      DIO_0        GPIO_ResetBits(TM_DIO_PORT,TM_DIO_PIN)
//相关函数声明
void TM1637_Init(void);
void TM1637_start(void);
void TM1637_stop(void);
void TM1637_write1Bit(unsigned char mBit);
void TM1637_write1Byte(unsigned char mByte);
void TM1637_writeCammand(unsigned char mData);
void TM1637_writeData(unsigned char addr, unsigned char mData);
#endif 
