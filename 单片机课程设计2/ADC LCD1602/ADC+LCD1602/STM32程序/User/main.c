#include "stm32f10x.h"
#include "bsp-lcd1602.h"
#include "delay.h"
#include "sys.h"
#include "adc.h"

int main(void)
{
int a,b,c,d;
	float temp;
	
	delay_init();	    	 //延时函数初始化	  	
	LCD1602_Init();
  ADC1_GPIO_Config();
  ADC_Config();  
	LCD1602_ShowStr(2,0,"adcvalue=0.0V",13);
	
	while(1)
	{
		b=ADC_GetConversionValue(ADC1);
	temp=(float)b*(3.4/4096);
	a=temp/1;
	c=temp*10;
	d=c%10;
	LCD_ShowNum(11,0,a);
	LCD_ShowNum(13,0,d);
	}
}



