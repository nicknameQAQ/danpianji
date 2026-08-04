
	   *********************************************************************/
#include<AT89X52.h>				   //将AT89X52.h头文件包含到主程序
#include<intrins.h>				   //将intrins.h头文件包含到主程序（调用其中的_nop_()空操作函数延时）
#define uint unsigned int 	  	   //变量类型宏定义，用uint表示无符号整形（16位）
#define uchar unsigned char		   //变量类型宏定义，用uchar表示无符号字符型（8位）

uchar max=0x00,min=0x00;		   //max是上限报警温度，min是下限报警温度

bit   s=0;						   //s是调整上下限温度时温度闪烁的标志位，s=0不显示200ms，s=1显示1s左右
bit   s1=0;						   //s1标志位用于上下限查看时的显示
void display1(uint z);		 	   //声明display1（）函数（display.h头文件中的函数，ds18b20.h要用应先声明）
#include"ds18b20.h"				   //将ds18b20.h头文件包含到主程序
#include"keyscan.h"				   //将keyscan.h头文件包含到主程序
#include"display.h"				   //将display.h头文件包含到主程序

/***********************主函数************************/
void main()
{
	 beer=1;					   //关闭蜂鸣器
	 led=1;						   //关闭LED灯
	 timer1_init(0);			   //初始化定时器1（未启动定时器1）
	 get_temperature(1);	 	   //首次启动DS18B20获取温度（DS18B20上点后自动将EEPROM中的上下限温度复制到TH和TL寄存器）
	 while(1)					   //主循环
	 {	 
		 keyscan();				   //按键扫面函数
		 get_temperature(0);	   //获取温度函数
		 //keyscan();				   //按键扫面函数
		 display(temp,temp_d*0.625);//显示函数
		 alarm();				   	//报警函数
		 //keyscan();	 		 	   //按键扫面函数
	 }
}   





