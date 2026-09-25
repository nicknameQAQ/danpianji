#include "reg52.h"
#include <stdlib.h>        //包含atof(),字符串->实数
#include <stdio.h>         //包含sprintf(),实数->字符串
#include <string.h>        //包含strcpy(),字符串复制
#include <math.h>		   //数学函数头文件
#include "yyxp.h"
#define uchar unsigned char
#define uint unsigned int

//sbit choose=P3^4;          //时钟和计算器选择键
/*************lcd****************/
sbit lcdrs=P2^7;           //LCD控制脚
sbit lcdrw=P2^6;
sbit lcden=P2^5;
bit idata form;            //lcd显示方式切换
uchar idata state;         //lcd状态字存储
uchar flag_bofang;
/************计算器*************/
//sbit reset=P3^0;           //计算器复位
//sbit back=P3^1;            //撤销上次键入键
//sbit save=P3^2;            //存储键，存储有上次的结果和另外某次计算值
bit idata eqsign;          //按等号标志
bit idata press;           //键按标志
bit idata savesign;        //可存储结果标志
bit idata sc;			   //复用标志
uchar idata process[30],proc; //记忆计算过程，以便错误恢复和撤销输入
uchar idata continu=0;        //连续计算运算符号存储
uchar idata pai[2][2];        //π 前有数据标志
uchar idata ferror;        //计算检错标志
uchar idata ywei;		   //屏幕移位
uchar idata count=0;       //输入计数
uchar idata count_num=0;   //组号计数
uchar idata result[15];    //计算结果立存，save
//uchar idata saveresult[15];//存储结果数组, sc+save
uchar idata jieguo[15];    //结果字符串
uchar idata bdate[2][14];  //待计算字符串二维组
uchar idata on_symbol;     //运算符号
uchar idata fsym[2];       //函数前符号
uchar idata ssym[2];       //存储组前符号
uchar idata bfun[2];       //计算值调用函数选择
uchar idata futojiao[2];   //幅度 to 角度变换
double idata date[2];      //计算值双精度变量组
double idata resultdate;   //双精度结果值
/***************时钟****************/
sbit stime=P3^7;           //时间设置键，同时用于计算器模式的复用键
uchar idata ms50=0;        //时间调节变量
uchar idata s=0;           //秒
uchar idata m=0;           //分
uchar idata h=0;           //小时

/******************函数声明**********************/

/***************延时******************/
void delay(uchar z);	
/*************lcd写命令***************/
void write_com(uchar com);
/*************lcd写数据***************/
void write_date(uchar date);
/*************lcd读状态***************/
void read_date(void);
/*************lcd写字符串*************/
void write_str(uchar *str);
/************液晶初始化***************/
void init(void );
/**************键盘扫描***************/
uchar keyscan(void);
/*************计算器复位**************/
void fuwei();          
/************error处理****************/
void callerror(void);
/***************撤销键入**************/
void huifu(void);
/**********函数组前符号处理***********/
uchar funqian(void);
/************运算符预处理*************/
void  cullars(uchar);
/***********输出存储数据预处理********/
char  memory(void);
/***********按键功能主处理************/
void process_date(uchar press_date);
/***********按键功能子处理************/
void calculator(uchar press_date);
/************************************************/
/*************LCD程序****************/
/**************延时******************/
void delay(uchar z)			   //0.2 ms
{
      uchar  x,y;
      for(x=z;x>0;x--)
      for(y=100;y>0;y--);
}

/**************写命令*****************/
void write_com(uchar com)
{
      lcdrs=1;
      lcdrw=1;
      lcden=1;
      P0=com;
      delay(5);
      lcden=0;
      delay(5);
      lcden=1;
}
/*************写数据******************/
void write_date(uchar date)
{
      lcdrs=1;
      lcdrw=0;
      lcden=1;
      P0=date;
      delay(5);
      lcden=0;
      delay(5);
      lcden=1;
}
/*************读AC 地址*****************/
void read_date(void)
{
      lcdrs=0;
      lcdrw=1;
	  delay(5);
	  P0=0xff;
      lcden=1;
      delay(5);
      state=P0;
      delay(5);
      lcden=0;delay(5);
	  state=state&127;
}
/*************写字符串******************/
void write_str(uchar *str)
{     
      uchar idata i;
      for(i=0;str[i]!='\0';i++)
         write_date(str[i]);
}
/***********液晶初始化***************/
void init(void )
{
      write_com(0x38);       //模式：8位数据，两行，5*7字体
      write_com(0x0c);       //开显示，无光标
      write_com(0x01);       //清屏	  
	  write_com(0x06);       //向左增量移动
}


/**************键盘扫描*************/
uchar keyscan(void)          //按行扫描，有键按下则返回键符号，否则返回null
{     uchar idata key0,key1,keyment=0;
      P1=0Xef;					   //行1
      key0=P1;
      key0=key0&0x0f;
      key1=key0;
      if(key0!=0x0f)
      {
           delay(5);               //键抖动处理
           P1=0Xef;
           key0=P1;
           key0=key0&0x0f;
           if(key0==key1)
           {
                switch(key0)
                {
                  case 0x0e:keyment='7';break;
                  case 0x0d:keyment='8';break;
                  case 0x0b:keyment='9';break;
                  case 0x07:flag_bofang=0;keyment=0xfd;break;  //0xfd除号在1602液晶中的代码
                }
                while(key0!=0x0f)  //键释放处理
                {
                  key0=P1;
                  key0=key0&0x0f;
                }
                press=1;		   //键按标志置1
                return(keyment);
           }
      }
      P1=0Xdf;					  //行2
      key0=P1;
      key0=key0&0x0f;
      key1=key0;
      if(key0!=0x0f)
      {
           delay(5);
           P1=0Xdf;
           key0=P1;
           key0=key0&0x0f;
           if(key0==key1)
           {
               switch(key0)
               {
                  case 0x0e:keyment='4';break;
                  case 0x0d:keyment='5';break;
                  case 0x0b:keyment='6';break;
                  case 0x07:flag_bofang=0;keyment='*';break;
               }
               while(key0!=0x0f)
               {
                  key0=P1;
                  key0=key0&0x0f;
               }
               press=1;
               return(keyment);
           }
      }
      P1=0Xbf;						 //行3
      key0=P1;
      key0=key0&0x0f;
      key1=key0;
      if(key0!=0x0f)
      {
           delay(5);
           P1=0Xbf;
           key0=P1;
           key0=key0&0x0f;
           if(key0==key1)
           {

               switch(key0)
               {
                   case 0x0e:keyment='1';break;
                   case 0x0d:keyment='2';break;
                   case 0x0b:keyment='3';break;
                   case 0x07:flag_bofang=0;keyment='-';break;
               }
               while(key0!=0x0f)
               {
                   key0=P1;
                   key0=key0&0x0f;
               }
               press=1;
               return(keyment) ;
           }
      }
      P1=0X7f;					  //行4
      key0=P1;
      key0=key0&0x0f;
      key1=key0;
      if(key0!=0x0f)
      {
           delay(5);
           P1=0X7f;
           key0=P1;
           key0=key0&0x0f;
           if(key0==key1)
           {
               switch(key0)
               {
                 case 0x0e:keyment='.';break;
                 case 0x0d:keyment='0';break;
                 case 0x0b:flag_bofang=1;keyment='=';break;
                 case 0x07:flag_bofang=0;keyment='+';break;
               }
               while(key0!=0x0f)
               {
                 key0=P1;
                 key0=key0&0x0f;
               }
               press=1;
               return(keyment);
           }
     }
/*     else  if(save==0)		   //储存数据键
     {	delay(5);
	    if(save==0)   
	    { if(sc==0)	   //输出前次计算结果键
	       {	
		      keyment='m';
              press=1;
              while(save==0);
              return(keyment);
		   }
		   else			   //更新存储的数据或输出存储的数据
		   {
              keyment='n';
              press=1;
			  sc=0;
              while(save==0);
              return(keyment);		      
		   }
		}  
     } */
	   return(0);	   //无键按下返回null
}

/**************计算器复位****************/
void fuwei()
{   uchar idata j,i;
    write_com(0x0f);	    //lcd设置
    write_com(0x01);
    write_com(0x81);
    resultdate=0;
    for(i=0;i<2;i++)
	{ date[i]=0;bfun[i]=0;pai[0][i]=0;pai[1][i]=0;fsym[i]=0;futojiao[i]=0; //各标志置0
	  for(j=0;j<14;j++)		//字符数组初始化
      {	
        bdate[i][j]='\0';
      }
	}
    count=0;				//各标志置0
    count_num=0;
	proc=0;
    ferror=0;
    eqsign=0;
	sc=0;
	savesign=0;
	ywei=0;
	press=0;
	on_symbol=0; 
}
/**************error处理******************/
void callerror(void)
{   uchar idata i;
    speak(cuowu);
    write_com(0x01);
    write_com(0x81);
    write_str("     error!     ");
    for(i=0;i<50;i++)delay(25); //持续显示 1s
}

/*****************运算符预处理*****************/
void  cullars(uchar csym)
{	read_date();		   //看第二组是否有输入
	ferror++;              //检错标志+1
	continu=csym;		   //存储运算符号
	if(ferror==2&&state<0x43){callerror();huifu();return;}// 第二组无输入且ferror=2，错误
	if(ferror==2&&state>0x42)	//第二组有输入且ferror=2，则进行连续计算
	{  
	   press=1;					//模拟按键‘=’和‘M+’；将本次的计算结果作为下一次计算的第一组
	   process_date('=');
	   press=1;
	   process_date('m');
	   ferror=1;				//恢复现场，避免变化
	   if(continu=='^'||continu=='%'){process[proc]='h';
	        proc++;process[proc]=continu;proc++;}
	   else{process[proc]=continu;proc++;}	  //恢复连续计算过程
	   flag_bofang=0;
	}
	if(continu=='%'){write_str(" f");}else write_date(' ');
	write_date(continu);//将用与连续计算的运算符号取出
	ywei=0;				   
	savesign=0;
	count_num=1;     //组标志加1
	count=0;		 //输入计数清零
	on_symbol=continu;  //运算符号调用
	write_com(0xc2);
	return;
}
/*************输出存储数据预处理************/
char  memory(void)
{	if((bdate[count_num][0]=='+'||bdate[count_num][0]=='-')&&count==1)//符号处理			
    {
	 	ssym[count_num]=bdate[count_num][0];
	 	bdate[count_num][0]='\0';
		bdate[count_num][1]='\0';
		count=0;
	}
    if(bdate[count_num][0]!='\0'){callerror();huifu();return(1);}	//错误输入方式处理
	return(0);
}
/**************按键功能主处理****************/
void process_date(uchar press_date)
{    
     if(form==0){write_com(0x0f);form=1;}   //显示方式切换 光标闪烁	
     if(eqsign==1&&press==1)fuwei();		//复位再次计算，记忆过程清零
	 press=0;
	 if(press_date=='c'){huifu();press_date=0;}
	 if(press_date==0)return;         	//无按键
	 process[proc]=press_date;
	 proc++;
	 calculator(press_date);
	 return;
}
/***************按键功能子处理***************/
void calculator(uchar press_date)
{	 
     uchar idata j=0;
	 /*************************************/
	 if((press_date<='9'&&press_date>='0')||(press_date=='.')) //数字键处理
     {	savesign=0;			  //不可更新存储数据
        if(sc==0)			  //是否为复用sc为0，是数字
        { 
           if(count<14-ywei)  //未调用函数时最多输入14位数，包括小数点
           {	
			       write_date(press_date);
                   bdate[count_num][count]=press_date; //储存键值
                   count++;
				      if(press_date=='.')
					speak(22);
					else if(press_date=='0') 
				//	speak(60);
				    speak(ling);
					else speak(press_date+1-0x30);
           }
           else {callerror();huifu();}  //超出14位出错,恢复

        }
      }
       /*************************************************/
      else if(press_date=='m')       //上次计算结果输出
      {	 
           if(memory()==1)return;						  //输出存储数据预处理
           for(j=0;result[j]!='\0';j++)write_date(result[j]);	        //输出存储值
           strcpy(bdate[count_num],result);
		   savesign=1;		  //此时可以更新saveresult中存储的数据
		   count=14;          //count=14是让本组数据已确定
	  }
      /****************'+','-'处理*****************/
       else if((press_date=='+')||(press_date=='-'))
       {    
		      if(count==0)      // 表示输入数据的符号，此时默认数值为0
              {    				   
                write_date(press_date);
                bdate[count_num][0]=press_date; 
				bdate[count_num][1]='0';
				read_date();			//读状态
	         
	            write_com(state+0x81);//光标还回原来位置
                count=1;
				if(press_date=='-')
				speak(fu);
              }
              else			   //为运算符号
              {  
			  while(yyxp_busy==0);
				 yydelay(2); 
				if(press_date=='+')
				speak(jia);
				else  if(press_date=='-')
				speak(jian); 
			    cullars(press_date);
				
              }			
       }
       /****************'*','/'处理*****************/
       else if(press_date==0xfd) //0xfd除号在1602液晶中的代码
       {	
	       if(sc==1)	   //幅度 to 角度
		   {  if(count!=0)
		      {futojiao[count_num]=1;sc=0;write_date(0xDF);count=14;}
			  else {callerror();huifu();}
		   }
     	   else { 
		         cullars(press_date);
				 while(yyxp_busy==0);
				 yydelay(2);
				 speak(chuyi);
				 }
       }
	   else if(press_date=='*') //0xfd除号在1602液晶中的代码
       {		       
     	    cullars(press_date);
			 while(yyxp_busy==0);
				 yydelay(2);
			speak(chengyi);
       }
       /*******************'='号处理*****************/
       else if(press_date=='=')
       { 
	         
            write_com(0x01);           //清屏
            write_com(0x80+0x40+1);
            write_date(press_date);    //显示等号
		    for(j=0;j<2;j++)		   //两运算组的函数处理
			{
			   	date[j]=atof(bdate[j]);  //俩组数据，字符串->实数
				if(fsym[j]=='-')date[j]=-date[j];	 //函数前的符号处理 
			}
			if(ferror==0)							 //仅仅函数计算
			   {  resultdate=date[0];}
            else									 //运算符号计算
			{
			   switch(on_symbol)
               {
                  case '+':resultdate=date[0]+date[1];break;
                  case '-':resultdate=date[0]-date[1];break;
                  case '*':resultdate=date[0]*date[1];break;
                  case 0xfd:resultdate=date[0]/date[1];break;       //0xfd除号在1602液晶中的代码
				  case '^':resultdate=pow(date[0],date[1]);break;	//pow()函数	x^y
				  case '%':resultdate=fmod(date[0],date[1]);break;  //fmod()函数 求余数（双精度）
               }
			}			
            sprintf(jieguo,"%g",resultdate);//结果实数->字符串,采用格式字符g,系统选择%f或%e输出格式，六位有效数字           
	        if((jieguo[0]>'9'||jieguo[0]<'0')&&(jieguo[1]>'9'||jieguo[1]<'0')&&jieguo[1]!='\0'&&jieguo[1]!='.')
			 {   callerror();fuwei();return;  }	 //错误处理，当组1或组2不合法时结果是几个英文字母提示	
			else strcpy(result,jieguo);   //没错误，保存结果，结果立存 M+键	
			if(flag_bofang==1)
				 {		                                        	   			 
            for(j=0;jieguo[j]!='\0';j++)               //显示结果
                 write_date(jieguo[j]);
				 while(yyxp_busy==0);
				 yydelay(2);
				 
				 speak(dengyu);
				 bofang(jieguo);
				 }
			for(;j<15;j++)write_date(' ');	 //让光标出屏幕
			eqsign=1;						 //按等号键标志置1
	   }
	   return;
}											 
/*********************主程序**********************/
void main()
{   
      fuwei();		//计算器初始化
      init();       //显示初始化	  
      TH0=0x3c;     //定时器0初始化
      TL0=0xb7;
      TMOD=0x1;     //定时器0，方式1
      TR0=1;        //开始计时
      ET0=1;
      EA=1;         //允许溢出中断设置
	  form=1;		//显示方式初始化
	  proc=0;		//计算工程存储初始化
	  	      write_com(0x81); 
      while(1)      
      {
	   process_date(keyscan()); //计算器
      }
}
/*********************end**********************/
/**********************************************/