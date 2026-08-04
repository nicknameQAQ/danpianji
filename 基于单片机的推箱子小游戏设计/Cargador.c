#include <reg51.h>
#include "data.h"
#include "order.h"
#include "lcdinit.c"
#include "function.c"

void int1(void) interrupt 2
{
	keyboard();
}

int main(){
	uchar i=1;
	error0=0;
	error1=0;
	error2=0;
	delay(50);
	EA=1;
	IT1=1;
	EX1=1;
//	wirte_bg();
	/*液晶初始化函数(文本区首地址D1,文本区首地址D2, 文本区宽度,  图形区首地址D1, 图形区首地址D2, 图形区宽度,   光标形状,  显示方式,  显示开关)*/
	lcd_init(0x00,0x00,0x14,0x50,0x01,0x14,0x00,MOD_XOR,0x0c);
//	wirte_bg();
	set_cgram();
	wirte_cgrom();
	cls();
	start();
	cls();
	pushbox();
	guan();
	while(1){
	}
	return(0);
}