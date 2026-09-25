/*****************************************/
/*********     ––’ﬂ »Œ     ********** ****/
/*********     2014.4.1     ********** ***/
/*****************************************/

#include<reg52.h>
#include"1602.h"
#include"pwm.h"
#include"xunji.h"

sbit POWER=P3^5;

void main (void)
{ 
	Timer0Init();
	Timer1Init();
	LCD_Init();
	LCD_Str(5, 0, "1421h");
	LCD_Str(0,0,"***smart car***");	
	LCD_Str(0,1,"*DANG HONG MIN*");	
	while(POWER) LCD_Flash(500);;
	DelayMs(50);
	LCD_Write_Command(0x01);
	LCD_Str(0,0,"***smart car***");	
 	while(1)
	{
		xunji(800);
	}
  	
}