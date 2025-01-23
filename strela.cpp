#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
strela::strela(float x,float y): x(x), y(y)
{
	vx = 3;
	vy = -3;
	ay= 0.15;
	s = 2;

}
void strela::kresli(){


	barva(100,255,118);
	if (s == 1){
	kruh(x,y,3);}
	else{
		obdelnik(x,y,x+4,y+2);
	}
}
void strela::pohni(){
	if (s== 1){


	x= x + vx;
	y= y + vy;
	vy= vy+ ay;}
	if(s == 2){
		ay=0.045;
		x= x +vx+2;
		y= y + vy+2;
		vy= vy+ ay;
	}
}
void strela::zmena(){
s++;
if (s ==3)
	s=1;
}
