sbit CS   = P1^2;
sbit Clk = P1^0;
sbit DATI = P1^1;
sbit DATO = P1^1;

/*******************************定义全局变量********************************/

unsigned int dat = 0x00;      //AD值
unsigned char CH;       //通道变量
unsigned char data dis[] = {0x00, 0x00,0x00,0x00};       //显示数值



unsigned char adc0832(unsigned char CH)
{
	unsigned char i,test,adval;
	adval = 0x00;
	test = 0x00;
	Clk = 0;       //初始化
	DATI = 1;
	_nop_();
	CS = 0;
	_nop_();
	Clk = 1;
	_nop_();
	
	
	if ( CH == 0x00 )      //通道选择
	{
		Clk = 0;
		DATI = 1;      //通道0的第一位
		_nop_();
		Clk = 1;
		_nop_();
		Clk = 0;
		DATI = 0;      //通道0的第二位
		_nop_();
		Clk = 1;
		_nop_();
	} 
	else
	{
		Clk = 0;
		DATI = 1;      //通道1的第一位
		_nop_();
		Clk = 1;
		_nop_();
		Clk = 0;
		DATI = 1;      //通道1的第二位
		_nop_();
		Clk = 1;
		_nop_();
	}
	
	Clk = 0;
	DATI = 1;
	for( i = 0;i < 8;i++ )      //读取前8位的值
	{
			_nop_();
			adval <<= 1;
			Clk = 1;
			_nop_();
			Clk = 0;
			if (DATO)
			adval |= 0x01;
			else
			adval |= 0x00;
	}
	for (i = 0; i < 8; i++)      //读取后8位的值
	{
		test >>= 1;
		if (DATO)
		test |= 0x80;
		else 
		test |= 0x00;
		_nop_();
		Clk = 1;
		_nop_();
		Clk = 0;
	}
	if (adval == test)      //比较前8位与后8位的值，如果不相同舍去。若一直出现显示为零，请将该行去掉
	dat = test;
	nop_();
	CS = 1;        //释放ADC0832
	DATO = 1;
	Clk = 1;
	return dat;
}


/****************************************************************************
函数功能:将0-255级换算成0.00-5.00的电压数值
入口参数:i
出口参数:
****************************************************************************/
void convdata(unsigned int dat1)
{
	unsigned int Vo;
	Vo=dat1*0.0196*10*5;	 
    dis[0] = Vo/1000;       //十位
    dis[1] = Vo%1000/100;      //个位	 
    dis[2] = Vo%100/10;     //小数点后第1位
	dis[3] = Vo%10;     //小数点后第1位
}

