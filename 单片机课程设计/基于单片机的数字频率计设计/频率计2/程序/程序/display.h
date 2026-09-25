#ifndef display_h
  #define display_h

//*******
  #define in 0
  #define out 0xff
//*********
  #define data_port  PA     //pa口线
  #define data_direc DDRA
  #define data_in    PINA
  #define data_out  PORTA
  #define rs 5              //接到pb
  #define rw 6
  #define e  7
//****************
  void clearall( void ); //清屏指令
  void yj_set( uchar xsh, uchar gb, uchar dt ); //显示设置
  void makecg( uchar address, uchar* cg ); //自造字库
  void showstr( uchar* str, uchar xpot, uchar ypot, uchar num ); //液晶显示

#endif
