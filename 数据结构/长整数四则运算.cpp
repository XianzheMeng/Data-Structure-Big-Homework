#include<iostream>
#include<string.h>
#include<math.h>

#define POS 0
#define NEG 1

struct Ind{
	Ind* front;
	int data;
	Ind* next;
};

using namespace std;

void convert(char* str,Ind* &ind){
	int pole=0;
	int integer=0;
	if(str[0]=='-'){
		pole=1;
		for(int i=0;i<strlen(str)-1;i++){
			str[i]=str[i+1];
		}
		str[strlen(str)-1]='\0';
	}
	char repo[17]={'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',0};
	for(int i=16-strlen(str);i<16;i++){
		repo[i]=str[i-16+strlen(str)];
	}
	char temprepo[5]={0};
	Ind* ptr=ind;
	for(int i=0;i<3;i++){
		for(int j=0;j<4;j++){
			temprepo[j]=repo[4*i+j];
		}
		integer=atoi(temprepo);
		ind->data=integer;
		if(pole==1){
			ind->data*=-1;
		}
		Ind* newind=(Ind*)malloc(sizeof(Ind));
		ind->next=newind;
		newind->front=ind;
		ind=newind;
	}
	for(int j=0;j<4;j++){
		temprepo[j]=repo[4*3+j];
	}
	integer=atoi(temprepo);
	ind->data=integer;
	if(pole==1){
		ind->data*=-1;
	}
	ind->next=ptr;
	ptr->front=ind;
	ind=ptr;
}

int add(Ind* ind1,Ind* ind2,Ind* addind){
	Ind* ptr=addind;
	for(int i=0;i<3;i++){
		addind->data=ind1->data+ind2->data;
		Ind* newind=(Ind*)malloc(sizeof(Ind));
		ind1=ind1->next;
		ind2=ind2->next;
		addind->next=newind;
		newind->front=addind;
		addind=newind;
	}
	addind->data=ind1->data+ind2->data;
	addind->next=ptr;
	ptr->front=addind;
	addind=ptr;
	int pole=POS;
	for(int i=0;i<4;i++){
		if(addind->data<0){
			pole=NEG;
			break;
		}
		else if(addind->data>0) break;
		addind=addind->next;
	}
	addind=ptr;
	if(pole==POS){
		for(int i=0;i<3;i++){
			addind=addind->front;
			if(addind->data>=10000){
				addind->data-=10000;
				addind->front->data+=1;
			}
			else if(addind->data<0){
				addind->data+=10000;
				addind->front->data-=1;
			}
		}
	}
	addind=ptr;
	if(pole==NEG){
		for(int i=0;i<3;i++){
			addind=addind->front;
			if(addind->data<=-10000){
				addind->data+=10000;
				addind->front->data-=1;
			}
			else if(addind->data>0){
				addind->data-=10000;
				addind->front->data+=1;
			}		
		}
	}
	return pole;
}

void ind2str(Ind* ind,int pole,char* str){
	char temp[5]={0};
	for(int i=0;i<4;i++){
		char a1[5]={'0'};
		char a2[5]={'0','0'};
		char a3[5]={'0','0','0'};
		memset(temp,0,sizeof(temp));
		sprintf(temp,"%d",abs(ind->data));
		if(strlen(temp)==3){
			strcat(a1,temp);
			strcpy(temp,a1);
		}
		if(strlen(temp)==2){
			strcat(a2,temp);
			strcpy(temp,a2);
		}
		if(strlen(temp)==1){
			strcat(a3,temp);
			strcpy(temp,a3);
		}
		ind=ind->next;
		strcat(str,temp);
	}
	char p[20]={'-','\0'};
	if(pole==1){
		strcat(p,str);
		memset(str,0,sizeof(str));
		strcpy(str,p);		
	}
}

void pre(char* str){
	for(int i=0;i<strlen(str);i++){
		if(str[i]==','){
			for(int j=i;j<strlen(str)-1;j++){
				str[j]=str[j+1];
			}
			str[strlen(str)-1]='\0';
		}
	}
}

void post(char* str){
	if(strcmp(str,"0000000000000000")==0){
		strcpy(str,"0");
		return;
	}
	if(str[0]!='-'){
		while(1){
			if(str[0]!='0'){
				break;
			}
			else{
				for(int j=0;j<strlen(str)-1;j++){
					str[j]=str[j+1];
				}
				str[strlen(str)-1]='\0';
			}
		}	
	}
	if(str[0]=='-'){
		while(1){
			if(str[1]!='0'){
				break;
			}
			else{
				for(int j=1;j<strlen(str)-1;j++){
					str[j]=str[j+1];
				}
				str[strlen(str)-1]='\0';
			}
		}			
	}
	for(int i=strlen(str)-4;i>=0;i-=4){
		for(int j=strlen(str);j>i;j--){
			str[j]=str[j-1];
		}
		str[i]=',';
	}
	if(str[0]==','){
		for(int i=0;i<strlen(str)-1;i++){
			str[i]=str[i+1];
		}
		str[strlen(str)-1]='\0';
	}
	if(str[0]=='-'&&str[1]==','){
		for(int i=1;i<strlen(str)-1;i++){
			str[i]=str[i+1];
		}
		str[strlen(str)-1]='\0';
	}
}

int main(){
	char num1[20]={0};
	char num2[20]={0};
	cin>>num1;
	cin>>num2;
	Ind* ind1=(Ind*)malloc(sizeof(Ind));
	ind1->front=NULL;
	ind1->next=NULL;
	Ind* ind2=(Ind*)malloc(sizeof(Ind));
	ind2->front=NULL;
	ind2->next=NULL;
	Ind* addind=(Ind*)malloc(sizeof(Ind));
	addind->front=NULL;
	addind->next=NULL;
	pre(num1);
	pre(num2);
	convert(num1,ind1);
	convert(num2,ind2);
	int pole=add(ind1,ind2,addind);
	char str[20]={0};
	ind2str(addind,pole,str);
	post(str);
	cout<<str;
	return 0;
}
