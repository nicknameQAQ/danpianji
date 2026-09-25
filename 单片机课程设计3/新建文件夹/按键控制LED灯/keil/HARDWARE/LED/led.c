#include "led.h"
#include "delay.h"
    
//LED IO初始化 
void LED_Init(void)
{
	 RCC->APB2ENR|=1<<4;    //使能PORTC时钟	   
	 GPIOC->CRL&=0X00000000; //清零GPIOC->CRL=GPIOC->CRL&0X00000000;
	 GPIOC->CRL|=0X00003333;//PC0~3推挽输出 输出速度最大50Mhz    GPIOC->CRL|0X33333333		
     GPIOC->ODR|=(1<<0)|(1<<1)|(1<<2)|(1<<3);//PC.0~3 输出高电平 "|"或操作
	 RCC->APB2ENR|=1<<3;
	 GPIOB->CRL&=0X00000000;
	 GPIOB->CRL|=0X00000003;
	 GPIOB->ODR|=1<0;
	
}
 
//跑马灯程序
//从LED0-LED3逐一点亮，然后从LED3-LED0逐一熄灭
void LED_Run_0(void){
    for(u8 count=5;count>0;count--){
    	u32 LED=0x00000001;//设置初始控制码	
	   //逐一点亮
	   for(u8 i=0;i<4;i++)
	   {
			GPIOC->ODR&=~LED;	//向GPIOC口写点亮LED的控制码
			Delay(30);
			LED<<=1;	//上一个控制码左移一位，获得下一个控制码
	    }
        Delay(30);
		LED>>=1;
	   // 逐一熄灭
	   for(u8 j=0;j<4;j++)
	   {
			GPIOC->ODR|=LED;	//向GPIOC口写点亮LED的控制码
			Delay(30);
			LED>>=1;	//上一个控制码左移一位，获得下一个控制码
	    }
	    Delay(30);
	}

}
