#include <reg51.h>
#define uint unsigned int
#define uchr unsigned char
sbit led0=P2^2;
sbit buzzer=P2^3;
sbit dula=P2^6;                //声明段选线选通端
sbit wela=P2^7;                //声明位选线选通端
sbit clear=P3^4;
uchr key,shi,ge,count;
uchr code tab[]={
0x3f,0x06,0x5b,0x4f,
0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,
0x39,0x5e,0x79,0x71,
0x76,0x79,0x38,0x3f};
/*延时子函数，xms为形参*/
void delay(uint xms)
{
    uint x,y;
    for(x=xms;x>0;x--)
        for(y=110;y>0;y--);
}

/*矩阵键盘扫描子程序*/
void matrixkeyscan()
{
    uint temp;
    P3=0xf7;                             //给P3口赋值
    temp=P3;
    temp=temp&0xf0;
    if(temp!=0xf0)                       //检测是否有键按下
    {
        delay(6);                       //延时去抖
        temp=P3;
        temp=temp&0xf0;
        if(temp!=0xf0)                   //再次检测是否有键按下
        {
            temp=P3;
            switch(temp)
            {
                case 0x77:
                    key=1;
                    break;
                case 0xb7:
                    key=2;
                    break;
                case 0xd7:
                    key=3;
                    break;
                case 0xe7:
                    key=4;
                    break;              //判断按下的是哪个键
            }
            while(temp!=0xf0)
            {
                temp=P3;
                temp=temp&0xf0;        
            }                           //等待按键释放
        }
    }
    P3=0xfb;
    temp=P3;
    temp=temp&0xf0;
    if(temp!=0xf0)
    {
        delay(6);
        temp=P3;
        temp=temp&0xf0;
        if(temp!=0xf0)
        {
            temp=P3;
            switch(temp)
            {
case 0x7b:
                    key=5;
                    break;
                case 0xbb:
                    key=6;
                    break;
                case 0xdb:
                    key=7;
                    break;
                case 0xeb:
                    key=8;
                    break;
            }
            while(temp!=0xf0)
            {
                temp=P3;
                temp=temp&0xf0;
            }
        }
    }
    P3=0xfd;
    temp=P3;
    temp=temp&0xf0;
    if(temp!=0xf0)
    {
        delay(6);
        temp=P3;
        temp=temp&0xf0;
        if(temp!=0xf0)
        {
            temp=P3;
            switch(temp)
            {
                case 0x7d:
                    key=9;
                    break;
                case 0xbd:
                    key=10;
                    break;
               case 0xdd:
                    key=11;
                    break;
                case 0xed:
                    key=12;
                    break;
            }
            while(temp!=0xf0)
            {
                temp=P3;
                temp=temp&0xf0;
            }
        }
    }
    P3=0xfe;
    temp=P3;
    temp=temp&0xf0;
    if(temp!=0xf0)
    {
        delay(6);
        temp=P3;
        temp=temp&0xf0;
        if(temp!=0xf0)
        {
            temp=P3;
            switch(temp)
            {
                case 0x7e:
                    key=13;
                    break;
                case 0xbe:
                    key=14;
                    break;
                case 0xde:
                    key=15;
                    break;
                case 0xee:
                    key=16;
                    break;
            }
            while(temp!=0xf0)
            {
                temp=P3;
                temp=temp&0xf0;
            }
        }
    }
}
/*主函数*/
void alarm()
{  
	buzzer=~buzzer;
	led0=~led0;
	delay(30);
}
    void display(uchr num)
	{
		if(key!=0)             //若有键按下，则进入该循环进行处理
        {
            
            dula=1;
            P0=tab[num/10];
            dula=0;
             P0=0x00;                       
            wela=1;
            P0=0xfe;
            wela=0;
            delay(100);

            dula=1;
            P0=tab[num%10];
            dula=0;
           P0=0x00;
		  wela=1;
		   P0=0xfd;
		   wela=0;
		   delay(100);
		   alarm();
		}
	}
	void main()
	{
	P0=0x00;
	while(1)
	{
	matrixkeyscan();
	display(key);
	}
	}
