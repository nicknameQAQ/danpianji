#include "key.h"
#include "delay.h"


//按键初始化函数 
void KEY_Init(void)
{	
	RCC->APB2ENR|=1<<4;     //使能PORTC时钟
	GPIOC->CRH&=0XFFFFF000;	//PC8、9设置成输入	  
	GPIOC->CRH|=0X00000888; 	//PC8上拉,PC8默认下拉		 
	GPIOC->ODR|=(1<<8)|(1<<9)|(1<<10);	   	
	RCC->APB2ENR|=1<<3;     //使能PORTC时钟
	GPIOB->CRL&=0XFFFFF000;	//PC8、9设置成输入	  
	GPIOB->CRL|=0X00000008; 	//PC8上拉,PC8默认下拉		 
	GPIOB->ODR|=(1<<0);	   		
} 
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//返回值：
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下

//注意此函数有响应优先级,KEY0>KEY1>KEY2
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0))
	{
		//delay_ms(10);//去抖动 
		Delay(20);
		key_up=0;
		if(KEY0==0)return 1;
		else if(KEY1==0)return 2;
        //else if(KEY2==0)return 3;
	}else if(KEY0==1&&KEY1==1)key_up=1; 	     
	return 0;// 无按键按下
}


