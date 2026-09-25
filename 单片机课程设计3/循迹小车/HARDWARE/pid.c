#include<pid.h>
#include "STC12C5A.h"
#include "ioConfig.h"
#include "string.h"
#include "Stdio.h"
#include "absacc.h"
#include "intrins.h"


xdata struct PID spid; // PID Control Structure
unsigned int rout; // PID Response (Output) 
unsigned int rin; // PID Feedback (Input)
unsigned int temper;
unsigned int set_temper;

/************************************************
              PID函数
*************************************************/ 
void PIDInit (struct PID *pp) 
{ 
  memset ( pp,0,sizeof(struct PID)); 
} 
/************************************************
              增量控制PID函数体 
51单片机最不擅长浮点数计算，转换成int型计算
*************************************************/
unsigned int PIDCalc( struct PID *pp, unsigned int NextPoint ) 
{ 
  unsigned int dError,Error,pError; 
  //增量法计算公式：
  //Pdt=Kp*[E(t)-E(t-1)]+Ki*E(t)+Kd*[E(t)-2*E(t-1)+E(t-2)]
  Error = set_temper - NextPoint;       // 偏差E(t) 
  pError=Error-pp->LastError;	 //E(t)-E(t-1)
  dError=Error-2*pp->LastError+pp->PrevError; //E(t)-2*E(t-1)+E(t-2) 
  pp->PrevError = pp->LastError; 
  pp->LastError = Error; 
  return ( 
            pp->Proportion * pError        //比例 
            + pp->Integral *Error  //积分项 
            + pp->Derivative * dError	  // 微分项
			);  
} 

/************************************************
				PID函数初始化
*************************************************/
void PIDBEGIN() 
{ 
  PIDInit(&spid); // Initialize Structure 
  spid.Proportion = 10; // Set PID Coefficients 
  spid.Integral = 5; 
  spid.Derivative =4; 
}