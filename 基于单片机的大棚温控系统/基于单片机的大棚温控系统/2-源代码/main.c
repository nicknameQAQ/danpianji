
#include "reg51.h"
#include "LCD.h"
#include "DS18B20.h"
#include "KEY.h"
void main()
{
	P1=0X0F;
	LCD_Init();  //lcd1602初始化
	sendChangeCmd();
	write_string(0,0,"Temp:         ");
	write_string(0,1,"Set :			");	
	while(1)
	{
		 display(getTmpValue());//读取温度
		 sendChangeCmd();		//发送读取温度指令
		 Key_borad();		//按键设置函数
		 if(Tp>tmp)  //得到温度值 大于设置值
		 	BEEP(1);
		 else
		 {_nop_();}

	}
}

