#include<reg51.h>
//定义一个数组,使数码管显示的数字和数组元素的下标相等
unsigned char code table[]={0x3f,0x06,0x5b,0x4f,0x66,
                            0x6d,0x7d,0x07,0x7f,0x6f};
sbit shuiman=P1^0;//水满
sbit sw5=P1^1;//水位5
sbit sw4=P1^2;//水位4
sbit sw3=P1^3;//水位3
sbit sw2=P1^4;//水位2
sbit sw1=P1^5;//水位1
sbit shougong=P1^6;//手工上水
sbit dianji=P1^7;//电机控制位
sbit state=P3^7;//电机工作指示
/*延时程序*/
void delay02s(void)
{
  unsigned char i,j,k;
  for(i=100;i>0;i--)
  for(j=100;j>0;j--)
  for(k=248;k>0;k--);
}
main()
{
	P0=0;
	while(1)
		{  
			if(shuiman==0&&sw5==1&&sw4==1&&sw3==1&&sw2==1&&sw1==1)//当唯一的闭合一个传感器单元:水位6时发生
			{
				dianji=1;//关电机
				state=1;//电机工作指示灯熄灭
            P0=table[6];//显示水位深度:6**** 已满
				delay02s();//延时一段时间让数码管显示给人以提示：水已满
			}
				//水位5
			if(shuiman==1&&sw5==0&&sw4==1&&sw3==1&&sw2==1&&sw1==1)//当唯一的闭合一个传感器单元:水位5时发生
			{
				P0=table[5];//显示水位深度:5
			}
			//水位4
			if(shuiman==1&&sw5==1&&sw4==0&&sw3==1&&sw2==1&&sw1==1)//当唯一的闭合一个传感器单元:水位4时发生
			{
				P0=table[4];//显示水位深度:4
			}
			//水位3
			if(shuiman==1&&sw5==1&&sw4==1&&sw3==0&&sw2==1&&sw1==1)//当唯一的闭合一个传感器单元:水位3时发生
			{
			  	P0=table[3];//显示水位深度:5
			}
			//水位2
	      if(shuiman==1&&sw5==1&&sw4==1&&sw3==1&&sw2==0&&sw1==1)//当唯一的闭合一个传感器单元:水位2时发生
			{
				P0=table[2];//显示水位深度:2
			}
			//水位低
			if(shuiman==1&&sw5==1&&sw4==1&&sw3==1&&sw2==1&&sw1==0)//当唯一的闭合一个传感器单元:水位1时发生
			{
				dianji=0;//开电机
				state=0;//电机工作指示灯打开
			P0=table[1];//显示水位深度:1
			}
			//手工上水
		    if(shougong==0)//当按手工上水按钮时发生
			{  
		      dianji=0;//开电机
				state=0;//电机工作指示灯打开
				P0=table[0];//显示0表示手工上水已有反应
				delay02s();//延时一段时间让数码管显示给人以提示：已开始手工上水
			}
		
		}
}
