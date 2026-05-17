/*
2
A 1 5
A 2 10
D 1 15
A 3 20
A 4 25
A 5 30
D 2 35
D 4 40
E 0 0
*/

#include<iostream>

#define EXIST 0
#define WAIT  1
#define OUT   2

struct Ind{
	char state;
	int license;
	int time;
};

struct Car{
	int state;
	int license;
	int time1;
	int time2;
	int place;
};

using namespace std;

void op(Ind* ind,int n){
	int count=0;
	int count_new=0;
	Car stack[20]={0};
	Car queue[50]={0};
	Ind temp;
	Car car[100]={0};
	int place[20]={0};//0是没车，1是有车 
	int stack_front=0,stack_rear=0;
	int queue_front=0,queue_rear=0;
	while(1){
		int action=ind[count].state;
		int lic=ind[count].license;
		int tim=ind[count].time;
		if(action=='A'){
			if(stack_rear==n){
				car[count_new].state=WAIT;
				car[count_new].license=ind[count].license;
				car[count_new].time1=ind[count].time;
				car[count_new].time2=ind[count].time;
				queue[queue_rear]=car[count_new];
				queue_rear=(queue_rear+1)%50;
				cout<<"等着吧您！\n";
				count_new++;
			}
			else{
				car[count_new].state=EXIST;
				car[count_new].license=ind[count].license;
				car[count_new].time1=ind[count].time;
				car[count_new].time2=ind[count].time;
				int temp;
				for(int i=0;i<n;i++){
					if(place[i]==0){
						temp=i;
						place[i]=1;
						break;
					}
				}
				stack[stack_rear]=car[count_new];
				stack_rear++;
				car[count_new].place=temp;
				cout<<car[count_new].license<<"请进！"<<temp+1<<"号位\n";
				count_new++;
			}
		}
		else if(action=='D'){
			int k=-1;
			for(int i=0;i<stack_rear;i++){
				if(stack[k].license==lic){
					k=i;
					break;
				}
			}
			for(int i=k;i<stack_rear-1;i++){
				stack[i]=stack[i+1];
			}
			stack_rear--;
			stack[k].state=OUT;
			cout<<stack[k].license<<"停留时间："<<tim-stack[k].time1<<"交费时间："<<tim-stack[k].time2<<"\n";
			place[stack[k].place]=0;
			if(queue_front!=queue_rear){
				stack[k]=queue[queue_front];
				queue[queue_front].time2=tim;
				queue[queue_front].place=stack[k].place;
				stack[k].place=1;
				cout<<queue[queue_front].license<<"请进！"<<stack[k].place+1<<"号位\n";
				place[stack[k].place]=1;
				queue_front=(queue_front+1)%50;
			}
		}
		else if(action=='E'){
			cout<<"收工了！\n";
			break;
		}
		count++;
	}
}

void input(Ind* ind){
	char action;
	int license,time;
	int count=0;
	while(1){
		cin>>action>>license>>time;
		ind[count].state=action;
		ind[count].license=license;
		ind[count].time=time;
		if(action=='E') break;
		count++;
	}
}

int main(){
	Ind ind[200]={0};
	int n;
	cin>>n;
	input(ind);
	op(ind,n);
	return 0;
}
