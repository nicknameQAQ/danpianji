#include"nugpio.h"
#include"stdio.h"

/*读取和写入都是调用PA0，例如PA0=1是置一，a=PA0是读取端口*/
void Input_init(void)
{
    GPIO_SetMode(PE, BIT2, GPIO_MODE_INPUT);
    GPIO_SetMode(PA, BIT8,  GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT6, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT7, GPIO_MODE_INPUT);
}

void Output_init(void)
{
	 GPIO_SetMode(PB, BIT5, GPIO_MODE_OUTPUT);
}
void gpio_int(void)
{ 
	/* Configure PB.2 as Input mode and enable interrupt by rising edge trigger */
	/*  PB.2 是输入模式下，上升沿中断 */
    GPIO_SetMode(PB, BIT2, GPIO_MODE_INPUT);
    GPIO_EnableInt(PB, 2, GPIO_INT_RISING);
    NVIC_EnableIRQ(GPB_IRQn);

    /* Configure PC.5 as Quasi-bidirection mode and enable interrupt by falling edge trigger */
	/* PC.5 是输入模式下，下降沿中断  */
    GPIO_SetMode(PC, BIT5, GPIO_MODE_QUASI);
    GPIO_EnableInt(PC, 5, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPC_IRQn);

    /* Enable interrupt de-bounce function and select de-bounce sampling cycle time is 1024 clocks of LIRC clock */
	/* 使能中断 */
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_1024);
    GPIO_ENABLE_DEBOUNCE(PB, BIT2);
    GPIO_ENABLE_DEBOUNCE(PC, BIT5);

}

unsigned char Get_Key_Input(void)
{
    unsigned char temp = 0;
    if(KEY1 == 0)
        temp |= 0x1;


    if(KEY2 == 0)
        temp |= 0x2;


    if(KEY3 == 0)
        temp |= 0x4;


    if(KEY4 == 0)
        temp |= 0x8;

    return   temp;
}
//中断入口
void GPB_IRQHandler(void)
{
    /* To check if PB.2 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PB, BIT2))
    {
        GPIO_CLR_INT_FLAG(PB, BIT2);
        printf("PB.2 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PB interrupts */
        PB->INTSRC = PB->INTSRC;
        printf("Un-expected interrupts.\n");
    }
}

void GPC_IRQHandler(void)
{
    /* To check if PC.5 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PC, BIT5))
    {
        GPIO_CLR_INT_FLAG(PC, BIT5);
        printf("PC.5 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PC interrupts */
        PC->INTSRC = PC->INTSRC;
        printf("Un-expected interrupts.\n");
    }
}



