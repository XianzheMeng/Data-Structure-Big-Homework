#include<iostream>
#include<math.h>

using namespace std;

struct Q{
	int up;
	int down;
}; 

int gcd(int a,int b){
	if(b==0) return a;
	else     return gcd(b,a%b);
}


void show(Q q){
	int temp=gcd(abs(q.up),abs(q.down));
	q.up/=temp;
	q.down/=temp;
	if(q.up>0&&q.down>0)   cout<<q.up<<"/"<<q.down;
	if(q.up>0&&q.down==0)  cout<<"NaN";
	if(q.up>0&&q.down<0)   cout<<"-"<<q.up<<"/"<<-q.down;
	if(q.up==0&&q.down>0)  cout<<"0";
	if(q.up==0&&q.down==0) cout<<"NaN";
	if(q.up==0&&q.down<0)  cout<<"0";
	if(q.up<0&&q.down>0)   cout<<"-"<<-q.up<<"/"<<q.down;
	if(q.up<0&&q.down==0)  cout<<"NaN";
	if(q.up<0&&q.down<0)   cout<<-q.up<<"/"<<-q.down;
	cout<<"\n";
	return;
}

Q add(Q q1,Q q2){
	Q q;
	q.up=q1.up*q2.down+q1.down*q2.up;
	q.down=q1.down*q2.down;
	return q;
}

Q sub(Q q1,Q q2){
	Q q;
	q.up=q1.up*q2.down-q1.down*q2.up;
	q.down=q1.down*q2.down;
	return q;
}

Q mul(Q q1,Q q2){
	Q q;
	q.up=q1.up*q2.up;
	q.down=q1.down*q2.down;
	return q;
}

int main(){
	int up1,down1,up2,down2;
	cin>>up1>>down1>>up2>>down2;
	if(down1==0||down2==0){
		cout<<"down!=0";
		return 0;
	}
	Q q1,q2;
	q1.up=up1;
	q1.down=down1;
	q2.up=up2;
	q2.down=down2;
	show(q1);
	show(q2);
	Q addq=add(q1,q2);
	show(addq);
	Q subq=sub(q1,q2);
	show(subq);
	Q mulq=mul(q1,q2);
	show(mulq);
	cout<<q1.up*(q1.down/abs(q1.down))<<" "<<q2.up*(q2.down/abs(q2.down))<<"\n";
	cout<<abs(q1.down)<<" "<<abs(q2.down);	
	return 0;
} 
