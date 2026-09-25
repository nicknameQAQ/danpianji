#ifndef cale_c
  #define cale_c
  #include "main.h"
//-------------------------------------------------------------------------
// //周期法   先笨方法
//-------------------------------------------------------------------------
  void cale_t( uchar* back )
  {
    ulint count_all; //存放周期 max 16m min 16k
    uint z_sh;      
    ulint y_sh;     
    uchar test;      
//*******************整数小数分离  
    count_all = t1_cont * 50000+TCNT1; 
	if(count_all>max_t)
	count_all=max_t;
    z_sh = max_t / count_all;          
	#if pc
    y_sh=max_t-count_all*z_sh;
	y_sh*=10;
    count_all/=100;
#else
    y_sh = ( max_t % count_all )* 1000; 
#endif
//*******************整数计算部分
    *back++ = z_sh / 100;
    z_sh = z_sh % 100;
    *back++ = z_sh / 10;
    *back++ = z_sh % 10; 
//***************小数计算 
    z_sh = y_sh / count_all; 
    *back++ = z_sh / 100;
    z_sh = z_sh % 100;

    *back = z_sh / 10;
    z_sh = z_sh % 10;
//************做四舍五入计算
    test=0; 
    if ( z_sh >= 5 )
    {
	send_char(z_sh);
      *back += 1; //
      if ( *back == 10 )
      {
	    test++;
        *back-- = 0;
        *back += 1; 
        if ( *back == 10 )
        {
	    test++;
          *back-- = 0; 
          *back += 1; //
          if ( *back == 10 )
          {
	    test++;
            *back-- = 0;
            *back += 1; //
            if ( *back == 10 )
            {
	    test++;
              *back-- = 0;
              *back += 1; 
            }
          }
        }
      }
    }
    back-=(4-test); 

//****************转到ascii码
    for ( test = 5; test > 0; test-- )
    {
      *back++ = * back + 0x30;
    }
back-=5;
//****************插进小数点
    back[5] = back[4]; 
    back[4] = back[3];
    back[3] = '.';

//****************去掉不要的0

    for ( test = 3; test > 0; test-- )
    { 
      if ( *back == 0x30 )
      {
        *back++ = * back - 0x30;
      }
      else
      {
        break;
      }
    }
  }

//-------------------------------------------------------------------------
//       //记数法 
//-------------------------------------------------------------------------
  void cale_f( uchar* back )
  {
    ulint count_all;  
    uchar test;       

    count_all = t1_cont * 50000+TCNT1; //原始值
//*****************计算 笨方法计算
    *back++   = count_all / 1000000; //最高位
    count_all = count_all % 1000000; //去掉最高位
    *back++   = count_all / 100000; //100k位
    count_all = count_all % 100000; //去掉
    *back++   = count_all / 10000;
    count_all = count_all % 10000;
    *back++   = count_all / 1000;
    count_all = count_all % 1000;
    *back++   = count_all / 100;
    count_all = count_all % 100;
    *back++   = count_all / 10;   //十位
    *back     = count_all % 10;   //个位

    back = back - 6;

//转到ascii字符
    for ( test = 7; test > 0; test-- )
    {
      *back++ = * back + 0x30;
    }
    back = back - 7; //指针退回
//处理前面的0
    while ( *back == 0x30 )
    {
      *back++ = * back - 0x30;
    }
  }
#endif
