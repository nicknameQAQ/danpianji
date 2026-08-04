#include<reg52.h>
#define uchar unsigned char
#define uint  unsigned int



//**************∫¡√Î*******************
void delayxms(uint ms)					   //—” ± x  ms
{
    uint a,j;

    for(a=0;a<ms;a++) 
	  for(j=0;j<110;j++);

}
///**********************—” ±∫Ø ˝**************************/
//void NRFDelay(uint t)
//{
//   uint x,y;
//   for(x=t;x>0;x--)
//    for(y=110;y>0;y--);
//}
//**************Œ¢√Î*******************
void delayxus(uint us)					   //—” ± x  us
{
	while(us--);

}