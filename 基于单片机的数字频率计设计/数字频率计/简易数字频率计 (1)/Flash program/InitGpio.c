#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
void InitGpio(void)
{

// Set GPIO A port pins,AL(Bits 7:0)(input)-AH(Bits 15:8) (output) 8bits
// Input Qualifier =0, none

     EALLOW;
     GpioMuxRegs.GPAMUX.all=0x1340;     
     GpioMuxRegs.GPADIR.all=0xE8FF;    	
     GpioMuxRegs.GPAQUAL.all=0x0000;	// Input qualifier disabled

// Set GPIO B port pins, configured as EVB signals
// Input Qualifier =0, none
// Set bits to 1 to configure peripherals signals on the pins
     GpioMuxRegs.GPBMUX.all=0x1140; 
    GpioMuxRegs.GPBDIR.all=0xE8FF; 
     GpioMuxRegs.GPBQUAL.all=0x0000;
     	// Input qualifier disabled
     GpioMuxRegs.GPDMUX.all=0x0;
     GpioMuxRegs.GPFMUX.all=0x0;
     GpioMuxRegs.GPEMUX.all=0x0;
     GpioMuxRegs.GPGMUX.all=0x0;
     GpioMuxRegs.GPDDIR.all=0x0; 
     GpioMuxRegs.GPEDIR.all=0x0; 
     GpioMuxRegs.GPFDIR.all=0x0; 
     GpioMuxRegs.GPGDIR.all=0x0; 
     GpioMuxRegs.GPDQUAL.all=0x0000;
     GpioMuxRegs.GPEQUAL.all=0x0000;
     GpioMuxRegs.GPFMUX.bit.SCIRXDA_GPIOF5 = 1; //外设功能SCI-RX引脚
     GpioMuxRegs.GPFMUX.bit.SCITXDA_GPIOF4 = 1; //外设功能SCI-TX引脚

     EDIS;
     


}
