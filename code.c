
//取格式,将单个数字变为一个固定字符(这里是'@')  "a1b223bb3"->"a@b@@@bb@" 
void get_str(uint8_t **str,char *nb){
	int i=0;
	while(**str!=0){
		if(!(**str>='0'&&**str<='9')){
			nb[i++]=**str;
		}
		else{
			nb[i++]='@';
		}
		(*str)++;
	}
	nb[i]=0;
}

//取格式,将连续的数字变为一个固定字符(这里是'@')  "a1b223bb3"->"a@b@bb@" 
void get_str1(uint8_t **str,char *nb){
	int i=0;
	uint8_t lid=0;
	while(**str!=0){
		if(!(**str>='0'&&**str<='9')){
			nb[i++]=**str;
			lid=0;
		}
		else{
			if(lid==0){
				nb[i++]='@';
				lid=1;
			}
		}
		(*str)++;
	}
	nb[i]=0;
}

//取数字,并且保留上一次取完数字的位置  "a1b223bb3"->1->233->3
uint16_t get_num(uint8_t **str){
	uint16_t n=0;
	while(!(**str>='0'&&**str<='9')){
		(*str)++;
	}
	while(**str>='0'&&**str<='9'){
		n=n*10+**str-'0';
		(*str)++;
	}
	return n;
}


//配合串口接收使用(16届国赛)
void uart_set(void){//串口数据处理
	if(rf){//receive flag,串口接收到数据时rf=1,执行
		uint8_t *p=rb;//rb为receive buff,保存串口接收到的数据
		uint8_t *p1=rb;
		char nb[RB_MAX]={0};//new buff,放处理过后的字符串
		char nb1[RB_MAX]={0};
		get_str(&p,nb);
		get_str1(&p1,nb1);
		if(strcmp(nb,"?")==0){
			printf("%s",iun);//iun为IDLE UN
		}
		else if(rb[0]=='S'&&rb[1]=='E'&&rb[2]=='T'&&rb[3]==':'&&code_check(rb)){//接收到"SET:...",设置密码,密码长度不确定
			memset(pwd_save,0,11);
			for(int i=0;i<strlen((char *)rb)-4;i++){
				pwd_save[i]=rb[i+4];
			}
			printf("OK");
		}
		else if(strcmp(nb,"TIME:@@@@@@")==0){//接收到"TIME:6位数字",设置时间
			sTime.Hours=(rb[5]-'0')*10+rb[6]-'0';
			sTime.Minutes=(rb[7]-'0')*10+rb[8]-'0';
			sTime.Seconds=(rb[9]-'0')*10+rb[10]-'0';
			HAL_RTC_SetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
			printf("OK");
		}
		else if(strcmp(nb,"DATE:@@@@@@")==0){//接收到"DATE:6位数字",设置日期
			sDate.Year=(rb[5]-'0')*10+rb[6]-'0';
			sDate.Month=(rb[7]-'0')*10+rb[8]-'0';
			sDate.Date=(rb[9]-'0')*10+rb[10]-'0';
			HAL_RTC_SetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
			printf("OK");
		}
		else if(strcmp(nb1,"PA@:@:@%:@")==0){//设置PA6/7的频率/占空比/时间,数字长度不定所以用sscanf,若数字个数不定可用get_num()
			int uch,ufre,ud,ut;
			sscanf((char *)rb,"PA%d:%d:%d%%:%d",&uch,&ufre,&ud,&ut);
			pch=uch-6;pf=ufre/1000.0;pd=ud;ot=ut;
			printf("OK");
		}
		else{
			printf("ERROR");
		}
		memset(rb,0,RB_MAX);
		rf=0;
	}
}