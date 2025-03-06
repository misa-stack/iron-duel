#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
strela::strela(float x,float y): x(x), y(y)
{
    ay = 0.0055;
}
void strela::kresli(){
	barva(100,255,118);
	kruh(x,y,3);
}
void strela::pohni(){
	x= x + vx;
	y= y + vy;
	vy= vy+ ay;
}
