#include<iostream>

using namespace std;

struct Complex{
	float re;
	float im;
}; 

void show(Complex cp){
	if(cp.re>0&&cp.im>0)   cout<<cp.re<<"+i"<<cp.im;
	if(cp.re>0&&cp.im==0)  cout<<cp.re;
	if(cp.re>0&&cp.im<0)   cout<<cp.re<<"-i"<<-cp.im;
	if(cp.re==0&&cp.im>0)  cout<<"i"<<cp.im;
	if(cp.re==0&&cp.im==0) cout<<"0";
	if(cp.re==0&&cp.im<0)  cout<<"-i"<<-cp.im;
	if(cp.re<0&&cp.im>0)   cout<<cp.re<<"+i"<<cp.im;
	if(cp.re<0&&cp.im==0)  cout<<cp.re;
	if(cp.re<0&&cp.im<0)   cout<<cp.re<<"-i"<<-cp.im;
	cout<<"\n";
	return;
}

Complex add(Complex cp1,Complex cp2){
	Complex cp;
	cp.re=cp1.re+cp2.re;
	cp.im=cp1.im+cp2.im;
	return cp;
}

Complex sub(Complex cp1,Complex cp2){
	Complex cp;
	cp.re=cp1.re-cp2.re;
	cp.im=cp1.im-cp2.im;
	return cp;
}

Complex mul(Complex cp1,Complex cp2){
	Complex cp;
	cp.re=cp1.re*cp2.re-cp1.im*cp2.im;
	cp.im=cp1.re*cp2.im+cp1.im*cp2.re;
	return cp;
}

int main(){
	float re1,im1,re2,im2;
	cin>>re1>>im1>>re2>>im2;
	Complex cp1,cp2;
	cp1.re=re1;
	cp1.im=im1;
	cp2.re=re2;
	cp2.im=im2;
	show(cp1);
	show(cp2);
	Complex addcp=add(cp1,cp2);
	show(addcp);
	Complex subcp=sub(cp1,cp2);
	show(subcp);
	Complex mulcp=mul(cp1,cp2);
	show(mulcp);
	cout<<cp1.re<<" "<<cp2.re<<"\n";
	cout<<cp1.im<<" "<<cp2.im;	
	return 0;
} 
