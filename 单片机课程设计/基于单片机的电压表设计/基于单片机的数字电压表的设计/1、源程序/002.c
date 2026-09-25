#include<reg51.h>
sbit START=P3^4;			   //ATART，ALE接口。0->1->0:启动AD转换。
sbit EOC=P3^3;			   //转换完毕由0变1.
#define OUTPORT P2 

												//AD转换函数，返回转换结果。												//转换结果是3位数，小数点在百位与十位之间。
unsigned int uiADTransform()
{
	unsigned int uiResult;
	START=1;			 //启动AD转换。
	START=0;
	while(EOC==0);		 //等待转换结束。
	uiResult=OUTPORT;	 //出入转换结果。
	uiResult=(100*uiResult)/51;		   //处理运算结果。
	return uiResult;
}