#include "led.h"
#include "exti.h"
#include "delay.h"

int main(void)
{		 
	
	LED_Init();	 //初始化与LED连接的硬件接口
	EXTIX_Init();//外部中断初始化
    while(1);
}
