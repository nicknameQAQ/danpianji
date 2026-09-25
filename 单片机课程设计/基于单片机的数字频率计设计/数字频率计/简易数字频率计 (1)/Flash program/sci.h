
/*定义宏*/
#define RBUF_LEN	20
#define WORK_ON 	1
#define WORK_OFF	0
#define ERROR_RECEIVE_OVERFLOW	51
#define ERROR_SEND_OVERFLOW		52
#define NO_ERROR 	0
#define TRUE 		1
#define FAULSE 		0


#define STOP 		1
#define START		2
#define NEXT		3
#define RESEND		4
#define TIMEINTERVAL	5
#define PHASEINTERVAL	6
#define OMEGA		7
#define SAMPLED_SIGNAL	13
#define FILTERED_SIGNAL 14
#define RESET		8
#define SAMPLE_FRE	10
#define WAIT		11
#define RESUME		12

#define STOP_FFINT 	SciaRegs.SCIFFTX.bit.TXFFIENA=0		//关FIFO中断
#define START_FFINT SciaRegs.SCIFFTX.bit.TXFFIENA=1		//开FIFO中断
#define RESET_TX	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=0	//复位SCI发送
#define START_TX	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1	//启动SCI发送
//#define STOP_TX 	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 0;
//#define START_TX	SciaRegs.SCIFFTX.bit.TXFIFOXRESET = 1;
#define STOP_RX		SciaRegs.SCIFFRX.bit.RXFIFORESET=0	//复位SCI接收
#define START_RX	SciaRegs.SCIFFRX.bit.RXFIFORESET=1	//启动SCI接收


#define AddDataToBuf(x) {output_buf[sci_para.add_data_pointer].data = (x); \
						sci_para.add_data_pointer++;\
						if(sci_para.add_data_pointer>=55)sci_para.add_data_pointer=5; \
						sci_para.send_data_len++;}  

#define DEFAULT_SCI_PARA {1000.0,WORK_OFF,PHASEINTERVAL,NO_ERROR,0,0,0,0,0}
/************************************************/
/*定义结构体*/
typedef struct _SCI_PARA
{
	float sample_rate;				//采样频率
	unsigned int work_status;		//系统工作状态，WORK_ON or WORK_OFF
	unsigned int feedback_data_type;//返回数据类型，TIMEINTERVAL or PHASEINTERVAL or OMEGA
	char error;				//错误标志
	char last_command;				//上一次发送的命令
	unsigned int receive_data_len;	//接收数据长度
	unsigned int send_data_len;				//发送数据长度
	unsigned int receive_buf_pointer;//接收数据位置指针
	unsigned int send_buf_pointer;	//发送数据位置指针
	unsigned int add_data_type;
	unsigned int add_data_pointer;
	char sci_status;
	float add_check;
}SCI_PARA;

typedef struct _HL_DATA
{
	char datal; //low 16 bits data
	char datah; //high 16 bits data
}HL_DATA;

typedef union _DECOV_DATA
{
	float data;
	HL_DATA hldata;
}DECOV_DATA;



/***************sci functions********************/
void SCI_Init(void); 
void receive_data_op(void);
void SendData_op(void);
interrupt void SCI_TX_isr(void); 
interrupt void SCI_RX_isr(void);

