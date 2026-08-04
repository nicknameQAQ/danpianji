#define uchar unsigned char
#define LCD_CHAR 0x14

sbit cd = P3^0;		/*通道选择*/
sbit rd = P3^1;		/*读操作信号*/
sbit wr = P3^2;		/*写操作信号*/
sbit error0 = P1^5;  /*出错提示1*/
sbit error1 = P1^6;  /*出错提示2*/
sbit error2 = P1^7;  /*出错提示3*/

/*读状态*/
uchar read_state(){
	uchar temp;
	P2 = 0xff;
	cd = 1;
	rd = 0;
	temp = P2;
	rd = 1;
	return(temp);
}

/*STA0指令读写状态,STA1数据读写状态 判断函数*/
void lcd_enable(){
	uchar i;
	for(i = 10; i > 0; i--)
		if((read_state() & 0x03) == 0x03)
			break;
	if(i==0)error0=1;/*若i==0，说明错误*/
	//else error0=0;
}

/*STA2数据自动读状态 判断函数*/
void atrd_enable(){
	uchar i;
	for(i = 10; i > 0; i--)
		if((read_state() & 0x04) == 0x04)
			break;
	if(i==0)error1=1;/*若i==0，说明错误*/
	//else error1=0;
}

/*STA3数据自动写状态 判断函数*/
void atwr_enable(){
	uchar i;
	for(i = 10; i > 0; i--)
		if((read_state() & 0x08) == 0x08)
			break;
	if(i==0)error2=1;/*若i==0，说明错误*/
	//else error2=0;
}

/*写无参数函数*/
void write_cmd0(uchar cmd){
	lcd_enable();
	cd=1;
	P2=cmd;
	wr=0;
	wr=1;
}

/*写单参数函数*/
void write_cmd1(uchar data1, uchar cmd){
	lcd_enable();
	cd=0;
	P2=data1;
	wr=0;
	wr=1;
	lcd_enable();
	cd=1;
	P2=cmd;
	wr=0;
	wr=1;
}

/*写双参数函数*/
void write_cmd2(uchar data1, uchar data2, uchar cmd){
	lcd_enable();
	cd=0;
	P2=data1;
	wr=0;
	wr=1;
	lcd_enable();
	cd=0;
	P2=data2;
	wr=0;
	wr=1;	
	lcd_enable();
	cd=1;
	P2=cmd;
	wr=0;
	wr=1;	
}

/*写数据函数*/
void write_data(uchar data0){
	lcd_enable();
	cd=1;
	P2=data0;
	wr=0;
	wr=1;	
}

/*读数据函数*/
uchar read_data(){
	char temp;
	lcd_enable();
	cd = 0;
	P2 = 0xff;
	rd = 0;
	temp = P2;
	rd = 1;
	return(temp);	/*若返回0,可能错误*/
}

/*自动写开始*/
void auto_write(){
	write_cmd0(AUT_WR);
}

/*自动读开始*/
void auto_read(){
	write_cmd0(AUT_RD);
}

/*自动写结束*/
void atwr_stop(){
	write_cmd0(AUT_WO);
}

/*自动读结束*/
void atrd_stop(){
	write_cmd0(AUT_RO);
}

/*数据一次写函数*/
void write_one(uchar data1, char way){
	atwr_enable();
	auto_write();
	write_cmd1(data1,way);
	atwr_stop();
}

/*数据一次读函数*/
uchar read_one(char way){
	uchar temp;
	atrd_enable();
	auto_read();
	write_cmd0(way);
	temp = read_data();
	atrd_stop();
	return(temp);
}

/*设置当前显示位置函数x,y从0开始表示单位为字符*/
void set_xy(uchar x, uchar y){
	int temp;
	temp = y * LCD_CHAR + x;
	write_cmd2(temp&0xff,temp/0xff,ADR_POS);			
}

void set_adr(uchar D1, uchar D2){
	write_cmd2(D1,D2,ADR_POS);
}

/*设置光标指针 x,y从0开始*/
void set_cur(char x, char y){
	write_cmd2(x,y,CUR_POS);
}

/*CGRAM偏置地址设置函数*/
void set_cgram(){
	write_cmd2(0x01,0x00,CGR_POS);//0000,1100,0000,0000 0C00
}

/*液晶初始化函数(文本区首地址D1,文本区首地址D2, 文本区宽度,  图形区首地址D1, 图形区首地址D2, 图形区宽度,   光标形状,  显示方式,  显示开关)*/
void lcd_init(uchar txtstpd1, uchar txtstpd2, uchar txtwid, uchar grhstpd1, uchar grhstpd2, uchar grhwid, uchar cur, uchar mod, uchar sw){
	write_cmd2(txtstpd1,txtstpd2,TXT_STP);								/*文本区首地址*/
	write_cmd2(txtwid,0x00,TXT_WID);			/*文本区宽度*/
	write_cmd2(grhstpd1,grhstpd2,GRH_STP);		/*图形区首地址*/
	write_cmd2(grhwid,0x00,GRH_WID);			/*图形区宽度*/
	write_cmd0(CUR_SHP | cur);					/*光标形状*/
	write_cmd0(mod);							/*显示方式*/
	write_cmd0(DIS_SW | sw);					/*显示开关*/
}