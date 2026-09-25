#include"LCD.H"
#include"AD0832.H"
#include"DAC0832.H"
#include"DS18B20.H"
unsigned char Serial[64]={0};
unsigned char dd[]={0};
unsigned char CTemp[]={0};
unsigned char code tab[]="0123456789. ";
unsigned char code T[8]={0x18,0x18,0x0F,0x1F,0x18,0x18,0x1F,0x0F};
void delayms(int dly)
{
int i;
char j;
for(i=0;i<dly;i++)
	for(j=0;j<120;j++);
}

int main (void)
{
unsigned char i=255,v=0,temp=0;
delayms(600);
LCD_Init();
DacInit();
WriteStringToLcd(0,0,"V1:");
WriteStringToLcd(7,0,"v");
WriteStringToLcd(0,1,"Temp:");
Write_self(T);
Write_selfString(8,1,0);
delayms(200);
WriteToDac0832(255);
 while(1)
 {
v=ReadAdc0832(0);
dd[0]=tab[v/51];
dd[1]=tab[10];
dd[2]=tab[10*v/51%10];
dd[3]=tab[100*v/510%10];
WriteStringToLcd(3,0,dd);
temp=ReadTempreture(Serial,9);
if(temp>100)
{
CTemp[0]=tab[temp/100];
CTemp[1]=tab[temp%100/10];
CTemp[2]=tab[temp%100%10];
}
else
{ 
CTemp[0]=tab[temp/10];
CTemp[1]=tab[temp%10];
CTemp[2]=tab[11];
}
delayms(200);
WriteStringToLcd(5,1,CTemp);
}
return 0;
}