#include <STC8.h>
#include "LCD1602.h"

void main(){
	#include "Register_configuration.h"
	LCD1602_Write_Reg(0x01);//清屏
	LCD1602_Write_Reg(0x0C);//显示控制
	LCD1602_Write_Reg(0x38);//功能设置
	LCD1602_Write_Reg(0x84);
	LCD1602_Write_Data('J');
  LCD1602_Write_Data('i');
  LCD1602_Write_Data('S');
	LCD1602_Write_Data('u');
	LCD1602_Write_Data('a');
	LCD1602_Write_Data('n');
	LCD1602_Write_Data('!');
	LCD1602_Write_Reg(0xC4);
	LCD1602_Write_Data('q');
	LCD1602_Write_Data('q');
	LCD1602_Write_Data('1');
	LCD1602_Write_Data('1');
	LCD1602_Write_Data('3');
	LCD1602_Write_Data('8');
	LCD1602_Write_Data('5');
	LCD1602_Write_Data('0');
	LCD1602_Write_Data('5');
	LCD1602_Write_Data('0');
	LCD1602_Write_Data('7');
	LCD1602_Write_Data('6');
	for(;;);
}