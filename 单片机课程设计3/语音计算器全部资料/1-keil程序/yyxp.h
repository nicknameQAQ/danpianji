 
sbit yyxp_busy = P3^2;
sbit yyxp_data = P3^3;
sbit yyxp_rest = P3^4;

#define dian 22
#define jia 61
#define jian 62
#define chengyi 63
#define chuyi 64
#define fu 58
#define dengyu 59
#define ling 60
#define de 67
#define cimi 68
#define cuowu 65
  
void yydelay(unsigned char x)
{
  unsigned char i,j;
  for(i=0;i<x;i++)
  for(j=0;j<24;j++);
}
void speak(int z)
{ 
  
  yyxp_rest = 1;
  yydelay(2);
  yyxp_rest = 0;
  yydelay(2);
  while(z>0)
  {	 
   	yyxp_data = 1;
	yydelay(1);
	yyxp_data=0;
	yydelay(1);
	z--;
  }

}


 void bofang(unsigned char *str) //1是总价2是单价	 
{
  unsigned char flag;
  flag=0;
  while(*str!='\0')
  {
   if(yyxp_busy==1)
    {
	   if(*str=='-')
	 //  speak(58);
	 speak(fu);		//
	   else if(*str=='.')
	   speak(22);
	   else if(*str=='0')
	   speak(ling);		  //零
	   else if(*str=='e')
	   {
	     
	 
		while(yyxp_busy==0)yydelay(2);
		speak(chengyi);		//的
		flag=1;
	   }
	   else speak(*str+1-0x30);
	   str++;
	   
	}
	yydelay(2);
  }	   
   if(flag==1)
   {
    while(yyxp_busy==0);
	speak(cimi);		  //次幂
   }
 }
 