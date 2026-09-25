
#include <stdio.h>
#include "M451Series.h"
#include "nuclk.h"
#include "nuadc.h"
#include "nuuart.h"
#include "nugpio.h"
unsigned char dianji=0,i=0;
unsigned char tx_buf[101],rx_buf[100],key[3];
long int timemain=0;
unsigned int adc=0;
void delay_ms(long unsigned int tem)
{int temp;
	while(tem--)
	{temp=8000;
	while(temp--)if(temp%5)PD4=1;
	else PD4=0;}
}
int main()
{

   
    SYS_Init();
    /* Init UART0 to 115200-8n1 for print message */
    UART0_Init(115200);
	  Input_init();
		Output_init();
    Open_ADC_Knob();
    printf("\n");
    printf("Simple Demo Code\n\n");

    printf("Please Input Any Key\n\n");
//   UART_Write(UART0, chh, 5);
	for(i=0;i<100;i++)tx_buf[i]=255;
	
	printf("AT+CIPSTART=\"TCP\",\"172.18.171.30\",8080\n\r\0");
	delay_ms(700);
//	printf("AT+CIPMODE=1\r\n");
		delay_ms(700);
//	printf("AT+CIPSEND\r\n");
    tx_buf[0]='8';
    tx_buf[1]='9';
		tx_buf[100]=0;
    rx_buf[7]=48;
    while(1)
		{		
		   	if(timemain%300==0)//adc
    	   {
    		  adc=Get_ADC_Knob()*330/4096;
    	    tx_buf[2]=adc>>8;tx_buf[3]=adc&0xff;
					 if(tx_buf[2]==0)tx_buf[2]=255;
    	   }
				 
    if(timemain%10==0)//
     	   {
    		      key[0]=  KEY3;// ?gpio 13   p4 ???
    	        key[1]= KEY4;// ?gpio 22   p15  ???
    	        if(key[0]==0)  tx_buf[5]=0x30;
    	        else    tx_buf[5]=0x31;
    	        if(key[1]==0)  tx_buf[4]=0x30;
    	         else    tx_buf[4]=0x31;
      	   }
    if(timemain%30==0)//io??
          {  
    	  if(rx_buf[7]==0x31) LED0=1,tx_buf[6]=0x31;
    	  else    if(rx_buf[7]==0x30&&dianji==0) LED0=0,tx_buf[6]=0x30;
    		if (adc>100)LED0=1,tx_buf[6]=0x31,dianji=1;
    		if(adc<60&&dianji==1)LED0=0,tx_buf[6]=0x30,dianji=0;
          }

//?????????????????????????????
   if(timemain%500==0) 
    	{		printf("AT+CIPSTART=\"TCP\",\"172.18.171.30\",8080\n\r\0");
					delay_ms(10);
				printf("AT+CIPSEND=100\r\0");
				delay_ms(10);
			//	 UART_Write(UART0, tx_buf, 100);
				printf("%s",tx_buf);delay_ms(10);
				
		//		UART_Read(UART0, rx_buf, 100);
				
			}//wifi??

 //  BsdTcpClient(PORT_NUM );//wifi??

timemain++;delay_ms(1);
if(timemain%100000==0) ;
if(timemain%200000==0) ;
if(timemain>=1000000) timemain=0;
		
		}

}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
