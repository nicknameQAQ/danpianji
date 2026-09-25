
#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File

//系统初始化函数
void InitSystem(void)
{
   	EALLOW; //在向EALLOW 保护型寄存器中写数前，需要执行该指令来解除保护
   	SysCtrlRegs.WDCR= 0x00E8;		// 设置看门狗模块
   		// 如果赋以0x00E8，则禁止看门狗模块，时钟预定标系数 = 1
   		
   	SysCtrlRegs.PLLCR.bit.DIV = 10;	// 将CPU的PLL倍频系数设为4
    
   	SysCtrlRegs.HISPCP.all = 0x1; //将高速时钟的预定标器设置成除以2模式
	SysCtrlRegs.LOSPCP.all = 0x2;/*低速时钟的预定标器设置成除以4*/
   	  // 根据需要使能各外设模块的时钟输入  
   	SysCtrlRegs.PCLKCR.bit.EVAENCLK=1;
   	SysCtrlRegs.PCLKCR.bit.EVBENCLK=1;
   	SysCtrlRegs.PCLKCR.bit.SCIAENCLK=1;
   	SysCtrlRegs.PCLKCR.bit.SCIBENCLK=0;
   	SysCtrlRegs.PCLKCR.bit.MCBSPENCLK=0;
   	SysCtrlRegs.PCLKCR.bit.SPIENCLK=0;
   	SysCtrlRegs.PCLKCR.bit.ECANENCLK=0;
   	SysCtrlRegs.PCLKCR.bit.ADCENCLK=1;
   	EDIS; // 执行该指令后，任何对EALLOW保护型寄存器的写操作都将被禁止 
}

