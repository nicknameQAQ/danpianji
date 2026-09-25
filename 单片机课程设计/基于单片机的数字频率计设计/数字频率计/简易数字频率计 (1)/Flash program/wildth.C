void init_evb_timer3(void)//定时器3的初始化
{
    EvbRegs.GPTCONB.all= 0x6052;//将定时器3,4配置成递增计数模式
   //没有事件启动ADC,定时器3,4的比较输出由各定时器比较逻辑独立驱动
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

 void wildth(void) //计算脉宽
 {    ////计算捕获4，5捕获的定时器3的脉冲数   
        if(Ny4>Ny5)
	      Ny7=(65535-Ny4)+Ny5+65536*(EvbTimer3InterruptCount-1);
	    else 
	      Ny7=Ny5-Ny4+65536*EvbTimer3InterruptCount;
	    WR1=Ny7/75.0;//WR1=Ny7*2.0/150;计算脉宽
		if(Ny1>781250&&flag3==0) 
		{
		  WR=WR1;
		  ZW=WR/TR*100;
		}
	  else
	    if(WRCount==7)//如果已计算了5个值,即数组已满,则禁止捕获4，5操作，否则继续存结果
	     {   
	      EvbRegs.CAPCONB.bit.CAP45EN=0;
	     }
	     else
	      WR0[WRCount++]=WR1; 
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


