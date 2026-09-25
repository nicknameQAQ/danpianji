#ifndef celiang_h
  #define celiang_h
//*********补偿系数
  #define bch 0x24
//*********测量模式
  #define try 0
  #define cef 1
  #define cet 2
//*********函数声明
  void refish();  //刷新所有的东西
  uchar hz_try(); //估计范围 1k

  void cl_f(); //频率法

  void cl_t(); //周期法

#endif
