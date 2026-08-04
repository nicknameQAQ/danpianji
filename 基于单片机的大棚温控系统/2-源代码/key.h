
void BEEP(uchar t);

sbit KEY0=P1^0; //按键加
sbit KEY1=P1^1; //按键减
sbit SPK=P1^7;  //蜂鸣器

int tmp=10;   // 预设温度

void Key_borad()		//按键设置函数
{
	if(KEY0==0)	   //如果按下加按键
	{
		while(KEY0==0); //释放
	    tmp++;		  //预设温度加一
	}
	if(KEY1==0)	   //如果按下减按键
	{
		while(KEY1==0); //释放
	    tmp--;		  //预设温度减一
		if(tmp<0)
			tmp=0;
	}
	write_com(0xc0+7);

	write_dat(tmp%100/10+0x30);
	write_dat(tmp%10+0x30);
	write_dat('.');
	write_dat('0');
	write_dat('0');
	write_dat(0xDF);	write_dat('C');//	℃
} 

void BEEP(uchar t)
{
 	uchar i;
	for(i=0;i<100;i++)
	{
	 	SPK = ~SPK;
		delayMs(t);
	}
	SPK = 0;
}
