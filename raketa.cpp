#include "raketa.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"

#define DEG2RAD(x) (x / 180.0 * M_PI)

raketa::raketa(float uhel, float prach, float x,float y): strela(uhel, prach, x, y)
{
	//nastavuji natoceni a rychlost strely
	natoceni = 0;
	ay = 0.1;
	vel=50;

}
void raketa::kresli(){
	//udavam pozici a natoceni rakety a nasledni ji kreslim
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
bool raketa::pohni(Krajina*k){
	//raketa se potrebuje natocit, jinak se hybe jako strela
	sx = x;
	sy = y;
	return strela::pohni(k);
}
