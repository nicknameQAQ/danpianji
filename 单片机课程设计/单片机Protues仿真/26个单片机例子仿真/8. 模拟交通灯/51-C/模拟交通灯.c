/*
*模拟交通灯*
*/
#include <reg51.h>
typedef unsigned char uint8;
typedef unsigned int uint16;

void delay(uint16 x)
{
	uint16 i,j;
	for(i = x; i > 0; i --)
		for(j = 114; j > 0; j --);		
}

#define    RED_EW_ON()	P0 &= ~0x01			//东西方向指示灯开
#define YELLOW_EW_ON()	P0 &= ~0x02
#define  GREEN_EW_ON()	P0 &= ~0x04

#define    RED_EW_OFF()	P0 |=  0x01			//东西方向指示灯关
#define YELLOW_EW_OFF()	P0 |=  0x02
#define  GREEN_EW_OFF()	P0 |=  0x04

#define  RED_SN_ON()	P0 &= ~0x08			//南北方向指示灯开  0000100 1111011
#define YELLOW_SN_ON()	P0 &= ~0x10
#define GREEN_SN_ON()	P0 &= ~0x20

#define  RED_SN_OFF()	P0 |=  0x08			//南北方向指示灯关
#define YELLOW_SN_OFF()	P0 |=  0x10
#define GREEN_SN_OFF()	P0 |=  0x20		

#define YELLOW_EW_BLINK() P0 ^= 0x02			//东西方向黄灯闪烁
#define YELLOW_SN_BLINK() P0 ^= 0x10			//南北方向黄灯闪烁

uint8 Flash_Count = 0, Operation_Type = 1;			//闪烁次数，操作类型变量

void Traffic_Light()								//交通灯切换子程序
{
	switch (Operation_Type)
	{
		case 1://东西方向绿灯与南北方向红灯亮
				RED_EW_OFF(); YELLOW_EW_OFF(); GREEN_EW_ON();
			    RED_SN_ON(); YELLOW_SN_OFF(); GREEN_SN_OFF();
				delay(2000);								//延时
				Operation_Type = 2;							//下一个操作
				break;

		case 2://东西方向黄灯开始闪烁，绿灯关闭
				delay(120);
				YELLOW_EW_BLINK();
				GREEN_EW_OFF();
				//闪烁5次
				if( ++Flash_Count != 10) return;
				Flash_Count = 0;
				Operation_Type = 3;							//下一个操作
				break;

		case 3://东西方向红灯与南北方向绿灯亮
				RED_EW_ON();  YELLOW_EW_OFF(); GREEN_EW_OFF();
				RED_SN_OFF(); YELLOW_SN_OFF(); GREEN_SN_ON();
			   //南北方向绿灯亮若干秒后切换
			   delay(2000);
			   Operation_Type = 4;							//下一个操作
			   break;

		case 4://南北方向红灯开始闪烁
			   delay(120);
			   YELLOW_SN_BLINK();
			   GREEN_SN_OFF();
			   //闪烁5次
			   if( ++ Flash_Count != 10) return;
			   Flash_Count = 0;
			   Operation_Type = 1;							//回到第一种操作
	}		   
}

//主程序
int main()
{
	while(1) Traffic_Light();
}