/**************************************************************** 

*****************************************************************/

#ifndef _DS18B20_H_
#define _DS18B20_H_
/*****************************************/
extern void Reset();
extern void WriteChar(uchar dat);
extern uint ReadChar();
extern uint ReadTemperature();
/*****************************************/
#endif