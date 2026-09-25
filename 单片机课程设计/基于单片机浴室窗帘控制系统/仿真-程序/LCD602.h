#ifndef __LCD602_H__
#define __LCD602_H__

void xiemingling(unsigned char mingling);
void xieshuju(unsigned char shuju);
void xiemingling(unsigned char mingling);
void chushihua();
//void write_num2(uchar hang,uchar add,uint date);
void write_string(uchar hang,uchar add,uchar *p);
//void write_guanbiao(uchar hang,uchar add,uchar date);
void write_zifu(uchar hang,uchar add,uchar date);
void write_num3(uchar hang,uchar add,uint date);
//void write_num5(uchar hang,uchar add,uint date);
//void write_num51(uchar hang,uchar add,uint date);
void write_num2(uchar hang,uchar add,uint date);
//void write_num1(uchar hang,uchar add,uint date);

#endif
