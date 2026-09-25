#include "DSP281x_Device.h"     // DSP281x Headerfile Include File
#include "DSP281x_Examples.h"   // DSP281x Examples Include File
interrupt void eva_timer2_isr(void);
void init_eva_timer1(void);
void init_eva_timer2(void);
void frequence(void);
Uint32  EvaTimer2InterruptCount=0;//定时器2溢出次数
double  FR=0.0;//最终被测频率
double  TR=0.0;//最终被测周期
double  FR1=0.0;//单次测得被测频率
double   N=0.0;
volatile Uint32 Ny1=0;//测频率时捕获1捕获的定时器2脉冲数
volatile Uint16 Ny2=0;//捕获1堆栈顶层值
volatile Uint16 Ny3=0;//捕获1堆栈底层值
int flag1=0;//被测信号频率为中频段（小于150/64MH，大于150M/64/50000=46。875）标志
int flag2=0;//被测信号频率为150/64MH以上标志
int flag3=0;//重新配置EV标志
int flag4=0;//定时器1比较中断发生标志
int flag5=0;//大于767953.125H（150M/64/200000*65532）小于150M/64 频率
int flag6=0;//数组满的标志
double  FR0[7];//求5次频率值后把其装入该数组，以便计算平均值
unsigned int FRCount=0;//存频率值的次数

interrupt void evb_timer3_isr(void);
void init_evb_timer3(void);
void wildth(void);
Uint32  EvbTimer3InterruptCount=0;//定时器3溢出次数
double  WR=0.0;//最终被测脉宽
double  ZW=0.0;//最终占空比
double  WR1=0.0;//单次测得被测脉宽
double  WR2=0.0;
double  ZW1=0.0;
volatile Uint16 Ny4=0;//捕获4堆栈底层值
volatile Uint16 Ny5=0;//捕获5堆栈底层值
volatile Uint32  Ny7=0;//捕获4，5捕获的定时器3的脉冲数
int temp1=0;//捕获4堆栈顶层值
int temp2=0;//捕获5先发生时，捕获5堆栈顶层应丢失的值，以确保每次都是先读捕获4的 值
int temp3=0;//捕获5堆栈顶层值
double  WR0[7];//求5次脉宽值后把其装入该数组，以便计算平均值
unsigned int WRCount=0;//存脉宽值的次数
//interrupt void SCI_TX_isr(void); 
//interrupt void SCI_RX_isr(void);
/*char tempp1[32],tempp2[32];
char message1[64];
char message2[64];
char message3[64];
char message5[64];
 int m=(int)FR,j=0,t=0,q=10,i=0;
 float n=0.0,p=0.0;
 do {
     tempp1[j]=(m%10+30);
     m/=10;
     t=j;
     j++;
     
     }while(m!=0);
  for(i=0;i<=t;i++,j--)
  {
   message1[i]=tempp1[j-1];
    } 
    t++;

  message1[t]=46;
  t=t+1;
  
  n=FR-(int)FR;
 do{
    message1[t]=((int)(n*q)%10+30);
    p=n*q-(int)(n*q);
    q*=10;
    t++;
    }while(p!=0.0);
char buffer[]={" "};*/
void main(void)
{
   InitSystem();//系统初始化
   InitGpio();  // GPIO初始化
   DINT;
   InitPieVectTable();//PIE向量表初始化
   InitPieCtrl();
   IER = 0x0000;
   IFR = 0x0000;
  // 重新设置PIE向量表中定时器2，3的上溢中断入口向量
   EALLOW;  
   PieVectTable.T2OFINT = &eva_timer2_isr;
   PieVectTable.T3OFINT = &evb_timer3_isr;
 //  PieVectTable.TXAINT = &SCI_TX_isr; 
 //  PieVectTable.RXAINT = &SCI_RX_isr;
   EDIS;  
//初始化EV
 //  SCI_Init();
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
  // PieCtrlRegs.PIEIER9.bit.INTx2 = 1;
  // PieCtrlRegs.PIEIER9.bit.INTx1 = 1;
   EnableInterrupts();//使能中断
   IER |= (  M_INT3 | M_INT4);//|M_INT9使能和定时器2，3的上溢中断相连的CPU INT3，INT4中断
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
   	      EvaRegs.T1PR = 2; //由中频段变高频处理,此时定时器2的溢出次数小于150M/16*61542/767953.125H/65536
          flag3 = 0;//清所有标志，重新复位
          flag1 = 0;
          EvaRegs.T1CNT = 4*N; //将定时器1的计数器值设为接近此时的周期值，以便快速重载周期值
          EvaRegs.T1CMPR = 1;  
          EvaRegs.T1CON.bit.TCLD10=2;//将比较寄存器的重载条件设为立即重载
          EvaRegs.T1CON.bit.TCLD10=0;//将比较寄存器的重载条件设为计数器值为0或等于周期寄存器的值时重载
          EvaRegs.T2CON.bit.TPS=5;
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
        }
        else
        { 
           temp2=EvbRegs.CAP5FIFO;//如果捕获5中断先发生,则读出捕获5堆栈顶层寄存器的值并丢失
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
                    FR=( FR0[2]+FR0[4]+FR0[3] )/3*0.999983;//计算平均值作为最终频率的结果
                    TR=1000000/FR;
              for(i=0;i<6;i++)    //把数组中脉宽的值从小到大排序
                for(j=0;j<6-i;j++)
                  if(WR0[j]>WR0[j+1])
                    {
                       k1=WR0[j];
                       WR0[j]=WR0[j+1];
                       WR0[j+1]=k1;
                    }
                    WR2=(WR0[2]+WR0[4]+WR0[3])/3;//计算平均值作为最终脉宽的结果
                    ZW1=WR2/TR*100;
				/*	if(FR>45000&&ZW1<25.0)
					{
					WR2=WR2/0.975;
					ZW1=ZW1/0.975;
					}*/
                    if(ZW1>0&&ZW1<100.0) 
				   {
				    WR=WR2;
                    ZW=ZW1;
				   }
        EvbRegs.CAPCONB.bit.CAP45EN=1;//鼓懿获4,5,
        EvaRegs.EVAIFRA.bit.T1CINT=1; //清定时器1的比较中断标志位
        FRCount=0;//重新开始装载数组
        WRCount=0;
        flag6=0;
    }
  else {;}//等待触发
 }
}
/*interrupt void SCI_TX_isr(void)
{
  int j=0, i=0,p=0,q=0,m=0;
  message1[j]='A';
  
   do {   
        j++;
        pt= Change(FR);
        message1[j]=*pt;
        pt++;    
      }while(*pt!='\0');
     
     message2[p]='B';  
  do {   
        p++;
        pt= Change(TR);
        message2[p]=*pt;
        pt++;    
      }while(*pt!='\0');
    
   message3[q]='C';
   do {   
        q++;
        pt=Change(WR);
        message3[q]=*pt;
        pt++;    
      }while(*pt!='\0');
    
   message5[m]='D';
   do {   
        m++;
        pt= Change(ZW);
        message5[m]=*pt;
        pt++;    
      }while(*pt!='\0');


    if(strncmp(buffer,"1")==0)
       for( i=0;i<strlen(message1);i++)
	{
	SciaRegs.SCITXBUF=message1[i];
	}

   if(strncmp(buffer,"2")==0)
       for( i=0;i<strlen(message2);i++)
	{
	SciaRegs.SCITXBUF=message2[i];
	}
  if(strncmp(buffer,"3")==0)
       for( i=0;i<strlen(message3);i++)
	{
	SciaRegs.SCITXBUF=message3[i];
	}
  if(strncmp(buffer,"5")==0)
       for( i=0;i<strlen(message5);i++)
	{
	SciaRegs.SCITXBUF=message5[i];
	}



*/
	/* 重新初始化PIE模块，为下一次SCI-A中断作准备*/
 //   PieCtrlRegs.PIEACK.all = 0x0100;   /* 清除对应PIE分组的中断响应位*/
//}   
/* SCI-A数据接收中断服务函数*/
/*interrupt void SCI_RX_isr(void)
{
	int i=0;
	
	 buffer[i]= SciaRegs.SCIRXBUF.all;
	if ((strncmp(buffer, "1",13) == 0)||(strncmp(buffer,"2")==0)||(strncmp(buffer,"3")==0)
	||(strncmp(buffer,"5")==0))
	{
		SciaRegs.SCIFFTX.bit.TXINTCLR = 1 ;	
		SciaRegs.SCIFFTX.bit.TXFIFOXRESET =1;	
	}	
	SciaRegs.SCIFFRX.bit.RXFIFORESET = 0;*/	/* 复位接收FIFO的指针*/  
//	SciaRegs.SCIFFRX.bit.RXFIFORESET = 1;	/* 使能接收FIFO操作*/
//	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;    /* 清除RXFIFINT中断标志位*/
//	PieCtrlRegs.PIEACK.all = 0x0100;   /* 清除对应PIE分组的中断响应位*/
//}


//===========================================================================
// No more.
//===========================================================================
