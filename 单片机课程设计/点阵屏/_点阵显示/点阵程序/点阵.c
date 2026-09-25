#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit KEY   = P3^5;

unsigned int cnt,m,stage,sec=1000;

unsigned char code image[][8] = {  
		{0xFF,0xE7,0xDB,0xDB,0xDB,0xDB,0xE7,0xFF},     //0
		{0xFF,0xE7,0xEF,0xEF,0xEF,0xEF,0xC3,0xFF},     //1
    {0xFF,0xE3,0xDF,0xDF,0xE7,0xFB,0xC3,0xFF},     //2
		{0xE3,0xDF,0xDF,0xE7,0xDF,0xDF,0xE3,0xFF},     //3
		{0xFF,0xEF,0xE7,0xEB,0xC3,0xEF,0xEF,0xFF},	  //4
		{0xFB,0xC3,0xFB,0xE3,0xDF,0xDF,0xE3,0xFF},		//5
		{0xE7,0xFB,0xFB,0xE3,0xDB,0xDB,0xE7,0xFF},    //6
		{0xFF,0xC3,0xDF,0xDF,0xDF,0xDF,0xDF,0xFF},    //7
		{0xE7,0xDB,0xDB,0xE7,0xDB,0xDB,0xE7,0xFF},    //8
		{0xE7,0xDB,0xDB,0xC7,0xDF,0xDF,0xE7,0xFF}     //9
};
unsigned char code image1[][8] = {              //心跳
		{0xFF,0x99,0x00,0x00,0x00,0x81,0xC3,0xE7},  //大心
		{0xFF,0xFF,0xDB,0x81,0x81,0xC3,0xE7,0xFF},  //中心
		{0xFF,0xFF,0xFF,0xDB,0xC3,0xE7,0xFF,0xFF},	 //小心
};

unsigned char code image3[][8] = {              //小坦克
	{0xFE,0xFF,0xE3,0xE3,0xF7,0xFF,0xFF,0xFF},
{0xFE,0xFF,0xFF,0xE3,0xE3,0xF7,0xFF,0xFF},
{0xFE,0xFF,0xFF,0xFF,0xE3,0xE3,0xF7,0xFF},
{0xFE,0xFF,0xFF,0xFF,0xF3,0xE3,0xF3,0xFF},
{0xFE,0xFF,0xFF,0xFF,0xE7,0xC7,0xE7,0xFF},
{0xFE,0xFF,0xFF,0xFF,0xCF,0x8F,0xCF,0xFF},
{0xFE,0xFF,0xFF,0xFF,0xDF,0x8F,0x8F,0xFF},
{0xFE,0xFF,0xFF,0xDF,0x8F,0x8F,0xFF,0xFF},
{0xFE,0xFF,0xDF,0x8F,0x8F,0xFF,0xFF,0xFF},
{0xFE,0xDF,0x8F,0x8F,0xFF,0xFF,0xFF,0xFF},
};

unsigned char code image2[] = {                  //0-->9平移
		0xFF,0xE7,0xDB,0x5A,0x5A,0xDB,0xE7,0xFF,0xFF,0xE7,0xEF,0x6E,0x6E,0xEF,0xC3,0xFF,
		0xFF,0xE3,0xDF,0x5E,0x66,0xFB,0xC3,0xFF,0xE3,0xDF,0xDF,0x66,0x5E,0xDF,0xE3,0xFF,
		0xFF,0xEF,0xE7,0x6A,0x42,0xEF,0xEF,0xFF,0xFB,0xC3,0xFB,0x62,0x5E,0xDF,0xE3,0xFF,
		0xE7,0xFB,0xFB,0x62,0x5A,0xDB,0xE7,0xFF,0xFF,0xC3,0xDF,0x5E,0x5E,0xDF,0xDF,0xFF,
		0xE7,0xDB,0xDB,0x66,0x5A,0xDB,0xE7,0xFF,0xE7,0xDB,0xDB,0x46,0x5E,0xDF,0xE7,0xFF,
};



void Delay10ms()		
{
	unsigned char i, j;

	i = 20;
	j = 113;
	do
	{
		while (--j);
	} while (--i);
}


void main()
{
    EA = 1;       //使能总中断
    ADDR3 = 0;
    TMOD = 0x01;  //设置T0为模式1
    TH0  = 0xFC;  //为T0赋初值0xFC67，定时1ms
    TL0  = 0x67;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
    while (1)
		{
			if(stage>3)stage=0;
			if(KEY==0)  
		{
			Delay10ms();
			if(KEY==1)stage++;
			
		}
			if(cnt >= sec)
			{
				cnt = 0;
				m++;
				
			}
		}
}
/* 定时器0中断服务函数 */
void InterruptTimer0() interrupt 1
{
    static unsigned char i = 0;  //动态扫描的索引
	  static unsigned char tmr = 0;  //250ms软件定时器
    static unsigned char index = 0;  //图片刷新索引

    TH0 = 0xFC;  //重新加载初值
    TL0 = 0x67;
    P0 = 0xFF;   //显示消隐
		cnt++;
	/***********************模式一：数字闪出***********************************/
		if(stage==0)                    
		{
			sec = 1000;
			if(m>=10)m=0;
			switch (i)
			{
					case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image[m][0]; break;
					case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image[m][1]; break;
					case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image[m][2]; break;
					case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image[m][3]; break;
					case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image[m][4]; break;
					case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image[m][5]; break;
					case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image[m][6]; break;
					case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image[m][7]; break;
					default: break;
			}
		}
	/**********************模式二：动态爱心************************************/
		else if(stage==1)
		{
			sec = 150;
			if(m>=3)m=0;
			switch (i)
			{
					case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image1[m][0]; break;
					case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image1[m][1]; break;
					case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image1[m][2]; break;
					case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image1[m][3]; break;
					case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image1[m][4]; break;
					case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image1[m][5]; break;
					case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image1[m][6]; break;
					case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image1[m][7]; break;
					default: break;
			}
		}
	/**********************模式三：坦克走动**************************************/
		else if(stage==2)
		{
			sec = 800;
			if(m>=10)m=0;
			switch (i)
			{
					case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image3[m][0]; break;
					case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image3[m][1]; break;
					case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image3[m][2]; break;
					case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image3[m][3]; break;
					case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image3[m][4]; break;
					case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image3[m][5]; break;
					case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image3[m][6]; break;
					case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image3[m][7]; break;
					default: break;
			}
		}
	/**********************模式四：数字平移****************************************/
		else if(stage==3)
		{
			switch (i)
				{
						case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image2[index+0]; break;
						case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image2[index+1]; break;
						case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image2[index+2]; break;
						case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image2[index+3]; break;
						case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image2[index+4]; break;
						case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image2[index+5]; break;
						case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image2[index+6]; break;
						case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image2[index+7]; break;
						default: break;
				}
			//以下代码完成每250ms改变一帧图像
			tmr++;
			if (tmr >= 250)  //达到250ms时改变一次图片索引
			{
					tmr = 0;
					index++;
					if (index >= 72)  //图片索引达到32后归零
					{
							index = 0;
					}
			}
		}

}
