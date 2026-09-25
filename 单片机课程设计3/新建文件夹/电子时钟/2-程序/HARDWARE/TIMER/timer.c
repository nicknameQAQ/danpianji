#include "timer.h"

/*****************************************************************
*定时器3初始化
******************************************************************/
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  					//使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = arr; 									//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  								//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);						//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);											//允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); 																				//使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 							//定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 				//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
/*****************************************************************
*定时器3中断服务函数
******************************************************************/
void TIM3_IRQHandler(void)
{
	bs++;//百分秒计时
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
/*****************************************************************
*电子时钟时间计时
******************************************************************/
void get_time(void)
{
	if(bs>99)
	{
		bs=0;s++;
		if(s==60)
		{
			s=0;m++;
			if(m==60)
			{
				m=0;h++;
				if(h==24)
				{h = 0;}
				date[0]=h/10;
				date[1]=h%10;					
			}
			date[2]=m/10;
			date[3]=m%10;
		}
		date[4]=s/10;
		date[5]=s%10;
	}
	date[6]=bs/10;
	date[7]=bs%10;//获取此时的时间值
}
