#ifndef _LCD1602_H_
#define _LCD1602_H_
#define	line_1	0x80            // 第一行写入地址
#define	line_2	0x40            // 第二行写入地址
extern unsigned char number[11];
extern void WriteInstruction (unsigned char dictate);
extern void WriteAddress(unsigned char x);
extern void WriteData(unsigned char y);
extern void LcdInitiate(void);
extern void Print_String(unsigned char line,unsigned char num,unsigned char  *str);
extern void display_number(unsigned char line,unsigned char num);
#endif
