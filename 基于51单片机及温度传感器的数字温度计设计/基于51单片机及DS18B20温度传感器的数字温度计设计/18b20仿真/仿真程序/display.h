/**********************************************************************
* 程序名;   ds18b20数码管动态显示头文件
* 功  能：  通过定时器0延时是数码管动态显示
* 编程者：  ZPZ
* 编程时间：2009/10/2   
**********************************************************************/
#ifndef __ds18b20_display_H__		 //定义头文件
#define __ds18b20_display_H__

#define uint unsigned int 	  		 //变量类型宏定义，用uint表示无符号整形（16位）
#define uchar unsigned char			 //变量类型宏定义，用uchar表示无符号字符型（8位）

sbit wei1=P2^4;				 		 //可位寻址变量定义，用wei1表示P2.4口
sbit wei2=P2^5;				 		 //用wei2表示P2.5口
sbit wei3=P2^6;				  		 //用wei3表示P2.6口
sbit wei4=P2^7;						 //用wei4表示P2.7口

uchar num=0;						 //定义num为全局无符号字符型变量，赋初值为‘0’
uchar code temperature1[]={ 0x3f,0x06,0x5b,0x4f,0x66,
                           0x6d,0x7d,0x07,0x7f,0x6f};	//定义显示码表0~9
uchar code temperature2[]={ 0xbf,0x86,0xdb,0xcf,0xe6,
                           0xed,0xfd,0x87,0xff,0xef};	//带小数点的0.~9.
uchar code temperature3[]={ 0x00,0x80,0x40,0x76,0x38};	//依次是‘不显示’‘.’‘-’‘H’‘L’

/*****************************延时子函数******************************/
void display_delay(uint t)		     //延时1ms左右
{
	uint i,j;
	for(i=t;i>0;i--)
    	for(j=120;j>0;j--);
}

/**************************定时器1初始化函数***************************/
void timer1_init(bit t)
{
    TMOD=0x10;						//设定定时器1工作在方式1，最大定时65.53ms
    TH0=0x3c;						//定时器赋初值，定时50ms
    TL0=0xb0;						
    EA=1;							//开总中断
    ET1=1;							//开定时器1中断
    TR1=t;							// 局部变量t为1启动定时器1，为0关闭定时器1
}
/**************************定时器1中断函数*****************************/
void timer1() interrupt 3
{
    TH0=0x3c;						//重新赋初值，定时50ms
    TL0=0xb0;
	num++;							//每进入一次定时器中断num加1（每50ms加1一次）
    if(num<5) 
	{s=1;if(w==1){beer=1;led=1;}else{beer=1;led=1;}}								
	else      						//进入4次中断，定时200ms时若报警标志位w为‘1’则启动报警，不为‘1’不启动
									//实现间歇性报警功能
	{s=0;if(w==1){beer=0;led=0;}else{beer=1;led=1;}}
	if(num>20)						//进入20次中断，定时1s
    { 
	   num=0;						//num归0，重新定开始定时1s
	   s1=0;						//定时1s时间到时自动关闭报警上下限显示功能
	   v1=1;						//定时1s时间到时自动关闭报警上下限查看功能
    }
	
}
/*********************调整报警上下限显示选择函数**********************/
void  selsct_1(uchar f,uchar k)	    //消除百位的0显示，及正负温度的显示选择
{
      if(f==0)						//若为正温度，百位为0则不显示百位，不为0则显示
	  {
		  if(k/100==0)  P0=temperature3[0];
		  else          P0=temperature1[k/100];
	  }
	  if(f==1)						//若为负温度，若十位为0，百位不显示，否则百位显示‘-’
	  {
	      if(k%100/10==0)   P0=temperature3[0];
		  else              P0=temperature3[2];
	  }
}

void  selsct_2(bit f,uchar k)		//消除十位的0显示，及正负温度的显示选择
{
      if(f==0)					   	//若为正温度，百位十位均为0则不显示十位，否则显示十位
	  {
		  if((k/100==0)&&(k%100/10==0))  
		      P0=temperature3[0];
		  else          P0=temperature1[k%100/10];
	  }
	  if(f==1)						//若为负温度，若十位为0，十位不显示，否则十位显示‘-’
	  {
	      if(k%100/10==0)   P0=temperature3[2];
		  else               P0=temperature1[k%100/10];
	  }		
}



/****************************主显示函数********************************/
void display(uchar t,uchar t_d)	   //用于实测温度、上限温度的显示
{
   uchar i;
   for(i=0;i<4;i++) 	  		   //依次从左至右选通数码管显示，实现动态显示
   {		  
       switch(i)
	   {
	      case 0:				   //选通第一个数码管
		  wei2=1;				   //关第二个数码管
		  wei3=1;				   //关第三个数码管
		  wei4=1;		  		   //关第四个数码管
	      wei1=0;				   //开第一个数码管		  
		  if(a==0){selsct_1(f,t);} //若a=0则在第一个数码管上显示测量温度的百位或‘-’
		  if(a==1)
		  {  		    
		      P0=temperature3[3];  //若a=1则在第一个数码管上显示‘H’		     
		  }
		  if(a==2)
		  {  		     
		      P0=temperature3[4];  //若a=2则在第一个数码管上显示‘L’
		  }					  
		  break;
	      case 1:				   //选通第二个数码管
		  wei1=1;		  
		  wei3=1;
		  wei4=1;		  
	      wei2=0;		  
		  if(a==0){selsct_2(f,t);} //若a=0则在第二个数码管上显示测量温度的十位或‘-’
		  if(a==1)				   //若a=1则在第二个数码管上显示上限报警温度的百位或‘-’
		  {  
		     if(s==0) selsct_1(f_max,max);//若s=0则显示第二个数码管，否则不显示
		     else P0=temperature3[0];		  //通过s标志位的变化实现调节上下限报警温度时数码管的闪烁
		     if(s1==1) selsct_1(f_max,max);//若s1=1则显示第二个数码管（s1标志位用于上下限查看时的显示）
		  }
		  if(a==2)				   //若a=2则在第二个数码管上显示下限报警温度的百位或‘-’
		  {  
		     if(s==0) selsct_1(f_min,min);
		     else P0=temperature3[0];
		     if(s1==1) selsct_1(f_min,min);
		  }			  			  			  
		  break;
	      case 2:				   //选通第三个数码管
		  wei1=1;
		  wei2=1;		  
		  wei4=1;		  
	      wei3=0;		  
		  if(a==0){P0=temperature2[t%10];}//若a=0则在第三个数码管上显示测量温度的个位
		  if(a==1)				   //若a=1则在第三个数码管上显示上限报警温度的十位或‘-’
		  {
		      if(s==0) selsct_2(f_max,max);//若s=0则显示第三个数码管，否则不显示
			  else P0=temperature3[0];
			  if(s1==1) selsct_2(f_max,max);//若s1=1则显示第三个数码管
		  }
		  if(a==2)				   //若a=2则在第三个数码管上显示下限报警温度的十位或‘-’
		  {
		      if(s==0) selsct_2(f_min,min);
			  else P0=temperature3[0];
			  if(s1==1) selsct_2(f_min,min);
		  }				  			  			   
		  break;
	      case 3:				  //选通第四个数码管
		  wei1=1;		 
		  wei2=1;		  
		  wei3=1;
		  wei4=0;		  
		  if(a==0){P0=temperature1[t_d];}//若a=0则在第四个数码管上显示测量温度的小数位
		  if(a==1)				  //若a=1则在第四个数码管上显示上限报警温度的个位
		  {
		     if(s==0) P0=temperature1[max%10];//若s=0则显示第四个数码管，否则不显示
			 else P0=temperature3[0];
			 if(s1==1) P0=temperature1[max%10];//若s1=1则显示第四个数码管
		  }
		  if(a==2)				  //若a=2则在第四个数码管上显示下限报警温度的个位
		  {
		     if(s==0) P0=temperature1[min%10];
			 else P0=temperature3[0];
			 if(s1==1) P0=temperature1[min%10];
		  }			  		   	  	       			 			   
		  break;		  
	   }
	 display_delay(10);			  //每个数码管显示3ms左右
	}
	
}
/****************************开机显示函数******************************/
void display1(uint z)			  //用于开机动画的显示
{
   uchar i,j;
   bit f=0;
   for(i=0;i<z;i++)				  //‘z’是显示遍数的设定
   {	   
	   	for(j=0;j<4;j++)		  //依次从左至右显示‘-’
	   {
	       switch(j)
		   {
			   case 0:
			          wei2=1;
			          wei3=1;
			          wei4=1;		  
		              wei1=0;  break;
					  P0=temperature3[2];//第一个数码管显示
			   case 1:
			          wei1=1;		  
			          wei3=1;
			          wei4=1;		  
		              wei2=0;break;
					  P0=temperature3[2];//第二个数码管显示

			   case 2:
			          wei1=1;
			          wei2=1;		  
			          wei4=1;		  
		              wei3=0;break;
					  P0=temperature3[2];//第三个数码管显示
			   case 3:
			          wei1=1;		 
			          wei2=1;		  
			          wei3=1;
			          wei4=0;break;
					  P0=temperature3[2];//第四个数码管显示
		   }
		  display_delay(400);	   //每个数码管显示200ms左右
	   }
 	   	   
	}
}



#endif