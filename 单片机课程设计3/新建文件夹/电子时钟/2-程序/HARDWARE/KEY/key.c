#include "key.h" 
#include "delay.h"

/*****************************************************************
*按键初始化
******************************************************************/
void key_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure; 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIO时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; //选中引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 				//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 	//输出速度100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 				//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure); 							//初始化PE0、1、2、3  
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;//选中引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 			//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 	//输出速度100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 			//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure); 							//初始化PE4、5、6  
	GPIO_ResetBits(GPIOE,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
}
/*****************************************************************
*判断按键是否按下
******************************************************************/
u8 iskeyinput(void)
{
	GPIOE->ODR=0x000f;
	if((KEYPORT&0x000f)==0x000f)
		return 0;
	else
		return 1;
}
/*****************************************************************
*按键识别
******************************************************************/
u8 key_identify(void)
{
	u8 linecode=0,rowcode=0;
	u8 i;
	u8 scancode=0x00ef;
	for(i=0;i<4;i++)
	{
		GPIOE->ODR=scancode;
		if((KEYPORT&0x000f)==0x000f)
		{
			rowcode++;
			scancode=scancode<<1|1;
		}
		else
		{
			if(line0==0)linecode=0;
			if(line1==0)linecode=3;
			if(line2==0)linecode=6;
			if(line3==0)linecode=9;
			break;
		}
	}
	return linecode+rowcode;
}
/*****************************************************************
*等待按键松开
******************************************************************/
void wait_key_release(u8 x)
{
	while(1)
	{
		GPIOE->ODR=0x000f;
		if(x==0)seg_disp2();
		else seg_disp1(f);
		if((KEYPORT&0x000f)==0x000f)
			break;
	}
}
/*****************************************************************
*按键扫描，获得键值1～12
******************************************************************/
u8 key_Scan(u8 x)
{
	u8 keycode=0;	
	if(iskeyinput())
	{
		delay_us(100);
		if(iskeyinput())
		{
			keycode=key_identify()+1;
			wait_key_release(x);			
		}
	}
	return keycode;
}
