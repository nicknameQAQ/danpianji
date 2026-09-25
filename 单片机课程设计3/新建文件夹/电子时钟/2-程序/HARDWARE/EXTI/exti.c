#include "exti.h"
#include "key.h"
#include "delay.h"

u8 exit_flag=0;//时间修改标志位

/*****************************************************************
*外部中断初始化
******************************************************************/
void exti_init(void)
{
	key_init(); 																					//按键对应的IO口初始化
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//PE3连接到中断线3
	
  EXTI_InitStructure.EXTI_Line = EXTI_Line3;							//LINE3
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;								//使能LINE3
  EXTI_Init(&EXTI_InitStructure);													//按照上述参数配置
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;						//外部中断0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;				//子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);						 									//按照上述参数配置	   
}
/*****************************************************************
*外部中断9的中断服务函数
******************************************************************/
void EXTI3_IRQHandler(void)
{
		exit_flag=1;
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
		EXTI_ClearITPendingBit(EXTI_Line3);			//清除Line9上的中断标志位
}


