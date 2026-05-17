#include<iostream>

using namespace std;

void dfs(int pos,int n,int* state,int* stack,int stack_front,int &stack_rear){
	int cur=state[pos];
	if(cur==1){
		stack[stack_rear++]=pos+1;
		state[pos]=2;
		for(int i=0;i<n;i++) dfs(i,n,state,stack,stack_front,stack_rear);
	}
	else if(cur==2){
		stack[stack_rear-1]=0;
		stack_rear--;
		state[pos]=3;
		for(int i=0;i<n;i++) dfs(i,n,state,stack,stack_front,stack_rear);
	}
	else{
		int note[100]={0};
		for(int i=0;i<stack_rear;i++){
			cout<<stack[i];
			if(note[stack[i]-1]==0){
				cout<<"ÈëÕ»";
				note[stack[i]-1]=1;
			}
			else{
				cout<<"³öÕ»";
			}
		}
		cout<<"\n";
	}
}

void diaodu(int n,int* state,int* stack){
	int count=0;
	int stack_front=0;
	int stack_rear=0;
	for(int i=0;i<n;i++){
		dfs(i,n,state,stack,stack_front,stack_rear);
	}
}

int main(){
	int state[100];
	int n;
	cin>>n;
	for(int i=0;i<n;i++){
		state[i]=1;
	}
	int stack[100]={0};
	diaodu(n,state,stack);
}
