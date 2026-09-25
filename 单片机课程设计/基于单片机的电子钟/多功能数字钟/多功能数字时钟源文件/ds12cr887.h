#ifndef __DS12CR887_H__
#define	__DS12CR887_H__
void writeds(unsigned char add,unsigned char dat);
unsigned char readds(unsigned char add);
void initds();
void writealarm(unsigned char ahour,unsigned char aminute,unsigned char asecond);
#endif