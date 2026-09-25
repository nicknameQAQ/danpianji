#include "insert_edit.h"
#include "LCD1602.h"

unsigned int idata Edit_location,Insert_node;//显示编辑当前位置,插入节点
extern bit Edit_state;
extern unsigned char idata cursor_position;
extern unsigned int Add,idata Add_backup;

void backspace(unsigned char*str){//退格
	unsigned int a;
	if(Edit_state){
		for(a=Insert_node;a<=Add;a++)str[a-1]=str[a];
		refresh_str(str,--Add<16?Add:Edit_location==Add+1?cursor_position++,--Edit_location:Edit_location);
		LCD1602_Write_Reg(cursor_position);
		if(!Add||Insert_node==Add){
			Edit_state=0;
			LCD1602_Write_Reg(0x0C);
		}
	}
	else{
		if(!Add&&!Add_backup)return;
		if(!Add)Add=Add_backup;
		if(Add>0)if(!--Add)Add_backup=0;
		refresh_str(str,Add);
	}
}

void cursor_left_shift(unsigned char*str){//光标左移
	if(cursor_position==0x80)return;
	if(Edit_state&&Add){
		if(cursor_position>0x88){
			Insert_node--;
			LCD1602_Write_Reg(--cursor_position);
		}
		else if(cursor_position==0x88&&Edit_location>16){
			Insert_node--;
			refresh_str(str,--Edit_location);
			LCD1602_Write_Reg(cursor_position);
		}
		else{
			Insert_node--;
			LCD1602_Write_Reg(--cursor_position);
		}
	}
	else{
		if(!Add&&!Add_backup)return;
		Edit_state=1;
		if(!Add)Add=Add_backup;
		Edit_location=Insert_node=Add;
		LCD1602_Write_Reg(0x0E);
		if(Add<16)LCD1602_Write_Reg(cursor_position=0x80+Add-1);
		else LCD1602_Write_Reg(cursor_position=0x8F);
	}
}

void Exit_editor(){//退出编辑
	Edit_state=0;
	LCD1602_Write_Reg(0x0C);
}

void cursor_right_shift(unsigned char*str){//光标右移
	if(Edit_state){
		if(Insert_node==Add){
			Edit_state=0;
			LCD1602_Write_Reg(0x0C);
		}
		else if(cursor_position<0x88){
			if(cursor_position==Add+127){
				Exit_editor();
				return;
			}
			Insert_node++;
			LCD1602_Write_Reg(++cursor_position);
		}
		else if(cursor_position==0x88&&Edit_location<Add){
			if(cursor_position==Add+127){
				Exit_editor();
				return;
			}
			Insert_node++;
			refresh_str(str,++Edit_location);
			LCD1602_Write_Reg(cursor_position);
		}
		else{
			if(cursor_position==Add+127){
				Exit_editor();
				return;
			}
			Insert_node++;
			LCD1602_Write_Reg(++cursor_position);
		}
	}
	else{
		if(!Add&&!Add_backup)return;
		Edit_state=1;
		if(!Add)Add=Add_backup;
		Edit_location=Add<16?Add:16;
		Insert_node=1;
		LCD1602_Write_Reg(0x0E);
		LCD1602_Write_Reg(cursor_position=0x80);
	}
}