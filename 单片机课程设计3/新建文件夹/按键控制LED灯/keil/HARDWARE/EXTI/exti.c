#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"

u8 ID=0;

//外部中断初始化函数
void EXTIX_Init(void)
{
 
    KEY_Init();//初始化按键对应io模式
    Ex_NVIC_Config(GPIO_C,8,FTIR); 		//下降沿触发
	Ex_NVIC_Config(GPIO_C,9,FTIR);		//下降沿触发
	Ex_NVIC_Config(GPIO_C,10,FTIR);		//下降沿触发
	Ex_NVIC_Config(GPIO_B,0,FTIR);		//下降沿触发
	
	MY_NVIC_Init(2,2,EXTI15_10_IRQn,2);    	//抢占2，子优先级2，组2 对应PC10
	MY_NVIC_Init(2,1,EXTI9_5_IRQn,2);  	//抢占2，子优先级1，组2 对应PC8,PC9
    MY_NVIC_Init(2,3,EXTI0_IRQn,2); 
} 


//外部中断10服务程序
void EXTI15_10_IRQHandler(void)
{	 
	//delay_ms(10);   //消抖	
	Delay(10);
	if(KEY2==0)	    //KEY0按键
	{
	  LED_Run_0();
	}
	
    EXTI->PR=1<<10;  //清除LINE10上的中断标志位  

}




//外部中断5_9服务中断程序 pc8,pc9
 void EXTI9_5_IRQHandler(void)
{		
	//delay_ms(10);   //消抖
    Delay(30);	
	
    if(EXTI_GetITStatus(EXTI_Line8)!=RESET){
	     LED0=!LED0;
	
	}
	 if(EXTI_GetITStatus(EXTI_Line9)!=RESET){
		 
		 for(u8 count=10;count>0;count--){
				LED0=0;
				LED1=1;
				Delay(10);
				LED0=1;
				LED1=0;
				Delay(10);
			}
	 }
  
//	if(KEY0==0)	    //KEY0按键
//	{
//	  LED0=!LED0;	
//		
//	}

//	if(KEY1==0)	    //KEY1按键
//		{
//			for(u8 count=10;count>0;count--){
//				LED0=0;
//				LED1=1;
//				Delay(10);
//				LED0=1;
//				LED1=0;
//				Delay(10);
//			}
//	}
// 
	EXTI->PR=1<<8;  //清除LINE8上的中断标志位  
	EXTI->PR=1<<9;  //清除LINE9上的中断标志位  	
}

void  EXTI0_IRQHandler (){
      Delay(10);
    if(KEY3==0){
	  LED0=!LED0;
	}
     EXTI->PR=1<<0;

}

ITStatus EXTI_GetITStatus(uint32_t EXTI_Line)
{
	ITStatus bitstatus = RESET;
    uint32_t enablestatus = 0;
  
    enablestatus =  EXTI->IMR & EXTI_Line;
   if (((EXTI->PR & EXTI_Line) != (uint32_t)RESET) && (enablestatus != (uint32_t)RESET))
   {
    bitstatus = SET;
   }
   else
   {
    bitstatus = RESET;
   }
  return bitstatus;
}
