#ifndef _ds18b20
#define _ds18b20
#include<reg52.h>
sbit DQ=P2^6;
/**********************************
函数名： delayNus(int dly)
函数功能：微妙级延时
传入参数：延时长度
传出参数：无
**********************************/
void delayNus(int dly);

/**********************************
函数名： char Rest18b20(void)
函数功能：18B20检测函数
传入参数：无
传出参数：0 失败 1成功
**********************************/
unsigned char Rest18b20(void);

/**********************************
函数名： WriteOneByte(unsigned char dat)
函数功能：向器件写入数据
传入参数：要写入的数据
传出参数：无
**********************************/
void WriteOneByte(unsigned char dat);

/**********************************
函数名： char  ReadOneByte(void)
函数功能：从器件读出数据
传入参数：无
传出参数：读出的数据
**********************************/
unsigned char  ReadOneByte(void);

/**********************************
函数名： unsigned char ReadRomSerial(unsigned char *s)
函数功能：读出器件ROM序列
传入参数：存放序列号的数组地址
传出参数：ROM序列号
**********************************/
extern unsigned char ReadRomSerial(unsigned char *s);

/**********************************
函数名： unsigned char MatchRom(unsigned char *s,unsigned char Num)
函数功能：匹配器件ROM序列
传入参数：存放序列号的数组地址
传出参数：1 匹配成功 0 匹配失败
**********************************/
unsigned char MatchRom(unsigned char *s,unsigned char Num);

/**********************************
函数名： unsigned char ReadTempreture(unsigned char *s,unsigned char Num)
函数功能：读取温度
传入参数：存放序列号的数组地址，器件编号
传出参数：温度值
**********************************/
extern unsigned char ReadTempreture(unsigned char *s,unsigned char Num);

/**********************************
函数名： Init18b20(void)
函数功能：器件初始化函数
传入参数：无
传出参数：无
**********************************/
extern void Init18b20(void);

/**********************************
函数名： SendByte(unsigned char dat)
函数功能：发送字节函数
传入参数：发送的数据
传出参数：无
**********************************/
extern void SendByte(unsigned char dat);

/**********************************
函数名： SendString(unsigned char *s)
函数功能：发送字符串函数
传入参数：发送的数据
传出参数：无
**********************************/
extern void SendString(unsigned char *s);

/**********************************
函数名： Initboard(void)
函数功能：波特率初始化
传入参数：发送的数据
传出参数：无
**********************************/
extern void Initboard(void);
#endif
/***************************************
注意：在主函数里头需建立一个存放ROM序列的一维数组

***************************************/