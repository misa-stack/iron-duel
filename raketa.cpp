#include "raketa.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"

raketa::raketa(float x,float y): x(x), y(y)
{

	vx = 4;
	vy = -3;
	ay = 0.1;

}
void raketa::kresli(){


	barva(100,255,118);
		obdelnik(x,y,x+4,y+2);

}
void raketa::pohni(){


	x= x + vx;
	y= y + vy;
	vy= vy+ ay;
}
