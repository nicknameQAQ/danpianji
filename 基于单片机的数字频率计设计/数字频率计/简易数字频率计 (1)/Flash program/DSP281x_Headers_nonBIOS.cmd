/*
//###########################################################################
//
// FILE:    DSP281x_Headers_nonBIOS.cmd
//
// TITLE:   DSP281x Peripheral registers linker command file 
//
// DESCRIPTION: 
// 
//          This file is for use in Non-BIOS applications.
//
//          Linker command file to place the peripheral structures 
//          used within the DSP28 headerfiles into the correct memory
//          mapped locations.
//
//          This version of the file includes the PieVectorTable structure.
//          For BIOS applications, please use the DSP281x_Headers_BIOS.cmd file
//          which does not include the PieVectorTable structure.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//      | 05 Mar 2003 | D.A. | Original based on DSP28 v0.58
//  1.00| 11 Sep 2003 | L.H. | Integrated into DSP28 header files
//      |             |      | Added missing eCAN file sections
// -----|-------------|------|-----------------------------------------------
//###########################################################################
*/

MEMORY
{
 PAGE 0:    /* Program Memory */
    /* For this example, H0 is split between PAGE 0 and PAGE 1 */  
   /* BEGIN is used for the "boot to HO" bootloader mode      */
   /* RESET is loaded with the reset vector only if           */
   /* the boot is from XINTF Zone 7.  Otherwise reset vector  */
   /* is fetched from boot ROM. See .reset section below      */
   
   RAMM0      : origin = 0x000000, length = 0x000400
   BEGIN      : origin = 0x3F8000, length = 0x000002             
   PRAMH0     : origin = 0x3F8002, length = 0x000FFE
   RESET      : origin = 0x3FFFC0, length = 0x000002       

 PAGE 1:    /* Data Memory */
   /* For this example, H0 is split between PAGE 0 and PAGE 1 */

   RAMM1    : origin = 0x000400, length = 0x000400
   DRAMH0   : origin = 0x3f9000, length = 0x001000  
 
 
   DEV_EMU     : origin = 0x000880, length = 0x000180     /* device emulation registers */
   PIE_VECT    : origin = 0x000D00, length = 0x000100     /* PIE Vector Table */
   FLASH_REGS  : origin = 0x000A80, length = 0x000060     /* FLASH registers */
   CSM         : origin = 0x000AE0, length = 0x000010     /* code security module registers */
   XINTF       : origin = 0x000B20, length = 0x000020     /* external interface registers */
   CPU_TIMER0  : origin = 0x000C00, length = 0x000008     /* CPU Timer0 registers (CPU Timer1 and Timer2 are reserved for BIOS)*/
   PIE_CTRL    : origin = 0x000CE0, length = 0x000020     /* PIE control registers */
   ECANA       : origin = 0x006000, length = 0x000040     /* eCAN control and status registers */ 
   ECANA_LAM   : origin = 0x006040, length = 0x000040     /* eCAN local acceptance masks */
   ECANA_MOTS  : origin = 0x006080, length = 0x000040     /* eCAN message object time stamps */
   ECANA_MOTO  : origin = 0x0060C0, length = 0x000040     /* eCAN object time-out registers */
   ECANA_MBOX  : origin = 0x006100, length = 0x000100     /* eCAN mailboxes */
   SYSTEM      : origin = 0x007010, length = 0x000020     /* System control registers */
   SPIA        : origin = 0x007040, length = 0x000010     /* SPI registers */
   SCIA        : origin = 0x007050, length = 0x000010     /* SCI-A registers */
   XINTRUPT    : origin = 0x007070, length = 0x000010     /* external interrupt registers */
   GPIOMUX     : origin = 0x0070C0, length = 0x000020     /* GPIO mux registers */
   GPIODAT     : origin = 0x0070E0, length = 0x000020     /* GPIO data registers */
   ADC         : origin = 0x007100, length = 0x000020     /* ADC registers */
   EVA         : origin = 0x007400, length = 0x000040     /* Event Manager A registers */
   EVB         : origin = 0x007500, length = 0x000040     /* Event Manager B registers */
   SCIB        : origin = 0x007750, length = 0x000010     /* SCI-B registers */
   MCBSPA      : origin = 0x007800, length = 0x000040     /* McBSP registers */
   CSM_PWL     : origin = 0x3F7FF8, length = 0x000008     /* Part of FLASHA.  CSM password locations. */
}

 
SECTIONS
{
   /* Setup for "boot to H0" mode: 
      The codestart section (found in DSP28_CodeStartBranch.asm)
      re-directs execution to the start of user code.  
      Place this section at the start of H0  */

   codestart        : > BEGIN,       PAGE = 0
   ramfuncs         : > PRAMH0       PAGE = 0  
   .text            : > PRAMH0,      PAGE = 0
   .cinit           : > PRAMH0,      PAGE = 0
   .pinit           : > PRAMH0,      PAGE = 0
   .switch          : > RAMM0,       PAGE = 0
   .reset           : > RESET,       PAGE = 0, TYPE = DSECT /* not used, */
   
   .stack           : > RAMM1,       PAGE = 1
   .ebss            : > DRAMH0,      PAGE = 1
   .econst          : > DRAMH0,      PAGE = 1      
   .esysmem         : > DRAMH0,      PAGE = 1


   PieVectTableFile : > PIE_VECT,   PAGE = 1

/*** Peripheral Frame 0 Register Structures ***/
   DevEmuRegsFile    : > DEV_EMU,     PAGE = 1
   FlashRegsFile     : > FLASH_REGS,  PAGE = 1
   CsmRegsFile       : > CSM,         PAGE = 1
   XintfRegsFile     : > XINTF,       PAGE = 1
   CpuTimer0RegsFile : > CPU_TIMER0,  PAGE = 1  
   PieCtrlRegsFile   : > PIE_CTRL,    PAGE = 1      

/*** Peripheral Frame 1 Register Structures ***/
   SysCtrlRegsFile   : > SYSTEM,      PAGE = 1
   SpiaRegsFile      : > SPIA,        PAGE = 1
   SciaRegsFile      : > SCIA,        PAGE = 1
   XIntruptRegsFile  : > XINTRUPT,    PAGE = 1
   GpioMuxRegsFile   : > GPIOMUX,     PAGE = 1
   GpioDataRegsFile  : > GPIODAT      PAGE = 1
   AdcRegsFile       : > ADC,         PAGE = 1
   EvaRegsFile       : > EVA,         PAGE = 1
   EvbRegsFile       : > EVB,         PAGE = 1
   ScibRegsFile      : > SCIB,        PAGE = 1
   McbspaRegsFile    : > MCBSPA,      PAGE = 1

/*** Peripheral Frame 2 Register Structures ***/
   ECanaRegsFile     : > ECANA        PAGE = 1
   ECanaLAMRegsFile  : > ECANA_LAM    PAGE = 1   
   ECanaMboxesFile   : > ECANA_MBOX   PAGE = 1
   ECanaMOTSRegsFile : > ECANA_MOTS   PAGE = 1
   ECanaMOTORegsFile : > ECANA_MOTO   PAGE = 1

/*** Code Security Module Register Structures ***/
   CsmPwlFile        : > CSM_PWL,     PAGE = 1
}


/******************* end of file ************************/