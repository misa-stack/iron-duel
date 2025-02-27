#include "raketa.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"

#define DEG2RAD(x) (x / 180.0 * M_PI)

raketa::raketa(float x,float y): x(x), y(y)
{
	natoceni = 0;
	vx = 4;
	vy = -3;
	ay = 0.1;

}
void raketa::kresli(){

	natoceni = atan2(y-sy,x-sx) - M_PI / 2;
	barva(100,255,118);
	bod(x + 10 * cos(DEG2RAD(90) + natoceni), y + 10 * sin(DEG2RAD(90) + natoceni));
	cara(x + 10 * cos(DEG2RAD(240) + natoceni), y + 10 * sin(DEG2RAD(240) + natoceni));
	cara(x + 10 * cos(DEG2RAD(300) + natoceni), y + 10 * sin(DEG2RAD(300) + natoceni));
	cara(x + 10 * cos(DEG2RAD(90) + natoceni), y + 10 * sin(DEG2RAD(90) + natoceni));
	trojuhelnik(x + 10 * cos(DEG2RAD(90) + natoceni), y + 10 * sin(DEG2RAD(90) + natoceni),
		    x + 10 * cos(DEG2RAD(240) + natoceni), y + 10 * sin(DEG2RAD(240) + natoceni),
		    x + 10 * cos(DEG2RAD(300) + natoceni), y + 10 * sin(DEG2RAD(300) + natoceni));
}
void raketa::pohni(){

	sx = x;
	sy = y;

	x= x + vx;
	y= y + vy;

	vy= vy+ ay;
}
