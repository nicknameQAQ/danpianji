//###########################################################################
//
// FILE:    Example_281xEvTimerPeriod.c
//
// TITLE:   DSP281x Event Manager GP Timer example program.
//
// ASSUMPTIONS:
//
//          This program requires the DSP281x V1.00 header files.  
//          As supplied, this project is configured for "boot to H0" operation.
//
//          Other then boot mode pin configuration, no other hardware configuration
//          is required.  
//
// DESCRIPTION:
//
//          This program sets up EVA Timer 1, EVA Timer 2, EVB Timer 3
//          and EVB Timer 4 to fire an interrupt on a period overflow.  
//          A count is kept each time each interrupt passes through
//          the interrupt service routine. 
//
//          EVA Timer 1 has the shortest period while EVB Timer4 has the
//          longest period.
//
//          Watch Variables:
//
//                EvaTimer1InterruptCount;
//                EvaTimer2InterruptCount;
//                EvbTimer3InterruptCount;
//                EvbTimer4InterruptCount;
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  1.00| 11 Sep 2003 | L.H. | No change since previous version (v.58 Alpha)
//###########################################################################


#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
#include "DSP281x_Sci.h"
/*
#pragma CODE_SECTION(eva_timer2_isr, "ramfuncs");
#pragma CODE_SECTION(evb_timer3_isr, "ramfuncs");
#pragma CODE_SECTION(SCI_TX_isr, "ramfuncs");
#pragma CODE_SECTION(SCI_RX_isr, "ramfuncs");
#pragma CODE_SECTION(frequence, "ramfuncs");
#pragma CODE_SECTION(wildth, "ramfuncs");
*/
interrupt void eva_timer2_isr(void);
interrupt void evb_timer3_isr(void);
interrupt void SCI_TX_isr(void); 
interrupt void SCI_RX_isr(void);
void init_eva_timer1(void);
void init_eva_timer2(void);
void init_evb_timer3(void);
void frequence(void);
void wildth(void);
// These are defined by the linker (see F2812_Flash.cmd)
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;

extern unsigned int econst_runstart;
extern unsigned int econst_loadstart;
extern unsigned int econst_loadend;

Uint32  EvaTimer2InterruptCount=0;//定时器2溢出次数
Uint32  EvbTimer3InterruptCount=0;//定时器3溢出次数
double  FR=0.0;//最终被测频率
double  TR=0.0;//最终被测周期
double  WR=0.0;//最终被测脉宽
double  ZW=0.0;//最终占空比
double  FR1=0.0;//单次测得被测频率
double  FR2=0.0;
 int  FR2count=0;
 int  FR4count=0;  
 double   FR5[11];
int  FR6count=0;
double    FR7[3];
int  FR7count=0;
double    FR8[6];
int  FR8count=0;
double    FR9[5];
int  FR9count=0;
double    FR10[11];
int  WR3count=0;
double    WR3[5];
 int  FR5count=0;  
  double   FR55[5];
  double FRR=0.0;
double  WR1=0.0;//单次测得被测脉宽
double  WR2=0.0;
double  ZW1=0.0;
double  WR4=0.0;
double   N=0.0;
volatile Uint32 Ny1=0;//测频率时捕获1捕获的定时器2脉冲数
volatile Uint16 Ny2=0;//捕获1堆栈顶层值
volatile Uint16 Ny3=0;//捕获1堆栈底层值
volatile Uint16 Ny4=0;//捕获4堆栈底层值
volatile Uint16 Ny5=0;//捕获5堆栈底层值
volatile Uint32  Ny7=0;//捕获4，5捕获的定时器3的脉冲数
int flag1=0;//被测信号频率为中频段（小于150/64MH，大于150M/64/50000=46。875）标志
int flag2=0;//被测信号频率为150/64MH以上标志
int flag3=0;//重新配置EV标志
int flag4=0;//定时器1比较中断发生标志
int flag5=0;//大于767953.125H（150M/64/200000*65532）小于150M/64 频率
int flag6=0;//数组满的标志
int temp1=0;//捕获4堆栈顶层值
int temp2=0;//捕获5先发生时，捕获5堆栈顶层应丢失的值，以确保每次都是先读捕获4的 值
int temp3=0;//捕获5堆栈顶层值
double  FR0[7];//求5次频率值后把其装入该数组，以便计算平均值
double   FR3[9];
double  WR0[7];//求5次脉宽值后把其装入该数组，以便计算平均值
unsigned int FRCount=0;//存频率值的次数
unsigned int WRCount=0;//存脉宽值的次数

char tempp1[11],tempp2[11],tempp3[11],tempp4[11];
char message1[11];
char message2[11];
char message3[11];
char message4[11];
char buffer[1];
 void change1()
 {
 long int m=(long int)FR,j=0,t=0,i=0,k=0;
 double n=0.0;
 message1[0]='A';
 do {
     tempp1[j]=(m%10);
      m/=10;
      t=j;
      j++;
     }while(m!=0);
  for(i=1;i<=t+1;i++,j--)
  {
   message1[i]=tempp1[j-1]+0x0030;
    } 
  message1[i++]='.';
 k=(long int)FR;
  n=FR-k;
 do{
 	 n*=10;
    message1[i]=((long  int)(n)%10)+0x0030;
    i++;
    }while(i<11);
}
void change2()
 {
 long int m=(long int)TR,j=0,t=0,i=0,k=0;
 double n=0.0;
 message2[0]='B';
 do {
     tempp2[j]=(m%10);
      m/=10;
      t=j;
      j++;
     }while(m!=0);
  for(i=1;i<=t+1;i++,j--)
  {
   message2[i]=tempp2[j-1]+0x0030;
    } 
  message2[i]='.';
  i=i+1;  
 k=(long int)TR;
  n=TR-k;
 do{
 	 n*=10;
    message2[i]=((long  int)(n)%10)+0x0030;
    i++;
    }while(i<11);
}
void change3()
 {
 long int m=(long int)WR,j=0,t=0,i=0,k=0;
double n=0.0;
 message3[0]='C';
 do {
     tempp3[j]=(m%10);
      m/=10;
      t=j;
      j++;
     }while(m!=0);
  for(i=1;i<=t+1;i++,j--)
  {
   message3[i]=tempp3[j-1]+0x0030;
    } 
  message3[i]='.';
  i=i+1;  
 k=(long int)WR;
  n=WR-k;
 do{
 	 n*=10;
    message3[i]=((long  int)(n)%10)+0x0030;
    i++;
    }while(i<11);
}
void change4()
 {
 long int m=(long int)ZW,j=0,t=0,i=0,k=0;
 double n=0.0;
 message4[0]='D';
 do {
     tempp4[j]=(m%10);
      m/=10;
      t=j;
      j++;
     }while(m!=0);
  for(i=1;i<=t+1;i++,j--)
  {
   message4[i]=tempp4[j-1]+0x0030;
    } 
   
  message4[i]='.';
  i=i+1;  
 k=(long int)ZW;
  n=ZW-k;
 do{
 	 n*=10;
    message4[i]=((long  int)(n)%10)+0x0030;
       
    i++;
    
    }while(i<11);
}
void main(void)
{  
   InitSystem();//系统初始化
   InitGpio();  // GPIO初始化
   DINT;
   InitPieVectTable();//PIE向量表初始化
   InitPieCtrl();

   MemCopy(&econst_loadstart, &econst_loadend, &econst_runstart);
   MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
   InitFlash();//该函数包含在DSP281x_SysCtrl.c中
   IER = 0x0000;
   IFR = 0x0000;
  // 重新设置PIE向量表中定时器2，3的上溢中断入口向量
   EALLOW;  
   PieVectTable.T2OFINT = &eva_timer2_isr;
   PieVectTable.T3OFINT = &evb_timer3_isr;
   PieVectTable.TXAINT = &SCI_TX_isr; 
   PieVectTable.RXAINT = &SCI_RX_isr;
   EDIS;  
//初始化EV
   
   SCI_Init();
   init_eva_timer1();
   init_eva_timer2();
   init_evb_timer3();
   EvaRegs.CAPCONA.all=0x0440;
   EvbRegs.CAPCONB.all=0x0290;   
   EvaRegs.CAPFIFOA.bit.CAP1FIFO = 0;
   EvbRegs.CAPFIFOB.all=0x0500;
//使能PIE中断分组3，4中的定时器溢出中断
   PieCtrlRegs.PIEIER3.all =M_INT4 ;
   PieCtrlRegs.PIEIER4.all = M_INT7;
   PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
   PieCtrlRegs.PIEIER9.bit.INTx1 = 1;
   EnableInterrupts();//使能中断
   IER |= (  M_INT3 | M_INT4|M_INT9);//使能和定时器2，3的上溢中断相连的CPU INT3，INT4中断
    // 使能全局中断以及实时调试中断 
    EINT;   // 使能全局中断位 INTM
    ERTM;   //使能全局实时调试中断 DBGM
//使能定时器1，2，3，捕获1，4，5的使能位，以便它们开始工作         
    EvaRegs.T1CON.bit.TENABLE=1;
    EvaRegs.T2CON.bit.TENABLE=1;
    EvbRegs.T3CON.bit.TENABLE=1;
  	EvaRegs.CAPCONA.bit.CAP12EN=1;
    EvbRegs.CAPCONB.bit.CAP45EN=1;

while(1)
{  
  if(EvaRegs.EVAIFRA.bit.T1CINT== 1)//读定时器1的比较中断标志位
  {
    if(flag4==0)
      {  
          EvaTimer2InterruptCount=0;//如果第一次比较，则清定时器2的溢出次数，并重新开始记录它的溢出次数
          flag4=1;//标志已比较中断过一次
      }
    else
     { 
          flag4=0; //标志第二次发生比较中断
          Ny2=EvaRegs.CAP1FIFO;//读捕获1堆栈的顶层值
   	      Ny3=EvaRegs.CAP1FIFO;//读捕获1堆栈的底层值
		 if((EvaTimer2InterruptCount<=10)&&(N<16383)&&(flag3 == 1)&&(flag1 == 1)&&(flag5==0))
   	   {
   	      EvaRegs.T1PR = 3; //由中频段变高频处理,此时定时器2的溢出次数小于150M/16*61542/767953.125H/65536
          flag3 = 0;//清所有标志，重新复位
          flag1 = 0;
          EvaRegs.T1CNT = 4*N; //将定时器1的计数髦瞪栉咏耸钡闹芷谥担员憧焖僦卦刂芷谥?
          EvaRegs.T1CMPR = 1;  
          EvaRegs.T1CON.bit.TCLD10=2;//将冉霞拇嫫鞯闹卦靥跫栉⒓粗卦?
          EvaRegs.T1CON.bit.TCLD10=0;//将比较寄存器的重载条件设剖髦滴?或等于周期寄存器的值时重载
          EvaRegs.T2CON.bit.TPS=5;
         EvaTimer2InterruptCount=0;
       }
         else     
   	      frequence();//计算频率，周期
   	      EvaTimer2InterruptCount=0;
       }
          EvaRegs.EVAIFRA.bit.T1CINT=1;//清定时器1比较中断标志位
   }
  if(EvbRegs.EVBIFRC.bit.CAP5INT == 1)//读捕获5中断标志位
       {
         if(EvbRegs.EVBIFRC.bit.CAP4INT==1)//判断是否先发生捕获4中断
          { //读捕获4,5堆栈的值
           temp1=EvbRegs.CAP4FIFO;
           Ny4=EvbRegs.CAP4FIFO;
           EvbRegs.EVBIFRC.bit.CAP4INT = 1;//清捕获4中断标志位
           temp3=EvbRegs.CAP5FIFO;
           Ny5=EvbRegs.CAP5FIFO;
           wildth();//计算脉宽
           EvbTimer3InterruptCount=0;
        }
        else
        { 
           temp2=EvbRegs.CAP5FIFO;//如果捕获5中断先发生,则读出捕获5堆栈ゲ慵存器的值并丢?
        }
        EvbRegs.EVBIFRC.bit.CAP5INT = 1;//清捕获5中断标志位
      }
           else
           if(flag6==1)
            { 
              int  i=0,j=0;
                   float32 t=0.0,k1=0.0;
              for(i=0;i<6;i++)    //把数组中频率的值从小到大排序
                 for(j=0;j<6-i;j++)
                   if(FR0[j]>FR0[j+1])
                     {
                       t=FR0[j];
                       FR0[j]=FR0[j+1];
                       FR0[j+1]=t;
                     }
                   
                    FR2=( FR0[2]+FR0[4]+FR0[3] )/3*0.999982;//计算平均值作为最终频率的结果
                   
               if((FR2>37.0&&FR2<46.875))
                 {
                                    
                     FR7[FR6count++]=FR2;
                if(FR6count==4)                         
                   {
                      int  i=0,j=0;
                      float32 t=0.0;
                 for(i=0;i<2;i++)    //把数组中频率的值从小到大排序
                   for(j=0;j<2-i;j++)
                     if(FR7[j]>FR7[j+1])
                     {
                       t=FR7[j];
                       FR7[j]=FR7[j+1];
                       FR7[j+1]=t;
                     }
                    FR=FR7[1];
                    TR=1000000.0/FR;
                                       FR6count=0;
                                    }
             
    }               
      else
	  
             if(FR2>400.0&&FR2<60000.0)
                 {
                                        
                     FR8[FR7count++]=FR2;
                if(FR7count==7)                         
                   {
                      int  i=0,j=0;
                      float32 t=0.0;
                 for(i=0;i<5;i++)    //把数组中频率的值从小到大排序
                   for(j=0;j<5-i;j++)
                     if(FR8[j]>FR8[j+1])
                     {
                       t=FR8[j];
                       FR8[j]=FR8[j+1];
                       FR8[j+1]=t;
                     }
                    FR=FR8[0];
                    TR=1000000.0/FR;
                                       FR7count=0;
                                    }
             
    }               
                   
       else
             if((FR2>1900000.0&&FR2<2100000.0)|(FR2>2800000.0&&FR2<=3100000.0)|(FR2>980000.0&&FR2<1100000.0))
                 {
                                        
                     FR5[FR4count++]=FR2;
                if(FR4count==8)                         
                   {
                      int  i=0,j=0;
                      float32 t=0.0;
                 for(i=0;i<6;i++)    //把数组中频率的值从小到大排序
                   for(j=0;j<6-i;j++)
                     if(FR5[j]>FR5[j+1])
                     {
                       t=FR5[j];
                       FR5[j]=FR5[j+1];
                       FR5[j+1]=t;
                     }
                    FRR=FR5[3];
					 FR4count=0;
					if(FRR>2600000.0&&FRR<=3100000.0)
					{
                       FR55[FR5count++]=FRR;
                if(FR5count==6)                         
                   {
                      int  i=0,j=0;
                      float32 t=0.0;
					for(i=0;i<4;i++)    //把数组中频率的值从小到大排序
                   for(j=0;j<4-i;j++)
                     if(FR55[j]>FR55[j+1])
                     {
                       t=FR55[j];
                       FR55[j]=FR55[j+1];  
                         FR55[j+1]=t;
                     }
				   FR=FR55[4];
					TR=1000000.0/FR;
                    FR5count=0;
					
					}
			}
               else     
			   {
			   FR=FRR;
               	TR=1000000.0/FR;
				 FR4count=0;
			   }
                                                   
                }                    
             
    }               
                   
                  
       else
                    if(FR2>3100000.0)
              {
                                        
                                       FR3[FR2count++]=FR2;
                                       if(FR2count==10)                         
                   {
                      int  i=0,j=0;
                   float32 t=0.0;
              for(i=0;i<8;i++)    //把数组中频率的值从小到大排序
                 for(j=0;j<8-i;j++)
                   if(FR3[j]>FR3[j+1])
                     {
                       t=FR3[j];
                       FR3[j]=FR3[j+1];
                       FR3[j+1]=t;
                     }
                        FR9[FR8count++]=FR3[3];
						 FR2count=0;
						 }
                if(FR8count==6)                         
                   {
                      int  i=0,j=0;
                      float32 t=0.0;
                 for(i=0;i<4;i++)    //把数组中频率的值从小到大排序
                   for(j=0;j<4-i;j++)
                     if(FR9[j]>FR9[j+1])
                     {
                       t=FR9[j];
                       FR9[j]=FR9[j+1];
                       FR9[j+1]=t;
                     }
                    FR=FR9[2];
                    TR=1000000.0/FR;
                                       FR8count=0;
                            }                    
                           
  }

    else
	  
             if(FR2>27000000.0&&FR2<37000000.0)
                 {
                                        
                     FR10[FR9count++]=FR2;
                if(FR9count==12)                         
                   {
                      int  i=0,j=0;
                      float32 t=0.0;
                 for(i=0;i<10;i++)    //把数组中频率的值从小到大排序
                   for(j=0;j<10-i;j++)
                     if(FR9[j]>FR9[j+1])
                     {
                       t=FR9[j];
                       FR9[j]=FR9[j+1];
                       FR9[j+1]=t;
                     }
                    FR=FR10[5];
                    TR=1000000.0/FR;
                                       FR9count=0;
                                    }
             
    }               
       
else         {
                      FR=FR2;
           TR=1000000/FR;
              }
              for(i=0;i<6;i++)    //把数组中脉宽的值从小到大排序
                for(j=0;j<6-i;j++)
                  if(WR0[j]>WR0[j+1])
                    {
                       k1=WR0[j];
                       WR0[j]=WR0[j+1];
                       WR0[j+1]=k1;
                    }
                    WR2=(WR0[2]+WR0[4]+WR0[3])/3;//计算平均值作为最终脉宽的结果
                  if(FR<1001.0&&FR>900.0)
                {
                    WR3[WR3count++]=WR2;
                 if(WR3count==6)                         
                   {
                      int  i=0,j=0;
                      float32 t=0.0;
                 for(i=0;i<4;i++)    //把数组中频率的值从小到大排序
                   for(j=0;j<4-i;j++)
                     if(WR3[j]>WR3[j+1])
                     {
                       t=WR3[j];
                       WR3[j]=WR3[j+1];
                      WR3[j+1]=t;
                     }
                    WR4=WR3[4];
                   	  ZW1=WR4/TR*100;
                     WR3count=0;
                     
                     }
                
                    if(ZW1>0&&ZW1<100.0) 
				   {
				    WR=WR4;
                    ZW=ZW1;
					EvbTimer3InterruptCount=0;
				   }
			}
			else
			  {
			    ZW1=WR2/TR*100;
                 if(ZW1>0&&ZW1<100.0) 
				   {
				    WR=WR2;
                    ZW=ZW1;
				   }

}

        EvbRegs.CAPCONB.bit.CAP45EN=1;//鼓懿获4,5,
        EvaRegs.EVAIFRA.bit.T1CINT=1; //清定时器1的比较中断标志位
        FRCount=0;//重新开始装载数组
        WRCount=0;
        flag6=0;
    }
  else {  change1();
     change2();
     change3();
     change4();}//等待触发
 }
}

 void init_eva_timer1(void)//定时器1初始化
{	

    EvaRegs.GPTCONA.all= 0x6052;//将定时器1,2配置成递增计数模式
   //没有事件启动ADC,定时器1,2的比较输出由各定时器比较逻辑独立驱动
   //定时器1比较输出设为高有效
    EvaRegs.T1PR = 3;       // 周期值
    EvaRegs.T1CMPR = 1;     // 比较值
    EvaRegs.EVAIFRA.bit.T1CINT= 1;//复位定时器1的比较中断标志
    EvaRegs.EVAIFRC.bit.CAP1INT = 1;//复位捕获单元1的中断标志
    EvaRegs.EVAIMRA.bit.PDPINTA= 0;//禁止功率驱动保护中断
    EvaRegs.EVAIMRA.bit.T1CINT= 0;//禁止定时器1的比较中断使能
    EvaRegs.EVAIMRC.bit.CAP1INT = 0;//禁止捕获单元1的中断使能
    EvaRegs.T1CNT= 0x0000;//定时器1的计数值初始化为0 
    EvaRegs.T1CON.all = 0x1012;//将定时器1配置为仿真挂起时立即停止,
    //连续增计数模式,禁止定时器1操作,用外部时钟,比较寄存器设为计数器值为0或等于周期寄存器的值时重载  
    //使能定时器的比较操作
}

void init_eva_timer2(void)//定时器2的初始化
{
    EvaRegs.GPTCONA.all=0x6052;
    EvaRegs.T2PR= 0xFFFF;       // 周期值
    EvaRegs.T2CNT = 0x0000; //初始化计数值
    EvaRegs.EVAIFRB.bit.T2OFINT= 1; //复位定时器2的上溢中断标志
    EvaRegs.EVAIMRB.bit.T2OFINT = 1;//使能定时器2的上溢中断使能
    EvaRegs.T2CON.all= 0x1502;//将定时器2配置为仿真挂起时立即停止,
    //连续增计数模式,预分频设为32分频,使用自硎鼓芪?禁止定时器2操作
    //使用内部时钟,比较寄存器设为计数器值为0或等于周期寄存器的值时重载
    //使用自身的周期寄存器
}

void init_evb_timer3(void)//定时器3的初始化
{
    EvbRegs.GPTCONB.all= 0x6052;//将定时器3,4配置成递增计数模式
   //没有事件启动ADC,定逼?,4的比较输出由各定时器比较逻辑独立驱?
   //定时器3比较输出设为高有效
    EvbRegs.T3PR= 0xFFFF;       // 周期值
    EvbRegs.T3CNT = 0x0000; //初始化计数值
    EvbRegs.EVBIFRA.bit.T3OFINT= 1; //复位定时器3的上溢中断标志
    EvbRegs.EVBIFRC.bit.CAP4INT = 1;//复位捕获单元4的中断标志
    EvbRegs.EVBIFRC.bit.CAP5INT= 1;//复位捕获单元5的中断标志
	EvbRegs.EVBIMRA.bit.PDPINTB= 0;//禁止功率驱动保护中断
    EvbRegs.EVBIMRA.bit.T3OFINT= 1;//使能定时器3的上溢中断使能 
    EvbRegs.EVBIMRC.bit.CAP4INT = 0;//禁止捕获单元4的中断使能
    EvbRegs.EVBIMRC.bit.CAP5INT= 0;//禁止捕获单元5的中断使能
    EvbRegs.T3CON.all= 0x1002;//将定时器3配置为仿真挂起时立即停止,
    //连续增计数模式,预分频设为0分频,禁止定时器3操作,使用内部时钟,
    //比较寄存器设为计数器值为0或等于周期寄存器的值时重载
}

void frequence(void)//计算频率
{    //计算捕获1捕获的定时器2的脉冲数
    if(Ny2>Ny3)
	    Ny1=(65535-Ny2)+Ny3+65536*(EvaTimer2InterruptCount-1);
	    else 
	    Ny1=Ny3-Ny2+65536*EvaTimer2InterruptCount;
	  if(flag3 == 0)   //判断被测信号频率范围   
         { 
           if(Ny1>1041666)
            {
            FR=9375000.0/Ny1;
            TR=1000000.0/FR;
             EvaTimer2InterruptCount=0;
            }
           if(Ny1>=200000&&Ny1<=1041666) //频率小于46.875Hz,直接计算
            {     
               FR1=9375000.0/Ny1;//FR1=1000000.0/Ny1/64*150*3;
               FR0[FRCount++]=FR1;
               EvaTimer2InterruptCount=0;
	           if(FRCount==7)//如果已计算5个值,即数组已满,则禁止定时器1的比较操作
	           { 
	            flag6=1;
	          //  EvaRegs.EVAIFRA.bit.T1CINT=0;
	           }
	           else
	             flag6=0;
       	   }     	
		  else if( Ny1>=4&&Ny1<200000)//被测频率小于150/64MH，大于150M/64/50000=46。875Hz
     	    {
       		 	if(flag1 == 0)//大于46.875H小于150M/64频率
       		 	{
       		 	    flag3 = 1;
       		 		flag1 = 1;   		        	
                   	N=200000.0/ Ny1;
				   if(N-(int)N==0.0&&N<16383)
				   {
				     EvaRegs.T1PR=4*N-1;// N=15384.6,T1PR最大为61542
     		      	   EvaRegs.T1CMPR=2*N-1;
     		      	   EvaRegs.T2CON.bit.TPS=3;
					    EvaTimer2InterruptCount=0;
				   
				   }  
                else   if(N<16383&&N-(int)N>0.0)	 //重新配置定时器1
                   {   EvaRegs.T1PR=4*N+3;// N=15384.6,T1PR最大为61542
     		      	   EvaRegs.T1CMPR=2*N+1;
     		      	   EvaRegs.T2CON.bit.TPS=3;//定时器2输入时钟预定标设为8分频
     		        EvaTimer2InterruptCount=0;
     		       }
                   else if(flag5==0) //大于767953.125H（150M/64/200000*65532）小于150M/64 频率 
                   {
                       flag5=1;
                       EvaRegs.T1PR=65529;
                       EvaRegs.T1CMPR=32764;
                	   EvaRegs.T2CON.bit.TPS=0;
					    EvaTimer2InterruptCount=0;
  			       }
                }
           }
             else
             {
 				   if(flag2==0)//大于150M/64 频率
 				 {   
 				       flag3 = 1;
 					   flag2 = 1;         
                   	   EvaRegs.T1PR=65529;
                       EvaRegs.T1CMPR=32764;
                	   EvaRegs.T2CON.bit.TPS=0;
					    EvaTimer2InterruptCount=0;
                 } 
             }
       }    
    else	    
	   if(flag3==1) //重新配置中频和高频之后重新计算
	     {
	        if(flag1 == 1)
              { 
                 if(flag5==1)
	              { 
	                FR1=4914750.0/Ny1*1000000;//FR1=150.0/2*65530/Ny1*1000000;
	                FR0[FRCount++]=FR1;
                    EvaTimer2InterruptCount=0;
	                if(FRCount==7)
	                { 
	                 flag6=1;
	           //      EvaRegs.EVAIFRA.bit.T1CINT=0;
	                }
	                else
	                flag6=0;
	             }            
                 else
                 { 
                  FR1=9375000.0/Ny1*(EvaRegs.T1PR+1);//FR1=1000000.0/Ny1/16*150*4*(N+1);
                  FR0[FRCount++]=FR1;
				   EvaTimer2InterruptCount=0;
	              if(FRCount==7)
	               {
	                flag6=1;
	          //      EvaRegs.EVAIFRA.bit.T1CINT=0;
	               }
	              else
	                flag6=0;
                 }
             }   
         else 
            if(flag2 == 1)  
             {   
                    FR1=4914750.0/Ny1*1000000;//FR1=150.0/2*65530/Ny1*1000000;
	                FR0[FRCount++]=FR1;
					 EvaTimer2InterruptCount=0;
	                if(FRCount==7)
	                {
	                flag6=1;
	       //         EvaRegs.EVAIFRA.bit.T1CINT=0;
	                }
	                else
	                flag6=0;
	         }
	     }
}
 void wildth(void) //计算脉宽
 {    ////计算捕获4，5捕获的定时器3的脉冲数   
        if(Ny4>Ny5)
	      Ny7=(65535-Ny4)+Ny5+65536*(EvbTimer3InterruptCount-1);
	    else 
	      Ny7=Ny5-Ny4+65536*EvbTimer3InterruptCount;
	    WR1=Ny7/75.0;//WR1=Ny7*2.0/150;计算脉宽
		if(Ny1>1041666&&flag3==0) 
		{
		  WR=WR1;
		  ZW=WR/TR*100;
                 EvbTimer3InterruptCount=0;
		}
	  else
	    if(WRCount==7)//如果已计算了5个值,即数组已满,则禁止捕获4，5操作，否则继续存结果
	     {   
	      EvbRegs.CAPCONB.bit.CAP45EN=0;
		  EvbTimer3InterruptCount=0;
	     }
	     else
	      WR0[WRCount++]=WR1; 
} 

interrupt void eva_timer2_isr(void)//定时器2溢出中断程序
   { 
     if((EvaTimer2InterruptCount>98)&&(flag3 ==1)&&((flag2 ==1)|(flag5==1)))
       {
          EvaRegs.T1PR = 3; //   75M*65530/65536/767953.125H，由767953.125H以上变低频处理，复位EV模块
          flag3 = 0;
          flag2 = 0;
          flag1 = 0;
          flag4 = 0;
          flag5 = 0;
          EvaRegs.T1CNT = 65525;
          EvaRegs.T1CMPR = 1;
          EvaRegs.T1CON.bit.TCLD10=2;
          EvaRegs.T1CON.bit.TCLD10=0;
          EvaRegs.T2CON.bit.TPS=5;
          EvaTimer2InterruptCount=0;
       }
 else
   if((EvaTimer2InterruptCount>=25)&&(flag3 ==1)&&(flag1 ==1)&&(flag5==0))
    { 
      EvaRegs.T1PR = 3; // 150M/16*8/65536/46.875，由中频变低频处理
      flag3 = 0;
      flag1 = 0;
      flag4 = 0;
      EvaRegs.T1CNT = 4*N;
      EvaRegs.T1CMPR = 1;
      EvaRegs.T1CON.bit.TCLD10=2;
      EvaRegs.T1CON.bit.TCLD10=0;
      EvaRegs.T2CON.bit.TPS=5;
      EvaTimer2InterruptCount=0;
    }
  else   
   {
   EvaTimer2InterruptCount++;
   }
   EvaRegs.EVAIFRB.bit.T2OFINT= 1;//复位定时器2的上溢中断标志
   PieCtrlRegs.PIEACK.all= PIEACK_GROUP3;//清除PIE分组3械牡闹卸舷煊ξ?
}
interrupt void evb_timer3_isr(void)//定时器3溢出中断
{  
  if(EvbRegs.EVBIFRC.bit.CAP4INT==1)//如果捕获4捕获中断标志已置位，则定时器3溢出次数加1
    { 
    EvbTimer3InterruptCount++;
    }
  else
    {
     EvbTimer3InterruptCount=0;
    }  
  EvbRegs.EVBIFRA.bit.T3OFINT = 1;//复位定时器3的上溢中断标志
  PieCtrlRegs.PIEACK.all= PIEACK_GROUP4;//清除PIE分组4中的的中断响应位
}

interrupt void SCI_TX_isr(void)
{
    int i=0;
	if(buffer[0]=='1')
	for(i=0;i<11;i++) SciaRegs.SCITXBUF=message1[i];
	else	if(buffer[0]=='2')
	for(i=0;i<11;i++)	SciaRegs.SCITXBUF=message2[i];
	else	if(buffer[0]=='3')	
    for(i=0;i<11;i++) SciaRegs.SCITXBUF=message3[i];
	else	if(buffer[0]=='5')
	for(i=0;i<11;i++) SciaRegs.SCITXBUF=message4[i];

	
	/* 重新初始化PIE模块，为下一蜸CI-A中断作准备*/
    PieCtrlRegs.PIEACK.all = 0x0100;   /* 清除对应PIE分组的中断响应位*/
}
/* SCI-A数据接收中断服务函数*/
interrupt void SCI_RX_isr(void)
{
	int i;
	
	for (i=0;i<1;i++) buffer[i]= SciaRegs.SCIRXBUF.all;
	if ((strncmp(buffer, "1") == 0)||(strncmp(buffer,"2")==0)||(strncmp(buffer,"3")==0)||(strncmp(buffer,"4")==0)||(strncmp(buffer,"5")==0))
	{
		SciaRegs.SCIFFTX.bit.TXINTCLR = 1 ;	
		SciaRegs.SCIFFTX.bit.TXFIFOXRESET =1;	
	}	
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;	/* 复位接收FIFO的指针*/  
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;	/* 使能接收FIFO操作*/
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;    /* 清除RXFIFINT中断标志位*/
	PieCtrlRegs.PIEACK.all = 0x0100;   /* 清除对应PIE分组的中断响应位*/
} 

//===========================================================================
// No more.
//===========================================================================
