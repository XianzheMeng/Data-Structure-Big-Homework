#include<iostream>
#include<string.h>

using namespace std;

void convert(char* str){
	char stack[100];
	char ministack[100];
	int count=0;
	int stackcount=0;
	int record=0;
	for(int i=0;i<strlen(str);i++){
		if(str[i]=='('){
			ministack[count++]=str[++i];
		}
		else if(str[i]==')'){
			for(int i=0;i<count;i++){
				stack[stackcount++]=ministack[i];
			}
			ministack[--count]=0;
		}
		else{
			for(int i=0;i<count;i++){
				stack[stackcount++]=ministack[i];
			}
			stack[stackcount++]=str[i];
		}
	}
	for(int i=stackcount-1;i>=0;i--){
		if(stack[i]=='A') cout<<"sae";
		else if(stack[i]=='B') cout<<"tsaedsae";
		else cout<<stack[i];
	}
}

int main(){
	char str[100];
	cin>>str;
	convert(str);
	return 0;
}
