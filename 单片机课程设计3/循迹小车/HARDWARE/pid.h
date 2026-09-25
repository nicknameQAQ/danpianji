#ifndef  __PID_H__
#define  __PID_H__

struct PID {
unsigned int Proportion; // 比例常数 Proportional Const
unsigned int Integral; // 积分常数 Integral Const
unsigned int Derivative; // 微分常数 Derivative Const
unsigned int LastError; // Error[-1]
unsigned int PrevError; // Error[-2]
};

extern struct PID;                                                     
extern xdata struct PID spid; // PID Control Structure

extern unsigned int rout; // PID Response (Output) 
extern unsigned int rin; // PID Feedback (Input)
extern unsigned int temper;
extern unsigned int set_temper;


void PIDBEGIN();
void PIDInit (struct PID *pp);
unsigned int PIDCalc( struct PID *pp, unsigned int NextPoint );



#endif