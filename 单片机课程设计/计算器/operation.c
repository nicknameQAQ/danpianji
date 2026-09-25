#include "operation.h"
#include <stdlib.h>
#include "display_driver.h"
#include <math.h>

#define PI 3.14159265

bit sign;
int tempw[6];

//#include "LCD1602.h"
//void xxx(){//检错
//	unsigned char a;
//	unsigned char*p=(unsigned char xdata*)0;
//	LCD1602_Write_Reg(0x80);
//	for(a=0;a<16;a++)
//	LCD1602_Write_Data(p[a]);
//	LCD1602_Write_Reg(0xC0);
//	for(;a<32;a++)
//	LCD1602_Write_Data(p[a]);
//}

double change_number(unsigned char*str){//转化成数字类型
	unsigned char a=0,b=0;
	double num;
	sign=0;
	while(str[a++]){
		if(str[a]==0x6F){
			sign=1;
			str[a]=0;
			num=atof(str);
			b=a;
			continue;
		}
		else if(str[a]=='\''){
			sign=1;
			str[a]=0;
			num+=atof(&str[b])/60;
			b=a;
			continue;
		}
		else if(str[a]=='\"'){
			sign=1;
			str[a]=0;
			num+=atof(&str[b])/3600;
		}
	}
	if(sign)return num;
	return atof(str);
}

bit determine_scope(unsigned char str){//确认数值范围
	if(str=='0'||str=='1'||str=='2'||str=='3'||str=='4'||str=='5'||
		 str=='6'||str=='7'||str=='8'||str=='9'||str=='.'||str=='-'||
	   str==0x6F||str=='\''||str=='\"')return 1;
	else return 0;
}

double calculate(double a,double b,unsigned char s){//计算
	switch(s){
		case '+':
			return a+b;
		case '_':
			return a-b;
		case 'x':
			return a*b;
		case 253:
		tempw[0]=(long int)(a*10000000/b)%10;
		if(tempw[0]>4)return a/b-(0.000001);else 	return a/b;
tempw[1]=(long int)(a*100000000/b)%10;
tempw[2]=(long int)(a*1000000000/b)%10;
tempw[3]=(long int)(a*10000000000/b)%10;
tempw[4]=(long int)(a*100000000000/b)%10;
tempw[5]=(long int)(a*1000000000000/b)%10;
		case 's':
			return sign?sin((b/180)*PI):sin(b);
		case 'c':
			return sign?cos((b/180)*PI):cos(b);
		case 't':
			return sign?tan((b/180)*PI):tan(b);
	}
	return 0;
}

double*multiplication(unsigned char*str,const unsigned int add,double*addr,unsigned char s){//运算
	bit sign1,sign2;
	unsigned char backup1;
	unsigned int a,backup=add-1;
	unsigned int backup3=add+1;
	unsigned int sign;
	double num1,num2;
	double*PN1,*PN2;
	str[add]=0;
	if(s=='s'||s=='c'||s=='t')str[add+1]=str[add+2]=0;
	while(!str[backup])backup--;
	if(add>=4&&str[backup-2]=='p'){
		sign1=1;
		PN1=(double xdata*)(unsigned int)(str[backup-1]<<8|str[backup]);
	}
	else if(add>=4&&str[backup-1]=='p'){
		sign1=1;
		PN1=(double xdata*)(unsigned int)(str[backup]<<8|str[backup+1]);
	}
	else{
		sign1=0;
		while(determine_scope(str[backup]))backup--;
		num1=change_number(&str[backup+1]);
	}
	while(!str[backup3])backup3++;
	sign=backup3;
	if(str[backup3]=='p'){
		sign2=1;
		PN2=(double xdata*)(unsigned int)(str[backup3+1]<<8|str[backup3+2]);
	}
	else{
		sign2=0;
		while(determine_scope(str[backup3]))backup3++;
	  backup1=str[backup3];
    str[backup3]=0;
	  num2=change_number(&str[sign]);
	  str[backup3]=backup1;
	}
	if(s=='s'||s=='c'||s=='t'){
		if(sign2){
		  *PN2=calculate(0,*PN2,s);
		}
		else{
			*addr=calculate(0,num2,s);
			for(a=add,backup1=0;a<backup3;a++,backup1++){
				switch(backup1){
					case 0:str[a]='p';break;
					case 1:str[a]=(unsigned int)addr>>8;break;
					case 2:str[a]=(unsigned int)addr;break;
					default:str[a]=0;
				}
			}
			addr++;
		}
	}
	else if(!sign1&&!sign2){
		*addr=calculate(num1,num2,s);
		for(a=backup+1,backup1=0;a<backup3;a++,backup1++){
			switch(backup1){
				case 0:str[a]='p';break;
				case 1:str[a]=(unsigned int)addr>>8;break;
				case 2:str[a]=(unsigned int)addr;break;
				default:str[a]=0;
			}
		}
		addr++;
	}
	else if(sign1&&sign2){
		*PN1=calculate(*PN1,*PN2,s);
		str[backup3]=str[backup3+1]=str[backup3+2]=0;
	}
	else if(sign1&&!sign2){
		*PN1=calculate(*PN1,num2,s);
		for(a=add+1;a<backup3;a++)str[a]=0;
	}
	else{
		*PN2=calculate(num1,*PN2,s);
		for(a=backup+1;a<add;a++)str[a]=0;
	}
	return addr;
}

double*priority_operation(const unsigned char*addr,const unsigned int offset,double*addn){//优先级运算
	unsigned int a;
	for(a=1;a<=offset;a++){
		if(addr[a]=='p')a+=2;
		else if(addr[a]=='s')addn=multiplication(addr,a,addn,'s');
		else if(addr[a]=='c')addn=multiplication(addr,a,addn,'c');
		else if(addr[a]=='t')addn=multiplication(addr,a,addn,'t');
	}
	for(a=1;a<=offset;a++){
		if(addr[a]=='p')a+=2;
		else if(addr[a]=='x')addn=multiplication(addr,a,addn,'x');
		else if(addr[a]==253)addn=multiplication(addr,a,addn,253);
	}
	for(a=1;a<=offset;a++){
		if(addr[a]=='p')a+=2;
		else if(addr[a]=='+')addn=multiplication(addr,a,addn,'+');
		else if(addr[a]=='_')addn=multiplication(addr,a,addn,'_');
	}
	return addn;
}

void Ready_operation(unsigned char*str,unsigned int add){//运算准备
	bit sign;
	unsigned char*p=(unsigned char xdata*)add;
	unsigned int a,b;
	double*addr=(double xdata*)(add*2+2);
	for(a=0,p[a]=0;a<=add;a++)p[a+1]=str[a];
	for(a=1;a<=add;a++)if(p[a]=='-')
	if(p[a+1]!='0'&&p[a+1]!='1'&&p[a+1]!='2'&&p[a+1]!='3'&&p[a+1]!='4'&&
		 p[a+1]!='5'&&p[a+1]!='6'&&p[a+1]!='7'&&p[a+1]!='8'&&p[a+1]!='9')p[a]='_';
	else if(p[a-1]=='0'||p[a-1]=='1'||p[a-1]=='2'||p[a-1]=='3'||p[a-1]=='4'||p[a-1]=='5'||
		      p[a-1]=='6'||p[a-1]=='7'||p[a-1]=='8'||p[a-1]=='9'||p[a-1]==')')p[a]='_';
	do{
		sign=0;
		for(a=1;a<=add;a++){
			if(p[a]=='p')a+=2;
			else if(p[a]=='('){
				sign=1;
				for(b=a+1;b<=add;b++){
					if(p[b]=='(')break;
					else if(p[b]=='p')b+=2;
					else if(p[b]==')'){
						p[a]=p[b]=0;
						addr=priority_operation(&p[a],b-a,addr);
						break;
					}
					else if(b==add){
						p[a]=0;
						addr=priority_operation(&p[a],b-a,addr);
					}
				}
			}
		}
	}while(sign);
	for(a=1;a<=add;a++){
		if(p[a]=='p')a+=2;
		else if(p[a]==')'){
			p[a]=0;
			addr=priority_operation(p,a,addr);
		}
	}
	priority_operation(p,add,addr);
	for(a=1;a<=add;a++)if(p[a]=='p'){
		display_result((double xdata*)(unsigned int)(p[a+1]<<8|p[a+2]),str,add);
		return;
	}
}