/*获取当前行，列*/
/*设置当前行，列*/
uchar g=0;
void delay(int c){
	int i, j;
	for(i = 0; i < c; i++)
		for(j = 0; j < 1000; j++);
}

/*清屏 320 = (160/8) * (128/8) = 20 * 16 = 320*/
void cls(void){
	int i;
	set_xy(0,0);
	for(i = 0; i < 320; i++)
		write_one(0x94,INC_WR);
}


uchar curx,cury;	/*纪录当前人物所在位置*/

uchar level_temp[8][8]={
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
};


void wirte_bg(void){
	int i;
	set_adr(0x50,0x01);
	for(i = 0; i < 2560; i++)
		write_one(bg[i],INC_WR);
}

void wirte_cgrom(void){
	int i;
//	set_adr(0x50,0x01);
//	for(i = 0; i < 2560; i++)
//		write_one(0xff,INC_WR);
	set_adr(0x00,0x0c);
	/*自定义字符写入CGROM*/
	for(i = 0; i < 848; i++)
		write_one(Lattice[i],INC_WR);
}

void start(void){
	uchar i;
	set_xy(0,0);
	for(i=0;i<20;i++)
		write_one(0x95,INC_WR);
	set_xy(0,15);
	for(i=0;i<20;i++)
		write_one(0x95,INC_WR);
	for(i=0;i<15;i++){
		set_xy(0,i);
		write_one(0x95,INC_WR);
		set_xy(19,i);
		write_one(0x95,INC_WR);
	}
	set_xy(18,1);
	write_one(0x96,INC_WR);
	set_xy(18,14);
	write_one(0x97,INC_WR);
	set_xy(1,1);
	write_one(0x98,INC_WR);
	set_xy(1,14);
	write_one(0x99,INC_WR);

	set_xy(7,6);
	write_one(0xaa,INC_WR);
	write_one(0xab,INC_WR);
	write_one(0xae,INC_WR);
	write_one(0xaf,INC_WR);
	write_one(0xb2,INC_WR);
	write_one(0xb3,INC_WR);
	set_xy(7,7);
	write_one(0xac,INC_WR);
	write_one(0xad,INC_WR);
	write_one(0xb0,INC_WR);
	write_one(0xb1,INC_WR);
	write_one(0xb4,INC_WR);
	write_one(0xb5,INC_WR);

	set_xy(6,8);
	write_one(0x9a,INC_WR);
	write_one(0x9b,INC_WR);
	write_one(0x9e,INC_WR);
	write_one(0x9f,INC_WR);
	write_one(0xa2,INC_WR);
	write_one(0xa3,INC_WR);
	write_one(0xa6,INC_WR);
	write_one(0xa7,INC_WR);
	set_xy(6,9);
	write_one(0x9c,INC_WR);
	write_one(0x9d,INC_WR);
	write_one(0xa0,INC_WR);
	write_one(0xa1,INC_WR);
	write_one(0xa4,INC_WR);
	write_one(0xa5,INC_WR);
	write_one(0xa8,INC_WR);
	write_one(0xa9,INC_WR);
	while(i){			/*此while语句判断确定键超级技巧*/
		switch(P1&0x1f){
			case 0x0f:
				i=0;
				break;				
		}
	}
}


void guan(void){
	/*推*/	
	set_xy(16,0);
	write_one(0xaa,INC_WR);
	write_one(0xab,INC_WR);
	set_xy(16,1);
	write_one(0xac,INC_WR);
	write_one(0xad,INC_WR);
	/*箱*/
	set_xy(16,2);
	write_one(0xae,INC_WR);
	write_one(0xaf,INC_WR);
	set_xy(16,3);
	write_one(0xb0,INC_WR);
	write_one(0xb1,INC_WR);
	/*子*/
	set_xy(16,4);
	write_one(0xb2,INC_WR);
	write_one(0xb3,INC_WR);
	set_xy(16,5);
	write_one(0xb4,INC_WR);
	write_one(0xb5,INC_WR);	
	/*第*/
	set_xy(16,8);
	write_one(0xd2,INC_WR);
	write_one(0xd3,INC_WR);	
	set_xy(16,9);
	write_one(0xd4,INC_WR);
	write_one(0xd5,INC_WR);
	/*几*/	
	set_xy(16,10);
	write_one(0xd6,INC_WR);
	write_one(0xd6+2*(g+1),INC_WR);	
	set_xy(16,11);
	write_one(0xd7,INC_WR);
	write_one(0xd7+2*(g+1),INC_WR);
	/*关*/	
	set_xy(16,12);
	write_one(0xce,INC_WR);
	write_one(0xcf,INC_WR);	
	set_xy(16,13);
	write_one(0xd0,INC_WR);
	write_one(0xd1,INC_WR);
	/*酷*/
	set_xy(18,0);
	write_one(0x9a,INC_WR);
	write_one(0x9b,INC_WR);	
	set_xy(18,1);
	write_one(0x9c,INC_WR);
	write_one(0x9d,INC_WR);
	/*玩*/	
	set_xy(18,2);
	write_one(0x9e,INC_WR);
	write_one(0x9f,INC_WR);	
	set_xy(18,3);
	write_one(0xa0,INC_WR);
	write_one(0xa1,INC_WR);
	/*制*/	
	set_xy(18,4);
	write_one(0xa2,INC_WR);
	write_one(0xa3,INC_WR);	
	set_xy(18,5);
	write_one(0xa4,INC_WR);
	write_one(0xa5,INC_WR);
	/*作*/	
	set_xy(18,6);
	write_one(0xa6,INC_WR);
	write_one(0xa7,INC_WR);	
	set_xy(18,7);
	write_one(0xa8,INC_WR);
	write_one(0xa9,INC_WR);
}




void printc(uchar i, uchar j, uchar c){
	set_xy(i*2,j*2);
	switch(c){	
		case 0:
			   write_one(0x94,INC_WR);
			   write_one(0x94,INC_WR);
			   set_xy(i*2,j*2+1);
			   write_one(0x94,INC_WR);
			   write_one(0x94,INC_WR);
			   break;					   
		case 1:		/*人物1*/
			   write_one(0x80,INC_WR);
			   write_one(0x81,INC_WR);
			   set_xy(i*2,j*2+1);
			   write_one(0x82,INC_WR);
			   write_one(0x83,INC_WR);
			   break;
		case 2:		/*砖头2*/
			   write_one(0x84,INC_WR);
			   write_one(0x85,INC_WR);
			   set_xy(i*2,j*2+1);
			   write_one(0x86,INC_WR);
			   write_one(0x87,INC_WR);
			   break;
		case 3:		/*箱子3*/
			   write_one(0x88,INC_WR);
			   write_one(0x89,INC_WR);
			   set_xy(i*2,j*2+1);
			   write_one(0x8a,INC_WR);
			   write_one(0x8b,INC_WR);
			   break;
		case 4:		/*目的4*/
			   write_one(0x8c,INC_WR);
			   write_one(0x8d,INC_WR);
			   set_xy(i*2,j*2+1);
			   write_one(0x8e,INC_WR);
			   write_one(0x8f,INC_WR);
			   break;
		case 5:		/*成功5*/
			   write_one(0x90,INC_WR);
			   write_one(0x91,INC_WR);
			   set_xy(i*2,j*2+1);
			   write_one(0x92,INC_WR);
			   write_one(0x93,INC_WR);
			   break;
	}
	
}



void pushbox(){
	uchar i,j;
	/*根据level.h中的值进行输出单个字符点阵为16*16，显示8*8个字符*/
	for(i = 0; i < 8; i++)
		for(j = 0; j < 8; j++){
			level_temp[i][j]=level[g][j][i];
			switch(level_temp[i][j]){	
				case 0:
					   printc(i,j,0);
					   break;					   
				case 1:		/*人物1*/
					   curx=i;
					   cury=j;
					   printc(i,j,1);
					   break;
				case 2:		/*砖头2*/
					   printc(i,j,2);
					   break;
				case 3:		/*箱子3*/
					   printc(i,j,3);
					   break;
				case 4:		/*目的4*/
					   printc(i,j,4);
					   break;
				case 5:		/*成功5*/
					   printc(i,j,5);
					   break;
			}
		}
	set_xy(curx*2,cury*2);
}

void pass(){
	uchar i,j,k=1;
	for(i = 0; i < 8; i++){
		if(k==0) break;
		for(j = 0; j < 8; j++)
			if(level[g][j][i]==4||level[g][j][i]==5)
				if(level_temp[i][j]==5)
					k=1;
				else{
					k=0;
					break;
				}
	}
	if(k==1){
		if(g<8)g+=1;
		else g=0;
		pushbox();
		guan();
	}
}

void keyboard(){
	switch(P1&0x1f){
		case 0x1e:		/*上 cury > 0 && */
				  if(level_temp[curx][cury-1]==0||level_temp[curx][cury-1]==4){

				      if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					      level_temp[curx][cury]=4;
				          printc(curx,cury,4);
					  }
					  else{
					      level_temp[curx][cury]=0;
				          printc(curx,cury,0);
					  }

				      cury=cury-1;
					  level_temp[curx][cury]=1;
				      printc(curx,cury,1);
				  }

				  else if(level_temp[curx][cury-1]==3){

				      if(level_temp[curx][cury-2]==0){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }

						  cury=cury-1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx][cury-1]=3;
						  printc(curx,cury-1,3);
					  }

					  else if(level_temp[curx][cury-2]==4){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }

						  cury=cury-1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx][cury-1]=5;
						  printc(curx,cury-1,5);
						  pass();
					  }

				  }

				  else if(level_temp[curx][cury-1]==5){

				      if(level_temp[curx][cury-2]==0){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }
						  cury=cury-1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx][cury-1]=3;
						  printc(curx,cury-1,3);
						      
					  }
					  else if(level_temp[curx][cury-2]==4){

					      if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }
					      
						  cury=cury-1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx][cury-1]=5;
						  printc(curx,cury-1,5);
						  pass();
					  }
					  pass();
				  }
				  while((P1&0x01)==0);
				  break;
		case 0x1d:	   /*下 cury < 7 && */
				  if(level_temp[curx][cury+1]==0||level_temp[curx][cury+1]==4){

				      if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					      level_temp[curx][cury]=4;
				          printc(curx,cury,4);
					  }
					  else{
					      level_temp[curx][cury]=0;
				          printc(curx,cury,0);
					  }

				      cury=cury+1;
					  level_temp[curx][cury]=1;
				      printc(curx,cury,1);
				  }

				  else if(level_temp[curx][cury+1]==3){

				      if(level_temp[curx][cury+2]==0){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }

						  cury=cury+1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx][cury+1]=3;
						  printc(curx,cury+1,3);
					  }

					  else if(level_temp[curx][cury+2]==4){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }

						  cury=cury+1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx][cury+1]=5;
						  printc(curx,cury+1,5);
						  pass();
					  }

				  }

				  else if(level_temp[curx][cury+1]==5){

				      if(level_temp[curx][cury+2]==0){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }
						  cury=cury+1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx][cury+1]=3;
						  printc(curx,cury+1,3);
						      
					  }
					  else if(level_temp[curx][cury+2]==4){

					      if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }
					      
						  cury=cury+1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx][cury+1]=5;
						  printc(curx,cury+1,5);
						  pass();
					  }
					  pass();
				  }
				  while((P1&0x02)==0);
				  break;
		case 0x1b:	   /*左 curx > 0 && */
				  if(level_temp[curx-1][cury]==0||level_temp[curx-1][cury]==4){

				      if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					      level_temp[curx][cury]=4;
				          printc(curx,cury,4);
					  }
					  else{
					      level_temp[curx][cury]=0;
				          printc(curx,cury,0);
					  }

				      curx=curx-1;
					  level_temp[curx][cury]=1;
				      printc(curx,cury,1);
				  }

				  else if(level_temp[curx-1][cury]==3){

				      if(level_temp[curx-2][cury]==0){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }

						  curx=curx-1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx-1][cury]=3;
						  printc(curx-1,cury,3);
					  }

					  else if(level_temp[curx-2][cury]==4){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }

						  curx=curx-1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx-1][cury]=5;
						  printc(curx-1,cury,5);
						  pass();
					  }

				  }

				  else if(level_temp[curx-1][cury]==5){

				      if(level_temp[curx-2][cury]==0){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }
						  curx=curx-1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx-1][cury]=3;
						  printc(curx-1,cury,3);
						      
					  }
					  else if(level_temp[curx-2][cury]==4){

					      if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }
					      
						  curx=curx-1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx-1][cury]=5;
						  printc(curx-1,cury,5);
						  pass();
					  }
					  pass();
				  }
				  while((P1&0x04)==0);
				  break;
		case 0x17:	   /*右 curx < 9 && */
				  if(level_temp[curx+1][cury]==0||level_temp[curx+1][cury]==4){

				      if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					      level_temp[curx][cury]=4;
				          printc(curx,cury,4);
					  }
					  else{
					      level_temp[curx][cury]=0;
				          printc(curx,cury,0);
					  }

				      curx=curx+1;
					  level_temp[curx][cury]=1;
				      printc(curx,cury,1);
				  }

				  else if(level_temp[curx+1][cury]==3){

				      if(level_temp[curx+2][cury]==0){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }

						  curx=curx+1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx+1][cury]=3;
						  printc(curx+1,cury,3);
					  }

					  else if(level_temp[curx+2][cury]==4){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }

						  curx=curx+1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx+1][cury]=5;
						  printc(curx+1,cury,5);
						  pass();
					  }

				  }

				  else if(level_temp[curx+1][cury]==5){

				      if(level_temp[curx+2][cury]==0){

				          if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }
						  curx=curx+1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx+1][cury]=3;
						  printc(curx+1,cury,3);
						      
					  }
					  else if(level_temp[curx+2][cury]==4){

					      if(level[g][cury][curx]==4||level[g][cury][curx]==5){
					          level_temp[curx][cury]=4;
				              printc(curx,cury,4);
					      }
						  else{
					          level_temp[curx][cury]=0;
				              printc(curx,cury,0);
					      }
					      
						  curx=curx+1;
						  level_temp[curx][cury]=1;
						  printc(curx,cury,1);
						  level_temp[curx+1][cury]=5;
						  printc(curx+1,cury,5);
						  pass();
					  }
					  pass();
				  }
				  while((P1&0x08)==0);
				  break;
	    case 0x0f:	   /*确定*/
				  break;
	}
}