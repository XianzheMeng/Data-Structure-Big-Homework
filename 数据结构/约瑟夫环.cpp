/*
input:  7 20
        3 1 7 2 4 8 4
output: 6 1 4 7 2 3 5
*/

#include<iostream>

struct Ind{
	int num;
	int pwd;
	Ind* next;
};

using namespace std;

void init(int n,Ind* &ind){
	Ind* temp=ind;
	for(int i=0;i<n-1;i++){
		cin>>ind->pwd;
		ind->num=i;
		ind->next=(Ind*)malloc(sizeof(Ind));
		ind=ind->next;
	}
	cin>>ind->pwd;
	ind->num=n-1;
	ind->next=temp;
	ind=temp;
	return;
}

int single(int n,int m,Ind* &ind){
	if(m>1){
		int temp=(m-1)%n;
		for(int i=0;i<temp-1+n;i++){
			ind=ind->next;
		}	
	}
	else{
		for(int i=0;i<n-1;i++){
			ind=ind->next;
		} 
	}
	Ind* t=ind->next;
	cout<<ind->next->num+1<<" ";
	ind->next=ind->next->next;
	ind=ind->next;
	int record=t->pwd;
	free(t);
	return record;
}

void group(int &n,int &m,Ind* &ind){
	int count=0,nn=n;
	for(int i=0;i<nn;i++){
		m=single(n,m,ind);
		n--;
	}
	return;
}

int main(){
	int n,m;
	cin>>n>>m;
	Ind* ind=(Ind*)malloc(sizeof(Ind));
	init(n,ind);
	group(n,m,ind);
	return 0;
}
