#ifndef DA0832_h
#define DA0832_h
#include<reg52.h>
#define DatPort P3
sbit CS1=P1^3;
sbit WR1=P1^4;
extern void DacInit(void);
extern void WriteToDac0832(unsigned char Dat); 
#endif