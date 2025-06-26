#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "vybuch.h"
#include <list>
#include "tank.h"

extern std::list<Tank*> tanky;
extern std::list<Tank*>::iterator a;
extern std::list<strela*> strely;
extern std::list<strela*>::iterator s;

strela::strela(float uhel, float prach, float x,float y): x(x), y(y)
{
	ay = 0.1;
	//zada rychlost strely
	vel=30;
	vx=prach*cos(uhel);
	vy=prach*sin(uhel);
	x+20*2*cos(uhel);
	y+20*2*sin(uhel);

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
	for(a = tanky.begin(); a != tanky.end(); a++)
	{
		if (sqrt ((*s)->x * (*s)->x + (*s)->y * (*s)->y)< (*a)->rk)
			return true;
		else
			return false;
	}


}
