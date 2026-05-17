#include<iostream>
#include<string.h>

struct Ind{
	char word;
	Ind* next;
};

using namespace std;

void init(char* &word1,char* &word2,Ind* &ind1,Ind* &ind2){
	Ind* temp=(Ind*)malloc(sizeof(Ind));
	temp=ind1;
	for(int i=0;i<strlen(word1)-1;i++){
		temp->word=word1[i];
		Ind* newtemp=(Ind*)malloc(sizeof(Ind));
		temp->next=newtemp;
		temp=newtemp;
	}
	temp->word=word1[strlen(word1)-1];
	temp->next=NULL;
	temp=ind2;
	for(int i=0;i<strlen(word2)-1;i++){
		temp->word=word2[i];
		Ind* newtemp=(Ind*)malloc(sizeof(Ind));
		temp->next=newtemp;
		temp=newtemp;
	}
	temp->word=word2[strlen(word2)-1];
	temp->next=NULL;
	return;
}

void show(Ind* ind){
	for(int i=0;ind!=NULL;i++){
		cout<<ind->word;
		ind=ind->next;
	}
}

void sub(Ind* ind1,Ind* unionset,Ind* &subind){
	Ind* record=(Ind*)malloc(sizeof(Ind));
	record=subind;
	while(ind1->next!=NULL){
		subind->word=ind1->word;
		Ind* newind=(Ind*)malloc(sizeof(Ind));
		newind->next=NULL;
		subind->next=newind;
		subind=newind;
		ind1=ind1->next;
	}
	subind->word=ind1->word;
	subind->next=NULL;
	subind=record;
	while(unionset!=NULL){
		if(subind->word==unionset->word){
			subind=subind->next;
			record=subind;
		}
		else{
			int flag=0;
			while(subind->next!=NULL&&subind->next->next!=NULL){
				if(subind->next->word==unionset->word){
					Ind* temp=(Ind*)malloc(sizeof(Ind));
					temp=subind->next;
					subind->next=subind->next->next;
					free(temp);
					subind=record;
					flag=1;
					break;
				}
				subind=subind->next;
			}
			if(flag==0){
				free(subind->next);
				subind->next=NULL;
			}
			
		}
		unionset=unionset->next;
		subind=record;
	}
}

void set(Ind* &ind1,Ind* &ind2){
	Ind* unionset=(Ind*)malloc(sizeof(Ind));
	Ind* ptr=(Ind*)malloc(sizeof(Ind));
	ptr=unionset;
	Ind* inda=(Ind*)malloc(sizeof(Ind));
	Ind* indb=(Ind*)malloc(sizeof(Ind));
	Ind* back=(Ind*)malloc(sizeof(Ind));
	inda=ind1;
	for(inda=ind1;inda!=NULL;){
		for(indb=ind2;indb!=NULL;){
			if(inda->word==indb->word){
				unionset->word=inda->word;
				Ind* newset=(Ind*)malloc(sizeof(Ind));
				newset->next=NULL;
				unionset->next=newset;
				back=unionset;
				unionset=newset;
				break;
			}
			indb=indb->next;
		}
		inda=inda->next;
	}
	back->next=NULL;
	free(unionset);
	Ind* ind1sub=(Ind*)malloc(sizeof(Ind));
	Ind* ind2sub=(Ind*)malloc(sizeof(Ind));
	unionset=ptr;
	sub(ind1,unionset,ind1sub);
	unionset=ptr;
	sub(ind2,unionset,ind2sub);
	cout<<"add:\n";
	show(ind1sub);show(unionset);show(ind2sub);cout<<"\n";
	cout<<"union:\n";
	show(unionset);
	cout<<"\n";
	cout<<"sub1:\n";
	show(ind1sub);cout<<"\n";
	cout<<"sub2:\n";
	show(ind2sub);
}

void paichong(char* &word1,char* &word2){
	for(int i=0;i<strlen(word1);i++){
		for(int j=i+1;j<strlen(word1);j++){
			if(word1[i]==word1[j]){
				for(int k=j;k<strlen(word1)-1;k++){
					word1[k]=word1[k+1];
				}
				word1[strlen(word1)-1]='\0';
				break;
			}
		}
	}
	for(int i=0;i<strlen(word2);i++){
		for(int j=i+1;j<strlen(word2);j++){
			if(word2[i]==word2[j]){
				for(int k=j;k<strlen(word2)-1;k++){
					word2[k]=word2[k+1];
				}
				word2[strlen(word2)-1]='\0';
				break;
			}
		}
	}
	for(int i=0;i<strlen(word1);i++){
		if(word1[i]==' '){
			for(int j=i;j<strlen(word1)-1;j++){
				word1[j]=word1[j+1];
			}
			word1[strlen(word1)-1]='\0';
		}
	}
	for(int i=0;i<strlen(word2);i++){
		if(word2[i]==' '){
			for(int j=i;j<strlen(word2)-1;j++){
				word2[j]=word2[j+1];
			}
			word2[strlen(word2)-1]='\0';
		}
	}
}

int main(){
	char* word1=(char*)malloc(100*sizeof(char));
	
	char* word2=(char*)malloc(100*sizeof(char));
	Ind* ind1=(Ind*)malloc(sizeof(Ind));
	Ind* ind2=(Ind*)malloc(sizeof(Ind));
	memset(word1,0,100);
	memset(word2,0,100);
	fgets(word1,100,stdin);
	fgets(word2,100,stdin);
	word1[strlen(word1)-1]='\0';
	word2[strlen(word2)-1]='\0';
	paichong(word1,word2);
	init(word1,word2,ind1,ind2);
	set(ind1,ind2);
	return 0;
}
