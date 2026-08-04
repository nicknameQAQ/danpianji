#include"cycle.h"
#include "string.h"
#include"data.h"

int n;	   //ĞÇÆÚ¼¸µÄÅĞ¶Ï

char w[7];
char d[14];		   //Äê·İµÄÅĞ¶Ï

void week()		   
{
  	n=p%7;		   
  switch(n)
  {
  	case 0: strcpy(w,"ĞÇÆÚÈÕ");break;
  	case 1: strcpy(w,"ĞÇÆÚÒ»");break;		   
  	case 2: strcpy(w,"ĞÇÆÚ¶ş");break;
  	case 3: strcpy(w,"ĞÇÆÚÈş"); break;
  	case 4: strcpy(w,"ĞÇÆÚËÄ");break;
  	case 5: strcpy(w,"ĞÇÆÚÎå");break;
  	case 6: strcpy(w,"ĞÇÆÚÁù");break;
  }
  
}
void lunar()
{
	switch(year)
	{
	case 2019: strcpy(d,"¼ºº¥Öí");break;
  	case 2020: strcpy(d,"¹ï×ÓÊó");break;		   
  	case 2021: strcpy(d,"ĞÁ³óÅ£");break;
  	case 2022: strcpy(d,"ÈÉÒú»¢"); break;
  	case 2023: strcpy(d,"¹ïÃ®ÍÃ");break;
  	case 2024: strcpy(d,"¼×³½Áú");break;
  	case 2025: strcpy(d,"ÒÒËÈÉß");break;
	case 2026: strcpy(d,"±ûÎçÂí");break;
  	case 2027: strcpy(d,"¶¡Î´Ñò");break;		   
  	case 2028: strcpy(d,"ÎìÉêºï");break;
  	case 2029: strcpy(d,"ÒÒÓÏ¼¦"); break;
  	case 2030: strcpy(d,"¸ıĞç¹·");break;
  	case 2031: strcpy(d,"ĞÁº¥Öí");break;
  	case 2032: strcpy(d,"ÈÉ×ÓÊó");break;
	}
}
