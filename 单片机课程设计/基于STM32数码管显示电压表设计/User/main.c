#include "stm32f10x.h"
#include "TM1637.h"
#include "SysTick.h"
#include "adc.h"
#include "led.h"
#include "beep.h"
#include "system.h"
unsigned char Data[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0X6F};
float volt=0;
unsigned char qian,bai,shi,ge;
int main()
{
	SysTick_Init(72);
	ADCx_Init();
	TM1637_Init();
	LED_Init();
	BEEP_Init();
	while(1)
	{ 
		beep=0;
		volt=Get_voltage();
		delay_ms(10);
		
    if(1<volt&&volt<500)//检测到电压指示灯闪烁
    {
			 led1=0;
			 led2=1;
			 delay_ms(100);
			 led1=1;
			 delay_ms(50);
		}
		else
		{
			 led2=0;
			 led1=1;
			 beep=1;
			 delay_ms(100);
			 led2=1;
			 delay_ms(50);
		}
		
		qian=(int)volt/1000%10;
		bai=(int)volt/100%10;
		shi=(int)volt/10%10; 
		ge=(int)volt%10;

    TM1637_writeCammand(0x44);
    TM1637_writeData(0xc0,Data[qian]);
    TM1637_writeData(0xc1,Data[bai]|0x80);
    TM1637_writeData(0xc2,Data[shi]);
    TM1637_writeData(0xc3,Data[ge]);
    TM1637_writeCammand(0x8a);    
	}
}
