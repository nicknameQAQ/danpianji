//巍巍机器人小卖部提供//
//淘宝地址：http://shop58277238.taobao.com//
//欢迎您的使用//

//本程序运行需要配本店的两个红外避障模块//

/**************************Timer0*********************************************/

#include <REGX51.H>
#include <intrins.h>


#define RightSenser P1_4    //左信号
#define LeftSenser  P1_5    //右信号

unsigned char a,b,k,j;de;

void delay10ms()   //延时函数
  {
    for(a=100;a>0;a--)
      for(b=225;b>0;b--);
   }




//********************************************************

//********************************************************
main()
{



	while(1)		
	{
P1_0 = 1 ;
P1_1 = 1 ;
P1_2 = 1 ; 
P1_3 = 1; 
             	
	if((!RightSenser) && LeftSenser)   // 如果右边假，左边真
              	{
				
	         	P1_0 = 0 ;
                P1_1 = 1 ;

                for (de=0; de<100; de++);   // 延时
            	}

   	if(RightSenser && (!LeftSenser))  // 如果左边假，右边真
             	{
                	P1_2 = 0 ; 
                    P1_3 = 1; 			

	
					for (de=0; de<100; de++);  // 延时
                  	}
	if((!RightSenser) && (!LeftSenser))  //左右都有信号，前进
	{
               P1_0 = 0 ;
               P1_1 = 1 ;
               P1_2 = 0 ; 
               P1_3 = 1; 
			   for (de=0; de<100; de++);  // 延时
	}

   	if((RightSenser) && (LeftSenser))  //无信号 停止
             	{
               P1_0 = 1 ;
               P1_1 = 1 ;
               P1_2 = 1 ; 
               P1_3 = 1; 

            
	
					for (de=0; de<100; de++);  // 延时

            	}


	}
	}




