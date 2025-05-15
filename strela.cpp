#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "vybuch.h"

strela::strela(float x,float y): x(x), y(y)
{
	ay = 0.1;
	//zada rychlost strely
	vel=30;
}
void strela::kresli(){

	// maluju strelu
	barva(100,255,118);

	kruh(x,y,3);

}
bool strela::pohni(Krajina *k){

	//hejbu se strelou
	x= x + vx;
	y= y + vy;
	vy= vy+ ay;
	if(k->jeHlina(x, y))
	{

		return true;
	}
	else
	{
		return false;
	}




}
