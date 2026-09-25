#define uchar unsigned char
#define uint unsigned int

uchar weizhi=0x80;//液晶指针位置跟随标志

void delay(uint t)		 //延时函数
{
	uchar i;
	while(t--)for(i=110;i>0;i--);
}

//浮点数位数计算函数返回值为浮点数位数，若为小数则为负
//如1.23e6返回6，1.23e-5返回-5
char lang(float dat)  
{
	char i=0;
	if(dat<0)dat=0-dat;
	if(dat>1)
	{
		while(dat>=1)
		{
			dat=dat/10;
			i++;
		}
		i=i-1;
	}
	else if((dat>0)&&dat<=1)
	{
		while(dat<1)
		{
			dat=dat*10;
			i++;
		}
		return i=0-i;
	}
	return i;
}

//阶乘函数 只能对小于35的整数进行计算，否则数据错误
float jiecheng(uchar dat)
{
	float ans=1;
	while(dat>0)
	{
		ans=ans*dat;
		dat=dat-1;
	}
	return ans;	
}

//组合计算函数
float zuhe(uchar n,uchar r)
{
	float ans,temp;
	ans=jiecheng(n)/jiecheng(r);
	temp=jiecheng(n-r);
	ans=ans/temp;
	return ans;
}

//排列计算函数
float pailie(uchar n,uchar r)
{
	float ans;
	ans=jiecheng(n)/jiecheng(n-r);
	return ans;
}

//累加计算函数
float leijia(float dat,float dat2)
{
	float ans;
	ans=(dat+dat2)*(dat2-dat+1)/2;
	return ans;
}

