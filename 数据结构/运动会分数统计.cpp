/*
4 3 2
thu pku hust whu
5 5 1 a 2 b 3 c 4 d 3 e
3 5 2 r 3 q 4 w 1 a 3 f
1 5 4 r 2 qw 1 q 3 h 3 h
2 3 4 q 3 w 3 wq
4 3 1 w 2 w 3 asa
*/

#include<iostream>

struct Position{
	int school;
	char name[20];
	int score;
};

using namespace std;

void input(int m,int w,Position** pos){
	int project,type;
	for(int i=0;i<m+w;i++){
		cin>>project;
		cin>>type;
		if(type==3){
			for(int j=0;j<3;j++){
				cin>>pos[project-1][j].school>>pos[project-1][j].name;
			}
			pos[project-1][0].score=5;
			pos[project-1][1].score=3;
			pos[project-1][2].score=2;
			pos[project-1][3].score=0;
		}
		else if(type==5){
			for(int j=0;j<5;j++){
				cin>>pos[project-1][j].school>>pos[project-1][j].name;
			}
			pos[project-1][0].score=7;
			pos[project-1][1].score=5;
			pos[project-1][2].score=3;
			pos[project-1][3].score=2;
			pos[project-1][4].score=1;		
		}
		else{
			cout<<"error!";
			exit(0);
		}
	}
	return;
}

void school(int n,int m,int w,Position** pos,char** schoolname){
	for(int k=0;k<n;k++){
		cout<<schoolname[k]<<"\n";
		for(int i=0;i<m+w;i++){
			for(int j=0;j<5;j++){
				if(pos[i][j].score==0) break;
				if(pos[i][j].school==k+1)
				cout<<i+1<<" "<<j+1<<" "<<pos[i][j].name<<" "<<pos[i][j].score<<"\n";
			}
		}
	}
}

void count(int n,int m,int w,Position** pos,char** schoolname){
	int schoolboy[10]={0},schoolgirl[10]={0},schoolgroup[10]={0}; 
	for(int k=0;k<n;k++){
		for(int i=0;i<m+w;i++){
			for(int j=0;j<5;j++){
				if(pos[i][j].score==0) break;
				if(pos[i][j].school==k+1){
					schoolgroup[k]+=pos[i][j].score;
					if(i<m){
						schoolboy[k]+=pos[i][j].score;
					}
					else{
						schoolgirl[k]+=pos[i][j].score;
					}
				}
				
			}
		}
	}
	for(int i=0;i<n;i++){
		cout<<schoolname[i]<<"\n";
		cout<<i<<" "<<schoolboy[i]<<" "<<schoolgirl[i]<<" "<<schoolgroup[i]<<"\n";
	}
	return;
}

int main(){
	int n,m,w;
	cin>>n>>m>>w;
	char* schoolname[10];
	for(int i=0;i<n;i++){
		schoolname[i]=(char*)malloc(100*sizeof(char));
	}
	for(int i=0;i<n;i++){
		cin>>schoolname[i];
	}
	Position** pos=(Position**)malloc((m+w)*sizeof(Position*));
	for(int i=0;i<m+w;i++){
		pos[i]=(Position*)malloc(n*sizeof(Position));
	}
	input(m,w,pos);
	school(n,m,w,pos,schoolname);
	count(n,m,w,pos,schoolname);
	return 0;
}
