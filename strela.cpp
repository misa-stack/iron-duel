#include "strela.h"
#include <SDL/SDL.h>
#include "pozadi.h"
#include "grafika.h"
#include "krajina.h"
strela::strela(float x,float y): x(x), y(y)
{
	//zada rychlost strely
	vx = 10;
	vy = -3;
	ay= 0.45;


}
void strela::kresli(){

// maluju strelu
	barva(100,255,118);

	kruh(x,y,3);

}
void strela::pohni(){

//hejbu se strelou
	x= x + vx;
	y= y + vy;
	vy= vy+ ay;



}
