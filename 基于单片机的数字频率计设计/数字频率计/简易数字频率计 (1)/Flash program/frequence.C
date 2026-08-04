 
 
 
 
 void init_eva_timer1(void)//定时器1初始化
{	

    EvaRegs.GPTCONA.all= 0x6052;//将定时器1,2配置成递增计数模式
   //没有事件启动ADC,定时器1,2的比较输出由各定时器比较逻辑独立驱动
   //定时器1比较输出设为高有效
    EvaRegs.T1PR = 2;       // 周期值
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
    //连续增计数模式,预分频设为32分频,使用自身使能位,禁止定时器2操作
    //使用内部时钟,比较寄存器设为计数器值为0或等于周期寄存器的值时重载
    //使用自身的周期寄存器
}
void frequence(void)//计算频率
{    //计算捕获1捕获的定时器2的脉冲数
    if(Ny2>Ny3)
	    Ny1=(65535-Ny2)+Ny3+65536*(EvaTimer2InterruptCount-1);
	    else 
	    Ny1=Ny3-Ny2+65536*EvaTimer2InterruptCount;
	  if(flag3 == 0)   //判断被测信号频率范围   
         { 
           if(Ny1>781250)
            {
            FR=7031250.0/Ny1;
            TR=1000000.0/FR;
            }
           if(Ny1>=150000&&Ny1<=781250) //频率小于46.875Hz,直接计算
            {     
               FR1=7031250.0/Ny1;//FR1=1000000.0/Ny1/64*150*3;
               FR0[FRCount++]=FR1;
	           if(FRCount==7)//如果已计算了5个值,即数组已满,则禁止定时器1的比较操作
	           { 
	            flag6=1;
	          //  EvaRegs.EVAIFRA.bit.T1CINT=0;
	           }
	           else
	             flag6=0;
       	   }     	
		  else if( Ny1>=3&&Ny1<150000)//被测频率小于150/64MH，大于150M/64/50000=46。875Hz
     	    {
       		 	if(flag1 == 0)//大于46.875H小于150M/64频率
       		 	{
       		 	    flag3 = 1;
       		 		flag1 = 1;   		        	
                   	N=150000.0/ Ny1;
                   if(N<16383)	 //重新配置定时器1
                   {   EvaRegs.T1PR=4*N+3;// N=15384.6,T1PR最大为61542
     		      	   EvaRegs.T1CMPR=2*N+1;
     		      	   EvaRegs.T2CON.bit.TPS=3;//定时器2输入时钟预定标设为8分频
     		       }
                   else if(flag5==0) //大于767953.125H（150M/64/200000*65532）小于150M/64 频率 
                   {
                       flag5=1;
                       EvaRegs.T1PR=65529;
                       EvaRegs.T1CMPR=32764;
                	   EvaRegs.T2CON.bit.TPS=0;
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
interrupt void eva_timer2_isr(void)//定时器2溢出中断程序
   { 
     if((EvaTimer2InterruptCount>98)&&(flag3 ==1)&&((flag2 ==1)|(flag5==1)))
       {
          EvaRegs.T1PR = 2; //   75M*65530/65536/767953.125H，由767953.125H以上变低频处理，复位EV模块
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
      EvaRegs.T1PR = 2; // 150M/16*8/65536/46.875，由中频变低频处理
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
