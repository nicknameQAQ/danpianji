#include "STC12C5A60S2.h"
#include <intrins.h>
/*************************************************
Name:       IAPIdle
Function:   Disable IAP Function
Parameter:  None
Return:     None
Note:       Make MCU in a Safe State
*************************************************/
void IAPIdle(void)
{
	IAP_CONTR = 0x00;
	IAP_CMD   = 0x00;
	IAP_TRIG  = 0x00;
	IAP_ADDRH = 0x80;
	IAP_ADDRL = 0x00;
}

/*************************************************
Name:       IAPReadByte
Function:   Read a Byte from IAP Area
Parameter:  addr
Return:     dat
Note:       
*************************************************/
unsigned char IAPReadByte(unsigned int addr)
{
	unsigned char dat;
	IAP_CONTR = 0x80;
	IAP_CMD   = 1;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr>>8;
	IAP_TRIG  = 0x5a;
	IAP_TRIG  = 0xa5;
	_nop_();
	dat = IAP_DATA;
	IAPIdle();
	return dat;
}

/*************************************************
Name:       IAPWriteByte
Function:   Write a Byte to IAP Area
Parameter:  addr, dat
Return:     None
Note:       
*************************************************/
void IAPWriteByte(unsigned int addr, unsigned char dat)
{
	IAP_CONTR = 0x80;
	IAP_CMD   = 2;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr>>8;
	IAP_DATA  = dat;
	IAP_TRIG  = 0x5a;
	IAP_TRIG  = 0xa5;
	_nop_();
	IAPIdle();
}

/*************************************************
Name:       IAPEraseSector
Function:   Erase a Sector Area
Parameter:  addr
Return:     None
Note:       
*************************************************/
void IAPEraseSector(unsigned int addr)
{
	IAP_CONTR = 0x80;
	IAP_CMD   = 3;
	IAP_ADDRL = addr;
	IAP_ADDRH = addr>>8;
	IAP_TRIG  = 0x5a;
	IAP_TRIG  = 0xa5;
	_nop_();
	IAPIdle();
}

/*************************************************
Name:       IAPWriteData
Function:   Write Data to IAP Area
Parameter:  addr, dat
Return:     None
Note:       
*************************************************/
void IAPWriteData(unsigned int addr, unsigned char length, unsigned char *p)
{
    unsigned char i;
	IAPEraseSector(addr);
    for(i = 0; i < length; i++)
    {
        IAPWriteByte((addr+i), *(p + i));
    }
}

/*************************************************
Name:       IAPReadData
Function:   Read Data from IAP Area
Parameter:  addr, dat
Return:     None
Note:       
*************************************************/
void IAPReadData(unsigned int addr, unsigned char length, unsigned char *p)
{
    unsigned char i;
    for(i = 0; i < length; i++)
    {
        *(p + i) = IAPReadByte(addr+i);
    }
}