#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

#define KEY0   PCin(8)   	
#define KEY1   PCin(9)
#define KEY2   PCin(10)	  
 
#define KEY3   PBin(0)	 

void KEY_Init(void);   //IO初始化
u8 KEY_Scan(u8 mode);  //按键扫描函数					    
#endif

