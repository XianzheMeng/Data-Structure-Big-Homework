#include<iostream>

#define ADD 1
#define SUB -1

struct Ind{
	float c;
	int e;
	Ind* next;
};

using namespace std;

void init(Ind* ind){
	int n;
	cin>>n;
	float* c=(float*)malloc(n*sizeof(float));
	int* e=(int*)malloc(n*sizeof(int));
	for(int i=0;i<n;i++){
		cin>>c[i]>>e[i];
	}
	Ind* ptr=ind;
	for(int i=0;i<n-1;i++){
		ind->c=c[i];
		ind->e=e[i];
		Ind* newind=(Ind*)malloc(sizeof(Ind));
		ind->next=newind;
		ind=newind;
	}
	ind->c=c[n-1];
	ind->e=e[n-1];
	ind->next=NULL;
	ind=ptr;
}

void show(Ind* ind){
	int record=0;
	while(ind!=NULL){
		if(ind->e==0){
			if(ind->c>0&&record==1){
				cout<<"+"<<ind->c;
				record=1;
			}
			else if(ind->c!=0){
				cout<<ind->c;
				record=1;
			}
			ind=ind->next;
			continue; 
		}
		if(ind->c!=0){
			if(record==1&&ind->c>0) cout<<"+";
			cout<<ind->c<<"x^"<<ind->e;
			record=1;
		}
		ind=ind->next;
	}
	if(record==0){
		cout<<"0";
	}
}

void add(Ind* ind1,Ind* ind2,int mode){
	Ind* inda=(Ind*)malloc(sizeof(Ind));
	Ind* record=ind1;
	Ind* record2=inda;
	while(ind1->next!=NULL){
		inda->e=ind1->e;
		inda->c=ind1->c;
		ind1=ind1->next;
		Ind* newind=(Ind*)malloc(sizeof(Ind));
		inda->next=newind;
		newind->next=NULL;
		inda=inda->next;
	}
	inda->e=ind1->e;
	inda->c=ind1->c;
	inda->next=NULL;
	ind1=record;
	inda=record2;
	Ind* indb=(Ind*)malloc(sizeof(Ind));
	record=ind2;
	record2=indb;
	while(ind2->next!=NULL){
		indb->e=ind2->e;
		indb->c=ind2->c*mode;
		ind2=ind2->next;
		Ind* newind=(Ind*)malloc(sizeof(Ind));
		indb->next=newind;
		newind->next=NULL;
		indb=indb->next;
	}
	indb->e=ind2->e;
	indb->c=ind2->c*mode;
	indb->next=NULL;
	ind2=record;
	indb=record2;
	Ind* ptr=inda;
	if(inda->e>indb->e){
		ptr=indb;
		while(indb->next!=NULL&&inda->e>indb->next->e){
			indb=indb->next;
		}
		Ind* temp=indb->next;
		indb->next=inda;
		indb=temp;
	}
	Ind* p=inda;
	while(inda!=NULL&&indb!=NULL){
		if(inda->e==indb->e){
			inda->c+=indb->c;
			p=inda;
			inda=inda->next;
			indb=indb->next;
		}
		else if(inda->e<indb->e){
			p=inda;
			inda=inda->next;
		}
		else{
			p->next=indb;
			Ind* temp=indb->next;
			indb->next=inda;
			indb=temp;
			p=p->next;
		}
	}
	if(indb!=NULL){
		p->next=indb;
	}
	inda=ptr;
	show(inda);
}

int main(){
	Ind* ind1=(Ind*)malloc(sizeof(Ind));
	Ind* ind2=(Ind*)malloc(sizeof(Ind));
	init(ind1);
	init(ind2);
	add(ind1,ind2,ADD);
	cout<<"\n";
	add(ind1,ind2,SUB);
	return 0;
}
