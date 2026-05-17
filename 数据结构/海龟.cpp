#include<iostream>
#include<graphics.h>
#include<math.h>

#define UP 0
#define DOWN 1
#define PI 3.1415926

typedef int penState;
typedef struct {float v,h; } aPoint;
typedef struct{
	int heading;
	penState pen;
	int color;
	aPoint Pos;
} *newTurtle;

newTurtle Turtle=(newTurtle)malloc(sizeof(newTurtle));

void StartTurtleGraphics(){
	initgraph(640,480);
	outtextxy(100,20,"ÃÏÏÜ†´");
	return;
}

void StartTurtle(){
	Turtle->heading=0;
	Turtle->pen=DOWN;
	Turtle->color=0;
	Turtle->Pos.v=320;
	Turtle->Pos.h=240;
	return;
}

void PenUP(){
	Turtle->pen=UP;
	return;
}

void PenDown(){
	Turtle->pen=DOWN;
	return;
}

int TurtleHeading(){
	return Turtle->heading;
}

aPoint* TurtlePos(){
	return &(Turtle->Pos);
}

void Move(int steps){
	int tempv=Turtle->Pos.v;
	int temph=Turtle->Pos.h;
	Turtle->Pos.v+=steps*cos(Turtle->heading*PI/360);
	Turtle->Pos.h+=steps*sin(Turtle->heading*PI/360);
	line(tempv,temph,Turtle->Pos.v,Turtle->Pos.h);
	return;
}

void Turn(int degrees){
	Turtle->heading=(Turtle->heading+degrees)%360;
	return;
}

void MoveTTo(aPoint newPos){
	if(Turtle->pen==DOWN) 
	line(Turtle->Pos.v,Turtle->Pos.h,newPos.v,newPos.h);
	Turtle->Pos.v=newPos.v;
	Turtle->Pos.h=newPos.h;
	return;
}

void TurnTTo(float angle){
	Turtle->heading=angle;
	return;
}

void SetTurtleColor(int color){
	Turtle->color=color;
	return;
}


int main(){
	StartTurtleGraphics();
	StartTurtle();
	MoveTTo({30,400});
	SetTurtleColor(2314);
	MoveTTo({500,200});
	Move(50);
	Turn(50);
	Move(-50);
	TurnTTo(-50);
	Move(50);
	PenUP();
	MoveTTo({0,0});
	while(1){}
	return 0;
}
