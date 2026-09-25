#include "stm32f4xx.h" 
#include "delay.h"
#include "key.h"
#include "seg.h"
#include "exti.h"
#include "timer.h"

u8 h=0,m=0,s=0,bs=0;//存储时间的变量
u8 date[8]={0,0,0,0,0,0,0,0};//数据
u16 address[8]={0x0101,0x0102,0x0104,0x0108,0x0110,0x0120,0x0140,0x0180};//位码

int main(void)
{ 
	gpio_init();								//数码管引脚初始化
	key_init();      						//初始化与按键连接的硬件接口
	exti_init();								//外部中断初始化
	TIM3_Int_Init(15-1,2100-1);	//定时器3初始化

	while(1)
	{
		if(exit_flag==1)
		{
			seg_chag(); //键盘修改时间
		}
		else
		{
			get_time(); //计时
			seg_disp2();//数码管动态显示			
		}
	}
}


