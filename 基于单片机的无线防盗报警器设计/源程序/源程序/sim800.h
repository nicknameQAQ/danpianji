#ifndef __SIM800C_H__
#define __SIM800C_H__

#define Yes		1
#define No		0

void Uart_Init(void);
void Send_String(unsigned char *str);
void Receive_message();
void Send_message(unsigned char *content);
void Delay_Ms (unsigned int a);





#endif