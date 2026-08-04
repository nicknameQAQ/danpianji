#include "DSP281x_Device.h"
#include "DSP281x_Examples.h" 
#define ADC_usDELAY  8000L//定义延迟时间1：8000us
#define ADC_usDELAY2  20L //定义延迟时间2：20us


//ADC模块的通用初始化函数
void InitAdc(void)
{
	extern void DSP28x_usDelay(Uint32 Count);//在DSP281x_usDelay.asm定义的函数
        AdcRegs.ADCTRL1.bit.RESET = 1;//复位ADC模块
	asm(" RPT #10 || NOP");		//必须等待约12个时钟周期，使ADC复位生效
  
	AdcRegs.ADCTRL3.bit.ADCBGRFDN = 0x3;	//对带隙参考源电路进行上电 
	DELAY_US(ADC_usDELAY);          //对ADC模块其余部分上电前的延时
	AdcRegs.ADCTRL3.bit.ADCPWDN = 1;	//对ADC模块的其余部分进行上电
	DELAY_US(ADC_usDELAY2);         // ADC上电完成后的延时
}



