#include "key_drive.h"
#include <STC8.h>
#include "operation.h"
#include "insert_edit.h"
#include "LCD1602.h"
#include "display_driver.h"

bit Edit_state;
unsigned char*str=(unsigned char xdata*)0;
unsigned char idata cursor_position;
unsigned int Add,idata Add_backup;
extern unsigned int idata Edit_location,Insert_node;

struct{
	unsigned char sign:1;
	unsigned char list:3;
}degree;

void write_str(unsigned char b){//写入字符
	unsigned int a;
	if(Edit_state){
		if(degree.sign){
			str[Insert_node-2]=b;
			refresh_str(str,Add<16?Add:Edit_location);
			LCD1602_Write_Reg(cursor_position);
		}
		else{
			for(a=Add++;a>=Insert_node;a--)str[a]=str[a-1];
			str[Insert_node-1]=b;
			Insert_node++;
			Edit_location++;
			refresh_str(str,Add<16?Add:Edit_location);
			LCD1602_Write_Reg(Add<16?++cursor_position:cursor_position);
		}
	}
	else{
		if(degree.sign)Add--;
		str[Add++]=b;
		refresh_str(str,Add);
	}
}

void keydrive(unsigned char key){//处理按键
//	LCD1602_Write_Reg(0xC0);
//	LCD1602_Write_Data(key/100+'0');
//	LCD1602_Write_Data(key/10%10+'0');
//	LCD1602_Write_Data(key%10+'0');
	switch(key){
		case 219://1
			degree.sign=0;
			write_str('1');
			break;
		case 215://2
			degree.sign=0;
			write_str('2');
			break;
		case 159://3
			degree.sign=0;
			write_str('3');
			break;
		case 235://4
			degree.sign=0;
			write_str('4');
			break;
		case 231://5
			degree.sign=0;
			write_str('5');
			break;
		case 175://6
			degree.sign=0;
			write_str('6');
			break;
		case 249://7
			degree.sign=0;
			write_str('7');
			break;
		case 245://8
			degree.sign=0;
			write_str('8');
			break;
		case 189://9
			degree.sign=0;
			write_str('9');
			break;
		case 250://等于
			Edit_state=cursor_position=degree.sign=degree.list=0;
			LCD1602_Write_Reg(0x0C);
		  Ready_operation(str,Add);
		  Add_backup=Add;
		  Add=0;
			break;
		case 246://0
			degree.sign=0;
			write_str('0');
			break;
		case 190://点
			degree.sign=0;
			write_str('.');
			break;
		case 95://加
			degree.sign=degree.list=0;
			write_str('+');
			break;
		case 111://减
			degree.sign=degree.list=0;
			write_str('-');
			break;
		case 125://乘
			degree.sign=degree.list=0;
			write_str('x');
			break;
		case 126://除
			degree.sign=degree.list=0;
			write_str(253);
			break;
		case 242://(
			degree.sign=degree.list=0;
			write_str('(');
			break;
		case 182://)
			degree.sign=degree.list=0;
			write_str(')');
			break;
		case 167://°'"
			write_str(degree.list==0?0x6F:degree.list==1?'\'':'\"');
			degree.sign=1;
		  if(++degree.list>=3)degree.list=0;
			break;
		case 241://sin
			degree.sign=degree.list=0;
			write_str('s');
		  write_str('i');
		  write_str('n');
		  write_str('(');
			break;
		case 181://cos
			degree.sign=degree.list=0;
			write_str('c');
		  write_str('o');
		  write_str('s');
		  write_str('(');
			break;
		case 61://tan
			degree.sign=degree.list=0;
			write_str('t');
		  write_str('a');
		  write_str('n');
		  write_str('(');
			break;
		case 119://退格
			backspace(str);
			break;
		case 133://左移
			cursor_left_shift(str);
			break;
		case 134://右移
			cursor_right_shift(str);
			break;
		case 103://归零
			degree.sign=degree.list=0;
			initialize_LCD1602();
			break;
	}
}

void key_scan(){//启动按键扫描
	IE&=0xFA;
	INTCLKO&=0x8F;
	AUXINTIF&=0x8E;
	IE2|=0x04;
}

void KEY1()interrupt 0{//按键触发
	key_scan();
}

void KEY2()interrupt 2{//按键触发
	key_scan();
}

void KEY3()interrupt 10{//按键触发
	key_scan();
}

void KEY4()interrupt 11{//按键触发
	key_scan();
}

void GET_KEY()interrupt 12{//读取按键值
	unsigned char delay;
	static unsigned char key,b;
	AUXINTIF&=0x8E;
	switch(b++){
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
			if(P3!=0xCC){
				key=P3;
				P3=0x33;
				for(delay=5;delay--;);
				key|=P3;
				P3=0xCC;
			}
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:if(b==20)key+=8;
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
			if(P3==0xCC){
				b=0;
				keydrive(key);
				AUXINTIF&=0x8E;
			  IE2&=0xFB;
				TCON&=0xF5;
			  IE|=0x05;
	      INTCLKO|=0x30;
			}break;
		case 26:
			 b=21;
		   if(key==119||key==133||key==134)keydrive(key);
	}
}