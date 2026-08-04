#include <reg52.h>
#include <intrins.h>

#define uchar unsigned char	 //宏定义方便以后用
#define uint unsigned int  
#define ulong unsigned long

//SHT11接口
sbit DATA = P2^5;
sbit SCK = P2^4;

typedef union		   //定义共同类型
{ 
  unsigned int i;	   //i表示测量得到的温湿度数据（int 形式保存的数据） 
  float f;			   //f表示测量得到的温湿度数据（float 形式保存的数据）
} value;

enum {TEMP,HUMI};

uchar TEMP_BUF[7];				 	//用于记录温度
uchar HUMI_BUF[5];				 	//用于记录湿度
 
int real_tempure=0;		//实际温度
uchar real_humi=0;


//LCD port
sbit LcdRs= P2^0;
sbit LcdRw= P2^1;
sbit LcdEn= P2^2;
sfr  DBPort= 0x80;		//P0=0x80,P1=0x90,P2=0xA0,P3=0xB0.数据端口


//向LCD写入命令或数据************************************************************
#define LCD_COMMAND			0      // Command
#define LCD_DATA			1      // Data
#define LCD_CLEAR_SCREEN	0x01      // 清屏
#define LCD_HOMING  		0x02      // 光标返回原点


//设置显示模式************************************************************
#define LCD_SHOW			0x04    //显示开
#define LCD_HIDE			0x00    //显示关	  

#define LCD_CURSOR			0x02 	//显示光标
#define LCD_NO_CURSOR		0x00    //无光标		     

#define LCD_FLASH			0x01    //光标闪动
#define LCD_NO_FLASH		0x00    //光标不闪动
//内部等待函数**************************************************************************
unsigned char LCD_Wait(void)
{
	LcdRs=0;
	LcdRw=1;	_nop_();
	LcdEn=1;	_nop_();
	_nop_();
	_nop_();
	_nop_();
	LcdEn=0;
	return DBPort;		
}

void LCD_Write(bit style, unsigned char input)
{
	LcdEn=0;
	LcdRs=style;
	LcdRw=0;		_nop_();
	DBPort=input;	_nop_();//注意顺序
	LcdEn=1;		_nop_();//注意顺序
	LcdEn=0;		_nop_();
	LCD_Wait();	
}

void LCD_SetDisplay(unsigned char DisplayMode)
{
	LCD_Write(LCD_COMMAND, 0x08|DisplayMode);	
}

//设置输入模式************************************************************
#define LCD_AC_UP			0x02
#define LCD_AC_DOWN			0x00      // default

#define LCD_MOVE			0x01      // 画面可平移
#define LCD_NO_MOVE			0x00      //default

void LCD_SetInput(unsigned char InputMode)
{
	LCD_Write(LCD_COMMAND, 0x04|InputMode);
}


//初始化LCD************************************************************
void LCD_Initial()
{
	LcdEn=0;
	LCD_Write(LCD_COMMAND,0x38);           //8位数据端口,2行显示,5*7点阵
	LCD_Write(LCD_COMMAND,0x38);
	LCD_SetDisplay(LCD_SHOW|LCD_NO_CURSOR);    //开启显示, 无光标
	LCD_Write(LCD_COMMAND,LCD_CLEAR_SCREEN);   //清屏
	LCD_SetInput(LCD_AC_UP|LCD_NO_MOVE);       //AC递增, 画面不动
}

//************************************************************************
void GotoXY(unsigned char x, unsigned char y)
{
	if(y==0)
		LCD_Write(LCD_COMMAND,0x80|x);
	if(y==1)
		LCD_Write(LCD_COMMAND,0x80|(x-0x40));
}

void Print(unsigned char *str)	 //在LCD上显示数据，内部调用
{
	while(*str!='\0')
	{
		LCD_Write(LCD_DATA,*str);
		str++;
	}
}

void LCD_Print(unsigned char x, unsigned char y, unsigned char *str) //指定的位置打印指定数据
{
  GotoXY(x,y);
  Print(str);
}

/********************************************************************
* 名称 : Delay_1ms()
* 功能 : 延时子程序
* 输入 : x 
* 输出 : 无
***********************************************************************/
void Delay(uint i)//延时
{
	uint x,j;
	for(j=0;j<i;j++)
	for(x=0;x<=148;x++);	
}


/***********************************************************************************************************************************************************/

//SHT11程序


#define noACK 0				//继续传输数据，用于判断是否结束通讯
#define ACK   1             //结束数据传输；
							//地址  命令  读/写
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0


//写字节程序
char SHTXX_write_byte(unsigned char value)   
{ 
	unsigned char i,error=0; 
	for (i=0x80;i>0;i/=2)             //shift bit for masking 高位为1，循环右移
	{ 
		if (i&value) DATA=1;          //和要发送的数相与，结果为发送的位
    	else DATA=0;                        
    	SCK=1;                          
    	_nop_();_nop_();_nop_();        //延时3us 
    	SCK=0;
	}
	DATA=1;                           //释放数据线
	SCK=1;                            
	error=DATA;                       //检查应答信号，确认通讯正常
	_nop_();_nop_();_nop_();
	SCK=0;        
	DATA=1;
	return error;                     //error=1 通讯错误
}

//读字节程序
char SHTXX_read_byte(unsigned char ack)
//----------------------------------------------------------------------------------
{ 
	unsigned char i,val=0;
	DATA=1;                           //释放数据线
	for(i=0x80;i>0;i>>=1)             //高位为1，循环右移
	{ 
		SCK=1;                         
    	if(DATA) val=(val|i);        //读一位数据线的值 
    	SCK=0;       
	}
	DATA=!ack;                        //如果是校验，读取完后结束通讯；
	SCK=1;                            
	_nop_();_nop_();_nop_();          //延时3us 
	SCK=0;   
	_nop_();_nop_();_nop_();       
	DATA=1;                           //释放数据线
	return val;
}

//启动传输
void SHTXX_transstart(void)
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{ 
   	DATA=1; SCK=0;                   //准备
   	_nop_();
  	SCK=1;
   	_nop_();
   	DATA=0;
   	_nop_();
  	SCK=0; 
  	 _nop_();_nop_();_nop_();
   	SCK=1;
   	_nop_();
  	DATA=1;     
  	_nop_();
   	SCK=0;     
}


//连接复位
void SHTXX_Init(void)
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{ 
	unsigned char i; 
	DATA=1; SCK=0;                    //准备
	for(i=0;i<9;i++)                  //DATA保持高，SCK时钟触发9次，发送启动传输，通迅即复位
	{ 
		SCK=1;
    	SCK=0;
	}
	SHTXX_transstart();                   //启动传输
}

 /*
//软复位程序
char SHTXX_softreset(void)
// resets the sensor by a softreset 
{ 
	unsigned char error=0; 
	SHTXX_Init();              //启动连接复位
	error+=SHTXX_write_byte(RESET);       //发送复位命令
	return error;                     //error=1 通讯错误
}
  */


           
//温湿度测量
char SHTXX_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
// 进行温度或者湿度转换，由参数mode决定转换内容；
{ 
//	enum {TEMP,HUMI};		 //已经在头文件中定义
	unsigned error=0;
	unsigned int i;

	SHTXX_transstart();                   //启动传输
	switch(mode)                     //选择发送命令
    {	
		case TEMP : error+=SHTXX_write_byte(MEASURE_TEMP); break;		   //测量温度
    	case HUMI : error+=SHTXX_write_byte(MEASURE_HUMI); break;		   //测量湿度
    	default     : break; 
	}
	for (i=0;i<65535;i++) if(DATA==0) break; //等待测量结束
	if(DATA) error+=1;                // 如果长时间数据线没有拉低，说明测量错误 
	*(p_value) =SHTXX_read_byte(ACK);    //读第一个字节，高字节 (MSB)
	*(p_value+1)=SHTXX_read_byte(ACK);    //读第二个字节，低字节 (LSB)
	*p_checksum =SHTXX_read_byte(noACK); //read CRC校验码
	return error;					 // error=1 通讯错误
}

//温湿度值标度变换及温度补偿
void SHTXX_calc(float *p_humidity ,float *p_temperature)
{ 

	float rh=*p_humidity;             // rh:      12位 湿度 
	float t=*p_temperature;           // t:       14位 温度
	float rh_lin;                     // rh_lin: 湿度 linear值
	float rh_true;                    // rh_true: 湿度 ture值
	float t_C;                        // t_C   : 温度 ℃

	t_C=t*0.01 - 40;                  //补偿温度
	rh_lin=-0.0000028*rh*rh + 0.0405*rh - 4;     //相对湿度非线性补偿
	rh_true=rh/33;   
	if(rh_true>26)
		rh_true=rh/33+1;
	if(rh_true>33)
		rh_true=rh/33+1.5;
	if(rh_true>40)
		rh_true=rh/33+3;	
	if(rh_true>50)
		rh_true=rh/33+3.7;	
	if(rh_true>70)
		rh_true=rh/33+3;	
	if(rh_true>90)
		rh_true=rh/33+1.6;	

    //rh_true=(t_C-25)*(0.01+0.00008*rh)+rh_lin;   //相对湿度对于温度依赖性补偿	，仿真的时候去掉补偿

		
	if(rh_true>100)rh_true=100;       //湿度最大修正
	if(rh_true<0.1)rh_true=0.1;       //湿度最小修正

	*p_temperature=t_C;               //返回温度结果
	*p_humidity=rh_true;              //返回湿度结果


}


void Covert_HT()
{
	value humi_val,temp_val;		//定义两个共同体，一个用于湿度，一个用于温度
	uchar checksum;			//CRC	 
	uchar error=0;
	error=0;					   //初始化error=0，即没有错误
	error+=SHTXX_measure((unsigned char*)&temp_val.i,&checksum,TEMP); //温度测量
	error+=SHTXX_measure((unsigned char*)&humi_val.i,&checksum,HUMI); //湿度测量
	if(error!=0) SHTXX_Init();                 ////如果发生错误，系统复位
	else
	{ 
		humi_val.f=(float)humi_val.i;                   //转换为浮点数
 		temp_val.f=(float)temp_val.i;                   //转换为浮点数
 		SHTXX_calc(&humi_val.f,&temp_val.f);            //修正相对湿度及温度
		
		real_tempure =temp_val.f+1;	   //修正一下偏
		real_humi    = humi_val.f-1;

		if(real_tempure>=0)
		{
  		TEMP_BUF[0]=real_tempure/100+'0';     		//温度百位
		
		if (TEMP_BUF[0]==0x30) TEMP_BUF[0]=0x20;
		TEMP_BUF[1]=real_tempure%100/10+'0';     	//温度十位	
		if (TEMP_BUF[1]==0x30 && TEMP_BUF[0]==0x30) TEMP_BUF[1]=0x20;
		TEMP_BUF[2]=real_tempure%10+'0';			//温度个位 
		
		}
		else  //负温度
		{
			TEMP_BUF[0]	='-';
			TEMP_BUF[1]=(real_tempure*(-1))%100/10+'0';     	//温度十位	
			if (TEMP_BUF[1]==0x30 && TEMP_BUF[0]==0x30) TEMP_BUF[1]=0x20;
			TEMP_BUF[2]=(real_tempure*(-1))%10+'0';			//温度个位
			
		}		
		TEMP_BUF[3]=0xdf;
		TEMP_BUF[4]=0x43;
		HUMI_BUF[0]=real_humi/100+'0';     		//湿度百位
  		if (HUMI_BUF[0]==0x30) HUMI_BUF[0]=0x20;
		HUMI_BUF[1]=real_humi%100/10+'0';     	//湿度十位	
		if (HUMI_BUF[1]==0x30 && HUMI_BUF[0]==0x30) HUMI_BUF[1]=0X20;  
		HUMI_BUF[2]=real_humi%10+'0';			//湿度个位
		HUMI_BUF[3]= '%';

	}
}

void main()
{
	LCD_Initial();
    SHTXX_Init();			   //启动连接复位
	LCD_Print(0,0,"RH:          "); 
	LCD_Print(0,1,"T:    C        "); 

	while(1)
	{	
		Covert_HT();
	  	LCD_Print(2,1,TEMP_BUF);
		LCD_Print(3,0,HUMI_BUF);
		Delay(10);

	} 
} 



