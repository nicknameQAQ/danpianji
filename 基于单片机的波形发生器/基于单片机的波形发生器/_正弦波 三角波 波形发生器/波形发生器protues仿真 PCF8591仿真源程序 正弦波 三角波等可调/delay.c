#include "delay.h"
#include <intrins.h>
void delay()    //ÑÓÊ±4-5¸öÎ¢Ãë  
{;;}  
  
void delay_1ms(uint z)  
{  
    uint x,y;  
    for(x=z;x>0;x--)  
        for(y=100;y>0;y--)  
            ;  
}  
  