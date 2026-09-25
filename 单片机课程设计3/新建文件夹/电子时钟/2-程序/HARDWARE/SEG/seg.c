#include "seg.h" 
#include "delay.h"

int8_t f=0;//设置数字修改的数码管位选标志

u16 LED[10] = {0x00C0,0x00F9,0x00A4,0x00B0,0x0099,0x0092,0x0082,0x00F8,0x0080,0x0090};//0—9的字形码

/*****************************************************************
*数码管引脚初始化
******************************************************************/
void gpio_init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure; 
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD,ENABLE); 				//使能GPIO时钟
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //选中引脚PE0、1、2、3
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 																		//普通输出模式
	 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 																	//推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 															//输出速度100MHz
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 																//无上下拉
	 GPIO_Init(GPIOC, &GPIO_InitStructure); 																					//初始化                                     
	 
	 GPIO_SetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);//使GPIOE0、1、2、3置1，输出高电平
	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7| GPIO_Pin_8;
	 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 GPIO_Init(GPIOD, &GPIO_InitStructure); 																					//初始化                                     
	 GPIO_ResetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);//
	 GPIO_SetBits(GPIOD, GPIO_Pin_8); 
}

/*****************************************************************
*数码管动态显示，共有两组函数，分别用于时间修改和正常计时的数码管现实
******************************************************************/
void seg_disp1(u8 x)
{
	u8 i=0;
	for(;i<8;i++)//位选，选择点亮的数码管
	{
		if(i==x&&i<6)
		{
			GPIOC->ODR = 0x00F7;//输送数字到数码管显示
			GPIOD->ODR = address[i];//选中数码管
			delay_us(de);
			GPIOD->ODR = 0x0100;//消隐
		}
		else
		{
			GPIOC->ODR = LED[date[i]];//输送数字到数码管显示
			GPIOD->ODR = address[i];//选中数码管
			delay_us(de);
			GPIOD->ODR = 0x0100;//消隐			
		}
	}		
}
void seg_disp2(void)
{
	u8 i=0;
	for(;i<8;i++)//位选，选择点亮的数码管
	{
		GPIOC->ODR = LED[date[i]];//输送数字到数码管显示
		GPIOD->ODR = address[i];//选中数码管
		delay_us(de);
		GPIOD->ODR = 0x0100;//消隐			
	}		
}
/*****************************************************************
*使用矩阵键盘进行时间修改
******************************************************************/
void seg_chag(void)
{
	u8 i=0;
	i=key_Scan(0);
	if(i>10&&exit_flag==1)//判断是否为规定键值进产生的中断
	{exit_flag=0;seg_disp2();}
	else//当为规定键值产生的中断时，进行时间修改设置
	{
		seg_disp1(f);
		while(1)
		{
			i=key_Scan(1);//扫描按键，并储存按键值
			if(i!=0)		 //判断按键是否按下
			{
				if(i<10)//当按键值小于10，判断范围是否合理后按键值显示
				{	
					if((f==0&&i>2)||(f==1&&date[0]==2&&i>3)||((f==2||f==4)&&i>5))
					{}
					else
					{date[f]=i;f++;}
				}								
				else if(i==11)//设置按键值为11时，使显示为0
				{	date[f]=0;f++;}					
				else if(i==10)//设置后退功能，并置_显示
				{ f--;
					if(f<0)f=0;
				}
				else//存储输入值，并退出时间设置，若相应值超范围，置零  																	
				{	f=0;exit_flag=0;
					h=date[0]*10+date[1];
					m=date[2]*10+date[3];
					s=date[4]*10+date[5];
					break;
				}
				if(f>5)f=6;//超过所规定的设置位后禁用时间修改									
			}
			seg_disp1(f);//实时显示修改值
		}			
	}
}
