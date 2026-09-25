#include "display_driver.h"
#include "LCD1602.h"
extern int tempw[6];
extern bit Edit_state;
extern unsigned char idata cursor_position;
extern unsigned int Add,idata Add_backup;

void initialize_LCD1602(){//初始化1602
	unsigned char a;
	Edit_state=Add=Add_backup=cursor_position=0;
	LCD1602_Write_Reg(0x80);
	for(a=0;a<16;a++)
  LCD1602_Write_Data(0x20);
	LCD1602_Write_Reg(0xC6);
	for(a=0;a<9;a++)
  LCD1602_Write_Data(0x20);
	LCD1602_Write_Data('0');
}

void refresh_str(unsigned char*str,unsigned int Addr){//刷新用户输入
	unsigned char a,b;
	if(Addr<=16){
		LCD1602_Write_Reg(0x80);
		for(a=0;a<16;a++)
		LCD1602_Write_Data(Addr>a?str[a]:0x20);
	}
	else{
		for(a=Addr-1,b=0x8F;a>=Addr-16;a--,b--){
			LCD1602_Write_Reg(b);
			LCD1602_Write_Data(str[a]);
		}
	}
}

void display_result(double*num,unsigned char*p,unsigned int add){//显示结果
	char str[10];
	unsigned char a,b,c,d,sign=0;
	unsigned long num1;
	for(a=0;a<10;a++)str[a]=0x20;
	if(*num<0){
		sign=1;
		str[9]='-';
		*num=-*num;
	}
	num1=(unsigned long)*num;
	if(num1<10)b=1;
	else if(num1<100)b=2;
	else if(num1<1000)b=3;
	else if(num1<10000)b=4;
	else if(num1<100000)b=5;
	else if(num1<1000000)b=6;
	else if(num1<10000000)b=7;
	else if(num1<100000000)b=8;
	else if(num1<1000000000)b=9;
	for(c=b;c>0;c--){
		for(a=0;a<9;a++)str[a]=str[a+1];
		switch(c){
			case 9:str[9]=num1/100000000%10+'0';break;
			case 8:str[9]=num1/10000000%10+'0';break;
			case 7:str[9]=num1/1000000%10+'0';break;
			case 6:str[9]=num1/100000%10+'0';break;
			case 5:str[9]=num1/10000%10+'0';break;
			case 4:str[9]=num1/1000%10+'0';break;
			case 3:str[9]=num1/100%10+'0';break;
			case 2:str[9]=num1/10%10+'0';break;
			case 1:str[9]=num1%10+'0';break;
		}
	}
	if(*num!=(unsigned long)*num){//小数
		b=++b+sign;
		for(a=0;a<9;a++)str[a]=str[a+1];
		str[9]='.';
    *num-=(unsigned long)*num;
		for(d=0;b+d<8;d++){
			for(a=0;a<9;a++)str[a]=str[a+1];
			str[9]=(unsigned long)(*num*=10)%10+'0';
		}
		for(a=9;a>0&&str[9]=='0';a--){
			for(d=9;d>0;d--)str[d]=str[d-1];
			str[0]=0x20;
		}
	}
	LCD1602_Write_Reg(0x80);
	for(a=0;a<16;a++)
	LCD1602_Write_Data(add>a?p[a]:0x20);
	LCD1602_Write_Reg(0xC0);
	for(a=0;str[a]!='\0';a++)
	LCD1602_Write_Data(str[a]);
	for(b=0;a<16;a++,b++)
	LCD1602_Write_Data(tempw[b]);
}