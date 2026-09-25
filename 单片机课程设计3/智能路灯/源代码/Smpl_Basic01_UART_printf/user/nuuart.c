#include"nuuart.h"


void UART0_Init(uint32_t uartband)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART module */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, uartband);
}
//发送函数  UART_Write(UART0, chh, 5);
//接收函数，会阻塞cpu但是有超时跳过
//UART_Read(UART_T* uart, uint8_t *pu8RxBuf, uint32_t u32ReadBytes)
