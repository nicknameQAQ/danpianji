#include "DSP281x_Device.h"
#include "sci.h"

			   
/*****************SCI参数全局变量***************/
volatile SCI_PARA sci_para;
/***********************************************/
/* 全局变量的定义*/
volatile char receive_buf[RBUF_LEN]={0};
volatile char send_command_buf[5]={0xff,0xff,0xff,0xff,0xff};
//ouput_buf[0] output_buf[1]:start flag, Inf -Inf
//output_buf[2]:error
//output_buf[3]:sample frequency
//output_buf[4]:feedback data type
DECOV_DATA output_buf[55]; 

/***********************************************/
//extern SGA_PARA sgapara;
//extern ADC_PARA adcpara;
//extern ANF_PARA anfpara;
/*****************初始化************************/
void SCI_Init(void)
{	
/*	sci_para.work_status = WORK_OFF;
	sci_para.sample_rate = 800.017;
	sci_para.feedback_data_type = PHASEINTERVAL;
	sci_para.last_command = 0;	
	sci_para.error = NO_ERROR;
	sci_para.receive_data_len = 0;
	sci_para.send_data_len = 0;
	sci_para.receive_buf_pointer = 0;
	sci_para.send_buf_pointer = 0;	
	sci_para.add_data_type = PHASEINTERVAL;
	sci_para.add_data_pointer = 2;
	sci_para.sci_status = WORK_OFF;
	sci_para.add_check = 0;
	output_buf[0].hldata.datal = 0x0; 	//'frame start'
	output_buf[0].hldata.datah = 0x7f80;//Inf
	output_buf[1].hldata.datal = 0x0;
	output_buf[1].hldata.datah = 0xff80;//-Inf
	output_buf[2].data = 0; 			//NO ERROR
	output_buf[3].data = sci_para.sample_rate;
	output_buf[4].data = PHASEINTERVAL; //
	*/
	
		
	EALLOW;
	GpioMuxRegs.GPFMUX.bit.SCIRXDA_GPIOF5 = 1; //外设功能SCI-RX引脚
	//GpioMuxRegs.GPFDIR.bit.GPIOF5 = 0;
    GpioMuxRegs.GPFMUX.bit.SCITXDA_GPIOF4 = 1; //外设功能SCI-TX引脚
	//GpioMuxRegs.GPFDIR.bit.GPIOF4 = 1;
	/*对SCI发送和接收中断的入口地址重新进行映射*/    	
	PieVectTable.TXAINT = &SCI_TX_isr; 
	PieVectTable.RXAINT = &SCI_RX_isr;
	EDIS;


	SciaRegs.SCICCR.all =0x0007;
	/* 1个停止位，无回送模式，*/
    /* 禁止奇偶校验，8位字符长度，*/
    /* 使用空闲线模式协议*/	
	SciaRegs.SCICTL1.all =0x0003;
	/* 使能TX和X操作 */
    /* 禁止接收错误中断和休眠模式*/
   	SciaRegs.SCIHBAUD = 0;
	// 波特率为115200; LSPCLK = 37.5MHz, infact is 114329
	SciaRegs.SCILBAUD = 40;	
	SciaRegs.SCICTL2.bit.TXINTENA = 0; //disable tx interrupt
	/*使能SCI-A用于数据发送的中断 */
	SciaRegs.SCICTL2.bit.RXBKINTENA = 0; //enable rx interrupt
	/*使能SCI-A用于数据接收的中断*/
	SciaRegs.SCIFFTX.all = 0xC064;
	/*位15 = 1 : 将发送FIFO从复位状态释放*/
	/*位14 = 1 : 使能FIFO 增强功能*/
	/*位6 = 1 :  清除TXFFINT标志位*/
	/*位5 = 0 :  不使能基于TX FIFO匹配的中断*/
	//位4-0:TX FIFO中断级别为4（决定匹配条件），
	//即发送到还剩4个字符时产生中断
	SciaRegs.SCIFFRX.all = 0xE061;
	/* 接收FIFO的中断级别为1,即接收到1个字符后产生中断*/	
	SciaRegs.SCICTL1.bit.SWRESET = 1;
	/* 将 SCI 模块从复位状态释放*/
	/*********************************/
    /* 使能PIE中的SCI_A_TX_INT中断*/
    PieCtrlRegs.PIEIER9.bit.INTx2 = 1;  
    /* 使能PIE中的SCI_A_RX_INT中断*/
    PieCtrlRegs.PIEIER9.bit.INTx1 = 1;
    /************************************/
	/* 使能CPU级中断 INT 9*/
    IER |= 0x100;

	RESET_TX; //stop send	
	asm(" nop");	
	START_TX; //start send
}


/********************** SCI-A数据发送中断服务函数**********************/
interrupt void SCI_TX_isr(void)
{
	unsigned int i;
	char temp;

    if(sci_para.send_data_len>0)
	{					
		for(i=0; i<3; i++)
		{		
			if(sci_para.send_buf_pointer>=5) //output_data中第0和1个数为标志
			{
				sci_para.send_data_len -= 1;
			}
			temp = output_buf[sci_para.send_buf_pointer].hldata.datal;//发送数据
			SciaRegs.SCITXBUF = (temp&0xff);
			SciaRegs.SCITXBUF = ((temp>>8)&0xff);	
			temp = output_buf[sci_para.send_buf_pointer].hldata.datah;
			SciaRegs.SCITXBUF = (temp&0xff);
			SciaRegs.SCITXBUF = ((temp>>8)&0xff);
			
			sci_para.send_buf_pointer++;			
			
			if(sci_para.send_buf_pointer>=55) sci_para.send_buf_pointer=0;			
			
			if(sci_para.send_data_len == 0)
			{				
				STOP_FFINT; 
				//stop FIFO TX interrupt, don't reset FIFO TX, because there may be data to send
				sci_para.sci_status = WORK_OFF;
				break;
			}			 
		}
	}
	else
	{
		STOP_FFINT;
		sci_para.sci_status = WORK_OFF;
	}		 

	SciaRegs.SCIFFTX.bit.TXINTCLR = 1 ;	
	/* 重新初始化PIE模块，为下一次SCI-A中断作准备*/
    PieCtrlRegs.PIEACK.all = 0x0100;   /* 清除对应PIE分组的中断响应位*/
}


/********************* SCI-A数据接收中断服务函数**********************/
interrupt void SCI_RX_isr(void)
{
	int i,number;
	
	number = SciaRegs.SCIFFRX.bit.RXFIFST; //接收到的数据个数

	for(i=0; i<number; i++)	//将接收到的数据放入接收缓存receive_buf
	{
		receive_buf[sci_para.receive_buf_pointer] = SciaRegs.SCIRXBUF.all&0x00ff;
		sci_para.receive_buf_pointer++;
		if(sci_para.receive_buf_pointer>=RBUF_LEN)
		{
			sci_para.receive_buf_pointer = 0;
		}
		sci_para.receive_data_len++;
		if(sci_para.receive_data_len>=RBUF_LEN)
		{
			sci_para.error = ERROR_RECEIVE_OVERFLOW;
			sci_para.receive_buf_pointer = 0;
			sci_para.receive_data_len = 0;
		}
	}	
	SciaRegs.SCIFFRX.bit.RXFFINTCLR = 1;    /* 清除RXFIFINT中断标志位*/
	PieCtrlRegs.PIEACK.all = 0x0100;   		/* 清除对应PIE分组的中断响应位*/
}


/*对接收到的数据进行解包，普通命令数据包格式"0xAA  command command"*/
/*第二次command是为了校验*/
/*采样频率命令数据格式为"0xAA  command  sample_freH sample_freL  sample_freH sample_freL"*/
/*sample_freH是sample_fre/100的整数部分，sample_freL是余数部分*/
void receive_data_op(void)
{
/*
	int p;		
	char data,data_check;
	unsigned int temp;

	if(sci_para.receive_data_len > 0)
	{		
		p = sci_para.receive_buf_pointer - sci_para.receive_data_len;		
		if(p<0) p=p+RBUF_LEN;
	
		data = receive_buf[p]; 		//从接收缓存读取一个数
		if(data==0xAA) 				//170,start of command,命令数据起始符
		{
			p++;
			if(p>=RBUF_LEN) p=p-RBUF_LEN;
			data = receive_buf[p]; 	//取命令名
			
			if(data == SAMPLE_FRE)	//如果命令是采样频率
			{
				if(sci_para.receive_data_len>=6)	//整个命令已经接收到
				{
					p++;
					if(p>=RBUF_LEN) p=p-RBUF_LEN;
					data = receive_buf[p]*100;
					p++;
					if(p>=RBUF_LEN) p=p-RBUF_LEN;
					data = data + receive_buf[p]; 	//取第一个采样频率值

					p++;
					if(p>=RBUF_LEN) p=p-RBUF_LEN;
					data_check = receive_buf[p]*100;
					p++;
					if(p>=RBUF_LEN) p=p-RBUF_LEN;
					data_check = data_check + receive_buf[p];	//取第二个采样频率值用于校验

					if(data == data_check)
					{						
						temp = (unsigned int)(1.875e7 / data);
						sci_para.sample_rate = 1.875e7 / temp;
						temp -= 1;
						EvaRegs.T1PR = temp;
						sgapara.coe = 2000 / sci_para.sample_rate; 	//sga计算时提前计算好的参数
						
						output_buf[3].data = sci_para.sample_rate;		//output_buf[3]:sample frequency
					}
					sci_para.receive_data_len -= 6;					
				}					
			}
			else		//除了采样频率之外的其他命令
			{
				if(sci_para.receive_data_len>=3)
				{
					p++;
					if(p>=RBUF_LEN) p=p-RBUF_LEN;
					data_check = receive_buf[p]; 	//取命第二个命令，用于校验

					if(data == data_check)
					{						
						if(data == TIMEINTERVAL)
						{							
							sci_para.feedback_data_type = TIMEINTERVAL;
							sci_para.add_data_type = TIMEINTERVAL;
							sci_para.add_data_pointer = 5;
							sci_para.send_buf_pointer = 0;
							sci_para.send_data_len = 0;	
							output_buf[4].data = TIMEINTERVAL;
						}
						else if(data == PHASEINTERVAL)
						{							
							sci_para.feedback_data_type = PHASEINTERVAL;
							sci_para.add_data_type = PHASEINTERVAL;
							sci_para.add_data_pointer = 5;
							sci_para.send_buf_pointer = 0;
							sci_para.send_data_len = 0;	
							output_buf[4].data = PHASEINTERVAL;
						}
						else if(data == OMEGA)
						{							
							sci_para.feedback_data_type = OMEGA;
							sci_para.add_data_type = OMEGA;
							sci_para.add_data_pointer = 5;
							sci_para.send_buf_pointer = 0;
							sci_para.send_data_len = 0;	
							output_buf[4].data = OMEGA;
						}
						else if(data == SAMPLED_SIGNAL)
						{
							sci_para.feedback_data_type = SAMPLED_SIGNAL;
							sci_para.add_data_type = SAMPLED_SIGNAL;
							sci_para.add_data_pointer = 5;
							sci_para.send_buf_pointer = 0;
							sci_para.send_data_len = 0;	
							output_buf[4].data = SAMPLED_SIGNAL;
						}
						else if(data == FILTERED_SIGNAL)
						{
							sci_para.feedback_data_type = FILTERED_SIGNAL;
							sci_para.add_data_type = FILTERED_SIGNAL;
							sci_para.add_data_pointer = 5;
							sci_para.send_buf_pointer = 0;
							sci_para.send_data_len = 0;	
							output_buf[4].data = FILTERED_SIGNAL;
						}
						else if(data == RESET)
						{							
							STOP_FFINT;
							RESET_TX;	
							EvaRegs.T1CON.bit.TENABLE = 0;	//stop adc convert
							//sci_para.work_status = WORK_OFF;							
							sci_para.sample_rate = 800.017;
							sci_para.error = NO_ERROR;
							//sci_para.feedback_data_type = PHASEINTERVAL;
							//sci_para.add_data_type = PHASEINTERVAL;
							output_buf[2].data = 0; //no error
							output_buf[3].data = 800.017;
							//output_buf[4].data = PHASEINTERVAL;							
							
							sci_para.sci_status = WORK_OFF;
							sci_para.add_data_pointer = 5;
							sci_para.send_buf_pointer = 0;
							sci_para.send_data_len = 0;						
							
							initial_anf();
							initial_sga();							
							
							//sci_para.work_status = WORK_ON;
							anfpara.err = NO_ERROR;
							adcpara.adc_pointer = 0;
							adcpara.start_flag = 1;							
							EvaRegs.T1PR = 23436;//800Hz							
							EvaRegs.T1CON.bit.TENABLE = 1;	//stop adc convert
							START_TX;
						}						
						else if(data == STOP)
						{		
							EvaRegs.T1CON.bit.TENABLE = 0;	//stop adc convert
							sci_para.work_status = WORK_OFF;							
							STOP_FFINT; //停止SCI中断	
							RESET_TX;
							sci_para.sci_status = WORK_OFF;	
							START_TX;
						}
						else if(data == START)
						{
							//initial_anf();
							//initial_sga();
							//adcpara.adc_pointer = 0;
							//adcpara.start_flag = 1;
							RESET_TX;
							STOP_FFINT;
							EvaRegs.T1CON.bit.TENABLE = 1; 	//start adc convert
							sci_para.work_status = WORK_ON; //system on
							
							sci_para.sci_status = WORK_OFF;
							sci_para.add_data_type = sci_para.feedback_data_type;
							sci_para.add_data_pointer = 5;
							sci_para.send_buf_pointer = 0;
							sci_para.send_data_len = 0;	
							START_TX;
						}
						else if(data == WAIT)
						{
							sci_para.add_data_type = WAIT;
							sci_para.sci_status = WORK_OFF;
							STOP_FFINT;
						}
						else if(data == RESUME)
						{
							sci_para.add_data_type = sci_para.feedback_data_type;
							sci_para.add_data_pointer = 5;							
							sci_para.send_buf_pointer = 0;
							sci_para.send_data_len = 0;
						}
					}					
					sci_para.receive_data_len -= 3;
				}					
			}
		}
		else
		{
			sci_para.receive_data_len--;
		}			
	}
	*/		
}


void SendData_op(void)
{

	if(sci_para.send_data_len>0)
	{
		if(sci_para.sci_status==WORK_OFF)
		{
			START_FFINT;
		}
	}

    //output_buf[2].data = (float)anfpara.err;
	
}

/*****************end********************/ 















